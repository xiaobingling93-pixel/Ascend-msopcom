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


#include "KernelMatcher.h"

using namespace std;

bool KernelMatcher::Match(uint64_t launchId, const string &kernelName) const
{
    if (IsMatchAll()) {
        return true;
    }
    if (config_.launchId != UINT64_MAX) {
        return launchId == config_.launchId;
    }
    if (!config_.kernelName.empty()) {
        return kernelName.find(config_.kernelName) != std::string::npos;
    }
    return false;
}

bool KernelMatcher::IsMatchAll() const
{
    return (config_.launchId == UINT64_MAX &&
            config_.kernelName.empty() &&
            config_.regId == UINT64_MAX);
}
