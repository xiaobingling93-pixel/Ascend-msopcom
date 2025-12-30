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
#include "ascendcl/AscendclOrigin.h"
#include "mockcpp/mockcpp.hpp"

TEST(AscendclOrigin, ascendcl_function_load_failed)
{
    MOCKER(&func_injection::register_function::FunctionRegister::Get)
        .stubs()
        .will(returnValue((void*)nullptr));
    AscendclOriginCtor();
    EXPECT_NE(aclrtSetOpExecuteTimeOutOrigin(0), ACL_ERROR_NONE);
    EXPECT_NE(aclrtCreateEventOrigin(nullptr), ACL_ERROR_NONE);
    EXPECT_NE(aclrtStreamWaitEventOrigin(nullptr, nullptr), ACL_ERROR_NONE);
    EXPECT_NE(aclrtRecordEventOrigin(nullptr, nullptr), ACL_ERROR_NONE);
    EXPECT_NE(aclrtResetEventOrigin(nullptr, nullptr), ACL_ERROR_NONE);
    EXPECT_NE(aclrtDestroyEventOrigin(nullptr), ACL_ERROR_NONE);
    GlobalMockObject::verify();
}
