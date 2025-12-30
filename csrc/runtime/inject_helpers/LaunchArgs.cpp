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

#include "LaunchArgs.h"
#include "KernelContext.h"

#include <algorithm>
#include <mutex>
#include "utils/InjectLogger.h"
#include "DeviceContext.h"

using namespace std;

constexpr size_t PTR_SIZE = sizeof(uintptr_t);
constexpr uint32_t ALIGN_SIZE = 8;
std::mutex g_launchMutex;

uint32_t GetAlignSize(uint32_t s)
{
    return (s + ALIGN_SIZE - 1) / ALIGN_SIZE * ALIGN_SIZE;
}

static bool FillArgsWithMemInfo(rtArgsEx_t &argsInfo, std::vector<uint8_t> &argsWithMemInfo, uint8_t *memInfo,
                                uint32_t headSize, uint32_t &memInfoOffset)
{
    if (headSize > argsInfo.argsSize) {
        ERROR_LOG("ExpandArgs failed, head size is larger than argsSize");
        return false;
    }
    uint32_t alignHeadSize = GetAlignSize(headSize);
    auto args = static_cast<uint8_t const*>(argsInfo.args);
    auto memInfoPtr = reinterpret_cast<uint8_t*>(&memInfo);
    memInfoOffset = alignHeadSize;
    argsWithMemInfo.resize(argsInfo.argsSize + alignHeadSize - headSize + PTR_SIZE, 0);
    std::copy_n(args, headSize, argsWithMemInfo.begin());
    std::copy_n(memInfoPtr, PTR_SIZE, argsWithMemInfo.begin() + alignHeadSize);
    std::copy_n(args + headSize, argsInfo.argsSize - headSize, argsWithMemInfo.begin() + alignHeadSize + PTR_SIZE);
    return true;
}

uint32_t GetKernelParamNum(rtArgsEx_t *argsInfo)
{
    if (argsInfo == nullptr) {
        return 0U;
    }

    if (argsInfo->hasTiling == 0) {
        if (argsInfo->hostInputInfoPtr != nullptr) {
            return argsInfo->hostInputInfoPtr->dataOffset / sizeof(uintptr_t);
        } else {
            uint32_t alignArgsSize = GetAlignSize(argsInfo->argsSize);
            return alignArgsSize / sizeof(uintptr_t);
        }
    } else {
        return argsInfo->tilingAddrOffset / sizeof(uintptr_t) + 1;
    }
}

uint32_t GetKernelParamNum(uint32_t argsSize)
{
    uint32_t alignArgsSize = GetAlignSize(argsSize);
    return alignArgsSize / sizeof(uintptr_t);
}

uint32_t GetArgsSize(rtArgsEx_t *argsInfo)
{
    if (argsInfo->hasTiling == 0) {
        if (argsInfo->hostInputInfoPtr != nullptr) {
            uint32_t headSize = argsInfo->hostInputInfoPtr->dataOffset;
            return GetAlignSize(headSize);
        } else {
            return GetAlignSize(argsInfo->argsSize);
        }
    }
    uint32_t headSize = argsInfo->tilingAddrOffset + PTR_SIZE;
    return GetAlignSize(headSize);
}

bool ExpandArgs(rtArgsEx_t *argsInfo, std::vector<uint8_t> &argsWithMemInfo, uint8_t *memInfo,
                std::vector<rtHostInputInfo_t> &hostInput, uint32_t &memInfoOffset)
{
    std::lock_guard<std::mutex> guard(g_launchMutex);
    if (memInfo == nullptr) {
        ERROR_LOG("MemInfo is nullptr, expand args failed");
        return false;
    }
    auto newArgInfo = *argsInfo;
    if (argsInfo->args == nullptr || argsInfo->isNoNeedH2DCopy == 1) { return false; }
    if (argsInfo->argsSize > MAX_ALL_PARAM_SIZE) {
        ERROR_LOG("ArgsSize=%u is too large, max allowed size is %zu", argsInfo->argsSize, MAX_ALL_PARAM_SIZE);
        return false;
    }
    auto args = static_cast<uint8_t const*>(argsInfo->args);
    auto memInfoPtr = reinterpret_cast<uint8_t*>(&memInfo);
    argsWithMemInfo.clear();
    if (argsInfo->hasTiling == 0) {
        if (newArgInfo.hostInputInfoPtr != nullptr) {
            uint32_t headSize = newArgInfo.hostInputInfoPtr->dataOffset;
            if (!FillArgsWithMemInfo(*argsInfo, argsWithMemInfo, memInfo, headSize, memInfoOffset)) {
                return false;
            }
        } else {
            uint32_t alignArgsSize = GetAlignSize(argsInfo->argsSize);
            memInfoOffset = alignArgsSize;
            argsWithMemInfo.resize(alignArgsSize + PTR_SIZE, 0);
            std::copy_n(args, argsInfo->argsSize, argsWithMemInfo.begin());
            std::copy_n(memInfoPtr, PTR_SIZE, argsWithMemInfo.begin() + alignArgsSize);
        }
    } else {
        uint32_t headSize = argsInfo->tilingAddrOffset + PTR_SIZE;
        if (DeviceContext::Local().NeedOverflowStatus()) { headSize += PTR_SIZE; }
        if (!FillArgsWithMemInfo(*argsInfo, argsWithMemInfo, memInfo, headSize, memInfoOffset)) {
            return false;
        }
        newArgInfo.tilingDataOffset += PTR_SIZE;
    }
    hostInput.resize(newArgInfo.hostInputInfoNum);
    if (newArgInfo.hostInputInfoPtr != nullptr) {
        /// 偏移多个host侧输入的dataoffset
        for (uint16_t i = 0; i < newArgInfo.hostInputInfoNum; ++i) {
            hostInput[i].dataOffset = (newArgInfo.hostInputInfoPtr + i)->dataOffset + PTR_SIZE;
            hostInput[i].addrOffset = (newArgInfo.hostInputInfoPtr + i)->addrOffset;
        }
    }
    newArgInfo.args = argsWithMemInfo.data();
    newArgInfo.argsSize = argsWithMemInfo.size();
    newArgInfo.hostInputInfoPtr = hostInput.data();
    *argsInfo = newArgInfo;
    return true;
}

bool ExpandArgs(void *&args, uint32_t& argsSize, std::vector<uint8_t> &argsWithMemInfo, uint8_t *memInfo)
{
    std::lock_guard<std::mutex> guard(g_launchMutex);
    if (memInfo == nullptr) {
        ERROR_LOG("MemInfo is nullptr, expand args failed");
        return false;
    }
    if (argsSize > MAX_ALL_PARAM_SIZE) {
        ERROR_LOG("ArgsSize=%u is too large, max allowed size is %zu", argsSize, MAX_ALL_PARAM_SIZE);
        return false;
    }
    uint32_t alignArgsSize = GetAlignSize(argsSize);
    argsWithMemInfo.resize(alignArgsSize + PTR_SIZE, 0);
    std::copy_n(static_cast<uint8_t const*>(args), argsSize, argsWithMemInfo.begin());
    std::copy_n(reinterpret_cast<uint8_t*>(&memInfo), PTR_SIZE, argsWithMemInfo.begin() + alignArgsSize);
    args = static_cast<void*>(argsWithMemInfo.data());
    argsSize = argsWithMemInfo.size();
    return true;
}
