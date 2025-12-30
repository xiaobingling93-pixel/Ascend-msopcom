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

#ifndef __MOCK_HELPER__

// 对LocalDevice::Wait函数进行插桩
// 使用时先使用SetMsg设置返回message，再设置MOCKER的invoke
class MockHelper {
public:
    static MockHelper &Instance()
    {
        static MockHelper waitMocker;
        return waitMocker;
    }

    static ssize_t WaitMockFuncF(LocalProcess *ptr, std::string &msg, uint32_t timeOut)
    {
        msg = Instance().msg_;
        return msg.size();
    }

    template <typename MsgT>
    inline void SetMsg(MsgT &&msg)
    {
        Instance().msg_ = std::forward<MsgT>(msg);
    }

    std::string msg_;

private:
    MockHelper() = default;
};

#endif
