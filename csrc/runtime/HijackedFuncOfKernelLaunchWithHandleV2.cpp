/* -------------------------------------------------------------------------
 * This file is part of the MindStudio project.
 * Copyright (c) 2025 Huawei Technologies Co.,Ltd.
 *
 * MindStudio is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * ------------------------------------------------------------------------- */


#include "HijackedFunc.h"

#include <algorithm>
#include <elf.h>
#include <string>
#include "RuntimeOrigin.h"
#include "ascendcl/AscendclOrigin.h"
#include "core/LocalProcess.h"
#include "core/FuncSelector.h"
#include "utils/InjectLogger.h"
#include "utils/Future.h"
#include "utils/FileSystem.h"
#include "utils/Ustring.h"
#include "core/PlatformConfig.h"
#include "core/BinaryInstrumentation.h"
#include "runtime/inject_helpers/BBCountDumper.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "runtime/inject_helpers/KernelReplacement.h"
#include "runtime/inject_helpers/DBITask.h"
#include "runtime/inject_helpers/MemoryDataCollect.h"
#include "runtime/inject_helpers/InstrReport.h"
#include "runtime/inject_helpers/ProfConfig.h"
#include "runtime/inject_helpers/ProfDataCollect.h"
#include "runtime/inject_helpers/ConfigManager.h"
#include "runtime/inject_helpers/LaunchArgs.h"
#include "runtime/inject_helpers/RegisterContext.h"
#include "RuntimeConfig.h"
#include "runtime/inject_helpers/DevMemManager.h"

using namespace std;

void HijackedFuncOfKernelLaunchWithHandleV2::InitParam(void *hdl, const uint64_t tilingKey,
    uint32_t blockDim, rtArgsEx_t *argsInfo,
    rtSmDesc_t *smDesc, rtStream_t stm, const rtTaskCfgInfo_t *cfgInfo)
{
    refreshParamFunc_ = [this, hdl, tilingKey, blockDim, argsInfo, smDesc, stm, cfgInfo]() {
        this->hdl_ = hdl;
        this->blockDim_ = blockDim;
        this->argsInfo_ = argsInfo;
        this->memInfo_ = nullptr;
        this->memSize_ = 0;
        this->stm_ = stm;
        this->argsVec_.clear();
        hostInput_.clear();
        if (argsInfo) {
            this->newArgsInfo_ = *argsInfo;
        }
        this->smDesc_ = smDesc;
        this->cfgInfo_ = cfgInfo;
        this->tilingKey_ = tilingKey;
    };
    refreshParamFunc_();
    devId_ = DeviceContext::GetRunningDeviceId();
    KernelContext::Instance().AddLaunchEvent(hdl, tilingKey, blockDim, argsInfo, stm);
    this->launchId_ = KernelContext::Instance().GetLaunchId();
    this->regId_ = KernelContext::Instance().GetRegisterId(launchId_);
    KernelContext::LaunchEvent event;
    KernelContext::Instance().GetLastLaunchEvent(event);
    this->isSink_ = event.isSink;
    if (argsInfo != nullptr) { KernelContext::Instance().SetArgsSize(argsInfo->argsSize); }
    if (IsSanitizer()) {
        if (cfgInfo != nullptr) { KernelContext::Instance().SetSimtUbDynamicSize(cfgInfo->localMemorySize); }
        KernelContext::Instance().SetKernelParamNum(GetKernelParamNum(argsInfo));
    }
    if (IsOpProf()) {
        this->profObj_ = MakeShared<ProfDataCollect>();
    }
    DBITaskConfig::Instance().argsSize_ = 0;  // avoid multi kernelLaunch,reset invalid argSize=0
    rtDevBinary_t binary;
    bool binaryGetSuccess = KernelContext::Instance().GetDevBinary(KernelContext::KernelHandlePtr{event.hdl}, binary);
    bool needMemLengthInfo = (IsOpProf() && profObj_->IsNeedDumpContext()) || IsSanitizer();
    if (binaryGetSuccess && needMemLengthInfo) {
        KernelContext::Instance().ParseMetaDataFromBinary(binary, argsInfo);
    }
    KernelContext::Instance().ArchiveMemInfo();
}

