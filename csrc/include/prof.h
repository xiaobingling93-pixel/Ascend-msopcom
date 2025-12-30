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

// 这个文件本身需要与被劫持的对象一样

#ifndef __PROF_H__
#define __PROF_H__

#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

static const int PROF_ERROR_INTERNAL_ERROR = 500000;

using VOID_PTR = void *;
using ProfCommandHandle = int32_t (*)(uint32_t type, void *data, uint32_t len);

#define PROF_OP_TIMESTAMP            0x0000100000000ULL    // aicore打点数据上报开关
#define PROF_TASK_TIME_L1_MASK       0x00000002ULL         // L1级别timeline开关
#define PROF_DEV_AICPU_CHANNEL       0x01000000000000ULL   // aicpu通道开关
constexpr int AICORE_MODULE_ID = 61;   // aicore打点通道module
constexpr int AICPU_MODULE_ID = 7;     // aicpu通道module
constexpr int MAX_HANDLE = 10000;      // 初始化aicpuHandleCallMap_使用
constexpr uint16_t MSPROF_REPORT_AIC_LEVEL = 3000;
constexpr uint32_t MSPROF_REPORT_AIC_TIMESTAMP_TYPE = 0;

#define PATH_LEN_MAX 1023
#define PARAM_LEN_MAX 4095
struct MsprofCommandHandleParams {
    uint32_t pathLen;
    uint32_t storageLimit;  // MB
    uint32_t profDataLen;
    char path[PATH_LEN_MAX + 1];
    char profData[PARAM_LEN_MAX + 1];
};

#define MSPROF_MAX_DEV_NUM 64
struct MsprofCommandHandle {
    uint64_t profSwitch;
    uint64_t profSwitchHi;
    uint32_t devNums;
    uint32_t devIdList[MSPROF_MAX_DEV_NUM];
    uint32_t modelId;
    uint32_t type;
    uint32_t cacheFlag;
    struct MsprofCommandHandleParams params;
};

#define MSPROF_REPORT_DATA_MAGIC_NUM 0x5A5AU
struct MsprofAicTimeStampInfo {
    uint64_t syscyc;   // dotting timestamp with system cycle
    uint32_t blockId;  // core block id
    uint32_t descId;   // dot Id for description
    uint64_t curPc;   // currrent pc for source line
}; // size 24

#define MSPROF_ADDTIONAL_INFO_DATA_LENGTH (232)
struct MsprofAdditionalInfo {  // for MsprofReportAdditionalInfo buffer data
#ifdef __cplusplus
    uint16_t magicNumber = MSPROF_REPORT_DATA_MAGIC_NUM;
#else
    uint16_t magicNumber;
#endif
    uint16_t level;
    uint32_t type;
    uint32_t threadId;
    uint32_t dataLen;
    uint64_t timeStamp;
    uint8_t data[MSPROF_ADDTIONAL_INFO_DATA_LENGTH];
};

enum ProfCtrlType {
    PROF_CTRL_INVALID = 0,
    PROF_CTRL_SWITCH,
    PROF_CTRL_REPORTER,
    PROF_CTRL_STEPINFO,
    PROF_CTRL_BUTT
};

enum MsprofCommandHandleType {
    PROF_COMMANDHANDLE_TYPE_INIT = 0,
    PROF_COMMANDHANDLE_TYPE_START,
    PROF_COMMANDHANDLE_TYPE_STOP,
    PROF_COMMANDHANDLE_TYPE_FINALIZE,
    PROF_COMMANDHANDLE_TYPE_MODEL_SUBSCRIBE,
    PROF_COMMANDHANDLE_TYPE_MODEL_UNSUBSCRIBE,
    PROF_COMMANDHANDLE_TYPE_MAX
};

int32_t MsprofRegisterCallback(uint32_t moduleId, ProfCommandHandle handle);
int32_t MsprofReportAdditionalInfo(uint32_t agingFlag, const VOID_PTR data, uint32_t length);
int32_t MsprofNotifySetDevice(uint32_t chipId, uint32_t deviceId, bool isOpen);
int32_t profSetProfCommand(VOID_PTR command, uint32_t len);
#ifdef __cplusplus
} // extern "C"
#endif

#endif // __PROF_H__