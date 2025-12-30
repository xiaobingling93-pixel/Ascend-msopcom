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


#include "core/Communication.h"

#include <chrono>
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

constexpr char const *MSOP_SOCKET_PATH_ENV = "MSOP_SOCKET_PATH";

void AssertReadMessage(CommunicationServer &server, std::size_t clientId, std::string const &message)
{
    std::string received;
    std::string buffer;
    while (received.size() < message.size()) {
        if (server.Read(clientId, buffer) > 0) {
            received += buffer;
        }
    }
    ASSERT_EQ(received, message);
}

void AssertWriteMessage(CommunicationServer &server, std::size_t clientId, std::string const &message)
{
    ssize_t sentBytes = 0;
    std::size_t totalSent = 0;
    while (totalSent < message.size()) {
        sentBytes = server.Write(clientId, message);
        if (sentBytes > 0) {
            totalSent += static_cast<std::size_t>(sentBytes);
        }
    }
    ASSERT_EQ(totalSent, message.size());
}

void AssertReadMessage(CommunicationClient &client, std::string const &message)
{
    std::string received;
    std::string buffer;
    while (received.size() < message.size()) {
        if (client.Read(buffer) > 0) {
            received += buffer;
        }
    }
    ASSERT_EQ(received, message);
}

void AssertWriteMessage(CommunicationClient &client, std::string const &message)
{
    ASSERT_EQ(static_cast<std::size_t>(client.Write(message)), message.size());
}

void TryConnect(Client &client)
{
    while (!client.Connect()) {
        // server 启动前 client 会连接失败，等待 100ms 后重试
        constexpr uint64_t connectRetryDuration = 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(connectRetryDuration));
    }
}

TEST(Communication, send_msg_from_server_to_client_expect_sucess)
{
    Server server(CommType::MEMORY);
    server.Start();
    Client client(CommType::MEMORY);
    std::string sendMsg = "hello";
    server.Write(0, sendMsg);
    std::string recvMsg;
    int len = client.Read(recvMsg);
    ASSERT_EQ(len, 5);
    ASSERT_STREQ(sendMsg.c_str(), recvMsg.c_str());
}

TEST(Communication, send_msg_from_client_to_server_expect_sucess)
{
    Server server(CommType::MEMORY);
    server.Start();
    Client client(CommType::MEMORY);
    std::string sendMsg = "hello";
    client.Write(sendMsg);
    std::string recvMsg;
    int len = server.Read(0, recvMsg);
    ASSERT_EQ(len, 5);
    ASSERT_STREQ(sendMsg.c_str(), recvMsg.c_str());
}

TEST(Communication, send_interleaving_msg_expect_sucess)
{
    Server server(CommType::MEMORY);
    server.Start();
    Client client(CommType::MEMORY);
    std::vector<std::string> msgs = {"first", "second", "third", "forth"};
    server.Write(0, msgs[0]);
    client.Write(msgs[1]);
    server.Write(0, msgs[2]);
    server.Write(0, msgs[3]);
    std::string recvMsg;
    (void)server.Read(0, recvMsg);
    ASSERT_STREQ(msgs[1].c_str(), recvMsg.c_str());
    (void)client.Read(recvMsg);
    ASSERT_STREQ(msgs[0].c_str(), recvMsg.c_str());
    (void)client.Read(recvMsg);
    ASSERT_STREQ(msgs[2].c_str(), recvMsg.c_str());
    (void)client.Read(recvMsg);
    ASSERT_STREQ(msgs[3].c_str(), recvMsg.c_str());
}

TEST(Communication, socket_comm_send_msg_from_server_to_client_expect_sucess)
{
    std::string sendMsg = "hello";
    Server server(CommType::SOCKET);
    server.Start();
    pid_t pid = fork();

    if (pid == 0) {
        Client client(CommType::SOCKET);
        TryConnect(client);
        AssertReadMessage(client, sendMsg);
        _exit(0);
    } else {
        // 保证服务端在客户端之后释放
        AssertWriteMessage(server, 0, sendMsg);
        waitpid(pid, nullptr, 0);
        ASSERT_NE(pid, -1);
    }
}

TEST(Communication, socket_comm_send_msg_from_client_to_server_expect_sucess)
{
    std::string sendMsg = "hello";
    Server server(CommType::SOCKET);
    server.Start();
    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        Client client(CommType::SOCKET);
        TryConnect(client);
        AssertWriteMessage(client, sendMsg);
        // 接收到退出指令后再退出客户端，防止服务端未完成数据读取前客户端已关闭
        AssertReadMessage(client, "quit");
        _exit(0);
    } else {
        AssertReadMessage(server, 0, sendMsg);
        // 通知客户端可以退出
        AssertWriteMessage(server, 0, "quit");
        // 保证服务端在客户端之后释放
        waitpid(pid, nullptr, 0);
    }
}

