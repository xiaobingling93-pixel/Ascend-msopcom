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

#ifndef HCCL_TYPES_H_
#define HCCL_TYPES_H_

#include <cstdint>
#include "acl.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief HCCL functions return value definition
 */
typedef enum {
    HCCL_SUCCESS = 0,               /**< success */
    HCCL_E_PARA = 1,                /**< parameter error */
    HCCL_E_PTR = 2,                 /**< empty pointer */
    HCCL_E_MEMORY = 3,              /**< memory error */
    HCCL_E_INTERNAL = 4,            /**< internal error */
    HCCL_E_NOT_SUPPORT = 5,         /**< not support feature */
    HCCL_E_NOT_FOUND = 6,           /**< not found specific resource */
    HCCL_E_UNAVAIL = 7,             /**< resource unavailable */
    HCCL_E_SYSCALL = 8,             /**< call system interface error */
    HCCL_E_TIMEOUT = 9,             /**< timeout */
    HCCL_E_OPEN_FILE_FAILURE = 10,  /**< open file fail */
    HCCL_E_TCP_CONNECT = 11,        /**< tcp connect fail */
    HCCL_E_ROCE_CONNECT = 12,       /**< roce connect fail */
    HCCL_E_TCP_TRANSFER = 13,       /**< tcp transfer fail */
    HCCL_E_ROCE_TRANSFER = 14,      /**< roce transfer fail */
    HCCL_E_RUNTIME = 15,            /**< call runtime api fail */
    HCCL_E_DRV = 16,                /**< call driver api fail */
    HCCL_E_PROFILING = 17,          /**< call profiling api fail */
    HCCL_E_CCE = 18,                /**< call cce api fail */
    HCCL_E_NETWORK = 19,            /**< call network api fail */
    HCCL_E_AGAIN = 20,              /**< try again */
    HCCL_E_REMOTE = 21,             /**< error cqe */
    HCCL_E_SUSPENDING = 22,         /**< error communicator suspending */
    HCCL_E_RESERVED                 /**< reserved */
} HcclResult;

typedef void *HcclComm;

const uint32_t HCCL_ROOT_INFO_BYTES =  4108; // 4108: root info length
const uint32_t COMM_NAME_MAX_LENGTH = 128; // group name max length
const uint32_t UDI_MAX_LENGTH = 128; // UDI max length
const uint32_t HCCL_COMM_CONFIG_INFO_BYTES = 24;

typedef struct HcclRootInfoDef {
    char internal[HCCL_ROOT_INFO_BYTES];
} HcclRootInfo;

typedef struct HcclCommConfigDef {
    char reserved[HCCL_COMM_CONFIG_INFO_BYTES];
    uint32_t hcclBufferSize;
    uint32_t hcclDeterministic;
    char hcclCommName[COMM_NAME_MAX_LENGTH];
    char hcclUdi[UDI_MAX_LENGTH];
    uint32_t hcclOpExpansionMode;   // 0:默认值  1:host  2:aicpu  3:aiv
    uint32_t hcclRdmaTrafficClass;
    uint32_t hcclRdmaServiceLevel;
} HcclCommConfig;

constexpr uint32_t HCCL_MAX_RANK_NUM = 32U;

struct MemDetails {
    uint64_t size;
    uint64_t addr;
    uint32_t key;
};

struct IbVerbsData {
    MemDetails remoteInput;
    MemDetails remoteOutput;
    MemDetails localInput;
    MemDetails localOutput;
    uint8_t res[24];
};

struct HcclCombinOpParam {
    uint64_t workSpace;
    uint64_t workSpaceSize;
    uint32_t rankId = 0;   // 当前卡rankId
    uint32_t rankNum = 0;
    uint64_t winSize = 0;  // 每个win大小
    uint64_t windowsIn[HCCL_MAX_RANK_NUM];
    uint64_t windowsOut[HCCL_MAX_RANK_NUM];
    uint8_t res[8328];
    uint8_t multiFlag;     // 多机标识，该场景下工具暂不支持
    IbVerbsData *data;     // 多机共享内存地址信息
};

HcclResult HcclBarrier(HcclComm comm, aclrtStream stream);
HcclResult HcclCommInitClusterInfo(const char *clusterInfo, uint32_t rank, HcclComm *comm);
HcclResult HcclCommInitClusterInfoConfig(const char *clusterInfo, uint32_t rank, HcclCommConfig *config,
                                         HcclComm *comm);
HcclResult HcclCommInitRootInfo(uint32_t nRanks, const HcclRootInfo *rootInfo, uint32_t rank, HcclComm *comm);
HcclResult HcclCommInitRootInfoConfig(uint32_t nRanks, const HcclRootInfo *rootInfo, uint32_t rank,
                                      const HcclCommConfig *config, HcclComm *comm);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // HCCL_TYPES_H_
