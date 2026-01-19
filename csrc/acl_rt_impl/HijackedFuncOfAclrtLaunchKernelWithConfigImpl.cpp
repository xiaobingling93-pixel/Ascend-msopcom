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
#include "core/FuncSelector.h"
#include "runtime/inject_helpers/ArgsHandleContext.h"
#include "runtime/inject_helpers/ArgsManager.h"
#include "runtime/inject_helpers/DevMemManager.h"
#include "runtime/inject_helpers/FuncManager.h"
#include "runtime/inject_helpers/InstrReport.h"
#include "runtime/inject_helpers/KernelReplacement.h"
#include "runtime/inject_helpers/LaunchManager.h"
#include "runtime/inject_helpers/LocalDevice.h"
#include "runtime/inject_helpers/MemoryDataCollect.h"
#include "runtime/inject_helpers/ProfConfig.h"
#include "runtime/inject_helpers/InstrReport.h"
#include "runtime/inject_helpers/BBCountDumper.h"
#include "runtime/inject_helpers/DBITask.h"
#include "runtime/inject_helpers/LaunchArgs.h"
#include "utils/InjectLogger.h"
#include "utils/Protocol.h"

using namespace std;

namespace {

void ReportKernelBinary(RegisterContextSP regCtx)
{
    auto const &elfData = regCtx->GetElfData();
    PacketHead head { PacketType::KERNEL_BINARY };
    std::string buffer(elfData.cbegin(), elfData.cend());
    LocalDevice::Local().Notify(Serialize(head, buffer.size()) + std::move(buffer));
}

} // namespace [Dummy]

HijackedFuncOfAclrtLaunchKernelWithConfigImpl::HijackedFuncOfAclrtLaunchKernelWithConfigImpl()
    : HijackedFuncType(AclRuntimeLibName(), "aclrtLaunchKernelWithConfigImpl") {}

bool HijackedFuncOfAclrtLaunchKernelWithConfigImpl::InitParam(
    aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream,
    aclrtLaunchKernelCfg *cfg, aclrtArgsHandle argsHandle, void *reserve)
{
    refreshParamFunc_ = [this, funcHandle, blockDim, stream, cfg, argsHandle, reserve]() {
        funcHandle_ = funcHandle;
        blockDim_ = blockDim;
        stream_ = stream;
        cfg_ = cfg;
        argsHandle_ = argsHandle;
        reserve_ = reserve;
        memInfo_ = nullptr;
        memSize_ = 0;
        skipSanitizer_ = false;
    };
    refreshParamFunc_();
    launchCtx_ = LaunchManager::Local().CreateContext(funcHandle, blockDim, stream, argsHandle);
    if (launchCtx_ == nullptr) {
        DEBUG_LOG("Create launch context failed.");
        return false;
    }

    funcCtx_ = launchCtx_->GetFuncContext();
    regId_ = funcCtx_->GetRegisterContext()->GetRegisterId();
    newArgsCtx_ = launchCtx_->GetArgsContext();
    devId_ = DeviceContext::GetRunningDeviceId();
    isSink_ = LaunchManager::GetOrCreateStreamInfo(stream).binded;
    if (IsOpProf()) {
        profObj_ = std::make_shared<ProfDataCollect>(launchCtx_);
    }
    bool needMemLengthInfo = (IsOpProf() && profObj_ && profObj_->IsNeedDumpContext()) || IsSanitizer();
    if (needMemLengthInfo) {
        auto &memInfo = LaunchManager::Local().GetCurrentMemInfo();
        launchCtx_->UpdateOpMemInfoByAdump(memInfo);
    }
    LaunchManager::Local().ArchiveMemInfo();
    return true;
}

void HijackedFuncOfAclrtLaunchKernelWithConfigImpl::ProfPre(const std::function<bool(void)> &func,
                                                            const std::function<void(const std::string &)> &bbCountTask,
                                                            aclrtStream stm)
{
    profObj_->ProfInit(nullptr, nullptr, false); // pc_start落盘txt文件
    if (profObj_->IsBBCountNeedGen()) {
        bbCountTask(ProfDataCollect::GetAicoreOutputPath(devId_));
    }
    profObj_->ProfData(stm, func);
}

