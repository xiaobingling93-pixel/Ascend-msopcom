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
#include "runtime/RuntimeOrigin.h"
#include "runtime/RuntimeConfig.h"
#undef private
#include "mockcpp/mockcpp.hpp"

TEST(RuntimeOrigin, rt_function_load_failed)
{
    MOCKER(&func_injection::register_function::FunctionRegister::Get)
        .stubs()
        .will(returnValue((void*)nullptr));
    MOCKER(rtKernelLaunch).stubs().will(returnValue(RT_ERROR_DRV_ERR));

    EXPECT_NE(rtMemsetOrigin(nullptr, 0, 0, 0), RT_ERROR_NONE);
    EXPECT_NE(rtMemcpyAsyncOrigin(nullptr, 0, nullptr, 0, RT_MEMCPY_DEVICE_TO_HOST, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtFreeOrigin(nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtMallocOrigin(nullptr, 0, RT_MEMORY_DEFAULT, 0), RT_ERROR_NONE);
    EXPECT_NE(rtMemcpyOrigin(nullptr, 0, nullptr, 0, RT_MEMCPY_HOST_TO_HOST), RT_ERROR_NONE);
    EXPECT_NE(rtMemcpyAsyncOrigin(nullptr, 0, nullptr, 0, RT_MEMCPY_HOST_TO_HOST, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtRegisterAllKernelOrigin(nullptr, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtDevBinaryRegisterOrigin(nullptr, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtDevBinaryUnRegisterOrigin(nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtFunctionRegisterOrigin(nullptr, nullptr, nullptr, nullptr, 0), RT_ERROR_NONE);
    EXPECT_NE(rtKernelLaunchWithHandleV2Origin(nullptr, 0, 0, nullptr, nullptr, nullptr, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtAicpuKernelLaunchExWithArgsOrigin(0, nullptr, 0, nullptr, nullptr, nullptr, 0), RT_ERROR_NONE);
    EXPECT_NE(rtKernelLaunchOrigin(nullptr, 0, nullptr, 0, nullptr, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtKernelLaunchWithFlagV2Origin(nullptr, 0, nullptr, nullptr, nullptr, 0, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtStreamSynchronizeOrigin(nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtCtxGetCurrentDefaultStreamOrigin(nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtKernelGetAddrAndPrefCntOrigin(nullptr, 0, nullptr, 0, nullptr, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtProfSetProSwitchOrigin(nullptr, 0), RT_ERROR_NONE);
    EXPECT_NE(rtGetSocVersionOrigin(nullptr, 0), RT_ERROR_NONE);
    EXPECT_NE(rtGetVisibleDeviceIdByLogicDeviceIdOrigin(0, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtStreamCreateOrigin(nullptr, 0), RT_ERROR_NONE);
    EXPECT_NE(rtStreamDestroyOrigin(nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtGetC2cCtrlAddrOrigin(nullptr, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtMallocHostOrigin(nullptr, 0, 0), RT_ERROR_NONE);
    EXPECT_NE(rtFreeHostOrigin(nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtStreamSynchronizeWithTimeoutOrigin(nullptr, 0), RT_ERROR_NONE);
    EXPECT_NE(rtDeviceStatusQueryOrigin(0, nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtGetDeviceOrigin(nullptr), RT_ERROR_NONE);
    EXPECT_NE(rtSetDeviceOrigin(0), RT_ERROR_NONE);
    GlobalMockObject::verify();
}
