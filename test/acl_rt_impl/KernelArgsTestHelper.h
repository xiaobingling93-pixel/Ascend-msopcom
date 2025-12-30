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
#include "runtime/inject_helpers/ArgsManager.h"
#include "runtime/inject_helpers/ArgsHandleContext.h"

class HijacedFuncOfKernelArgsTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        AscendclImplOriginCtor();
    }

    void SetUp() override
    {
        funcHandle_ = &placeholder_;
        argsHandle_ = &placeholder_;
        param_ = &placeholder_;
        paramSize_ = sizeof(placeholder_);
        paramHandle_ = nullptr;
        bufferAddr_ = &placeholder_;
        dataSize_ = sizeof(placeholder_);
        argsHandleCtx_ = std::make_shared<ArgsHandleContext>(funcHandle_, argsHandle_);
    }

    void TearDown() override
    {
        GlobalMockObject::verify();
    }

    ArgsContextSP argsHandleCtx_;
    uint64_t placeholder_;
    aclrtFuncHandle funcHandle_;
    aclrtArgsHandle argsHandle_;
    aclrtParamHandle paramHandle_;
    size_t dataSize_;
    void *bufferAddr_;
    void *param_;
    size_t paramSize_;
};
