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
#include "runtime/inject_helpers/ArgsManager.h"

using namespace std;

class ArgsManagerTest : public testing::Test {
public:
    void SetUp() override { }

    void TearDown() override
    {
        GlobalMockObject::verify();
        ArgsManager::Instance().Clear();
    }
};

TEST_F(ArgsManagerTest, input_valid_then_create_args_handle_context_expect_success)
{
    uint64_t placeholder;
    aclrtFuncHandle funcHandle = &placeholder;
    aclrtArgsHandle argsHandle = &placeholder;
    auto expect = ArgsManager::Instance().CreateContext(funcHandle, argsHandle);
    ASSERT_NE(expect, nullptr);
    auto ctx = ArgsManager::Instance().GetContext(argsHandle);
    ASSERT_EQ(expect, ctx);
}
