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
#include "utils/FileSystem.h"
#include "utils/InjectLogger.h"
#include "core/LocalProcess.h"
#include "runtime/inject_helpers/KernelContext.h"
#include "RuntimeConfig.h"

HijackedFuncOfFunctionRegister::HijackedFuncOfFunctionRegister()
    : HijackedFuncOfFunctionRegister::HijackedFuncType(
    std::string(RuntimeLibName()), std::string("rtFunctionRegister")) {}
void HijackedFuncOfFunctionRegister::Pre(void *binHandle, const void *stubFunc, const char_t *stubName,
    const void *kernelInfoExt, uint32_t funcMode)
{
    this->binHandle_ = binHandle;
    this->stubFunc_ = stubFunc;
    if (binHandle == nullptr || stubFunc == nullptr) {
        WARN_LOG("rtFunctionRegister Hijacked binHandle or stubFunc is nullptr.");
        return ;
    }
    // 存储hdl和stubFunc的对应关系
    KernelContext::Instance().AddFuncRegisterEvent(binHandle, stubFunc, stubName, kernelInfoExt, funcMode);
}

rtError_t HijackedFuncOfFunctionRegister::Post(rtError_t ret)
{
    return ret;
}
