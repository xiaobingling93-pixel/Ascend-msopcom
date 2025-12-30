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
#include "acl_rt_impl/AscendclImplOrigin.h"
#include "runtime/inject_helpers/StubFuncContext.h"
#include "runtime/inject_helpers/TilingFuncContext.h"
#include "runtime/inject_helpers/FuncManager.h"
#include "runtime/inject_helpers/RegisterManager.h"

using namespace std;

class FuncContextTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        AscendclImplOriginCtor();
    }

    void SetUp() override
    {
        auto sp = std::make_shared<RegisterContext>();
        string kernelName = "abc";
        MOCKER_CPP(&RegisterManager::GetContext).stubs().will(returnValue(sp));
        MOCKER_CPP(&RegisterContext::GetKernelName).stubs().will(returnValue(kernelName));
        uint64_t data{};
        void *hdl = &data;
        void *funcHandle = &data;
        tilingFuncCtx_ = FuncManager::Instance().CreateContext(hdl, uint64_t(0), funcHandle);
        stubFuncCtx_ = FuncManager::Instance().CreateContext(hdl, kernelName.c_str(), funcHandle);
        ASSERT_NE(stubFuncCtx_, nullptr);
        ASSERT_NE(tilingFuncCtx_, nullptr);
    }

    void TearDown() override
    {
        GlobalMockObject::verify();
        FuncManager::Instance().Clear();
    }

    FuncContextSP stubFuncCtx_;
    FuncContextSP tilingFuncCtx_;
};

TEST_F(FuncContextTest, mock_kernel_addr_then_test_get_start_pc_expect_success)
{
    MOCKER_CPP(&RegisterContext::GetKernelOffsetByName).stubs().will(returnValue(1));
    uint64_t aicAddr = 123;
    MOCKER_CPP(&aclrtGetFunctionAddrImplOrigin).stubs().with(any(), outBoundP((void**)&aicAddr), any()).will(returnValue(ACL_SUCCESS));

    ASSERT_EQ(stubFuncCtx_->GetStartPC(), aicAddr - 1);
}

TEST_F(FuncContextTest, mock_kernel_addr_fail_then_test_get_start_pc_expect_success)
{
    MOCKER_CPP(&RegisterContext::GetKernelOffsetByName).stubs().will(returnValue(1));
    uint64_t aicAddr = 123;
    MOCKER_CPP(&aclrtGetFunctionAddrImplOrigin).stubs().with(any(), outBoundP((void**)&aicAddr), any()).will(returnValue(aclError(1)));

    ASSERT_EQ(stubFuncCtx_->GetStartPC(), 0);
}

TEST_F(FuncContextTest, mock_zero_kernel_addr_then_test_get_start_pc_expect_success)
{
    MOCKER_CPP(&RegisterContext::GetKernelOffsetByName).stubs().will(returnValue(1));
    MOCKER_CPP(&aclrtGetFunctionAddrImplOrigin).stubs().will(returnValue(ACL_SUCCESS));

    ASSERT_EQ(stubFuncCtx_->GetStartPC(), 0);
}
