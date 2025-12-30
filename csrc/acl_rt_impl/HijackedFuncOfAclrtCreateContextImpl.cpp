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
#include "HijackedFunc.h"
#include "core/FuncSelector.h"
#include "utils/InjectLogger.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "runtime/inject_helpers/ProfConfig.h"

HijackedFuncOfAclrtCreateContextImpl::HijackedFuncOfAclrtCreateContextImpl()
    : HijackedFuncType("acl_rt_impl", "aclrtCreateContextImpl") {}

aclError HijackedFuncOfAclrtCreateContextImpl::Call(aclrtContext *context, int32_t deviceId)
{
    Pre(context, deviceId);
    if (this->originfunc_ == nullptr) {
        ERROR_LOG("HijackedFuncOfAclrtCreateContextImpl originfunc is nullptr.");
        return ACL_ERROR_INTERNAL_ERROR;
    }
    if (IsOpProf() && ProfConfig::Instance().IsSimulator()) {
        deviceId = 0;
    }
    aclError ret = this->originfunc_(context, deviceId);
    DeviceContext::Local().SetDeviceId(deviceId);

    return Post(ret);
}
