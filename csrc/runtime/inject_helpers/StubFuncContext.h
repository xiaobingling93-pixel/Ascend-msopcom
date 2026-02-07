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

#include <map>
#include <string>

#include "acl.h"
#include "runtime.h"
#include "FuncContext.h"

/**
 * 适用于注册时有Kernel Name信息的Function注册方式,
 * 包含rt接口和aclrt接口, 当前仅实现aclrt接口.
 */
class StubFuncContext : public FuncContext {
public:
    StubFuncContext(RegisterContextSP regCtx, const char *kernelName, void *funcHandle);
    uint64_t GetKernelPC() const override;
    uint64_t GetStartPC() const override;

    bool GetTilingKey(uint64_t &tilingKey) const override { return false; }
    FuncContextSP Clone(const RegisterContextSP &regCtx) const override;
};
