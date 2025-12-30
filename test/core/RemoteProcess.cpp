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
#include "core/LocalProcess.h"
#include "core/RemoteProcess.h"

// 针对多进程间的信息交互，在UT中采用多线程进行模拟
// 针对用例的设计，需要覆盖基础的接口
// 说明：对于阻塞接口，需要多线程实现；其他可以单独实现

TEST(RemoteProcess, notify_expect_success)
{
    LocalProcess local;
    RemoteProcess remote(CommType::MEMORY);
    remote.Notify("test");
    std::string recvMsg;
    (void)LocalProcess::GetInstance(CommType::MEMORY).Wait(recvMsg);
    ASSERT_STREQ(recvMsg.c_str(), "test");
}

TEST(RemoteProcess, wait_expect_success)
{
    RemoteProcess remote(CommType::MEMORY);
    std::string sendMsg = "hello";
    LocalProcess::GetInstance(CommType::MEMORY).Notify(sendMsg);
    std::string recvMsg;
    remote.Wait(recvMsg);
    ASSERT_STREQ(recvMsg.c_str(), sendMsg.c_str());
}