void HijackedFuncOfAclrtLaunchKernelWithConfigImpl::Pre(
    aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream,
    aclrtLaunchKernelCfg *cfg, aclrtArgsHandle argsHandle, void *reserve)
{
    if (!InitParam(funcHandle, blockDim, stream, cfg, argsHandle, reserve)) {
        DEBUG_LOG("Invalid param, stop hijack this launch.");
        return;
    }
    auto bbCountTask = [this](const std::string &outputPath = "") {
        DBITaskConfig::Instance().argsSize_ = launchCtx_->GetArgsContext()->GetLastParamOffset();
        auto stubCtx = BBCountDumper::Instance().Replace(launchCtx_, outputPath);
        if (stubCtx == nullptr) {
            return;
        }
        funcCtx_ = stubCtx;
        launchCtx_->SetDBIFuncCtx(funcCtx_);
        funcHandle_ = funcCtx_->GetFuncHandle();
        memSize_ = BBCountDumper::Instance().GetMemSize(regId_, outputPath);
        memInfo_ = InitMemory(memSize_);
        if (memInfo_ != nullptr) {
            newArgsCtx_ = launchCtx_->GetArgsContext()->Clone();
            newArgsCtx_->ExpandArgs(&memInfo_, sizeof(uintptr_t), DBITaskConfig::Instance().argsSize_);
            auto argsHandleCtx = std::static_pointer_cast<ArgsHandleContext>(newArgsCtx_);
            argsHandle_ = argsHandleCtx->GenerateArgsHandle();
        }
    };
    if (IsOpProf()) {
        if (ProfConfig::Instance().IsSimulator()) {
            profObj_->ProfInit(nullptr, nullptr, false); // 完全切换至aclrt接口时需要删除该函数入参
        } else {
            auto func = [funcHandle, blockDim, stream, cfg, argsHandle, reserve]() {
                return (aclrtLaunchKernelWithConfigImplOrigin(funcHandle, blockDim, stream, cfg, argsHandle, reserve) == ACL_SUCCESS);
            };
            ProfPre(func, bbCountTask, stream);
        }
    }
    if (IsSanitizer()) {
        this->SanitizerPre();
    }
}

aclError HijackedFuncOfAclrtLaunchKernelWithConfigImpl::Call(
    aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream,
    aclrtLaunchKernelCfg *cfg, aclrtArgsHandle argsHandle, void *reserve)
{
    Pre(funcHandle, blockDim, stream, cfg, argsHandle, reserve);
    if (originfunc_ == nullptr) {
        ERROR_LOG("%s Hijacked func pointer is nullptr.", __FUNCTION__);
        return EmptyFunc();
    }
    if (IsOpProf() && profObj_ && !profObj_->IsNeedRunOriginLaunch()) {
        return Post(ACL_ERROR_NONE);
    }
    return Post(originfunc_(funcHandle_, blockDim, stream, cfg, argsHandle_, reserve));
}

void HijackedFuncOfAclrtLaunchKernelWithConfigImpl::SanitizerPre()
{
    std::string kernelName = launchCtx_->GetFuncContext()->GetKernelName();
    skipSanitizer_ = SkipSanitizer(kernelName);
    if (skipSanitizer_) {
        return;
    }
    if (isSink_) { return; }
    ReportKernelSummary(launchCtx_);
    ReportKernelBinary(launchCtx_->GetFuncContext()->GetRegisterContext());
    memInfo_ = __sanitizer_init(blockDim_);
    if (memInfo_ == nullptr) {
        return;
    }
    // expand args
    auto argsCtx = launchCtx_->GetArgsContext();
    if (!argsCtx->ExpandArgs(&memInfo_, sizeof(uintptr_t), DBITaskConfig::Instance().argsSize_)) {
        WARN_LOG("Expand sanitizer kernel args failed.");
        return;
    }
    auto argsHandleCtx = std::static_pointer_cast<ArgsHandleContext>(argsCtx);
    argsHandle_ = argsHandleCtx->GenerateArgsHandle();

    auto newFuncCtx = RunDBITask(launchCtx_);
    // 似乎动态插桩的argsHandle不需要更新funcHandle也能行，先这样吧
    if (newFuncCtx) {
        funcCtx_ = newFuncCtx;
        launchCtx_->SetDBIFuncCtx(funcCtx_);
        funcHandle_ = funcCtx_->GetFuncHandle();
    }
}

void HijackedFuncOfAclrtLaunchKernelWithConfigImpl::SanitizerPost()
{
    if (skipSanitizer_) {
        // 对于 <<<>>> 场景，编译器也会在算子调用符处插入 __sanitizer_finalize，因此为了防止
        // 编译器插入的 __sanitizer_finalize 生效，需要在此处将记录内存状态设置为失效
        DevMemManager::Instance().SetMemoryInitFlag(false);
    } else if (isSink_) {
        rtStreamSynchronizeOrigin(stream_);
        KernelDumper::Instance().LaunchDumpTask(stream_, true);
    } else if (memInfo_) {
        if (launchCtx_ == nullptr) {
            return;
        }

        // wait for kernel execution done, and catch potential exception
        rtStreamSynchronizeOrigin(stream_);

        auto const &elfData = funcCtx_->GetRegisterContext()->GetElfData();
        std::map<std::string, Elf64_Shdr> headers;
        if (!GetSectionHeaders(elfData, headers)) {
            return;
        }

        auto allocHeaders = GetAllocSectionHeaders(headers);
        auto startPC = funcCtx_->GetStartPC();
        ReportSectionsMalloc(startPC, allocHeaders);
        __sanitizer_finalize(memInfo_, blockDim_);
        ReportSectionsFree(startPC, allocHeaders);
    }
}

