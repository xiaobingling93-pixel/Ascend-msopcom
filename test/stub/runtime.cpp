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


#include "runtime.h"
#include <cstring>
#include <malloc.h>
#include <algorithm>

using namespace std;

RTS_API rtError_t rtDevBinaryRegister(const rtDevBinary_t *bin, void **hdl)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtFunctionRegister(void *binHandle, const void *stubFunc, const char *stubName,
                                     const void *kernelInfoExt, uint32_t funcMode)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtRegisterAllKernel(const rtDevBinary_t *bin, void **hdl)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtKernelLaunch(const void *stubFunc, uint32_t blockDim, void *args,
                                 uint32_t argsSize, rtSmDesc_t *smDesc, rtStream_t stm)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtKernelLaunchWithHandleV2(void *hdl, const uint64_t tilingKey, uint32_t blockDim,
                                             rtArgsEx_t *argsInfo, rtSmDesc_t *smDesc, rtStream_t stm,
                                             const rtTaskCfgInfo_t *cfgInfo)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtDevBinaryUnRegister(void *hdl)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtFree(void *devPtr)
{
    free(devPtr);
    return RT_ERROR_NONE;
}
 
RTS_API rtError_t rtMalloc(void **devPtr, uint64_t size, rtMemType_t type, const uint16_t moduleId)
{
    *devPtr = malloc(size);
    return RT_ERROR_NONE;
}
 
RTS_API rtError_t rtMemset(void *devPtr, uint64_t destMax, uint32_t val, uint64_t cnt)
{
    fill((uint8_t*)devPtr, (uint8_t*)devPtr + cnt, val);
    return RT_ERROR_NONE;
}
 
RTS_API rtError_t rtMemcpy(void *dst, uint64_t destMax, const void *src, uint64_t cnt, rtMemcpyKind_t kind)
{
    copy_n((uint8_t*)src, cnt, (uint8_t*)dst);
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtMallocHost(void **hostPtr, uint64_t size, uint16_t moduleId)
{
    *hostPtr = malloc(size);
    return RT_ERROR_NONE;
}

rtError_t rtFreeHost(void *hostPtr)
{
    free(hostPtr);
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtGetDeviceCount(int32_t *cnt)
{
    *cnt = 8; // 8卡机器
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtGetDevice(int32_t *devId)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtSetDevice(int32_t device)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtDeviceReset(int32_t device)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtStreamCreate(rtStream_t *stream, int32_t priority)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtStreamSynchronize(rtStream_t stream)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtStreamDestroy(rtStream_t stream)
{
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtGetC2cCtrlAddr(uint64_t *addr, uint32_t *fftsLen)
{
    return RT_ERROR_NONE;
}