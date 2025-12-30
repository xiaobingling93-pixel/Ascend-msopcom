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
#include <cstdint>
#include <sstream>
#include <map>

#define private public
#include "utils/InjectLogger.h"
#undef private
#include <gtest/gtest.h>

#include "utils/FileSystem.h"
#include "mockcpp/mockcpp.hpp"

TEST(InjectLogger, constructor_env_log_level_0)
{
    std::string logLv = "0";
    MOCKER(&GetEnv).stubs().will(returnValue(logLv));
    InjectLogger logger();
    GlobalMockObject::verify();
}

TEST(InjectLogger, constructor_env_log_level_1)
{
    std::string logLv = "1";
    MOCKER(&GetEnv).stubs().will(returnValue(logLv));
    InjectLogger logger;
    GlobalMockObject::verify();
}

TEST(InjectLogger, constructor_env_log_level_2)
{
    std::string logLv = "2";
    MOCKER(&GetEnv).stubs().will(returnValue(logLv));
    InjectLogger logger;
    GlobalMockObject::verify();
}

TEST(InjectLogger, constructor_env_log_level_3)
{
    std::string logLv = "3";
    MOCKER(&GetEnv).stubs().will(returnValue(logLv));
    InjectLogger logger;
    GlobalMockObject::verify();
}

TEST(InjectLogger, constructor_env_log_level_4)
{
    std::string logLv = "4";
    MOCKER(&GetEnv).stubs().will(returnValue(logLv));
    InjectLogger logger;
    GlobalMockObject::verify();
}

TEST(InjectLogger, invalid_log_level)
{
    auto &logger = InjectLogger::Instance();
    logger.Log(static_cast<LogLv>(10U), "format", "func", "test log");
}

TEST(InjectLogger, normal_log_when_info_is_set)
{
    ERROR_LOG("test log");
    WARN_LOG("test log");
    INFO_LOG("test log");
    DEBUG_LOG("test log");
}