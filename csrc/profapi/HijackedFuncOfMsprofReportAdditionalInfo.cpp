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

// 该文件主要实现注入函数的功能，其配合被劫持函数的别名，实现新的劫持函数功能
#include "HijackedFunc.h"
#include "core/FuncSelector.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/DeviceContext.h"
#include "runtime/inject_helpers/ProfDataCollect.h"
#include "utils/FileSystem.h"
#include "utils/InjectLogger.h"
#include "include/prof.h"

HijackedFuncOfMsprofReportAdditionalInfo::HijackedFuncOfMsprofReportAdditionalInfo()
    : HijackedFuncType("profapi", "MsprofReportAdditionalInfo") {}

void HijackedFuncOfMsprofReportAdditionalInfo::Pre(uint32_t agingFlag, const VOID_PTR data, uint32_t length)
{
    if (IsOpProf()) {
        int32_t device = DeviceContext::GetRunningDeviceId();
        auto *hostData = static_cast<struct MsprofAdditionalInfo *>(data);
        std::string outputPath = ProfDataCollect::GetAicoreOutputPath(device);
        if (outputPath.empty() || hostData == nullptr ||
            hostData->level != MSPROF_REPORT_AIC_LEVEL || hostData->type != MSPROF_REPORT_AIC_TIMESTAMP_TYPE ||
            (!KernelContext::Instance().GetMC2Flag() && !KernelContext::Instance().GetLcclFlag())) {
            return;
        }
        std::string binFile = JoinPath({outputPath, "aic_timestamp.bin"});
        if (WriteBinary(binFile, reinterpret_cast<const char*>(&hostData->data[0]), hostData->dataLen,
            std::ios::app) == 0) {
            WARN_LOG("Write bin failed, bin is %s", binFile.c_str());
        }
    }
}
