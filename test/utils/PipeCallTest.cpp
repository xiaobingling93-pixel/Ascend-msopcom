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


#include "utils/PipeCall.h"

#include <fcntl.h>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

#include "mockcpp/mockcpp.hpp"

namespace {

void RunCatCommand(void)
{
    std::vector<std::string> argv = {"cat"};
    std::string input = "some contents";
    std::string output;
    ASSERT_TRUE(PipeCall(argv, output, input));
    ASSERT_EQ(output, input);
}

} // namespace [Dummy]

TEST(PipeCall, pipe_call_unknown_command_expect_return_false)
{
    std::vector<std::string> argv = {"unknown-command"};
    std::string output;
    ASSERT_FALSE(PipeCall(argv, output));
}

TEST(PipeCall, pipe_call_falied_due_to_pipe_return_non_zero)
{
    std::vector<std::string> argv = {"test"};
    std::string output;
    MOCKER(&pipe).stubs().will(returnValue(1));
    ASSERT_FALSE(PipeCall(argv, output));
    GlobalMockObject::verify();
}

TEST(PipeCall, pipe_call_cat_with_stdin_expect_success_and_return_correct_output)
{
    RunCatCommand();
}

TEST(PipeCall, pipe_call_cat_with_really_large_stdin_expect_success_and_return_correct_output)
{
    static constexpr std::size_t totalLen = 4UL * 1024UL * 1024UL;
    static constexpr std::size_t bufLen = 1024;
    char buf[bufLen] {};

    // generate random input
    std::string input;
    int fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK, 0);
    ASSERT_NE(fd, -1);
    ssize_t nBytes = 0;
    while (input.size() < totalLen && (nBytes = read(fd, buf, bufLen)) > 0) {
        input.append(buf, static_cast<std::size_t>(nBytes));
    }
    close(fd);
    ASSERT_TRUE(input.size() >= totalLen);

    std::vector<std::string> argv = {"cat"};
    std::string output;
    ASSERT_TRUE(PipeCall(argv, output, input));
    ASSERT_EQ(output, input);
}

TEST(PipeCall, pipe_call_cat_with_multiple_threads_expect_success_and_return_correct_output)
{
    static constexpr std::size_t threadCount = 100;
    std::vector<std::thread> workers;
    for (std::size_t idx = 0UL; idx < threadCount; ++idx) {
        workers.emplace_back(RunCatCommand);
    }
    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

TEST(PipeCall, pipe_call_cat_with_multiple_processes_expect_success_and_return_correct_output)
{
    static constexpr std::size_t processCount = 100;
    std::vector<pid_t> pids;
    for (std::size_t idx = 0UL; idx < processCount; ++idx) {
        pid_t pid = fork();
        if (pid < 0) {
            return;
        } else if (pid == 0) {
            RunCatCommand();
            _exit(EXIT_SUCCESS);
        } else {
            pids.emplace_back(pid);
        }
    }
    for (pid_t pid : pids) {
        int status;
        waitpid(pid, &status, 0);
        ASSERT_EQ(WEXITSTATUS(status), EXIT_SUCCESS);
    }
}
