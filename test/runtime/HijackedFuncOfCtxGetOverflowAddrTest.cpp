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


#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"
#include "runtime/HijackedFunc.h"
#include "core/FuncSelector.h"
#include "runtime/inject_helpers/KernelContext.h"

class HijackedFuncOfCtxGetOverflowAddrTest : public testing::Test {
    void TearDown() override
    {
        KernelContext::OpMemInfo &opMemInfo = KernelContext::Instance().GetOpMemInfo();
        opMemInfo.hasOverflowAddr = false;
        opMemInfo.overflowAddr = 0UL;
        GlobalMockObject::verify();
    }
};

TEST_F(HijackedFuncOfCtxGetOverflowAddrTest, call_post_with_not_sanitizer_expect_just_return)
{
    MOCKER(&IsSanitizer).stubs().will(returnValue(false));

    HijackedFuncOfCtxGetOverflowAddr instance;
    ASSERT_EQ(instance.Post(RT_ERROR_NONE), RT_ERROR_NONE);
}

TEST_F(HijackedFuncOfCtxGetOverflowAddrTest, call_post_with_null_overflow_addr_expect_just_get_empty_meminfo)
{
    MOCKER(&IsSanitizer).stubs().will(returnValue(true));

    HijackedFuncOfCtxGetOverflowAddr instance;
    instance.Pre(nullptr);
    instance.Post(RT_ERROR_NONE);
    KernelContext::OpMemInfo &opMemInfo = KernelContext::Instance().GetOpMemInfo();
    ASSERT_FALSE(opMemInfo.hasOverflowAddr);
    ASSERT_EQ(opMemInfo.overflowAddr, 0UL);
}

TEST_F(HijackedFuncOfCtxGetOverflowAddrTest, call_post_with_valid_overflow_addr_expect_get_correct_meminfo)
{
    MOCKER(&IsSanitizer).stubs().will(returnValue(true));

    int dummy{};
    void *overflowAddr = &dummy;
    HijackedFuncOfCtxGetOverflowAddr instance;
    instance.Pre(&overflowAddr);
    instance.Post(RT_ERROR_NONE);
    KernelContext::OpMemInfo &opMemInfo = KernelContext::Instance().GetOpMemInfo();
    ASSERT_TRUE(opMemInfo.hasOverflowAddr);
    ASSERT_EQ(opMemInfo.overflowAddr, reinterpret_cast<uint64_t>(overflowAddr));
}
