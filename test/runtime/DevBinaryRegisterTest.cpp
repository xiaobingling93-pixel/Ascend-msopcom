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
#include "core/LocalProcess.h"
#include "mockcpp/mockcpp.hpp"
#include "runtime/HijackedFunc.h"
#include "utils/FileSystem.h"
#include "core/FuncSelector.h"

TEST(rtDevBinaryRegister, call_pre_function_with_null_input_expect_return)
{
    rtDevBinary_t *bin = nullptr;
    void **hdl = nullptr;
    HijackedFuncOfDevBinaryRegister instance;
    instance.Pre(bin, hdl);
}

TEST(rtDevBinaryRegister, call_post_function_with_null_input_expect_return)
{
    rtDevBinary_t *bin = nullptr;
    void **hdl = nullptr;
    HijackedFuncOfDevBinaryRegister instance;
    instance.Pre(bin, hdl);
    auto ret = instance.Post(RT_ERROR_NONE);
    ASSERT_EQ(ret, RT_ERROR_NONE);
}

TEST(rtDevBinaryRegister, call_function_with_null_input_expect_return)
{
    rtDevBinary_t *bin = nullptr;
    void **hdl = nullptr;
    HijackedFuncOfDevBinaryRegister instance;
    auto ret = instance.Call(bin, hdl);
    ASSERT_EQ(ret, RT_ERROR_RESERVED);
}

TEST(rtDevBinaryRegister, call_pre_function_with_fuck_input_expect_return)
{
    char fuckData[8] = {0};
    rtDevBinary_t a = {.magic = 0x1, .version = 0x1, .data = fuckData, .length = 8};
    rtDevBinary_t *bin = &a;
    void **hdl = nullptr;
    MOCKER(&LocalProcess::Wait).stubs().will(returnValue(0));
    HijackedFuncOfDevBinaryRegister instance;
    instance.Pre(bin, hdl);
    GlobalMockObject::verify();
}

TEST(rtDevBinaryRegister, call_pre_and_post_function_with_bin_by_null_data_and_hdl_input_expect_return)
{
    rtDevBinary_t a = {.magic = 0x1, .version = 0x1, .data = nullptr, .length = 8};
    rtDevBinary_t *bin = &a;
    char hdlData[1] = {0};
    void **hdl = reinterpret_cast<void **>(&hdlData);
    HijackedFuncOfDevBinaryRegister instance;
    instance.Pre(bin, hdl);
    auto ret = instance.Post(RT_ERROR_NONE);
    ASSERT_EQ(ret, RT_ERROR_NONE);
}

TEST(rtDevBinaryRegister, call_function_with_bin_and_null_hdl_input_to_write_expect_return)
{
    char fuckData[8] = {0};
    rtDevBinary_t a = {.magic = 0x1, .version = 0x1, .data = fuckData, .length = 8};
    rtDevBinary_t *bin = &a;
    void **hdl = nullptr;
    MOCKER(WriteBinary).stubs().will(returnValue(1UL));
    HijackedFuncOfDevBinaryRegister instance;
    instance.Pre(bin, hdl);
    GlobalMockObject::verify();
}

TEST(rtDevBinaryRegister, call_function_with_bin_and_null_hdl_input_to_write_false_expect_return)
{
    char fuckData[8] = {0};
    rtDevBinary_t a = {.magic = 0x1, .version = 0x1, .data = fuckData, .length = 8};
    rtDevBinary_t *bin = &a;
    void **hdl = nullptr;
    MOCKER(WriteBinary).stubs().will(returnValue(0UL));
    HijackedFuncOfDevBinaryRegister instance;
    instance.Pre(bin, hdl);
    GlobalMockObject::verify();
}

TEST(rtDevBinaryRegister, call_function_with_bin_and_null_hdl_input_to_write_by_coverage_expect_return)
{
    char fuckData[8] = {0};
    rtDevBinary_t a = {.magic = 0x1, .version = 0x1, .data = fuckData, .length = 8};
    rtDevBinary_t *bin = &a;
    void **hdl = nullptr;
    MOCKER(WriteBinary).stubs().will(returnValue(1UL));
    HijackedFuncOfDevBinaryRegister instance;
    instance.Pre(bin, hdl);
    GlobalMockObject::verify();
}
