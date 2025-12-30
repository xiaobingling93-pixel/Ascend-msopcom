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
#include "utils/InjectLogger.h"
#include "core/FuncSelector.h"
#include "inject_helpers/MemoryDataCollect.h"

HijackedFuncOfCtxGetOverflowAddr::HijackedFuncOfCtxGetOverflowAddr()
    : HijackedFuncOfCtxGetOverflowAddr::HijackedFuncType("runtime", "rtCtxGetOverflowAddr"),
      overflowAddr_ {} { }

void HijackedFuncOfCtxGetOverflowAddr::Pre(void **overflowAddr)
{
    overflowAddr_ = overflowAddr;
}

rtError_t HijackedFuncOfCtxGetOverflowAddr::Post(rtError_t ret)
{
    if (IsSanitizer()) {
        if (overflowAddr_ == nullptr) {
            WARN_LOG("Overflow addr is invalid.");
            return ret;
        }

        auto &opMemInfo = KernelContext::Instance().GetOpMemInfo();
        opMemInfo.hasOverflowAddr = true;
        opMemInfo.overflowAddr = reinterpret_cast<uint64_t>(*overflowAddr_);
    }
    return ret;
}
