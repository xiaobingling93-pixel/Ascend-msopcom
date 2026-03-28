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


#ifndef __CORE_REMOTE_PROCESS_H__
#define __CORE_REMOTE_PROCESS_H__

#include <string>
#include "Communication.h"

// RemoteProcess类是远端进程的抽象，主要是工具所在的进程
// 该代码在工具代码仓中集成时，建议改成Process就好。
// 该类主要提供与LocalProcess协同的能力，各个工具最好共用该能力
// 具体行为包括：
// 1. 日志操作
// 2. 事件通知
class RemoteProcess {
public:
    explicit RemoteProcess(CommType type);
    ~RemoteProcess();

    /** 启动服务端
     * @description 启动服务端，在此之前可以设置 SetMsgHandlerHook
     * 回调，防止回调设置前一些客户端已经连接
     */
    void Start() const;

    /** 等待 LocalProcess 消息
     * @param clientIdx 要等待的 LocalProcess 客户端编号，此编号代表客户端的连接顺序
     * 从 0 开始计数
     * @param msg 接收到的消息
     * @param timeOut 等待超时时间，单位毫秒
     * @return >= 0 接收成功，并且返回值表示实际接收到的字节数
     *         <  0 接收失败
     */
    ssize_t Wait(std::size_t clientId, std::string& msg) const;

    /** 向 LocalProcess 发送消息
     * @param clientIdx 要发送的 LocalProcess 客户端编号，此编号代表客户端的连接顺序
     * 从 0 开始计数
     * @param msg 要发送的消息
     * @return >= 0 发送成功，并且返回值表示实际发送的字节数
     *         <  0 发送失败
     */
    ssize_t Notify(std::size_t clientId, const std::string& msg) const;

    /** 设置消息接收通知的回调函数
     * @param func 通知回调函数
     */
    void SetMsgHandlerHook(ClientMsgHandlerHook &&hook) const;

private:
    Server* server_ = nullptr;
}; // class LocalProcess

#endif // __CORE_REMOTE_PROCESS_H__
