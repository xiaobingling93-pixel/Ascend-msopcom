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

 
#include "LocalDevice.h"
 
#include <map>
#include <thread>
#include <mutex>
#include <chrono>
#include "utils/Future.h"
#include "utils/InjectLogger.h"
#include "runtime/inject_helpers/ThreadContext.h"

LocalDevice &LocalDevice::Local(CommType type)
{
    return GetInstance(ThreadContext::Instance().GetDeviceId(), type);
}
 
LocalDevice &LocalDevice::GetInstance(int32_t deviceId, CommType type)
{
    static std::unordered_map<int32_t, LocalDevice> devices;
    static std::mutex mutex{};
    std::lock_guard<std::mutex> guard(mutex);
    auto it = devices.find(deviceId);
    if (it == devices.end()) {
        it = devices.emplace(deviceId, type).first;
    }
    return it->second;
}
