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

#pragma once

#include <map>
#include <string>

#include "utils/Singleton.h"
#include "utils/Protocol.h"

class IPCMemManager : public Singleton<IPCMemManager, false> {
public:
    enum class IPCMemActor : uint8_t {
        SHARER = 0,  // 共享者
        SHAREE       // 被共享者
    };

    struct IPCMemInfo {
        IPCMemActor actor;  // 在 IPC 共享内存中的角色
        const void *devPtr; // 共享内存地址
    };

    // 记录当前进程在不同共享内存关系（key）中对应的信息
    std::map<std::string, IPCMemInfo> ipcMemInfoMap;
};
