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
#include <dlfcn.h>
#include "runtime/HijackedFunc.h"
#include "mockcpp/mockcpp.hpp"
#include "core/LocalProcess.h"

TEST(rtFunctionRegister, call_pre_func_expect_return)
{
    void *binHandle = nullptr;
    const void *stubFunc = nullptr;
    const char_t *stubName = nullptr;
    const void *kernelInfoExt = nullptr;
    uint32_t funcMode = 0;
    HijackedFuncOfFunctionRegister instance;
    instance.Pre(binHandle, stubFunc, stubName, kernelInfoExt, funcMode);
}

TEST(rtFunctionRegister, call_post_func_expect_return)
{
    void *binHandle = nullptr;
    const void *stubFunc = nullptr;
    const char_t *stubName = nullptr;
    const void *kernelInfoExt = nullptr;
    uint32_t funcMode = 0;
    HijackedFuncOfFunctionRegister instance;
    instance.Pre(binHandle, stubFunc, stubName, kernelInfoExt, funcMode);
    auto ret = instance.Post(RT_ERROR_NONE);
    ASSERT_EQ(ret, RT_ERROR_NONE);
}

TEST(rtFunctionRegister, call_function_with_null_input_expect_return)
{
    void *binHandle = nullptr;
    const void *stubFunc = nullptr;
    const char_t *stubName = nullptr;
    const void *kernelInfoExt = nullptr;
    uint32_t funcMode = 0;
 
    MOCKER(&func_injection::FunctionLoader::Get).stubs().will(returnValue((void*)nullptr));
    HijackedFuncOfFunctionRegister instance;
    auto ret = instance.Call(binHandle, stubFunc, stubName, kernelInfoExt, funcMode);
    ASSERT_EQ(ret, RT_ERROR_RESERVED);
    GlobalMockObject::verify();
}

TEST(rtFunctionRegister, call_function_with_input)
{
    void *binHandle = new char [20];
    const void *stubFunc = new char [20];
    const char_t *stubName = nullptr;
    const void *kernelInfoExt = nullptr;
    uint32_t funcMode = 0;
 
    HijackedFuncOfFunctionRegister instance;
    auto ret = instance.Call(binHandle, stubFunc, stubName, kernelInfoExt, funcMode);
    ASSERT_EQ(ret, RT_ERROR_RESERVED);
    GlobalMockObject::verify();
}
