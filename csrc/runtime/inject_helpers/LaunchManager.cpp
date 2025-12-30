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

#include "LaunchManager.h"

#include <unordered_map>
#include "ArgsManager.h"
#include "FuncManager.h"
#include "DeviceContext.h"

using namespace std;

// init static class variable
std::unordered_map<aclrtStream, StreamInfo> LaunchManager::streamInfos_;

LaunchManager &LaunchManager::Local()
{
    int32_t deviceId = DeviceContext::Local().GetDeviceId();
    static unordered_map<int32_t, LaunchManager> insts;
    static mutex mtx;
    lock_guard<mutex> guard(mtx);
    return insts[deviceId];
}

LaunchContextSP LaunchManager::CreateContext(aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream, aclrtArgsHandle argsHandle)
{
    const auto &streamInfo = GetOrCreateStreamInfo(stream);
    LaunchParam param{blockDim, stream, streamInfo.binded, contexts_.size()};
    auto argsCtx = ArgsManager::Instance().GetContext(argsHandle);
    auto funcCtx = FuncManager::Instance().GetContext(funcHandle);
    if (!argsCtx || !funcCtx) {
        return nullptr;
    }
    auto ctx = make_shared<LaunchContext>(funcCtx, argsCtx, param);
    contexts_.emplace_back(std::move(ctx));
    return contexts_.back();
}

LaunchContextSP LaunchManager::CreateContext(aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream,
                                             ArgsContextSP argsContext)
{
    const auto &streamInfo = GetOrCreateStreamInfo(stream);
    LaunchParam param{blockDim, stream, streamInfo.binded, contexts_.size()};
    auto funcCtx = FuncManager::Instance().GetContext(funcHandle);
    if (!argsContext || !funcCtx) {
        return nullptr;
    }
    auto ctx = make_shared<LaunchContext>(funcCtx, argsContext, param);
    contexts_.emplace_back(std::move(ctx));
    return contexts_.back();
}

LaunchContextSP LaunchManager::GetContext(uint64_t launchId) const
{
    if (launchId >= contexts_.size()) {
        return nullptr;
    }
    return contexts_[launchId];
}

LaunchContextSP LaunchManager::GetLastContext() const
{
    if (contexts_.empty()) {
        return nullptr;
    }
    return contexts_.back();
}
