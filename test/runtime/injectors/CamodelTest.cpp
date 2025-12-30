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


#include <vector>
#include <gtest/gtest.h>
#include "camodel/Camodel.h"
#include "camodel/CamodelHelper.h"
#include "dvc_log_api.h"
#include "mockcpp/mockcpp.hpp"
#include "utils/FileSystem.h"

using namespace std;

DvcLogCbFnUnion_t gCallbacks[4];
string gReceivedData;

int RecordDvcCallback(DvcLogType_t logType, DvcLogCbFnUnion_t fnUnion)
{
    gCallbacks[static_cast<int>(logType)] = fnUnion;
    return 0;
}

void RecordMsg(ProfConfig *b, const string &msg)
{
    gReceivedData = msg;
}

class CamodelTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        // only mock once in all camodel test case
        MOCKER(&GetEnv).stubs().will(returnValue(string("true")));
        MOCKER(&DvcSetLogLevelOrigin).stubs().will(returnValue(int(0)));
        MOCKER(&DvcAttachLogCallbackOrigin).stubs().will(invoke(RecordDvcCallback));
        MOCKER(&ProfConfig::SendMsg).stubs().will(invoke(RecordMsg));
        MessageOfProfConfig conf{};
        conf.pmSamplingEnable = true;
        ProfConfig::Instance().Init(conf);
        CamodelHelper::Instance().Enable();
        CamodelCtor();
    }

    static void TearDownTestCase()
    {
        ProfConfig::Instance().Reset();
        GlobalMockObject::verify();
    }

    void SetUp() override { }

    void TearDown() override { }
};

TEST_F(CamodelTest, mock_DvcAttachLogCallback_then_call_dvc_instr_poped_log_expect_receive_data)
{
    vector<char> desc(500, 'a');
    DvcInstrLogEntry_t instrLog{
        0, 0, 1, desc.data(), desc.data()
    };
    gReceivedData.clear();
    auto cb = gCallbacks[static_cast<uint32_t>(DvcLogType::DVC_INSTR_POPPED_LOG)].instrLogCb;
    cb(0, &instrLog);
    // 我们需要等待监听线程接受到消息后并处理，否则该函数就会结束，mock也结束，那么就会报错
    sleep(1);
    ASSERT_GT(gReceivedData.length(), 0);
}

TEST_F(CamodelTest, mock_DvcAttachLogCallback_then_call_dvc_instr_log_expect_receive_data)
{
    vector<char> desc(500, 'a');
    DvcInstrLogEntry_t instrLog{
        0, 0, 1, desc.data(), desc.data()
    };
    gReceivedData.clear();
    auto cb = gCallbacks[static_cast<uint32_t>(DvcLogType::DVC_INSTR_LOG)].instrLogCb;
    cb(0, &instrLog);
    // 我们需要等待监听线程接受到消息后并处理，否则该函数就会结束，mock可能也结束，那么就会报错
    sleep(1);
    ASSERT_GT(gReceivedData.length(), 0);
}

TEST_F(CamodelTest, mock_DvcAttachLogCallback_then_call_dvc_mte_log_expect_receive_data)
{
    DvcMteLogEntry_t mteSendLog{
        0, 0, "send_cmd", {"intf", 0, 1, 1, 1, 1, 1, 1}
    };
    gReceivedData.clear();
    auto cb = gCallbacks[static_cast<uint32_t>(DvcLogType::DVC_MTE_LOG)].mteLogCb;
    cb(0, &mteSendLog);
    // 我们需要等待监听线程接受到消息后并处理，否则该函数就会结束，mock可能也结束，那么就会报错
    sleep(1);
    ASSERT_GT(gReceivedData.length(), 0);
    gReceivedData.clear();
    DvcMteLogEntry_t mteRecvLog {
        0, 0, "recv_rsp", {"intf", 0, 1, 1, 1, 1, 1}
    };
    cb(0, &mteRecvLog);
    sleep(1);
    ASSERT_GT(gReceivedData.length(), 0);
}

TEST_F(CamodelTest, mock_DvcAttachLogCallback_then_call_dvc_icache_log_expect_receive_data)
{
    gReceivedData.clear();
    DvcIcacheLogEntry_t icacheLog {
        0, 0, "miss_read", {0, 1, 1, 1}
    };
    auto cb = gCallbacks[static_cast<uint32_t>(DvcLogType::DVC_ICACHE_LOG)].icacheLogCb;
    cb(0, &icacheLog);
    // 我们需要等待监听线程接受到消息后并处理，否则该函数就会结束，mock可能也结束，那么就会报错
    sleep(1);
    ASSERT_GT(gReceivedData.length(), 0);
}
