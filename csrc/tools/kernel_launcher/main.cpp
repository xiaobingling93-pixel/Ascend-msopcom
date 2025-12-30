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


#include <cstdlib>
#include <getopt.h>

#include "KernelConfigParser.h"
#include "KernelRunner.h"
#include "Launcher.h"
#include "utils/InjectLogger.h"

using ArgumentFunc = uint32_t (*)();

namespace {
std::string g_filePath = "";

uint32_t SetBinFilePath()
{
    DEBUG_LOG("set bin file path, path is %s", optarg);
    g_filePath = optarg;
    return EXIT_SUCCESS;
}

uint32_t GetWrapperInputParameter(const int32_t &retVal)
{
    static const std::map<int32_t, ArgumentFunc> PARSER_FUNCS = {
        {static_cast<int32_t>('c'), &SetBinFilePath},
    };

    const std::map<int32_t, ArgumentFunc>::const_iterator funcIter = PARSER_FUNCS.find(retVal);
    if (funcIter == PARSER_FUNCS.end()) {
        return EXIT_SUCCESS;
    }
    return funcIter->second();
}

bool GetKernelConfig(const std::string &filePath, KernelConfig &kernelConfig)
{
    KernelConfigParser parser;
    kernelConfig = parser.GetKernelConfig(filePath);
    return parser.isGetConfigSuccess_;
}
}

int main(int argc, char **argv)
{
    int32_t retVal;
    do {
        retVal = getopt(argc, argv, "c:");
        uint32_t ret = GetWrapperInputParameter(retVal);
        if (ret == EXIT_FAILURE) {
            return  EXIT_FAILURE;
        }
    } while (retVal != -1);

    KernelConfig kernelConfig;
    if (!GetKernelConfig(g_filePath, kernelConfig)) {
        return  EXIT_FAILURE;
    }
    if (kernelConfig.isOldVersion) {
        KernelRunner kernelRunner;
        kernelRunner.Run(kernelConfig);
    } else {
        Launcher launcher;
        launcher.Run(kernelConfig);
    }
    return 0;
}
