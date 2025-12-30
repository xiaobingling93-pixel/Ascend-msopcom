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


#include <map>

#include "utils/TypeTraits.h"
#include "InteractHelper.h"

PacketType QueryResponsePacketType(PacketType type)
{
    static const std::map<PacketType, PacketType> MP{{PacketType::IPC_RECORD, PacketType::IPC_RESPONSE},
        {PacketType::KERNEL_RECORD, PacketType::KERNEL_RECORD_RESPONSE}};
    auto it = MP.find(type);
    if (it == MP.end()) {
        ERROR_LOG("Failed to query response packet type (%u).", static_cast<uint32_t>(type));
        return PacketType::INVALID;
    }
    return it->second;
}

void IPCInteract(const IPCMemRecord &record)
{
    IPCResponse response{};
    if (!InteractHelper::Interact(PacketType::IPC_RECORD, record, response)) {
        ERROR_LOG("Error happened with IPC interact: Failed to interact with server. (Operation type:%u).",
            EnumToUnderlying(record.type));
        return;
    }
    if (response.type != record.type) {
        ERROR_LOG("Error happened: mismatched IPC response received (Operation type:%u).",
                  EnumToUnderlying(response.type));
    } else if (response.status != ResponseStatus::SUCCESS) {
        ERROR_LOG("Error happened: failure response received (Operation type:%u).", EnumToUnderlying(response.status));
    } else {
        DEBUG_LOG("Succeed in receiving response.");
    }
}
