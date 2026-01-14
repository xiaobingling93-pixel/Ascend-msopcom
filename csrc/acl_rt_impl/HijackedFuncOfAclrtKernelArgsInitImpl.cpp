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
#include "utils/InjectLogger.h"
#include "runtime/inject_helpers/ArgsManager.h"

HijackedFuncOfAclrtKernelArgsInitImpl::HijackedFuncOfAclrtKernelArgsInitImpl()
    : HijackedFuncType(AclRuntimeLibName(), "aclrtKernelArgsInitImpl") {}

void HijackedFuncOfAclrtKernelArgsInitImpl::Pre(
    aclrtFuncHandle funcHandle, aclrtArgsHandle *argsHandle
)
{
    funcHandle_ = funcHandle;
    argsHandle_ = argsHandle;
}

aclError HijackedFuncOfAclrtKernelArgsInitImpl::Post(aclError ret)
{
    if (ret != ACL_SUCCESS || funcHandle_ == nullptr ||
        argsHandle_ == nullptr || *argsHandle_ == nullptr) {
        return ret;
    }
    auto ctx = ArgsManager::Instance().CreateContext(funcHandle_, *argsHandle_);
    if (!ctx) {
        DEBUG_LOG("Create args context failed");
    }
    return ret;
}
