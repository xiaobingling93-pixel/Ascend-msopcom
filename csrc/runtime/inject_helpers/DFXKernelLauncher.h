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

#ifndef __RUNTIME_INJECT_HELPERS_DFX_KERNEL_LAUNCHER_H__
#define __RUNTIME_INJECT_HELPERS_DFX_KERNEL_LAUNCHER_H__

#include <map>
#include <set>
#include <string>
#include <vector>
#include <mutex>
#include "acl.h"
#include "utils/Singleton.h"
#include "core/PlatformConfig.h"

class DFXKernelLauncher : public Singleton<DFXKernelLauncher, true> {
public:
    friend class Singleton<DFXKernelLauncher, true>;
    bool CallClearL2Cache(uint32_t blockDim, void *stream,  std::vector<void *> &kernelArgs);
    bool CallEmptyKernel(void *stream);
private:
    ~DFXKernelLauncher();
    bool CallKernel(const std::string &kernelName, const std::string &kernelPath, uint32_t blockDim, void *stream,  std::vector<void *> &kernelArgs);
    void Init(const std::string &kernelName, const std::string &kernelPath);
    static std::string GetL2CacheKernelPath();
    static bool CheckSupportSeries(const std::vector<ChipProductType> &chipSeries);
    static std::string GetEmptyKernelPath();

    std::mutex mtx_;
    std::set<std::string> kernelSet_;
    std::string kernelPath_;
    std::map<std::string, aclrtBinHandle> binHandleMap_;
    std::map<std::string, aclrtFuncHandle> funcHandleMap_;
};

#endif // __RUNTIME_INJECT_HELPERS_DFX_KERNEL_LAUNCHER_H__
