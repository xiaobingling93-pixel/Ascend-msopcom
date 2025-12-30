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
#include <vector>
#include "mockcpp/mockcpp.hpp"

#include "runtime/inject_helpers/ArgsRawContext.h"


using namespace std;

class ArgsRawContextTest : public testing::Test {
public:
    static void SetUpTestCase() {}

    void SetUp() override {}

    void TearDown() override
    {
        GlobalMockObject::verify();
    }
};

TEST_F(ArgsRawContextTest, test_ExpandArgs_expect_success)
{
    void *args = nullptr;
    std::string str = "aa";
    void *add = &str;
    ArgsRawContext test(args, 32, true);

    MOCKER(aclrtMallocImplOrigin).expects(atMost(4)).will(returnValue(ACL_SUCCESS));
    MOCKER(aclrtFreeImplOrigin).expects(atMost(4)).will(returnValue(ACL_SUCCESS));
    MOCKER(aclrtMallocImplOrigin).expects(atMost(4)).will(returnValue(ACL_SUCCESS));
    EXPECT_TRUE(test.ExpandArgs(args, 32));
    EXPECT_TRUE(test.ExpandArgs(args, 32));
}

TEST_F(ArgsRawContextTest, test_Save_expect_failed)
{
    void *args = nullptr;
    ArgsRawContext test(args, 32, true);
    DumperContext context;
    OpMemInfo info;
    EXPECT_FALSE(test.Save("path", context, info, true));
}

TEST(ArgsRawContext, create_args_raw_with_placeholder_then_call_get_tiling_data_expect_eq)
{
    vector<uint8_t> argsData(100, 1);
    uint32_t addrOffset = 10;
    uint32_t dataOffset = 20;
    argsData[addrOffset] = 11;
    argsData[dataOffset] = 22;
    vector<aclrtPlaceHolderInfo> placeHolderArray;
    placeHolderArray.push_back(aclrtPlaceHolderInfo{addrOffset, dataOffset});
    ArgsRawContext argsContext(argsData.data(), argsData.size(), placeHolderArray);
    vector<uint8_t> tilingData(2);
    EXPECT_FALSE(argsContext.GetTilingData(tilingData));
    ASSERT_EQ(tilingData[0], 22);
    ASSERT_EQ(tilingData[1], 1);
}
