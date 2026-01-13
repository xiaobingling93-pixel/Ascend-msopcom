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


#include <iostream>
#include <thread>
#include "HijackedFunc.h"
#include "RuntimeOrigin.h"
#include "core/FuncSelector.h"
#include "utils/InjectLogger.h"
#include "core/PlatformConfig.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "runtime/inject_helpers/LocalDevice.h"
#include "runtime/inject_helpers/ProfConfig.h"
#include "runtime/inject_helpers/ThreadContext.h"
#include "utils/Protocol.h"
#include "utils/Serialize.h"
#include "RuntimeConfig.h"

HijackedFuncOfSetDevice::HijackedFuncOfSetDevice()
    : HijackedFuncType(RuntimeLibName(), "rtSetDevice"), devId_{0}, socVersion_{} { }

void HijackedFuncOfSetDevice::Pre(int32_t devId)
{
    this->devId_ = devId;
    if (IsSanitizer()) {
        if (rtGetSocVersionOrigin(socVersion_, sizeof(socVersion_)) != RT_ERROR_NONE) {
            ERROR_LOG("rtGetSocVersionOrigin call failed.");
            return;
        }
        DEBUG_LOG("get soc version %.2048s with device id %d", socVersion_, this->devId_);
        if (std::string(socVersion_).find("Ascend910_95") == std::string::npos) {
            return;
        }
        /// a5芯片需要修改simt warp stack size，保证动态插桩后算子能正常运行；
        if (rtDeviceSetLimitOrigin(devId, RT_LIMIT_TYPE_SIMT_WARP_STACK_SIZE, 0x8000) != RT_ERROR_NONE) {
            ERROR_LOG("rtDeviceSetLimitOrigin call failed.");
        }
    }
}

rtError_t HijackedFuncOfSetDevice::Call(int32_t devId)
{
    Pre(devId);
    if (this->originfunc_ == nullptr) {
        ERROR_LOG("HijackedFuncOfSetDevice originfunc is nullptr.");
        return RT_ERROR_RESERVED;
    }
    if (IsOpProf() && ProfConfig::Instance().IsSimulator()) {
        devId = 0;
    }
    rtError_t ret = this->originfunc_(devId);
    return Post(ret);
}

rtError_t HijackedFuncOfSetDevice::Post(rtError_t ret)
{
    ThreadContext::Instance().SetDeviceId(devId_);
    DeviceContext::Instance(devId_).SetDeviceId(this->devId_);
    if (IsSanitizer()) {
        DeviceContext::Instance(devId_).SetSocVersion(socVersion_);

        PacketHead head = { PacketType::DEVICE_SUMMARY };
        DeviceSummary summary{};
        summary.device = static_cast<uint32_t>(GetDeviceTypeBySocVersion(socVersion_));
        summary.deviceId = this->devId_;
        LocalDevice::GetInstance(this->devId_).Notify(Serialize(head, summary));
    }
    return ret;
}
