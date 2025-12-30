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
#include "runtime/inject_helpers/KernelContext.h"
#include "core/FuncSelector.h"
#include "utils/InjectLogger.h"
namespace {
    constexpr uint32_t MAX_DEVICE_CNT = 16U;
}
HijackedFuncOfAclrtGetDeviceCountImpl::HijackedFuncOfAclrtGetDeviceCountImpl()
    : HijackedFuncType("acl_rt_impl", "aclrtGetDeviceCountImpl") {}

aclError HijackedFuncOfAclrtGetDeviceCountImpl::Call(uint32_t *count)
{
    Pre(count);
    if (IsOpProf() && ProfConfig::Instance().IsSimulator() && (count != nullptr)) {
        *count = MAX_DEVICE_CNT;
        return Post(ACL_SUCCESS);
    }

    if (this->originfunc_ == nullptr) {
        ERROR_LOG("HijackedFuncOfGetDeviceCount originfunc is nullptr.");
        return ACL_ERROR_INTERNAL_ERROR;
    }
    aclError ret = this->originfunc_(count);

    return Post(ret);
}