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
#include "core/FuncSelector.h"
#include "runtime/inject_helpers/RegisterContext.h"
#include "runtime/inject_helpers/RegisterManager.h"
#include "runtime/inject_helpers/ProfDataCollect.h"
#include "runtime/inject_helpers/ProfConfig.h"

HijackedFuncOfAclrtBinaryLoadImpl::HijackedFuncOfAclrtBinaryLoadImpl()
    : HijackedFuncType("acl_rt_impl", "aclrtBinaryLoadImpl") {}

void HijackedFuncOfAclrtBinaryLoadImpl::Pre(const aclrtBinary binary, aclrtBinHandle *binHandle)
{
    bin_ = const_cast<const aclrtBinary>(binary);
    binHandle_ = binHandle;
}

aclError HijackedFuncOfAclrtBinaryLoadImpl::Post(aclError ret)
{
    if (ret != ACL_SUCCESS || bin_ == nullptr || binHandle_ == nullptr) {
        return ret;
    }
    auto ctx = RegisterManager::Instance().CreateContext(bin_, *binHandle_, RT_DEV_BINARY_MAGIC_ELF);
    if (!ctx) {
        return ret;
    }
    if (IsOpProf() && ProfConfig::Instance().IsSimulator()) {
        ProfDataCollect::SaveObject(ctx);
    }
    return ret;
}
