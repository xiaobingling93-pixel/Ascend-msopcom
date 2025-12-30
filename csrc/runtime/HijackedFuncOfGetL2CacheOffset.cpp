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
#include <cinttypes>

#include "HijackedFunc.h"
#include "RuntimeConfig.h"
#include "utils/InjectLogger.h"
#include "core/FuncSelector.h"


HijackedFuncOfGetL2CacheOffset::HijackedFuncOfGetL2CacheOffset()
    : HijackedFuncOfGetL2CacheOffset::HijackedFuncType(
    std::string(RuntimeLibName()), std::string("rtGetL2CacheOffset")) {}


rtError_t HijackedFuncOfGetL2CacheOffset::Call(uint32_t deviceId, uint64_t *offset)
{
    if (this->originfunc_ == nullptr) {
        ERROR_LOG("HijackedFuncOfGetL2CacheOffset originfunc is nullptr.");
        return RT_ERROR_RESERVED;
    }
    rtError_t ret = this->originfunc_(deviceId, offset);
    if (offset == nullptr) {
        ERROR_LOG("HijackedFuncOfGetL2CacheOffset offset is nullptr.");
        return ret;
    }
    DEBUG_LOG("l2CacheOffset is %" PRIu64, *offset);
    if (IsSanitizer()) {
        *offset = 0;
    }
    return ret;
}
