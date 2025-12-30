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

// 本文件用于构造一个空实现的runtime库，使kernel-launcher编译构建时完成对libruntime.so的链接

RTS_API rtError_t rtSetDevice(int32_t devId)
{
    (void)devId;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtDeviceReset(int32_t devId)
{
    (void)devId;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtMalloc(void **devPtr, uint64_t size,
    rtMemType_t type, const uint16_t moduleId)
{
    (void)devPtr;
    (void)size;
    (void)type;
    (void)moduleId;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtFree(void *devPtr)
{
    (void)devPtr;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtMallocHost(void **hostPtr, uint64_t size,
    const uint16_t moduleId)
{
    (void)hostPtr;
    (void)size;
    (void)moduleId;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtFreeHost(void *hostPtr)
{
    (void)hostPtr;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtMemcpy(void *dst, uint64_t destMax, const void *src,
    uint64_t cnt, rtMemcpyKind_t kind)
{
    (void)dst;
    (void)destMax;
    (void)src;
    (void)cnt;
    (void)kind;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtStreamCreate(rtStream_t *stream, int32_t priority)
{
    (void)stream;
    (void)priority;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtStreamSynchronize(rtStream_t stm)
{
    (void)stm;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtStreamDestroy(rtStream_t stream)
{
    (void)stream;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtDevBinaryRegister(const rtDevBinary_t *bin, void **hdl)
{
    (void)bin;
    (void)hdl;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtDevBinaryUnRegister(void *hdl)
{
    (void)hdl;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtRegisterAllKernel(const rtDevBinary_t *bin, void **hdl)
{
    (void)bin;
    (void)hdl;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtFunctionRegister(void *binHandle, const void *stubFunc,
    const char *stubName, const void *kernelInfoExt, uint32_t funcMode)
{
    (void)binHandle;
    (void)stubFunc;
    (void)stubName;
    (void)kernelInfoExt;
    (void)funcMode;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtKernelLaunch(const void *stubFunc, uint32_t blockDim, void *args,
    uint32_t argsSize, rtSmDesc_t *smDesc, rtStream_t stm)
{
    (void)stubFunc;
    (void)blockDim;
    (void)args;
    (void)argsSize;
    (void)smDesc;
    (void)stm;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtKernelLaunchWithHandleV2(void *hdl, const uint64_t tilingKey, uint32_t blockDim,
                                             rtArgsEx_t *argsInfo, rtSmDesc_t *smDesc, rtStream_t stm,
                                             const rtTaskCfgInfo_t *cfgInfo)
{
    (void)hdl;
    (void)tilingKey;
    (void)blockDim;
    (void)argsInfo;
    (void)smDesc;
    (void)stm;
    (void)cfgInfo;
    return RT_ERROR_NONE;
}

RTS_API rtError_t rtGetC2cCtrlAddr(uint64_t *addr, uint32_t *fftsLen)
{
    (void)addr;
    (void)fftsLen;
    return RT_ERROR_NONE;
}
