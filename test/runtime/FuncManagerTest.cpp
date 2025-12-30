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
#include <vector>

#include "mockcpp/mockcpp.hpp"
#include "runtime/inject_helpers/FuncManager.h"
#include "runtime/inject_helpers/FuncContext.h"
#include "runtime/inject_helpers/RegisterManager.h"
#include "utils/FileSystem.h"

using namespace std;

class FuncManagerTest : public testing::Test {
public:
    void SetUp() override { }

    void TearDown() override
    {
        GlobalMockObject::verify();
        FuncManager::Instance().Clear();
    }
};

TEST_F(FuncManagerTest, create_no_reg_context_input_then_create_func_context_expect_nullptr)
{
    uint64_t data{};
    void *hdl = &data;
    const char *kernelName = "abc";
    void *funcHandle = &data;
    auto ctx = FuncManager::Instance().CreateContext(hdl, kernelName, funcHandle);
    ASSERT_EQ(ctx, nullptr);
}

TEST_F(FuncManagerTest, mock_reg_context_then_create_func_context_expect_success)
{
    auto sp = std::make_shared<RegisterContext>();
    MOCKER_CPP(&RegisterManager::GetContext).stubs().will(returnValue(sp));

    uint64_t data{};
    void *hdl = &data;
    const char *kernelName = "abc";
    void *funcHandle = &data;
    auto ctx = FuncManager::Instance().CreateContext(hdl, kernelName, funcHandle);
    ASSERT_NE(ctx, nullptr);
    auto gotCtx = FuncManager::Instance().GetContext(funcHandle);
    ASSERT_EQ(ctx, gotCtx);
}

TEST_F(FuncManagerTest, create_no_reg_context_input_then_create_tiling_func_context_expect_nullptr)
{
    uint64_t data{};
    void *hdl = &data;
    void *funcHandle = &data;
    auto ctx = FuncManager::Instance().CreateContext(hdl, uint64_t(0), funcHandle);
    ASSERT_EQ(ctx, nullptr);
}

TEST_F(FuncManagerTest, mock_reg_context_then_creat_create_tiling_func_context_expect_success)
{
    auto sp = std::make_shared<RegisterContext>();
    string kernelName = "abc";
    MOCKER_CPP(&RegisterManager::GetContext).stubs().will(returnValue(sp));
    MOCKER_CPP(&RegisterContext::GetKernelName).stubs().will(returnValue(kernelName));

    uint64_t data{};
    void *hdl = &data;
    void *funcHandle = &data;
    auto ctx = FuncManager::Instance().CreateContext(hdl, uint64_t(0), funcHandle);
    ASSERT_NE(ctx, nullptr);
    auto gotCtx = FuncManager::Instance().GetContext(funcHandle);
    ASSERT_EQ(ctx, gotCtx);
}
