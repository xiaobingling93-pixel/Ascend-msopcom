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

TEST_F(HijacedFuncOfKernelArgsTest, mock_get_context_then_call_args_finalize_expect_ok)
{
    MOCKER_CPP(&ArgsHandleContext::Finalize).stubs();
    MOCKER_CPP(&ArgsManager::GetContext).stubs().will(returnValue(argsHandleCtx_));
    HijackedFuncOfAclrtKernelArgsFinalizeImpl inst;
    ASSERT_EQ(inst.Call(argsHandle_), ACL_SUCCESS);
}

TEST_F(HijacedFuncOfKernelArgsTest, mock_null_get_context_then_call_args_finalize_expect_ok)
{
    MOCKER_CPP(&ArgsHandleContext::Finalize).stubs();
    HijackedFuncOfAclrtKernelArgsFinalizeImpl inst;
    ASSERT_EQ(inst.Call(argsHandle_), ACL_SUCCESS);
}

TEST_F(HijacedFuncOfKernelArgsTest, input_null_then_call_args_finalize_expect_ok)
{
    MOCKER_CPP(&ArgsHandleContext::Finalize).stubs();
    HijackedFuncOfAclrtKernelArgsFinalizeImpl inst;
    ASSERT_EQ(inst.Call(nullptr), ACL_SUCCESS);
}
