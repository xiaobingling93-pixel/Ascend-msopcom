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
#include "KernelConfigParser.h"
#undef private

#include <string>
#include <gtest/gtest.h>
#include "mockcpp/mockcpp.hpp"
#include "utils/FileSystem.h"

TEST(KernelConfigParser, set_kernel_name_success)
{
    const std::string kernelName = "kernelName";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetKernelName(kernelName), true);
}

TEST(KernelConfigParser, set_kernel_name_fail_due_to_kernel_name_empty)
{
    const std::string kernelName = "";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetKernelName(kernelName), false);
}

TEST(KernelConfigParser, set_kernel_name_fail_due_to_kernel_name_too_long)
{
    std::vector<char> kernelNameVec(1024, 1); // 定义一个1024长度的char数组，初始化值为1
    kernelNameVec[kernelNameVec.size() - 1] = '\0'; // 末尾元素设置为'\0'，保证字符串结束
    const std::string kernelName(kernelNameVec.data());
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetKernelName(kernelName), false);
}

TEST(KernelConfigParser, set_bin_path_success)
{
    const std::string binPath = "binPath";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetBinPath(binPath), true);
}

TEST(KernelConfigParser, set_block_dim_success)
{
    const std::string blockDim = "16";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetBlockDim(blockDim), true);
}

TEST(KernelConfigParser, set_block_dim_fail_due_to_block_dim_empty)
{
    const std::string blockDim = "";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetBlockDim(blockDim), false);
}

TEST(KernelConfigParser, set_device_id_success)
{
    const std::string deviceId = "1";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetDeviceId(deviceId), true);
}

TEST(KernelConfigParser, set_device_id_fail_due_to_device_id_empty)
{
    const std::string deviceId = "";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetDeviceId(deviceId), false);
}

TEST(KernelConfigParser, set_magic_success)
{
    const std::string magic = "RT_DEV_BINARY_MAGIC_ELF_AIVEC";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetMagic(magic), true);
}

TEST(KernelConfigParser, set_magic_fail_due_to_magic_empty)
{
    const std::string magic = "";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetMagic(magic), false);
}

TEST(KernelConfigParser, set_ffts_success)
{
    const std::string arg = "Y";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetFfts(arg), true);
}

TEST(KernelConfigParser, set_ffts_fail_due_to_arg_empty)
{
    const std::string arg = "";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetFfts(arg), true);
}

TEST(KernelConfigParser, set_input_path_success)
{
    const std::string path = "./test";
    KernelConfigParser parser;
    parser.inputCount_ = 1U; // 1个输入
    ASSERT_EQ(parser.SetInputPath(path), true);
}

TEST(KernelConfigParser, set_input_path_fail_due_to_arg_empty)
{
    const std::string path = "";
    KernelConfigParser parser;
    parser.inputCount_ = 1U; // 1个输入
    ASSERT_EQ(parser.SetInputPath(path), false);
}

TEST(KernelConfigParser, set_input_path_success_due_to_null_data)
{
    const std::string path = "n";
    KernelConfigParser parser;
    parser.inputCount_ = 1U; // 1个输入
    ASSERT_EQ(parser.SetInputPath(path), true);
}

TEST(KernelConfigParser, set_input_size_success)
{
    const std::string arg = "64";
    KernelConfigParser parser;
    parser.inputCount_ = 1U; // 1个输入
    ASSERT_EQ(parser.SetInputSize(arg), true);
}

TEST(KernelConfigParser, set_input_size_fail_due_to_no_input_path_first)
{
    const std::string arg = "64";
    KernelConfigParser parser;
    parser.inputCount_ = 0U; // 无输入
    ASSERT_EQ(parser.SetInputSize(arg), false);
}

TEST(KernelConfigParser, set_input_size_fail_due_to_arg_empty)
{
    const std::string arg = "";
    KernelConfigParser parser;
    parser.inputCount_ = 1U; // 1个输入
    ASSERT_EQ(parser.SetInputSize(arg), false);
}