// timeline 以及 pcsampling功能需要插end 和 start桩
uint64_t HijackedFuncOfKernelLaunchWithHandleV2::PrepareDbiTaskForInstrProf(uint8_t mode, uint8_t *&memInfo)
{
    refreshParamFunc_();
    uint64_t memSize = INSTR_PROF_MEMSIZE;
    KernelMatcher::Config matchConfig;
    std::string path = GetEnv(DEVICE_PROF_DUMP_PATH_ENV);
    std::string pluginPath = mode == INSTR_PROF_MODE_BIU_PERF ?
            ProfConfig::Instance().GetPluginPath(ProfDBIType::INSTR_PROF_END) :
            ProfConfig::Instance().GetPluginPath(ProfDBIType::INSTR_PROF_START);
    std::vector<std::string> extraArgs = mode == INSTR_PROF_MODE_BIU_PERF ? std::vector<std::string>() :
        std::vector<std::string>{START_STUB_COMPILER_ARGS};   
    DBITaskConfig::Instance().Init(BIType::CUSTOMIZE, pluginPath, matchConfig, path, extraArgs);
    memInfo = InitMemory(memSize);
    if (!ExpandArgs(&newArgsInfo_, argsVec_, memInfo, hostInput_, DBITaskConfig::Instance().argsSize_) ||
        !RunDBITask(&hdl_, tilingKey_)) {
        ERROR_LOG("End or start stub run failed.");
    }
    return memSize;
}

void HijackedFuncOfKernelLaunchWithHandleV2::ProfPreForInstrProf(const std::function<bool(void)> &func,
                                                                 const std::function<void(const std::string &)> &bbCountTask,
                                                                 rtStream_t stm)
{
    auto funcStub = [this]() {
        return (rtKernelLaunchWithHandleV2Origin(hdl_, tilingKey_, blockDim_, argsInfo_, smDesc_, stm_, cfgInfo_) == RT_ERROR_NONE);
    };
    if (ProfConfig::Instance().IsPCSamplingEnabled() && KernelContext::Instance().HasSimtSymbol()) {
        uint8_t *memInfo = nullptr;
        uint64_t memSize= PrepareDbiTaskForInstrProf(INSTR_PROF_MODE_PC_SAMPLING, memInfo);
        KernelContext::LaunchEvent event;
        uint64_t tiling = 0;
        if (KernelContext::Instance().GetLastLaunchEvent(event)) {
            tiling = event.tilingKey;
        }
        KernelContext::KernelHandlePtr hdlPtr{hdl_};
        uint64_t kernelAddr;
        if (!KernelContext::Instance().GetDeviceContext().GetKernelAddr(
            KernelContext::KernelHandleArgs{hdlPtr.value, nullptr, tiling}, kernelAddr)) {
            WARN_LOG("Can not get kernel addr for kernel start stub.");
        }
        WriteStringToFile(JoinPath({ProfDataCollect::GetAicoreOutputPath(devId_), "pc_start_pcsampling.txt"}),
            NumToHexString(kernelAddr), std::fstream::out | std::fstream::binary);
        profObj_->InstrProfData(stm, funcStub);
        profObj_->GenRecordData(memSize, memInfo, PCOFFSET_RECORD);
    }
    if (ProfConfig::Instance().IsTimelineEnabled()) {
        uint8_t *memInfo = nullptr;
        PrepareDbiTaskForInstrProf(INSTR_PROF_MODE_BIU_PERF, memInfo);
        ProfPre(funcStub, bbCountTask, stm);
    } else {
        ProfPre(func, bbCountTask, stm);
    }
}

void HijackedFuncOfKernelLaunchWithHandleV2::ProfPre(const std::function<bool(void)> &func,
                                                     const std::function<void(const std::string &)> &bbCountTask,
                                                     rtStream_t stm)
{
    KernelContext::LaunchEvent event;
    KernelContext::Instance().GetLaunchEvent(launchId_, event);
    profObj_->ProfInit(event.hdl, nullptr, false); // pc_start落盘txt文件
    profObj_->ProfData(stm, func);
    if (profObj_->IsBBCountNeedGen()) {
        refreshParamFunc_();
        bbCountTask(ProfDataCollect::GetAicoreOutputPath(devId_));
    }
}

