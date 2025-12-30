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

#include <cinttypes>

#include "HijackedFunc.h"
#include "core/FuncSelector.h"
#include "runtime/inject_helpers/FuncManager.h"
#include "utils/InjectLogger.h"

HijackedFuncOfAclrtBinaryGetFunctionByEntryImpl::HijackedFuncOfAclrtBinaryGetFunctionByEntryImpl()
    : HijackedFuncType("acl_rt_impl", "aclrtBinaryGetFunctionByEntryImpl") {}

void HijackedFuncOfAclrtBinaryGetFunctionByEntryImpl::Pre(
    aclrtBinHandle binHandle, uint64_t funcEntry, aclrtFuncHandle *funcHandle)
{
    binHandle_ = binHandle;
    funcEntry_ = funcEntry;
    funcHandle_ = funcHandle;
}

aclError HijackedFuncOfAclrtBinaryGetFunctionByEntryImpl::Post(aclError ret)
{
    if (ret != ACL_SUCCESS || binHandle_ == nullptr ||
        funcHandle_ == nullptr || *funcHandle_ == nullptr) {
        return ret;
    }
    auto ctx = FuncManager::Instance().CreateContext(binHandle_, funcEntry_, *funcHandle_);
    if (!ctx) {
        DEBUG_LOG("Create func context failed for funcEntry=%" PRIu64, funcEntry_);
    }
    return ret;
}
