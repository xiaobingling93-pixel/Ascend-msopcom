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

 
#include "KernelArgsTestHelper.h"
#include "runtime/inject_helpers/ArgsContext.h"
TEST_F(HijacedFuncOfKernelArgsTest, input_valid_args_then_call_args_init_expect_ok)
{
    HijackedFuncOfAclrtKernelArgsInitImpl inst;
    ASSERT_EQ(inst.Call(funcHandle_, &argsHandle_), ACL_SUCCESS);
}

TEST_F(HijacedFuncOfKernelArgsTest, mock_null_context_then_call_args_init_expect_ok)
{
    ArgsContextSP nullCtx;
    MOCKER_CPP(&ArgsManager::CreateContext,
        ArgsContextSP(ArgsManager::*)(aclrtFuncHandle, aclrtArgsHandle)).stubs().will(returnValue(nullCtx));
    HijackedFuncOfAclrtKernelArgsInitImpl inst;
    ASSERT_EQ(inst.Call(funcHandle_, &argsHandle_), ACL_SUCCESS);
}

TEST_F(HijacedFuncOfKernelArgsTest, input_null_then_call_args_init_expect_ok)
{
    MOCKER_CPP(&ArgsManager::CreateContext,
        ArgsContextSP(ArgsManager::*)(aclrtFuncHandle, aclrtArgsHandle)).stubs().will(returnValue(nullptr));
    HijackedFuncOfAclrtKernelArgsInitImpl inst;
    ASSERT_EQ(inst.Call(nullptr, &argsHandle_), ACL_SUCCESS);
}
