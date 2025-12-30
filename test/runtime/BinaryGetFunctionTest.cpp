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
#include <cstdlib>
#include "mockcpp/mockcpp.hpp"
#include "acl_rt_impl/HijackedFunc.h"
#include "acl_rt_impl/AscendclImplOrigin.h"
#include "runtime/inject_helpers/RegisterManager.h"
#include "runtime/inject_helpers/RegisterContext.h"

class BinaryGetFunctionTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        AscendclImplOriginCtor();
    }

    void SetUp() override {}

    void TearDown() override
    {
        GlobalMockObject::verify();
    }
};

TEST_F(BinaryGetFunctionTest, fake_hijacked_input_then_test_call_expect_ok)
{
    auto sp = std::make_shared<RegisterContext>();
    MOCKER_CPP(&RegisterManager::GetContext).stubs().will(returnValue(sp));
    uint64_t placeholder{};
    aclrtBinHandle binHandle = &placeholder;
    const char *kernelName = "abc";
    aclrtFuncHandle funcHandle = &placeholder;

    HijackedFuncOfAclrtBinaryGetFunctionImpl inst;
    ASSERT_EQ(inst.Call(binHandle, kernelName, &funcHandle), ACL_SUCCESS);
}

TEST_F(BinaryGetFunctionTest, input_nullptr_then_test_call_expect_no_core_dump)
{
    HijackedFuncOfAclrtBinaryGetFunctionImpl inst;
    ASSERT_EQ(inst.Call(nullptr, nullptr, nullptr), ACL_SUCCESS);
}
