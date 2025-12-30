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
#include "runtime/HijackedFunc.h"
#undef private
#undef protected
#include "core/FuncSelector.h"
#include "mockcpp/mockcpp.hpp"

TEST(rtStreamSynchronizeWithTimeout, coverage_call_function_with_expect_return)
{
    rtStream_t stream;
    int32_t timeout = 1;
    HijackedFuncOfStreamSynchronizeWithTimeout instance;
    instance.Call(stream, timeout);
    GlobalMockObject::verify();
}

TEST(rtStreamSynchronizeWithTimeout, coverage_call_originfunc_with_expect_return)
{
    rtStream_t stream;
    int32_t timeout = 1;
    HijackedFuncOfStreamSynchronizeWithTimeout instance;

    auto func = [](rtStream_t, int32_t) ->
        rtError_t { return RT_ERROR_NONE; };
    instance.originfunc_ = func;

    instance.Call(stream, timeout);
    GlobalMockObject::verify();
}
