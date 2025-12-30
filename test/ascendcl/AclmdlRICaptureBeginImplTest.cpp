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
#include "acl_rt_impl/HijackedFunc.h"
#include "runtime/inject_helpers/LaunchManager.h"

TEST(AclmdlRICaptureBeginImpl, call_pre_function_expect_stream_record_sink)
{
    HijackedFuncOfAclmdlRICaptureBeginImpl instance;
    int stream{};
    instance.Pre(&stream, ACL_MODEL_RI_CAPTURE_MODE_GLOBAL);
    auto &streamInfo = LaunchManager::GetOrCreateStreamInfo(&stream);
    ASSERT_TRUE(streamInfo.binded);
}