void HijackedFuncOfKernelLaunchWithHandleV2::ProfPost()
{
    if (profObj_->IsBBCountNeedGen()) {
        rtError_t bbLaunchRet = rtStreamSynchronizeOrigin(this->stm_);
        if (bbLaunchRet != RT_ERROR_NONE) {
            WARN_LOG("BB count kernel launch failed, ret is %d.", bbLaunchRet);
        } else {
            profObj_->GenBBcountFile(regId_, this->memSize_, this->memInfo_);
        }
    }
    std::string path = GetEnv(DEVICE_PROF_DUMP_PATH_ENV);
    if (profObj_->IsMemoryChartNeedGen()) {
        ProfConfig::Instance().RestoreMemoryByMode();
        rtStreamSynchronizeOrigin(stm_);
        refreshParamFunc_();
        auto blockDim = GetCoreNumForDbi(blockDim_);
        memSize_ = BLOCK_MEM_SIZE * blockDim;
        DBITaskConfig::Instance().Init(BIType::CUSTOMIZE, ProfConfig::Instance().GetPluginPath(), {}, path);
        memInfo_ = InitMemory(memSize_);
        if (ExpandArgs(&newArgsInfo_, argsVec_, memInfo_, hostInput_, DBITaskConfig::Instance().argsSize_) &&
            RunDBITask(&hdl_, tilingKey_) && originfunc_ != nullptr) {
            originfunc_(hdl_, tilingKey_, blockDim_, &newArgsInfo_, smDesc_, stm_, cfgInfo_);
            rtError_t memchartLaunchRet = rtStreamSynchronizeOrigin(this->stm_);
            if (memchartLaunchRet != RT_ERROR_NONE) {
                WARN_LOG("Memory chart kernel launch failed, ret is %d.", memchartLaunchRet);
            } else {
                profObj_->GenDBIData(memSize_, memInfo_);
            }
        }
    }
    std::string socVersion = DeviceContext::Local().GetSocVersion();
    if (profObj_->IsOperandRecordNeedGen(socVersion)) {
        rtStreamSynchronizeOrigin(stm_);
        refreshParamFunc_();
        uint64_t sizePerAllType = static_cast<uint32_t>(OperandType::END) * sizeof(OperandRecord) + SIMT_THREAD_GAP;
        memSize_ = sizeof(OperandHeader) + (sizePerAllType * (MAX_THREAD_NUM + 1) + BLOCK_GAP) * GetCoreNumForDbi(blockDim_);
        DBITaskConfig::Instance().Init(BIType::CUSTOMIZE,
            ProfConfig::Instance().GetPluginPath(ProfDBIType::OPERAND_RECORD), {}, path);
        memInfo_ = InitMemory(memSize_);
        if (ExpandArgs(&newArgsInfo_, argsVec_, memInfo_, hostInput_, DBITaskConfig::Instance().argsSize_) &&
            RunDBITask(&hdl_, tilingKey_) && originfunc_ != nullptr) {
            originfunc_(hdl_, tilingKey_, blockDim_, &newArgsInfo_, smDesc_, stm_, cfgInfo_);
            rtError_t opRecordLaunchRet = rtStreamSynchronizeOrigin(this->stm_);
            if (opRecordLaunchRet != RT_ERROR_NONE) {
                WARN_LOG("Operand record kernel launch failed, ret is %d.", opRecordLaunchRet);
            } else {
                profObj_->GenRecordData(memSize_, memInfo_, OPERAND_RECORD);
            }
        }
    }
    profObj_->PostProcess();
}

void HijackedFuncOfKernelLaunchWithHandleV2::SanitizerPre()
{
    std::string kernelName = KernelContext::Instance().GetLaunchName();
    if (!(this->skipSanitizer_ = SkipSanitizer(kernelName))) {
        if (isSink_) { return; }
        ReportKernelSummary(launchId_);
        KernelContext::Instance().ReportKernelBinary(KernelContext::KernelHandlePtr{this->hdl_});
        RunDBITask(&this->hdl_, this->tilingKey_);
        KernelContext::LaunchEvent event;
        if (!KernelContext::Instance().GetLastLaunchEvent(event)) { return; }

        rtDevBinary_t binary;
        KernelContext::KernelHandlePtr hdl{event.hdl};
        if (!KernelContext::Instance().GetDevBinary(hdl, binary, true) &&
            !KernelContext::Instance().GetDevBinary(hdl, binary, false)) { return; }

        std::map<std::string, Elf64_Shdr> headers;
        if (!GetSectionHeaders(binary, headers)) {
            return;
        }

        sections_ = GetAllocSectionHeaders(headers);
        ReportSectionsMalloc(event.pcStartAddr, sections_);
        auto &opMemInfo = KernelContext::Instance().GetOpMemInfo();
        ReportOverflowMalloc(opMemInfo);
        /// 入队算子输入个数和tiling
        MemoryManage::Instance().CacheMemoryCount(opMemInfo.inputParamsAddrInfos.size() + 1);
        /// 入队空的extra触发MemoryManage类的内存筛选
        if (opMemInfo.inputParamsAddrInfos.size() > 0) {
            MemoryManage::Instance().CacheMemory<MemoryOpType::MALLOC>(0x0,
                opMemInfo.inputParamsAddrInfos[0].memInfoSrc, 0x0, false);
        }
        this->memInfo_ = __sanitizer_init(this->blockDim_);
        if (this->memInfo_) {
            ExpandArgs(&this->newArgsInfo_, this->argsVec_, this->memInfo_, hostInput_, DBITaskConfig::Instance().argsSize_);
        }
    }
    auto ret = aclrtSetOpExecuteTimeOutOrigin(12);
    DEBUG_LOG("after aclrtSetOpExecuteTimeOutOrigin ret %d.", ret);
}

