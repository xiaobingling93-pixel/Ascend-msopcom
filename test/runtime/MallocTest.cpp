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
#include "runtime/HijackedFunc.h"
#undef private
#include "core/FuncSelector.h"
#include "core/LocalDevice.h"
#include "core/DomainSocket.h"
#include "runtime/RuntimeOrigin.h"
#include "mockcpp/mockcpp.hpp"

TEST(rtMalloc, sanitizer_call_pre_function_with_malloc_args_expect_save_correct_params)
{
    MOCKER(IsSanitizer).stubs().will(returnValue(true));

    void *devPtr = nullptr;
    HijackedFuncOfMalloc instance;
    instance.Pre(&devPtr, 64, RT_MEMORY_DEFAULT, 0);
    ASSERT_EQ(instance.devPtr_, &devPtr);
    ASSERT_EQ(instance.size_, 64ULL);
    GlobalMockObject::verify();
}

TEST(rtMalloc, sanitizer_call_post_function_with_malloc_args_expect_return)
{
    MOCKER(IsSanitizer).stubs().will(returnValue(true));
    MOCKER(&DomainSocketClient::Connect).stubs().will(returnValue(true));
    MOCKER(rtGetDeviceOrigin).stubs().will(returnValue(RT_ERROR_NONE));
    MOCKER(&LocalDevice::Notify).stubs().will(returnValue(0));

    void *devPtr = nullptr;
    HijackedFuncOfMalloc instance;
    instance.Pre(&devPtr, 64, RT_MEMORY_DEFAULT, 0);
    ASSERT_EQ(instance.Post(RT_ERROR_NONE), RT_ERROR_NONE);
    GlobalMockObject::verify();
}
