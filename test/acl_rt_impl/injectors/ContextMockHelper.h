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
#include <vector>

#include "mockcpp/mockcpp.hpp"
#include "acl_rt_impl/AscendclImplOrigin.h"
#include "runtime/inject_helpers/LaunchManager.h"
#include "runtime/inject_helpers/FuncManager.h"
#include "runtime/inject_helpers/ArgsManager.h"
#include "runtime/inject_helpers/RegisterManager.h"
#include "runtime/inject_helpers/ArgsHandleContext.h"
#include "runtime/inject_helpers/TilingFuncContext.h"

class ContextMockHelper : public testing::Test {
public:
    void SetUp() override
    {
        AscendclImplOriginCtor();
        regCtx_ = std::make_shared<RegisterContext>();
        MOCKER_CPP(&RegisterManager::GetContext).stubs().will(returnValue(regCtx_));
        std::string kernelName = "abc";
        MOCKER_CPP(&RegisterContext::GetKernelName).stubs().will(returnValue(kernelName));
        funcHandle_ = &placeholder_;
        argsHandle_ = &placeholder_;
        binHandle_ = &placeholder_;
        argsBinCtx_ = ArgsManager::Instance().CreateContext(funcHandle_, argsHandle_);
        tilingFuncCtx_ = FuncManager::Instance().CreateContext(binHandle_, uint64_t(0), funcHandle_);
        ASSERT_NE(argsBinCtx_, nullptr);
        ASSERT_NE(tilingFuncCtx_, nullptr);
    }

    void TearDown() override
    {
        GlobalMockObject::verify();
        LaunchManager::Local().Clear();
        RegisterManager::Instance().Clear();
        FuncManager::Instance().Clear();
    }

    uint64_t placeholder_;
    void *funcHandle_;
    void *argsHandle_;
    void *binHandle_;
    RegisterContextSP regCtx_;
    ArgsContextSP argsBinCtx_;
    FuncContextSP tilingFuncCtx_;
};
