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

#ifndef KERNEL_LAUNCHER_RUNTIME_API_H
#define KERNEL_LAUNCHER_RUNTIME_API_H

#include <cstdint>
#include <string>

#include "runtime.h"

class RuntimeApi {
public:

    bool CheckRtResult(rtError_t result, const std::string &apiName) const;

    int GetMagic(const std::string &magic) const;

    rtError_t RtSetDevice(int32_t device) const;

    rtError_t RtDeviceReset(int32_t device) const;

    rtError_t RtMalloc(void **devPtr, uint64_t size, rtMemType_t type, const uint16_t moduleId = 0U) const;

    rtError_t RtFree(void *devPtr) const;

    rtError_t RtMallocHost(void **hostPtr, uint64_t size, const uint16_t moduleId = 0U) const;

    rtError_t RtFreeHost(void *hostPtr) const;

    rtError_t RtMemcpy(void *dst, uint64_t destMax, const void *src, uint64_t count, rtMemcpyKind_t kind) const;

    rtError_t RtStreamCreate(rtStream_t *stream, int32_t priority) const;

    rtError_t RtStreamSynchronize(rtStream_t stream) const;

    rtError_t RtStreamDestroy(rtStream_t stream) const;

    rtError_t RtDevBinaryRegister(const rtDevBinary_t *bin, void **handle) const;

    rtError_t RtDevBinaryUnRegister(void *handle) const;

    rtError_t RtRegisterAllKernel(const rtDevBinary_t *bin, void **handle) const;

    rtError_t RtFunctionRegister(void *binHandle, const void *stubFunc, const char *stubName, const void *devFunc,
                                 uint32_t funcMode) const;

    rtError_t RtKernelLaunch(const void *stubFunc, uint32_t blockDim, void *args,
        uint32_t argsSize, rtStream_t stream) const;

    rtError_t RtKernelLaunchWithHandleV2(void *hdl, const uint64_t tilingKey, uint32_t blockDim, rtArgsEx_t *argsInfo,
                                         rtSmDesc_t *smDesc, rtStream_t stm, const rtTaskCfgInfo_t *cfgInfo) const;

    rtError_t RtGetC2cCtrlAddr(uint64_t *addr, uint32_t *fftsLen) const;
};

#endif // KERNEL_LAUNCHER_RUNTIME_API_H
