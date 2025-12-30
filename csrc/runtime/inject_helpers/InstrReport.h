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

#include <cstdint>
#include <string>

#include "utils/Protocol.h"
#include "core/PlatformConfig.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/RegisterContext.h"
#include "runtime/inject_helpers/LaunchContext.h"

extern "C" {
uint8_t *__sanitizer_init(uint64_t blockDim);
void __sanitizer_finalize(uint8_t *memInfo, uint64_t blockDim);
} // extern "C"

/**
 * @brief 获取最后一次 launch 的算子对应的算子类型
 */
KernelType GetCurrentKernelType(void);

KernelType GetCurrentKernelType(uint32_t magic, const std::string &kernelName);
/**
 * @brief 获取最后一次 launch 的算子对应的架构类型
 */
std::string GetCurrentArchName(void);

std::string GetTargetArchName(const FuncContextSP &funcCtx);

std::string GetArchName(KernelType kernelType, const std::string &socVersion);

bool SkipSanitizer(std::string const &kernelName);

void ReportKernelSummary(uint64_t launchId);

void ReportKernelSummary(LaunchContextSP launchCtx);
