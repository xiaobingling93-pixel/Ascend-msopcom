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


#include <string>
#include <vector>
#include <sys/types.h>

#include <gtest/gtest.h>

#include "mockcpp/mockcpp.hpp"

#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/KernelMatcher.h"
#include "runtime/RuntimeOrigin.h"
#include "utils/FileSystem.h"

using namespace std;

TEST(KernelMatcher, input_empty_config_then_call_match_expect_true)
{
    KernelMatcher::Config config;
    constexpr uint64_t testNum = 10;
    KernelMatcher matcher{config};
    for (uint64_t launchId = 0; launchId < testNum; launchId++) {
        EXPECT_TRUE(matcher.Match(launchId, ""));
    }
}

TEST(KernelMatcher, input_launch_id_config_then_call_match_expect_match_id_first)
{
    string launchName = "abcd";
    MOCKER(&KernelContext::GetLaunchName).stubs().will(returnValue(launchName));
    KernelMatcher::Config config;
    config.launchId = 1;
    config.kernelName = "abcd";
    KernelMatcher matcher{config};
    EXPECT_FALSE(matcher.Match(0, "abcd"));
    EXPECT_TRUE(matcher.Match(1, "abcd"));
    GlobalMockObject::verify();
}

TEST(KernelMatcher, input_kernel_name_then_call_match_expect_match_launch_id_fail)
{
    string launchName = "abcd";
    MOCKER(&KernelContext::GetLaunchName).stubs().will(returnValue(launchName));
    KernelMatcher::Config config;
    config.kernelName = "aaaa";
    KernelMatcher matcher{config};
    EXPECT_FALSE(matcher.Match(0, "aaab"));
    EXPECT_FALSE(matcher.Match(1, "aaac"));
    GlobalMockObject::verify();
}

TEST(KernelMatcher, input_kernel_name_then_call_match_expect_match_launch_name_success)
{
    string launchName = "abcd";
    MOCKER(&KernelContext::GetLaunchName).stubs().will(returnValue(launchName));
    KernelMatcher::Config config;
    config.kernelName = "abcd";
    EXPECT_EQ(config.launchId, ~0ULL);
    KernelMatcher matcher{config};
    EXPECT_TRUE(matcher.Match(10, "abcd"));
    GlobalMockObject::verify();
}

