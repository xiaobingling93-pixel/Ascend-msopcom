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


#include <cstdint>
#include <string>
#include <vector>

#include "runtime/inject_helpers/ArgsHandleContext.h"
#include "runtime/inject_helpers/ArgsManager.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/KernelReplacement.h"
#include "runtime/inject_helpers/LaunchManager.h"
#include "runtime/inject_helpers/RegisterContext.h"
#include "runtime/inject_helpers/KernelMetaDataParser.h"
#include "utils/ElfLoader.h"
#include "utils/InjectLogger.h"

#include "LaunchContext.h"

using namespace std;
namespace {
constexpr char const *MIX_AIC_TAIL = "_mix_aic";
constexpr char const *MIX_AIV_TAIL = "_mix_aiv";
}
bool DumpConfig(const string &outputDir, const DumperContext &config)
{
    nlohmann::json jsonData;
    if (!config.ToJson(jsonData)) {
        ERROR_LOG("Kernel context save failed, create json failed");
        return false;
    }
    string content = jsonData.dump();
    string binFilePath = JoinPath({outputDir, KERNEL_CONFIG_NAME});
    size_t written = WriteBinary(binFilePath, content.data(), content.size());
    if (written != content.size()) {
        ERROR_LOG("Kernel context save failed, json write failed");
        return false;
    }
    return true;
}

bool LaunchContext::Save(const std::string &outputPath) const
{
    RegisterContextSP regCtx = funcCtx_->GetRegisterContext();
    string kernelObjPath = JoinPath({outputPath, KERNEL_BIN_NAME});
    if (!regCtx->Save(kernelObjPath)) {
        ERROR_LOG("save aicore failed");
        return false;
    }
    auto &memInfoHistory = LaunchManager::Local().GetMemInfoHistory();
    if (param_.launchId >= memInfoHistory.size()) {
        ERROR_LOG("Invalid launch index: %lu, size: %lu", param_.launchId, memInfoHistory.size());
        return false;
    }
    DumperContext config{};
    if (!argsCtx_->Save(outputPath, config, memInfoHistory[param_.launchId], IsSink())) {
        ERROR_LOG("Kernel context save failed, dump kernel args failed");
        return false;
    }
    auto &devCtx = ::DeviceContext::Local();
    uint64_t tilingKey {};
    bool hasTilingKey = funcCtx_->GetTilingKey(tilingKey);
    config.isFFTS = memInfoHistory[param_.launchId].isFFTS;
    config.magic = regCtx->GetMagic();
    config.blockDim = param_.blockDim;
    config.kernelName = funcCtx_->GetKernelName();
    RemoveSuffix(config.kernelName, {MIX_AIC_TAIL, MIX_AIV_TAIL});
    config.tilingKey = tilingKey;
    config.binPath = JoinPath({outputPath, KERNEL_BIN_NAME});
    config.hasTilingKey = hasTilingKey;
    config.devId = devCtx.GetDeviceId();
    config.visibleDevId = devCtx.GetVisibleDeviceId();
    config.isAclNew = true;
    return DumpConfig(outputPath, config);
}

FuncContextSP LaunchContext::GetDBIFuncCtx() const
{
    if (dbiFuncCtx_.expired()) {
        DEBUG_LOG("dbiFuncCtx_ is expired");
        return nullptr;
    }
    return dbiFuncCtx_.lock();
}

void LaunchContext::UpdateOpMemInfoByAdump(OpMemInfo &memInfo) const
{
    uint32_t magic = funcCtx_->GetRegisterContext()->GetMagic();
    if (memInfo.isForSetException) {
        DEBUG_LOG("rtSetExceptionExtInfo is called, not parse binary file");
        SetFftsInfo(memInfo, magic);
        memInfo.isForSetException = false;
        return;
    }
    memInfo.Clear();
    std::string kernelName = funcCtx_->GetKernelName();

    // overflow 地址信息不应该被覆盖
    bool hasOverflowAddr = memInfo.hasOverflowAddr;
    uint64_t overflowAddr = memInfo.overflowAddr;
    if (!funcCtx_->GetRegisterContext()->ParseMetaData(kernelName, memInfo)) {
        return;
    }
    memInfo.hasOverflowAddr = hasOverflowAddr;
    memInfo.overflowAddr = overflowAddr;
    SetFftsInfo(memInfo, magic);

    vector<uint8_t> tilingData;
    // raw args do not know size
    tilingData.resize(memInfo.tilingDataSize);
    if (argsCtx_->GetTilingData(tilingData)) {
        // 当前只能根据是否有 tiling 来区分是否为动态算子
        UpdateInputSizeWithTilingData(tilingData, memInfo);
    } else {
        DEBUG_LOG("Current op is static shape without tiling data. Skip adump parsing.");
    }
}
