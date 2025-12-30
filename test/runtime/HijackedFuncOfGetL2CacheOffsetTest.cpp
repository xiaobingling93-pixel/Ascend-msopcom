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
#undef protected
#undef private
#include "core/FuncSelector.h"
#include "mockcpp/mockcpp.hpp"

TEST(rtGetL2CacheOffset, sanitizer_call_expect_return_zero_l2_cache_offset)
{
    MOCKER(IsSanitizer).stubs().will(returnValue(true));
    auto func = [](uint32_t, uint64_t *) -> rtError_t { return RT_ERROR_NONE; };
    HijackedFuncOfGetL2CacheOffset instance;
    instance.originfunc_ = func;
    uint64_t l2CacheOffset = 100;
    instance.Call(0, &l2CacheOffset);
    ASSERT_EQ(l2CacheOffset, 0);
    GlobalMockObject::verify();
}

TEST(rtGetL2CacheOffset, prof_call_expect_return_origin_l2_cache_offset)
{
    MOCKER(IsOpProf).stubs().will(returnValue(true));
    auto func = [](uint32_t, uint64_t *) -> rtError_t { return RT_ERROR_NONE; };
    HijackedFuncOfGetL2CacheOffset instance;
    instance.originfunc_ = func;
    uint64_t l2CacheOffset = 100;
    instance.Call(0, &l2CacheOffset);
    ASSERT_EQ(l2CacheOffset, 100);
    GlobalMockObject::verify();
}


