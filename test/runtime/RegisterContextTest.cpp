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
#include "mockcpp/mockcpp.hpp"
#include "acl_rt_impl/HijackedFunc.h"
#include "runtime/inject_helpers/RegisterContext.h"
#include "runtime/inject_helpers/RegisterManager.h"
#include "acl_rt_impl/AscendclImplOrigin.h"
#include "utils/FileSystem.h"
#include "utils/ElfLoader.h"
using namespace std;

static bool GetValidNameFromBinary(const char *data,
    uint64_t length,
    std::vector<std::string> &kernelNames,
    std::vector<uint64_t> &kernelOffsets)
{
    kernelNames.emplace_back("valid_kernel_1234_mix_aic");
    kernelNames.emplace_back("valid_kernel_1235_mix_aiv");
    kernelOffsets.emplace_back(0);
    kernelOffsets.emplace_back(1);
    return true;
}

static void CreateKernelJsonFile(std::string const &path, std::string const &content)
{
    std::ofstream ofs(path);
    ofs.write(content.c_str(), content.length());
}

class RegisterContextTest : public testing::Test {
public:
    void SetUp() override
    {
        AscendclImplOriginCtor();
        MOCKER(&GetSymInfoFromBinary).stubs().will(invoke(GetValidNameFromBinary));
        MOCKER(&HasStaticStub).stubs().will(returnValue(true));
        const char *path = "empty.o";
        uint32_t data{};
        vector<char> elfData(100, 1);
        aclrtBinHandle binHandle = &data;
        MOCKER_CPP(&ReadBinary).stubs().with(any(), outBound(elfData)).will(returnValue(size_t(1)));
        regCtx_ = RegisterManager::Instance().CreateContext(path, binHandle, RT_DEV_BINARY_MAGIC_ELF);
        ASSERT_NE(regCtx_, nullptr);
    }

    void TearDown() override
    {
        GlobalMockObject::verify();
        RegisterManager::Instance().Clear();
    }

    RegisterContextSP regCtx_;
};

TEST_F(RegisterContextTest, parse_magic_str_expect_return_corrent_magic_number)
{
    uint32_t magic{};
    ASSERT_TRUE(ParseMagicStr("RT_DEV_BINARY_MAGIC_ELF_AIVEC", magic));
    ASSERT_EQ(magic, RT_DEV_BINARY_MAGIC_ELF_AIVEC);
    ASSERT_TRUE(ParseMagicStr("RT_DEV_BINARY_MAGIC_ELF_AICUBE", magic));
    ASSERT_EQ(magic, RT_DEV_BINARY_MAGIC_ELF_AICUBE);
    ASSERT_TRUE(ParseMagicStr("RT_DEV_BINARY_MAGIC_ELF", magic));
    ASSERT_EQ(magic, RT_DEV_BINARY_MAGIC_ELF);
    ASSERT_TRUE(ParseMagicStr("RT_DEV_BINARY_MAGIC_ELF_AICPU", magic));
    ASSERT_EQ(magic, RT_DEV_BINARY_MAGIC_ELF_AICPU);
    ASSERT_FALSE(ParseMagicStr("UNKNOWN_MAGIC", magic));
}

TEST_F(RegisterContextTest, read_magic_from_not_exist_kernel_json_expect_return_false)
{
    std::string jsonPath("/tmp/not_exist.json");
    std::string magicStr;
    ASSERT_FALSE(ReadMagicFromKernelJson(jsonPath, magicStr));
}

TEST_F(RegisterContextTest, read_magic_when_json_parse_failed_expect_return_false)
{
    std::string jsonPath("/tmp/kernel.json");
    CreateKernelJsonFile(jsonPath, "{");
    std::string magicStr;
    ASSERT_FALSE(ReadMagicFromKernelJson(jsonPath, magicStr));
    std::remove(jsonPath.c_str());
}

TEST_F(RegisterContextTest, read_magic_from_json_without_magic_expect_return_false)
{
    std::string jsonPath("/tmp/kernel.json");
    CreateKernelJsonFile(jsonPath, "{}");
    std::string magicStr;
    ASSERT_FALSE(ReadMagicFromKernelJson(jsonPath, magicStr));
    std::remove(jsonPath.c_str());
}

TEST_F(RegisterContextTest, read_magic_from_json_with_valid_magic_expect_return_true)
{
    std::string jsonPath("/tmp/kernel.json");
    CreateKernelJsonFile(jsonPath, "{\"magic\":\"RT_DEV_BINARY_MAGIC_ELF_AIVEC\"}");
    ASSERT_TRUE(Chmod(jsonPath, 0640));
    std::string magicStr;
    ASSERT_TRUE(ReadMagicFromKernelJson(jsonPath, magicStr));
    ASSERT_EQ(magicStr, "RT_DEV_BINARY_MAGIC_ELF_AIVEC");
    std::remove(jsonPath.c_str());
}

TEST_F(RegisterContextTest, input_valid_tiling_key_then_get_kernel_name_expect_success)
{
    string expect = "valid_kernel_1234_mix_aic";
    ASSERT_EQ(regCtx_->GetKernelName(1234), expect);
}

TEST_F(RegisterContextTest, input_invalid_tiling_key_then_get_kernel_name_expect_fail)
{
    string expect = "";
    ASSERT_EQ(regCtx_->GetKernelName(0), expect);
}

TEST_F(RegisterContextTest, input_valid_kernel_name_then_get_offset_expect_success)
{
    string name = "valid_kernel_1235_mix_aiv";
    uint64_t expect = 1;
    ASSERT_EQ(regCtx_->GetKernelOffsetByName(name), expect);
}

TEST_F(RegisterContextTest, input_no_suffix_kernel_name_then_get_offset_expect_success)
{
    string name = "valid_kernel_1235";
    uint64_t expect = 1;
    ASSERT_EQ(regCtx_->GetKernelOffsetByName(name), expect);
}

TEST_F(RegisterContextTest, input_invalid_tiling_key_then_get_offset_expect_fail)
{
    string name = "abc";
    uint64_t expect = 0;
    ASSERT_EQ(regCtx_->GetKernelOffsetByName(name), expect);
}

TEST_F(RegisterContextTest, save_expect_true)
{
    MOCKER(&ElfLoader::LoadHeader).stubs().will(returnValue(true));
    EXPECT_TRUE(regCtx_->Save("empty path"));
}

TEST_F(RegisterContextTest, save_expect_false)
{
    MOCKER(&ElfLoader::LoadHeader).stubs().will(returnValue(true));
    MOCKER(&WriteBinary).stubs().will(returnValue(32));
    EXPECT_FALSE(regCtx_->Save("empty path"));
}

TEST_F(RegisterContextTest, Clone_expect_success)
{
    MOCKER(&IsExist).stubs().will(returnValue(true));
    MOCKER(&CopyFile).stubs().will(returnValue(true));
    EXPECT_NE(regCtx_->Clone("empty path4"), nullptr);
}

TEST_F(RegisterContextTest, mock_bin_file_then_call_clone_from_bin_expect_success)
{
    // create register context from data
    vector<char> elfData(100, 1);
    aclrtBinHandle binHandle = elfData.data();
    const void *data = static_cast<const void*>(elfData.data());
    auto ctx = RegisterManager::Instance().CreateContext(data, elfData.size(), binHandle, RT_DEV_BINARY_MAGIC_ELF, nullptr);
    EXPECT_NE(ctx->Clone("mock_path"), nullptr);
}
