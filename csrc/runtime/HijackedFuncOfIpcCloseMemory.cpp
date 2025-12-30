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

#include "HijackedFunc.h"
#include "RuntimeConfig.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "runtime/inject_helpers/InteractHelper.h"
#include "core/FuncSelector.h"
#include "utils/InjectLogger.h"

HijackedFuncOfIpcCloseMemory::HijackedFuncOfIpcCloseMemory()
    : HijackedFuncType(RuntimeLibName(), "rtIpcCloseMemory") {}

rtError_t HijackedFuncOfIpcCloseMemory::Call(const void *ptr)
{
    DEBUG_LOG("enter HijackedFuncOfIpcCloseMemory.");

    if (this->originfunc_ == nullptr) {
        ERROR_LOG("HijackedFuncOfIpcCloseMemory originfunc is nullptr.");
        return RT_ERROR_RESERVED;
    }
    rtError_t ret = this->originfunc_(ptr);
    if (ret != RT_ERROR_NONE) {
        ERROR_LOG("error happened when calling rtIpcCloseMemory.");
        return ret;
    }

    if (IsSanitizer()) {
        IPCMemRecord record{};
        record.type = IPCOperationType::UNMAP_INFO;
        record.unmapInfo.addr = reinterpret_cast<uint64_t>(ptr);
        IPCInteract(record);
    }

    DEBUG_LOG("leave HijackedFuncOfIpcCloseMemory.");

    return Post(ret);
}