void HijackedFuncOfAclrtLaunchKernelWithConfigImpl::DoOperandRecord()
{
    std::string socVersion = DeviceContext::Local().GetSocVersion();
    if (!profObj_->IsOperandRecordNeedGen(socVersion)) {
        return;
    }
    KernelMatcher::Config matchConfig;
    std::string path = GetEnv(DEVICE_PROF_DUMP_PATH_ENV);
    DBITaskConfig::Instance().Init(BIType::CUSTOMIZE,
                                   ProfConfig::Instance().GetPluginPath(PluginType::OPERAND_RECORD), matchConfig, path);
    refreshParamFunc_();
    uint64_t sizePerAllType = static_cast<uint32_t>(OperandType::END) * sizeof(OperandRecord) + SIMT_THREAD_GAP;
    memSize_ = sizeof(OperandHeader) + (sizePerAllType * 2049 + BLOCK_GAP) *  GetCoreNumForDbi(blockDim_);
    auto argsCtx = launchCtx_->GetArgsContext()->Clone();
    memInfo_ = InitMemory(memSize_);
    if (memInfo_ == nullptr || argsCtx == nullptr || !argsCtx->ExpandArgs(&memInfo_, sizeof(uintptr_t), DBITaskConfig::Instance().argsSize_)) {
        WARN_LOG("memory chart gen failed, because of ExpandArgs failed");
        return;
    }
    auto argsHandleCtx = std::static_pointer_cast<ArgsHandleContext>(argsCtx);
    argsHandle_ = argsHandleCtx->GenerateArgsHandle();
    auto newFuncCtx = RunDBITask(launchCtx_);
    if (newFuncCtx) {
        funcCtx_ = newFuncCtx;
        funcHandle_ = funcCtx_->GetFuncHandle();
        launchCtx_->SetDBIFuncCtx(funcCtx_);
        originfunc_(funcHandle_, blockDim_, stream_, cfg_, argsHandle_, reserve_);
        aclError ret = aclrtSynchronizeStreamImplOrigin(stream_);
        if (ret == ACL_SUCCESS) {
            profObj_->GenRecordData(memSize_, memInfo_, OPERAND_RECORD);
        } else {
            WARN_LOG("Run operand record func failed");
        }
    }
    memInfo_ = nullptr;
}

void HijackedFuncOfAclrtLaunchKernelWithConfigImpl::ProfPost()
{
    if (profObj_->IsBBCountNeedGen()) {
        aclrtSynchronizeStreamImplOrigin(stream_);
        profObj_->GenBBcountFile(regId_, memSize_, memInfo_);
    }
    if (profObj_->IsMemoryChartNeedGen()) {
        KernelMatcher::Config matchConfig;
        std::string path = GetEnv(DEVICE_PROF_DUMP_PATH_ENV);
        DBITaskConfig::Instance().Init(BIType::CUSTOMIZE, ProfConfig::Instance().GetPluginPath(), matchConfig, path);
        refreshParamFunc_();
        auto blockDim = GetCoreNumForDbi(blockDim_);
        memSize_ = BLOCK_MEM_SIZE * blockDim;
        auto argsCtx = launchCtx_->GetArgsContext()->Clone();
        memInfo_ = InitMemory(memSize_);
        if (memInfo_ == nullptr || argsCtx == nullptr ||
            !argsCtx->ExpandArgs(&memInfo_, sizeof(uintptr_t), DBITaskConfig::Instance().argsSize_)) {
            WARN_LOG("memory chart gen failed, because of ExpandArgs failed");
            return;
        }
        auto argsHandleCtx = std::static_pointer_cast<ArgsHandleContext>(argsCtx);
        argsHandle_ = argsHandleCtx->GenerateArgsHandle();
        auto newFuncCtx = RunDBITask(launchCtx_);
        if (newFuncCtx) {
            funcCtx_ = newFuncCtx;
            funcHandle_ = funcCtx_->GetFuncHandle();
            launchCtx_->SetDBIFuncCtx(funcCtx_);
            originfunc_(funcHandle_, blockDim_, stream_, cfg_, argsHandle_, reserve_);
            aclrtSynchronizeStreamImplOrigin(stream_);
            profObj_->GenDBIData(memSize_, memInfo_);
        }
        memInfo_ = nullptr;
    }
    DoOperandRecord();
    profObj_->PostProcess();
}

aclError HijackedFuncOfAclrtLaunchKernelWithConfigImpl::Post(aclError ret)
{
    if (ret != ACL_SUCCESS) {
        return ret;
    }
    if (launchCtx_ == nullptr) {
        return ret;
    }
    if (IsSanitizer()) {
        SanitizerPost();
    }
    if (IsOpProf() && profObj_) {
        if (ProfConfig::Instance().IsSimulator()) {
            aclrtSynchronizeStreamImplOrigin(stream_);
            profObj_->ProfData();
        } else {
            ProfPost();
        }
    }
    DevMemManager::Instance().Free();
    return ret;
}
