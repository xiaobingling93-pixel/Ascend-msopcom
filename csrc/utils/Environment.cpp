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

#include "Environment.h"
#include <set>
#include <unistd.h>
#include <climits>
#include "FileSystem.h"
#include "utils/InjectLogger.h"
#include "Ustring.h"
void JoinWithSystemEnv(std::map<std::string, std::string> envs,
                       std::vector<std::string> &outEnv, bool combineMode)
{
    static std::set<std::string> needJoinEnv = {"LD_PRELOAD", "LD_LIBRARY_PATH", "PATH"};
    /// append/combined system envs into program defined envs
    char **systemEnvs = environ;
    while (systemEnvs && *systemEnvs != nullptr) {
        if (!combineMode) {
            // only add new env
            outEnv.emplace_back(*systemEnvs);
            systemEnvs++;
            continue;
        }
        std::string tmpSysEnvs = *systemEnvs;
        size_t pos = tmpSysEnvs.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        std::string key = tmpSysEnvs.substr(0, pos);
        auto it = envs.find(key);
        if (it != envs.end() && needJoinEnv.find(it->first) != needJoinEnv.end()) {
            if (pos == (tmpSysEnvs.size() - 1)) {
                // handle env value is empty
                tmpSysEnvs.replace(0, pos + 1, it->first + "=" + it->second);
            } else {
                tmpSysEnvs.replace(0, pos + 1, it->first + "=" + it->second + ":");
            }
            envs.erase(it);
        }
        outEnv.emplace_back(tmpSysEnvs);
        systemEnvs++;
    }

    /// append envs not in system envs
    for (auto &pair: envs) {
        std::string envStr = pair.first + "=" + pair.second;
        outEnv.push_back(envStr);
    }
}

bool GetAscendHomePath(std::string &ascendHomePath)
{
    std::string pathFromEnv = GetEnv("ASCEND_HOME_PATH");
    if (pathFromEnv.empty()) {
        ERROR_LOG("no $ASCEND_HOME_PATH env, please set it by source <CANN-install-path>/ascend-toolkit/set_env.sh");
        return false;
    }

    char buf[PATH_MAX];
    if (realpath(pathFromEnv.c_str(), buf) == nullptr) {
        ERROR_LOG("no such path for CANN: [%s]", pathFromEnv.c_str());
        return false;
    }

    ascendHomePath = buf;
    return true;
}

static bool ExtractSocVersion(const std::string& path, std::string& socVersion)
{
    size_t libPos = path.rfind("/lib");
    if (libPos == std::string::npos) {
        return false;
    }

    size_t ascendPos = path.rfind("Ascend", libPos);
    if (ascendPos == std::string::npos) {
        return false;
    }

    std::string ascendSegment = path.substr(ascendPos, libPos - ascendPos);
    if (ascendSegment.size() < 9 || ascendSegment.substr(0, 6) != "Ascend") {
        return false;
    }

    for (int i = 6; i < 9; ++i) {
        if (!std::isdigit(static_cast<unsigned char>(ascendSegment[i]))) {
            return false;
        }
    }
    const size_t maxLen = 6 + 3 + 8;  // Ascend\\d{3}[0-9a-zA-Z_]{0,8}
    if (ascendSegment.size() > maxLen) {
        return false;
    }
    for (size_t i = 9; i < ascendSegment.size(); ++i) {
        char c = ascendSegment[i];
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
            return false;
        }
    }
    socVersion = ascendSegment;
    return true;
}

bool GetSocVersionFromEnvVar(std::string &socVersion)
{
    std::string ascendHomePath;
    if (!GetAscendHomePath(ascendHomePath)) {
        return false;
    }

    char const *ldEnv = secure_getenv("LD_LIBRARY_PATH");
    if (ldEnv == nullptr) {
        return false;
    }
    std::string pathFromEnv = ldEnv;
    std::vector<std::string> envs;
    Split(pathFromEnv, std::back_inserter(envs), ":");

    for (const std::string &path: envs) {
        if (!StartsWith(path, ascendHomePath)) {
            continue;
        }
        if (ExtractSocVersion(path, socVersion)) {
            return true;
        }
    }
    return false;
}