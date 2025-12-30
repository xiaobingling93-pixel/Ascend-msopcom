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
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "core/FuncSelector.h"
#include "mockcpp/mockcpp.hpp"

TEST(rtCtxCreateV2, call_pre_function_with_valid_device_id_expect_return_nullptr)
{
    HijackedFuncOfCtxCreateV2 instance;
    instance.Pre(nullptr, 0, 1, RT_DEVICE_MODE_SINGLE_DIE);
    instance.Call(nullptr, 0, 1, RT_DEVICE_MODE_SINGLE_DIE);
    EXPECT_EQ(DeviceContext::Local().GetDeviceId(), 1);
}

TEST(rtCtxCreateV2, call_pre_function_with_valid_device_id_expect_return)
{
    MOCKER(IsOpProf).stubs().will(returnValue(true));
    ProfConfig::Instance().profConfig_.isSimulator = true;

    HijackedFuncOfCtxCreateV2 instance;
    auto func = [](void**, uint32_t, int32_t, rtDeviceMode) ->
        rtError_t { return RT_ERROR_NONE; };
    instance.originfunc_ = func;

    instance.Pre(nullptr, 0, 1, RT_DEVICE_MODE_SINGLE_DIE);
    instance.Call(nullptr, 0, 1, RT_DEVICE_MODE_SINGLE_DIE);
    EXPECT_EQ(DeviceContext::Local().GetDeviceId(), 1);
}
