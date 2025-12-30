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
#include "runtime/inject_helpers/ArgsHandleContext.h"

HijackedFuncOfAclrtKernelArgsAppendImpl::HijackedFuncOfAclrtKernelArgsAppendImpl()
    : HijackedFuncType("acl_rt_impl", "aclrtKernelArgsAppendImpl") {}

void HijackedFuncOfAclrtKernelArgsAppendImpl::Pre(
    aclrtArgsHandle argsHandle, void *param, size_t paramSize,
    aclrtParamHandle *paramHandle)
{
    argsHandle_ = argsHandle;
    param_ = param;
    paramSize_ = paramSize;
    paramHandle_ = paramHandle;
}

aclError HijackedFuncOfAclrtKernelArgsAppendImpl::Post(aclError ret)
{
    if (ret != ACL_SUCCESS || paramHandle_ == nullptr) {
        return ret;
    }
    ArgsContextSP ctx = ArgsManager::Instance().GetContext(argsHandle_);
    if (!ctx) {
        DEBUG_LOG("Get args context failed");
        return ret;
    }
    auto argsHandleCtx = std::static_pointer_cast<ArgsHandleContext>(ctx);
    argsHandleCtx->CacheArgsAppendOp(param_, paramSize_, *paramHandle_);
    return ret;
}
