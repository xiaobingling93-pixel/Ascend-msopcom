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
#include "runtime/inject_helpers/ProfConfig.h"
#include "core/FuncSelector.h"
#include "utils/InjectLogger.h"

HijackedFuncOfAclrtGetSocNameImpl::HijackedFuncOfAclrtGetSocNameImpl()
    : HijackedFuncType("acl_rt_impl", "aclrtGetSocNameImpl") {}

const char* HijackedFuncOfAclrtGetSocNameImpl::Call()
{
    if (IsOpProf() && ProfConfig::Instance().IsSimulator()) {
        const std::string &simSocVersion = ProfConfig::Instance().GetSocVersion();
        if (!simSocVersion.empty()) {
            DEBUG_LOG("SocVersion replace success, version is %s", simSocVersion.c_str());
            return simSocVersion.c_str();
        }
    }
    if (originfunc_ != nullptr) {
        return originfunc_();
    }
    return nullptr;
}

