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

#include "FuncManager.h"
#include "RegisterManager.h"
#include "StubFuncContext.h"
#include "TilingFuncContext.h"

using namespace std;

FuncContextSP FuncManager::CreateContext(void *binHandle, const char *kernelName, void *funcHandle)
{
    auto regCtx = RegisterManager::Instance().GetContext(binHandle);
    if (!regCtx) {
        return nullptr;
    }
    FuncContextSP ctx = std::make_shared<StubFuncContext>(regCtx, kernelName, funcHandle);
    contexts_[funcHandle] = ctx;
    return ctx;
}

FuncContextSP FuncManager::CreateContext(void *binHandle, uint64_t funcEntry, void *funcHandle)
{
    auto regCtx = RegisterManager::Instance().GetContext(binHandle);
    if (!regCtx) {
        return nullptr;
    }
    FuncContextSP ctx = std::make_shared<TilingFuncContext>(regCtx, funcEntry, funcHandle);
    contexts_[funcHandle] = ctx;
    return ctx;
}

FuncContextSP FuncManager::GetContext(void *funcHandle) const
{
    auto it = contexts_.find(funcHandle);
    if (it == contexts_.end()) {
        return nullptr;
    }
    return it->second;
}

FuncContextSP FuncManager::GetContext(void *binHandle, uint64_t funcEntry) const
{
    auto funcHandleIt = fakeFuncHandleTable_.find({binHandle, funcEntry});
    if (funcHandleIt == fakeFuncHandleTable_.end()) {
        return nullptr;
    }
    auto it = contexts_.find(funcHandleIt->second);
    if (it == contexts_.end()) {
        return nullptr;
    }
    return it->second;
}
