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

// 该文件主要实现注入函数的功能，其配合被劫持函数的别名，实现新的劫持函数功能

#include "HijackedFunc.h"
#include "core/FuncSelector.h"
#include "core/HijackedFuncTemplate.h"
#include "acl.h"
#include "acl_rt_impl/AscendclImplOrigin.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "runtime/inject_helpers/LocalDevice.h"
#include "runtime/inject_helpers/ProfConfig.h"
#include "utils/InjectLogger.h"

HijackedFuncOfAclrtGetDeviceImpl::HijackedFuncOfAclrtGetDeviceImpl()
    : HijackedFuncType("acl_rt_impl", "aclrtGetDeviceImpl") {}

aclError HijackedFuncOfAclrtGetDeviceImpl::Call(int32_t *devId)
{
    if (this->originfunc_ == nullptr) {
        ERROR_LOG("HijackedFuncOfAclrtGetDeviceImpl originfunc is nullptr.");
        return ACL_ERROR_INTERNAL_ERROR;
    }
    aclError ret = this->originfunc_(devId);

    if (IsOpProf() && ProfConfig::Instance().IsSimulator()) {
        // If this line is missing, a context empty error will be reported.
        if (devId != nullptr) {
            *devId = DeviceContext::Local().GetDeviceId();
        } else {
            ERROR_LOG("HijackedFuncOfAclrtGetDeviceImpl devId is null, cannot assign device id.");
            return ACL_ERROR_INTERNAL_ERROR;
        }
    }

    return Post(ret);
}
