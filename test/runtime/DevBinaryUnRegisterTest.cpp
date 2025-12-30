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
#include "mockcpp/mockcpp.hpp"
#include "runtime/HijackedFunc.h"
#include "core/LocalProcess.h"

TEST(rtDevBinaryUnRegister, call_pre_function_with_null_input_expect_return)
{
    void *hdl = nullptr;
    HijackedFuncOfDevBinaryUnRegister instance;
    instance.Pre(hdl);
}

TEST(rtDevBinaryUnRegister, call_post_function_with_null_input_expect_return)
{
    void *hdl = nullptr;
    HijackedFuncOfDevBinaryUnRegister instance;
    instance.Pre(hdl);
    auto ret = instance.Post(RT_ERROR_NONE);
    ASSERT_EQ(ret, RT_ERROR_NONE);
}

TEST(rtDevBinaryUnRegister, call_function_with_null_input_expect_return)
{
    void *hdl = nullptr;
    HijackedFuncOfDevBinaryUnRegister instance;
    auto ret = instance.Call(hdl);
    ASSERT_EQ(ret, RT_ERROR_RESERVED);
}

TEST(rtDevBinaryUnRegister, call_pre_function_with_fuck_input_expect_return)
{
    char fuckData[8] = {0};
    void *hdl = &fuckData;
    HijackedFuncOfDevBinaryUnRegister instance;
    instance.Pre(hdl);
    GlobalMockObject::verify();
}

TEST(rtDevBinaryUnRegister, call_post_function_with_fuck_input_expect_return)
{
    char fuckData[8] = {0};
    void *hdl = &fuckData;
    HijackedFuncOfDevBinaryUnRegister instance;
    MOCKER(&LocalProcess::Notify).stubs().will(returnValue(0));
    instance.Pre(hdl);
    auto ret = instance.Post(RT_ERROR_NONE);
    ASSERT_EQ(ret, RT_ERROR_NONE);
    GlobalMockObject::verify();
}
