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

#include "TilingFuncContext.h"
#include "acl_rt_impl/AscendclImplOrigin.h"

using namespace std;

TilingFuncContext::TilingFuncContext(RegisterContextSP regCtx, uint64_t funcEntry, void *funcHandle)
    : FuncContext(regCtx, funcHandle), tilingKey_(funcEntry)
{
    kernelName_ = regCtx_->GetKernelName(funcEntry);
}

bool TilingFuncContext::GetTilingKey(uint64_t &tilingKey) const
{
    tilingKey = tilingKey_;
    return true;
}

uint64_t TilingFuncContext::GetKernelPC() const
{
    if (funcHandle_) {
        return FuncContext::GetKernelPC();
    }
    return 0;
}

uint64_t TilingFuncContext::GetStartPC() const
{
    if (funcHandle_) {
        return FuncContext::GetStartPC();
    }
    return 0;
}

FuncContextSP TilingFuncContext::Clone(const RegisterContextSP &regCtx) const
{
    if (funcHandle_) {
        aclrtBinHandle binHandle = regCtx->GetHandle();
        aclrtFuncHandle funcHandle;
        auto ret = aclrtBinaryGetFunctionByEntryImplOrigin(binHandle, tilingKey_, &funcHandle);
        if (ret != ACL_SUCCESS) {
            return nullptr;
        }
        FuncContextSP newFuncContext = make_shared<TilingFuncContext>(regCtx, tilingKey_, funcHandle);
        return newFuncContext;
    }
    return nullptr;
}
