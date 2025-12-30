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

#pragma once

#include <unordered_map>
#include <memory>

#include "acl.h"
#include "runtime.h"
#include "runtime/inject_helpers/FuncContext.h"
#include "utils/Singleton.h"

using FuncContextSP = std::shared_ptr<FuncContext>;

/**
 * 管理所有FuncContext的增删查.
 * 当前只实现增加和查找功能，可通过funcHandle或者binHandle+tilingKey查找
 */
class FuncManager : public Singleton<FuncManager, false> {
public:
    FuncContextSP CreateContext(void *binHandle, const char *kernelName, void *funcHandle);

    FuncContextSP CreateContext(void *binHandle, uint64_t funcEntry, void *funcHandle);

    FuncContextSP GetContext(void *funcHandle) const;

    FuncContextSP GetContext(void *binHandle, uint64_t funcEntry) const;

#if defined (__BUILD_TESTS__)
    void Clear()
    {
        contexts_.clear();
        fakeFuncHandleTable_.clear();
    }
#endif

private:
    std::unordered_map<void *, FuncContextSP> contexts_;
    std::map<std::pair<void *, uint64_t>, void *> fakeFuncHandleTable_;
};
