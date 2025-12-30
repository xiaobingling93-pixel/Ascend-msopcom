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
#include "core/FunctionLoader.h"

TEST(FunctionLoader, set_library_and_get_expect_success)
{
    func_injection::FunctionLoader lib("runtime");
    lib.Set("rtKernelLaunch");
    auto ptr = lib.Get("rtKernelLaunch");
    ASSERT_NE(ptr, nullptr);
}

TEST(FunctionLoaderRegister, register_func_expect_success)
{
    auto instance = func_injection::register_function::FunctionRegister::GetInstance();
    auto lib = ::std::unique_ptr<func_injection::FunctionLoader>(new func_injection::FunctionLoader("runtime"));
    instance->Register("runtime", std::move(lib));
    instance->Register("runtime", "rtKernelLaunch");
    auto ptr = instance->Get("runtime", "rtKernelLaunch");
    ASSERT_NE(ptr, nullptr);
}