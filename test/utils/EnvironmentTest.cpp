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


#define private public
#include "utils/Environment.h"
#undef private
#include <gtest/gtest.h>

#include "mockcpp/mockcpp.hpp"

TEST(EnvironmentTest, combine_env_with_false_mode)
{
    std::map<std::string, std::string> envs;
    std::vector<std::string> outEnv;
    bool combineMode = false;
    JoinWithSystemEnv(envs, outEnv, combineMode);
}

TEST(EnvironmentTest, get_soc_version_failed_due_to_null_ascend_home_path)
{
    std::string socVersion = "";

    MOCKER(GetAscendHomePath)
        .expects(once())
        .will(returnValue(false));

    ASSERT_EQ(GetSocVersionFromEnvVar(socVersion), false);
    GlobalMockObject::verify();
}

TEST(EnvironmentTest, get_soc_version_failed_due_to_null_ld_library_path)
{
    std::string socVersion = "";
    char *env = nullptr;

    MOCKER(GetAscendHomePath)
        .expects(once())
        .will(returnValue(true));
    MOCKER(&secure_getenv)
        .expects(once())
        .will(returnValue(env));

    ASSERT_EQ(GetSocVersionFromEnvVar(socVersion), false);
    GlobalMockObject::verify();
}

TEST(EnvironmentTest, get_soc_version_normal_done)
{
    std::string socVersion = "";
    std::string ascendHomePath = "/test2";
    char *ldLibraryPath = "/test1/Ascend111A1/lib:/test2/Ascend222A2/lib:/tes3/Ascend333A3/lib";

    MOCKER(GetAscendHomePath)
        .expects(once())
        .with(outBound(ascendHomePath))
        .will(returnValue(true));
    MOCKER(&secure_getenv)
        .expects(once())
        .will(returnValue(ldLibraryPath));

    ASSERT_EQ(GetSocVersionFromEnvVar(socVersion), true);
    GlobalMockObject::verify();
}

TEST(EnvironmentTest, get_soc_version_failed_due_to_regex_match_nothing)
{
    std::string socVersion = "";
    std::string ascendHomePath = "/test3";
    char *ldLibraryPath = "/test1/Ascend111A1/lib:/test2/Ascend222A2/lib";

    MOCKER(GetAscendHomePath)
        .expects(once())
        .with(outBound(ascendHomePath))
        .will(returnValue(true));
    MOCKER(&secure_getenv)
        .expects(once())
        .will(returnValue(ldLibraryPath));

    ASSERT_EQ(GetSocVersionFromEnvVar(socVersion), false);
    GlobalMockObject::verify();
}