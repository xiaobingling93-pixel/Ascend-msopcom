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


#include <thread>
#include <gtest/gtest.h>
#include "core/LocalProcess.h"
#include "core/RemoteProcess.h"
#include "mockcpp/mockcpp.hpp"

// 针对多进程间的信息交互，在UT中采用多线程进行模拟
// 针对用例的设计，需要覆盖基础的接口
// 说明：对于阻塞接口，需要多线程实现；其他可以单独实现

TEST(LocalProcess, send_log_expect_success)
{
    RemoteProcess remote(CommType::MEMORY);
    LocalProcess::GetInstance(CommType::MEMORY).Log(LogLevel::ERROR, "something happens.");
    std::string recvMsg;
    (void)remote.Wait(0, recvMsg);
    ASSERT_STREQ(recvMsg.c_str(), "[LOG][ERROR] something happens.;");
}

TEST(LocalProcess, notify_no_block_expect_success)
{
    RemoteProcess remote(CommType::MEMORY);
    LocalProcess::GetInstance(CommType::MEMORY).Notify("test");
    std::string recvMsg;
    (void)remote.Wait(0, recvMsg);
    std::string notifyMsg = "test";
    ASSERT_STREQ(recvMsg.c_str(), notifyMsg.c_str());
}

TEST(LocalProcess, wait_expect_success)
{
    RemoteProcess remote(CommType::MEMORY);
    std::string sendMsg = "hello";
    remote.Notify(0, sendMsg);
    std::string recvMsg = "";
    LocalProcess::GetInstance(CommType::MEMORY).Wait(recvMsg);
    ASSERT_STREQ(recvMsg.c_str(), sendMsg.c_str());
}

TEST(LocalProcess, wait_api_test_return_success)
{
    MOCKER(&LocalProcess::Wait).stubs().will(returnValue(ssize_t(0)));
    std::string msg;
    ASSERT_EQ(Wait(msg), 0);
    GlobalMockObject::verify();
}

TEST(LocalProcess, log_api_test)
{
    MOCKER(&LocalProcess::Log).stubs().will(returnValue(0));
    std::string log = "test";
    DebugLog(log);
    InfoLog(log);
    WarnLog(log);
}
