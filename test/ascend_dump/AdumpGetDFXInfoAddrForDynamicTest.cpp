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


#include <gtest/gtest.h>
#define private public
#define protected public
#include "ascend_dump/HijackedFunc.h"
#undef private
#undef protected
#include "core/DomainSocket.h"
#include "core/FuncSelector.h"
#include "core/LocalProcess.h"
#include "mockcpp/mockcpp.hpp"
#include "runtime/inject_helpers/ArgsManager.h"

TEST(AdumpGetDFXInfoAddrForDynamic, test_call_function_without_originfunc)
{
    uint32_t space = 1;
    uint64_t atomicIndex = 2;
    HijackedFuncOfAdumpGetDFXInfoAddrForDynamic instance;
    instance.Call(space, atomicIndex);
}

TEST(AdumpGetDFXInfoAddrForDynamic, test_call_function_witht_originfunc_and_check_expect_return)
{
    uint32_t space = 1;
    uint64_t atomicIndex = 2;
    HijackedFuncOfAdumpGetDFXInfoAddrForDynamic instance;
    auto func = [](uint32_t, uint64_t &) ->
        void* { return nullptr; };
    instance.originfunc_ = func;
    instance.Call(space, atomicIndex);
    EXPECT_EQ(ArgsManager::Instance().GetLatestAdumpInfo().argsSpace, 1);
}
