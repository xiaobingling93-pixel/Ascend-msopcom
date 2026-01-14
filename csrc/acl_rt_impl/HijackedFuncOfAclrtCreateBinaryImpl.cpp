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
#include "runtime/inject_helpers/RegisterManager.h"

HijackedFuncOfAclrtCreateBinaryImpl::HijackedFuncOfAclrtCreateBinaryImpl()
    : HijackedFuncType(AclRuntimeLibName(), "aclrtCreateBinaryImpl") {}

void HijackedFuncOfAclrtCreateBinaryImpl::Pre(const void *data, size_t dataLen)
{
    data_ = static_cast<const char*>(data);
    dataLen_ = dataLen;
}

aclrtBinary HijackedFuncOfAclrtCreateBinaryImpl::Post(aclrtBinary bin)
{
    if (bin == nullptr || data_ == nullptr || dataLen_ == 0) {
        return bin;
    }
    RegisterManager::Instance().CacheElfData(bin, data_, dataLen_);
    return bin;
}