TEST(Communication, socket_comm_send_interleaving_msg_expect_sucess)
{
    std::vector<std::string> messages = {"first", "second", "third", "forth"};
    Server server(CommType::SOCKET);
    server.Start();
    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        Client client(CommType::SOCKET);
        TryConnect(client);
        AssertWriteMessage(client, messages[0]);
        AssertWriteMessage(client, messages[1]);
        AssertReadMessage (client, messages[2]);
        AssertWriteMessage(client, messages[3]);
        // 接收到退出指令后再退出客户端，防止服务端未完成数据读取前客户端已关闭
        AssertReadMessage(client, "quit");
        _exit(0);
    } else {
        AssertReadMessage (server, 0, messages[0] + messages[1]);
        AssertWriteMessage(server, 0, messages[2]);
        AssertReadMessage (server, 0, messages[3]);
        // 通知客户端可以退出
        AssertWriteMessage(server, 0, "quit");
        // 保证服务端在客户端之后释放
        waitpid(pid, nullptr, 0);
    }
}

TEST(Communication, socket_comm_multiple_clients_connect_to_server_expect_sucess)
{
    std::vector<std::string> messages = {"first", "second", "third", "forth"};
    Server server(CommType::SOCKET);
    server.Start();
    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        Client client0(CommType::SOCKET);
        Client client1(CommType::SOCKET);
        Client client2(CommType::SOCKET);
        Client client3(CommType::SOCKET);
        TryConnect(client0);
        TryConnect(client1);
        TryConnect(client2);
        TryConnect(client3);
        AssertWriteMessage(client0, messages[0]);
        AssertWriteMessage(client1, messages[1]);
        AssertReadMessage (client2, messages[2]);
        AssertWriteMessage(client3, messages[3]);
        // 接收到退出指令后再退出客户端，防止服务端未完成数据读取前客户端已关闭
        AssertReadMessage(client0, "quit");
        _exit(0);
    } else {
        AssertReadMessage(server, 0, messages[0]);
        AssertReadMessage(server, 1, messages[1]);
        AssertWriteMessage(server, 2, messages[2]);
        AssertReadMessage(server, 3, messages[3]);
        // 通知客户端可以退出
        AssertWriteMessage(server, 0, "quit");
        // 保证服务端在客户端之后释放
        waitpid(pid, nullptr, 0);
    }
}

TEST(Communication, socket_comm_send_message_in_client_connect_callback_expect_sucess)
{
    std::string message = "hello";
    std::mutex mutex;
    std::condition_variable cv;
    std::size_t receivedNum = 0;
    std::string received;
    Server server(CommType::SOCKET);
    server.SetClientConnectHook([&](ClientId id) {
        // 每个客户端连接时触发读取
        std::string buffer;
        while (received.size() < message.size()) {
            server.Read(id, buffer);
            received += buffer;
        }
        // 通知主线程消息接收完毕
        std::unique_lock<std::mutex> lock(mutex);
        ++receivedNum;
        cv.notify_one();
    });
    server.Start();
    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        Client client(CommType::SOCKET);
        TryConnect(client);
        AssertWriteMessage(client, message);
        // 接收到退出指令后再退出客户端，防止服务端未完成数据读取前客户端已关闭
        AssertReadMessage(client, "quit");
        _exit(0);
    } else {
        {
            // 等待回调函数中所有消息接收完毕，再进行消息完整性校验
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [&]{ return receivedNum > 0; });
        }
        ASSERT_EQ(message, received);

        // 通知客户端可以退出
        AssertWriteMessage(server, 0, "quit");

        // 保证服务端在客户端之后释放
        waitpid(pid, nullptr, 0);
    }
}
TEST(Communication, socket_client_connect_fail) // SocketClient connet失败
{
    Client client(CommType::SOCKET);
    bool ret = client.Connect();
    EXPECT_FALSE(ret);
}
 
TEST(Communication, socket_client_read_fail) // SocketClient read失败
{
    std::string msg = "hello";
    Client client(CommType::SOCKET);
    int ret = client.Read(msg);
    ASSERT_EQ(ret, -1);
}
 
TEST(Communication, socket_client_write_fail) // SocketClient write失败
{
    std::string msg = "hello";
    Client client(CommType::SOCKET);
    int ret = client.Write(msg);
    ASSERT_EQ(ret, -1);
}

TEST(Communication, socket_client_init_and_destruct_with_or_without_env_var)
{
    constexpr char const *msopSocketPathEnv = "MSOP_SOCKET_PATH";
    const char *originalSocketPath = secure_getenv(msopSocketPathEnv);
    const char *tempEnvSocketPath = "./temp/socket_path";
    int nonZeroValue = 1;
    setenv(msopSocketPathEnv, tempEnvSocketPath, nonZeroValue);
    Client client1(CommType::SOCKET);

    unsetenv(msopSocketPathEnv);
    Client client2(CommType::SOCKET);
    setenv(msopSocketPathEnv, originalSocketPath, nonZeroValue);
}