HijackedFuncOfKernelLaunchWithHandleV2::HijackedFuncOfKernelLaunchWithHandleV2()
    : HijackedFuncOfKernelLaunchWithHandleV2::HijackedFuncType(
    std::string(RuntimeLibName()), std::string("rtKernelLaunchWithHandleV2")) {}

void HijackedFuncOfKernelLaunchWithHandleV2::Pre(void *hdl, const uint64_t tilingKey, uint32_t blockDim,
    rtArgsEx_t *argsInfo, rtSmDesc_t *smDesc, rtStream_t stm, const rtTaskCfgInfo_t *cfgInfo)
{
    LogRtArgsExt(argsInfo);
    InitParam(hdl, tilingKey, blockDim, argsInfo, smDesc, stm, cfgInfo);
    if (argsInfo == nullptr) {
        WARN_LOG("argsInfo is null, stop hijackting.");
        return;
    }
    auto bbCountTask = [this, tilingKey](const std::string &outputPath = "") {
        DBITaskConfig::Instance().argsSize_ = GetArgsSize(&newArgsInfo_);
        if (BBCountDumper::Instance().Replace(&hdl_, tilingKey, launchId_, outputPath)) {
            memSize_ = BBCountDumper::Instance().GetMemSize(regId_, outputPath);
            memInfo_ = InitMemory(memSize_);
            if (memInfo_ != nullptr) {
                ExpandArgs(&newArgsInfo_, argsVec_, memInfo_, hostInput_, DBITaskConfig::Instance().argsSize_);
            }
        }
    };

    if (IsOpProf()) {
        if (ProfConfig::Instance().IsSimulator()) {
            profObj_->ProfInit(hdl, nullptr, false);
        } else {
            auto func = [hdl, tilingKey, blockDim, argsInfo, smDesc, stm, cfgInfo]() {
                return (rtKernelLaunchWithHandleV2Origin(hdl, tilingKey, blockDim, argsInfo, smDesc, stm, cfgInfo) == RT_ERROR_NONE);
            };
            ProfPreForInstrProf(func, bbCountTask, stm);
        }
    }

    if (IsSanitizer()) {
        SanitizerPre();
    }
}

rtError_t HijackedFuncOfKernelLaunchWithHandleV2::Call(void *hdl, const uint64_t tilingKey, uint32_t blockDim,
    rtArgsEx_t *argsInfo, rtSmDesc_t *smDesc, rtStream_t stm, const rtTaskCfgInfo_t *cfgInfo)
{
    Pre(hdl, tilingKey, blockDim, argsInfo, smDesc, stm, cfgInfo);
    if (originfunc_ == nullptr) {
        ERROR_LOG("HijackedFuncOfKernelLaunchWithHandleV2 Hijacked func pointer is nullptr.");
        return EmptyFunc();
    }
    if (IsOpProf() && profObj_ && !profObj_->IsNeedRunOriginLaunch()) {
        return Post(RT_ERROR_NONE);
    }
    return Post(originfunc_(this->hdl_, tilingKey, blockDim, &this->newArgsInfo_, smDesc, stm, cfgInfo));
}

void HijackedFuncOfKernelLaunchWithHandleV2::SanitizerPost()
{
    if ((this->memInfo_ || isSink_) && !this->skipSanitizer_) {
        // wait for kernel execution done, and catch potential exception
        rtStreamSynchronizeOrigin(this->stm_);
        if (isSink_) {
            KernelDumper::Instance().LaunchDumpTask(stm_);
            return;
        }

        KernelContext::LaunchEvent event;
        KernelContext::Instance().GetLaunchEvent(launchId_, event);

        ReportOpMallocInfo(&this->newArgsInfo_, KernelContext::Instance().GetOpMemInfo());
        __sanitizer_finalize(this->memInfo_, this->blockDim_);
        ReportSectionsFree(event.pcStartAddr, sections_);
        ReportOverflowFree(KernelContext::Instance().GetOpMemInfo());
        ReportOpFreeInfo(KernelContext::Instance().GetOpMemInfo());
    }
}

rtError_t HijackedFuncOfKernelLaunchWithHandleV2::Post(rtError_t ret)
{
    if (!this->argsInfo_) {
        return ret;
    }
    // 将拷贝出的信息落盘
    if (IsSanitizer()) {
        SanitizerPost();
    }
    if (IsOpProf() && profObj_) {
        if (ProfConfig::Instance().IsSimulator()) {
            rtStreamSynchronizeOrigin(this->stm_);
            profObj_->ProfData();
        } else {
            ProfPost();
        }
    }
    KernelContext::Instance().ClearArgsInfo();
    DevMemManager::Instance().Free();
    return ret;
}