TEST(KernelConfigParser, set_input_path_fail_due_to_input_count_not_match)
{
    const std::string arg = "64";
    KernelConfigParser parser;
    parser.inputCount_ = 100U; // 构造异常的input count，与输入不匹配
    ASSERT_EQ(parser.SetInputSize(arg), false);
}

TEST(KernelConfigParser, set_input_path_fail_due_to_invalid_char)
{
    const std::string arg = "n64";
    KernelConfigParser parser;
    parser.inputCount_ = 1U; // 1个输入
    KernelConfig::Param param;
    param.type = "input";
    parser.kernelConfig_.params.emplace_back(param);
    ASSERT_EQ(parser.SetInputSize(arg), false);
}

TEST(KernelConfigParser, set_output_size_success)
{
    const std::string arg = "64;64";
    KernelConfigParser parser;
    parser.outputCount_ = 2U; // 2个输入
    ASSERT_EQ(parser.SetOutputSize(arg), true);
}

TEST(KernelConfigParser, set_output_size_fail_due_to_invalid_char)
{
    const std::string arg = "as;dew";
    const std::string argName = "as;dew";
    KernelConfigParser parser;
    parser.SetOutputName(argName);
    ASSERT_EQ(parser.SetOutputSize(arg), false);
}

TEST(KernelConfigParser, set_output_name_and_size_success)
{
    const std::string arg = "1024;1024";
    const std::string argName = "z1;z2";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetOutputName(argName), true);
    ASSERT_EQ(parser.SetOutputSize(arg), true);
}

TEST(KernelConfigParser, set_output_size_fail_due_to_name_count_error)
{
    const std::string arg = "1;2";
    const std::string argName = "as;dew;sd";
    KernelConfigParser parser;
    parser.SetOutputName(argName);
    ASSERT_EQ(parser.SetOutputSize(arg), false);
}

TEST(KernelConfigParser, set_workspace_size_success)
{
    const std::string arg = "64;64";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetWorkspaceSize(arg), true);
}

TEST(KernelConfigParser, set_workspace_size_fail_due_to_invalid_char)
{
    const std::string arg = "asd";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetWorkspaceSize(arg), false);
}

TEST(KernelConfigParser, set_output_dir_success)
{
    const std::string arg = "./out";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetOutputDir(arg), true);
}

TEST(KernelConfigParser, set_tiling_data_path_success)
{
    const std::string path = "./test;64";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetTilingDataPath(path), true);
}

TEST(KernelConfigParser, set_tiling_data_success_due_to_arg_empty)
{
    const std::string path = "";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetTilingDataPath(path), true);
}

TEST(KernelConfigParser, set_tiling_data_fail_due_to_missing_size)
{
    const std::string path = "./test;";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetTilingDataPath(path), false);
}

TEST(KernelConfigParser, set_tiling_key_path_success)
{
    const std::string path = "1";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetTilingKey(path), true);
}

TEST(KernelConfigParser, set_tiling_key_success_due_to_arg_empty)
{
    const std::string path = "";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetTilingKey(path), true);
}

TEST(KernelConfigParser, set_tiling_key_fail_due_to_invalid_char)
{
    const std::string path = "n";
    KernelConfigParser parser;
    ASSERT_EQ(parser.SetTilingKey(path), false);
}

TEST(KernelConfigParser, get_kernel_config_success)
{
    const std::string path = "./test";
    KernelConfigParser parser;
    KernelConfig config;
    MOCKER(&KernelConfigParser::GetJsonFromBin)
        .stubs().will(returnValue(true));

    auto res = parser.GetKernelConfig(path);
    GlobalMockObject::verify();
}

TEST(KernelConfigParser, get_json_from_bin_fail_due_to_read_binary_fail)
{
    const std::string path = "./test";
    nlohmann::json testJson;
    KernelConfigParser parser;
    MOCKER(ReadBinary)
        .expects(exactly(1))
        .will(returnValue(0U)); // 返回size为0
    ASSERT_EQ(parser.GetJsonFromBin(path, testJson), false);
    GlobalMockObject::verify();
}
