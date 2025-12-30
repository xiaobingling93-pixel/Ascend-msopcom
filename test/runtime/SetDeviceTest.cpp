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
#include "runtime/inject_helpers/ProfConfig.h"
#undef private
#undef protected
#include "core/FuncSelector.h"
#include "core/LocalDevice.h"
#include "core/DomainSocket.h"
#include "runtime/RuntimeOrigin.h"
#include "mockcpp/mockcpp.hpp"
#include "runtime/inject_helpers/KernelContext.h"

TEST(rtSetDevice, sanitizer_call_pre_function_with_devid_expect_save_correct_devid)
{
    MOCKER(IsSanitizer).stubs().will(returnValue(true));
    MOCKER(IsOpProf).stubs().will(returnValue(false));

    int32_t devId = 123;
    HijackedFuncOfSetDevice instance;
    instance.Pre(devId);
    ASSERT_EQ(instance.devId_, devId);
    instance.Call(devId);
    GlobalMockObject::verify();
}

TEST(rtSetDevice, sanitizer_call_post_function_with_get_soc_version_success_expect_return)
{
    MOCKER(IsSanitizer).stubs().will(returnValue(true));
    MOCKER(IsOpProf).stubs().will(returnValue(false));
    MOCKER(&DomainSocketClient::Connect).stubs().will(returnValue(true));
    MOCKER(rtGetDeviceOrigin).stubs().will(returnValue(RT_ERROR_NONE));
    MOCKER(&LocalDevice::Notify).stubs().will(returnValue(0));
    MOCKER(&rtGetSocVersionOrigin).stubs().will(returnValue(RT_ERROR_NONE));

    int32_t devId = 123;
    HijackedFuncOfSetDevice instance;
    instance.Pre(devId);
    ASSERT_EQ(instance.Post(RT_ERROR_NONE), RT_ERROR_NONE);
    GlobalMockObject::verify();
}

TEST(rtSetDevice, sanitizer_call_post_function_with_get_soc_version_failed_expect_return)
{
    MOCKER(IsSanitizer).stubs().will(returnValue(true));
    MOCKER(IsOpProf).stubs().will(returnValue(false));
    MOCKER(&DomainSocketClient::Connect).stubs().will(returnValue(true));
    MOCKER(rtGetDeviceOrigin).stubs().will(returnValue(RT_ERROR_NONE));
    MOCKER(&LocalDevice::Notify).stubs().will(returnValue(0));
    MOCKER(&rtGetSocVersionOrigin).stubs().will(returnValue(RT_ERROR_RESERVED));

    int32_t devId = 123;
    HijackedFuncOfSetDevice instance;
    instance.Pre(devId);
    ASSERT_EQ(instance.Post(RT_ERROR_NONE), RT_ERROR_NONE);
    GlobalMockObject::verify();
}

TEST(rtSetDevice, opprof_call_with_originfunc)
{
    MOCKER(IsOpProf).stubs().will(returnValue(true));
    ProfConfig::Instance().profConfig_.isSimulator = true;

    int32_t devId = 123;

    HijackedFuncOfSetDevice instance;

    auto func = [](int32_t) ->
        rtError_t { return RT_ERROR_NONE; };
    instance.originfunc_ = func;

    instance.Pre(devId);
    ASSERT_EQ(instance.devId_, devId);
    instance.Call(devId);
    GlobalMockObject::verify();
}
