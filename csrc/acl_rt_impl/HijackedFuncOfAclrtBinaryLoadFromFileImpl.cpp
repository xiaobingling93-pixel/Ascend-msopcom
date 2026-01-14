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
#include "core/FuncSelector.h"
#include "runtime/inject_helpers/ProfConfig.h"
#include "runtime/inject_helpers/ProfDataCollect.h"
#include "runtime/inject_helpers/RegisterContext.h"
#include "runtime/inject_helpers/RegisterManager.h"
#include "utils/InjectLogger.h"

HijackedFuncOfAclrtBinaryLoadFromFileImpl::HijackedFuncOfAclrtBinaryLoadFromFileImpl()
    : HijackedFuncType(AclRuntimeLibName(), "aclrtBinaryLoadFromFileImpl") {}

void HijackedFuncOfAclrtBinaryLoadFromFileImpl::Pre(
    const char* binPath, aclrtBinaryLoadOptions *options, aclrtBinHandle *binHandle)
{
    binPath_ = binPath;
    binHandle_ = binHandle;
    options_ = options;
}

aclError HijackedFuncOfAclrtBinaryLoadFromFileImpl::Post(aclError ret)
{
    if (ret != ACL_SUCCESS || binPath_ == nullptr || binHandle_ == nullptr) {
        return ret;
    }

    uint32_t magic = RT_DEV_BINARY_MAGIC_ELF;
    std::string magicStr;
    std::string jsonPath(binPath_);

    size_t lastDot = jsonPath.find_last_of('.');
    std::string suffix{};
    if (lastDot != std::string::npos) {
        suffix = jsonPath.substr(lastDot);
    }

    if (suffix == ".o") {
        jsonPath = jsonPath.substr(0, lastDot) + ".json";
        if (!ReadMagicFromKernelJson(jsonPath, magicStr) || !ParseMagicStr(magicStr, magic)) {
            WARN_LOG("Parse magic from kernel JSON failed.");
        }
    } else if (suffix == ".json") {
        DEBUG_LOG("The kernel file path : %.2048s is invalid", binPath_);
        return ret;
    } else {
        DEBUG_LOG("The kernel file path : %.2048s is invalid", binPath_);
    }

    auto ctx = RegisterManager::Instance().CreateContext(binPath_, *binHandle_, magic, options_);
    if (!ctx) {
        return ret;
    }
    if (IsOpProf() && ProfConfig::Instance().IsSimulator()) {
        ProfDataCollect::SaveObject(ctx);
    }
    return ret;
}
