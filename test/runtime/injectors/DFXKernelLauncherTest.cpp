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
#define private public
#include "mockcpp/mockcpp.hpp"
#include "runtime/inject_helpers/DFXKernelLauncher.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "acl_rt_impl/AscendclImplOrigin.h"
#include "utils/FileSystem.h"
#include "utils/Environment.h"

using namespace std;

TEST(DFXKernelLauncher, test_init_success_910B)
{
    GlobalMockObject::verify();
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtBinaryGetFunctionImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&DeviceContext::GetSocVersion)
        .stubs()
        .will(returnValue(std::string("Ascend910B1")));
    MOCKER_CPP(ReadBinary)
        .stubs()
        .will(returnValue(64U));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(true));
    DFXKernelLauncher::Instance().Init("xyz", "123");
    EXPECT_TRUE(DFXKernelLauncher::Instance().kernelSet_.find("xyz") != DFXKernelLauncher::Instance().kernelSet_.end());
    EXPECT_TRUE(DFXKernelLauncher::Instance().kernelSet_.size() == 1);
    DFXKernelLauncher::Instance().kernelSet_.clear();
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, test_init_success_310P)
{
    GlobalMockObject::verify();
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtBinaryGetFunctionImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&DeviceContext::GetSocVersion)
        .stubs()
        .will(returnValue(std::string("Ascend310P1")));
    MOCKER_CPP(ReadBinary)
        .stubs()
        .will(returnValue(64U));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(true));
    DFXKernelLauncher::Instance().Init("xyz", "123");
    EXPECT_TRUE(DFXKernelLauncher::Instance().kernelSet_.size() == 1);
    DFXKernelLauncher::Instance().kernelSet_.clear();
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, test_init_failed_with_regist_kernel_failed)
{
    GlobalMockObject::verify();
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
        .stubs()
        .will(returnValue(std::string("Ascend910B1")));
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
        .stubs()
        .will(returnValue(ACL_ERROR_BAD_ALLOC));
    MOCKER_CPP(ReadBinary)
        .stubs()
        .will(returnValue(64U));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(true));
    DFXKernelLauncher::Instance().Init("xyz", "123");
    EXPECT_TRUE(DFXKernelLauncher::Instance().kernelSet_.size() == 0);
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, test_init_failed_with_regist_func_failed)
{
    GlobalMockObject::verify();
    MOCKER(&DeviceContext::GetSocVersion)
        .stubs()
        .will(returnValue(std::string("Ascend910B1")));
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtBinaryGetFunctionImplOrigin)
        .stubs()
        .will(returnValue(ACL_ERROR_BAD_ALLOC)); // rtFunctionRegisterOrigin return false
    MOCKER_CPP(ReadBinary)
        .stubs()
        .will(returnValue(64U));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(true));
    DFXKernelLauncher::Instance().Init("xyz", "123");
    EXPECT_TRUE(DFXKernelLauncher::Instance().kernelSet_.size() == 0);
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, test_clear_l2_cache_success)
{
    GlobalMockObject::verify();
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
        .stubs()
        .will(returnValue(std::string("Ascend910B1")));
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtBinaryGetFunctionImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtKernelArgsGetHandleMemSizeImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtMallocHostImplOrigin)
            .stubs()
            .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtKernelArgsGetMemSizeImplOrigin)
            .stubs()
            .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtKernelArgsInitByUserMemImplOrigin)
            .stubs()
            .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtLaunchKernelWithConfigImplOrigin)
            .stubs()
            .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtSynchronizeStreamImplOrigin)
            .stubs()
            .will(returnValue(ACL_SUCCESS));
    MOCKER_CPP(ReadBinary)
        .stubs()
        .will(returnValue(64U));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(true));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(true));
    DFXKernelLauncher::Instance().Init("ClearL2Cache", "123");
    std::vector<void *> inputArgs;
    EXPECT_TRUE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    DFXKernelLauncher::Instance().kernelSet_.clear();
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, test_clear_l2_cache_failed)
{
    GlobalMockObject::verify();
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
            .stubs()
            .will(returnValue(std::string("Ascend910B1")));
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
            .stubs()
            .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtBinaryGetFunctionImplOrigin)
            .stubs()
            .will(returnValue(ACL_SUCCESS));

    MOCKER(&aclrtKernelArgsGetHandleMemSizeImplOrigin)
            .stubs()
            .will(returnValue(ACL_ERROR_INTERNAL_ERROR)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0));
    MOCKER(&aclrtMallocHostImplOrigin)
            .stubs()
            .will(returnValue(ACL_ERROR_INTERNAL_ERROR)).then(returnValue(0)).then(returnValue(0)).then(returnValue(ACL_ERROR_INTERNAL_ERROR)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0));
    MOCKER(&aclrtKernelArgsGetMemSizeImplOrigin)
            .stubs()
            .will(returnValue(ACL_ERROR_INTERNAL_ERROR)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0)).then(returnValue(0));
    MOCKER(&aclrtKernelArgsInitByUserMemImplOrigin)
            .stubs()
            .will(returnValue(ACL_ERROR_INTERNAL_ERROR)).then(returnValue(0)).then(returnValue(0));
    MOCKER(&aclrtLaunchKernelWithConfigImplOrigin)
            .stubs()
            .will(returnValue(ACL_ERROR_INTERNAL_ERROR)).then(returnValue(0));
    MOCKER(&aclrtSynchronizeStreamImplOrigin)
            .stubs()
            .will(returnValue(ACL_ERROR_INTERNAL_ERROR));
    MOCKER_CPP(ReadBinary)
            .stubs()
            .will(returnValue(64U));
    MOCKER(GetAscendHomePath)
            .stubs()
            .will(returnValue(true));
    DFXKernelLauncher::Instance().Init("ClearL2Cache", "123");
    std::vector<void *> inputArgs;
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    DFXKernelLauncher::Instance().kernelSet_.clear();
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, test_clear_l2_cache_fail_with_init_failed)
{
    GlobalMockObject::verify();
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
        .stubs()
        .will(returnValue(std::string("invalid"))); // init failed
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtBinaryGetFunctionImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtLaunchKernelWithConfigImplOrigin)
        .stubs()
        .will(returnValue(ACL_SUCCESS));
    MOCKER(&aclrtKernelArgsGetHandleMemSizeImplOrigin)
        .stubs()
        .will(returnValue(ACL_ERROR_INTERNAL_ERROR));
    MOCKER_CPP(ReadBinary)
        .stubs()
        .will(returnValue(64U));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(true));
    DFXKernelLauncher::Instance().Init("ClearL2Cache", "123");
    std::vector<void *> inputArgs;
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, test_clear_l2_cache_failed_with_rt_error)
{
    GlobalMockObject::verify();
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
        .stubs()
        .will(returnValue(std::string("Ascend910B1")));
    MOCKER(GetAscendHomePath)
        .stubs()
        .will(returnValue(false));
    // default it will return success, so we mock it to failed
    MOCKER(&aclrtBinaryLoadFromFileImplOrigin)
        .stubs()
        .will(returnValue(ACL_ERROR_INTERNAL_ERROR));
    DFXKernelLauncher::Instance().Init("ClearL2Cache", "123");
    std::vector<void *> inputArgs;
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallKernel("ClearL2Cache", "path", 20, nullptr, inputArgs));
    DFXKernelLauncher::Instance().kernelSet_.clear();
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, CheckSupportSeries_expect_return_success)
{
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
            .stubs()
            .will(returnValue(std::string("Ascend910B1")));
    std::vector<ChipProductType> chipSeries = {ChipProductType::ASCEND910B_SERIES, ChipProductType::ASCEND910_93_SERIES};
    EXPECT_TRUE(DFXKernelLauncher::Instance().CheckSupportSeries(chipSeries));
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, CheckSupportSeries_expect_return_failed)
{
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
            .stubs()
            .will(returnValue(std::string("Ascend310P3")));
    std::vector<ChipProductType> chipSeries = {ChipProductType::ASCEND910B_SERIES, ChipProductType::ASCEND910_93_SERIES};
    EXPECT_FALSE(DFXKernelLauncher::Instance().CheckSupportSeries(chipSeries));
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, GetEmptyKernelPath_expect_return_failed)
{
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion)
            .stubs()
            .will(returnValue(std::string("Ascend910B1")));
    MOCKER(&ProfConfig::GetMsopprofPath)
            .stubs()
            .will(returnValue(std::string("")));
    EXPECT_TRUE(DFXKernelLauncher::Instance().GetEmptyKernelPath().empty());
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, GetEmptyKernelPath_expect_return_true)
{
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion).expects(atMost(4))
            .will(returnValue(std::string("Ascend910B1")));
    MOCKER(&ProfConfig::GetMsopprofPath)
            .stubs()
            .will(returnValue(std::string("/path")));
    EXPECT_STREQ(DFXKernelLauncher::Instance().GetEmptyKernelPath().c_str(), "/path/lib64/empty_kernel_dav-c220-vec.o");
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, GetL2CacheKernelPath_expect_return_failed)
{
    DFXKernelLauncher::Instance().kernelSet_.clear();
    MOCKER(&DeviceContext::GetSocVersion).expects(atMost(4))
            .will(returnValue(std::string("Ascend910B1")));
    MOCKER(&ProfConfig::GetMsopprofPath)
            .stubs()
            .will(returnValue(std::string("")));
    EXPECT_TRUE(DFXKernelLauncher::Instance().GetL2CacheKernelPath().empty());
    GlobalMockObject::verify();
}

TEST(DFXKernelLauncher, CallClearL2Cache_expect_return_fail)
{
    DFXKernelLauncher::Instance().kernelSet_.clear();
    std::vector<void *> param = {};
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallClearL2Cache(1, nullptr, param));
}

TEST(DFXKernelLauncher, CallEmptyKernel_expect_return_fail)
{
    DFXKernelLauncher::Instance().kernelSet_.clear();
    EXPECT_FALSE(DFXKernelLauncher::Instance().CallEmptyKernel(nullptr));
    GlobalMockObject::verify();
}