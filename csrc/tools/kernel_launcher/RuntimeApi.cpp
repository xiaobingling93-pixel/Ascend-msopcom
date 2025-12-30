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


#include "RuntimeApi.h"

#include <algorithm>
#include <map>

#include "utils/InjectLogger.h"

using namespace std;

namespace {
const std::map<int32_t, std::string> RT_ERROR_MAP = {
    {107000, "param invalid"},
    {107001, "invalid device id"},
    {107002, "current context null"},
    {107003, "stream not in current context"},
    {107004, "model not in current context"},
    {107005, "stream not in model"},
    {107006, "event timestamp invalid"},
    {107007, "event timestamp reversal"},
    {107008, "memory address unaligned"},
    {107009, "open file failed"},
    {107010, "write file failed"},
    {107011, "error subscribe stream"},
    {107012, "error subscribe thread"},
    {107013, "group not set"},
    {107014, "group not create"},
    {107015, "callback not register to stream"},
    {107016, "invalid memory type"},
    {107017, "invalid handle"},
    {107018, "invalid malloc type"},
    {107019, "wait timeout"},
    {107020, "task timeout"},
    {207000, "feature not support"},
    {207001, "memory allocation error"},
    {207002, "memory free error"},
    {207003, "aicore over flow"},
    {207004, "no device"},
    {207006, "no permission"},
    {207007, "no event resource"},
    {207008, "no stream resource"},
    {207009, "no notify resource"},
    {207010, "no model resource"},
    {207011, "no cdq resource"},
    {207012, "over limit"},
    {207013, "queue is empty"},
    {207014, "queue is full"},
    {207015, "repeated init"},
    {207016, "aivec over flow"},
    {207017, "common over flow"},
    {207018, "device oom"},
    {507000, "runtime internal error"},
    {507001, "ts internel error"},
    {507002, "task full in stream"},
    {507003, "task empty in stream"},
    {507004, "stream not complete"},
    {507005, "end of sequence"},
    {507006, "event not complete"},
    {507007, "context release error"},
    {507008, "soc version error"},
    {507009, "task type not support"},
    {507010, "ts lost heartbeat"},
    {507011, "model execute failed"},
    {507012, "report timeout"},
    {507013, "sys dma error"},
    {507014, "aicore timeout"},
    {507015, "aicore exception"},
    {507016, "aicore trap exception"},
    {507017, "aicpu timeout"},
    {507018, "aicpu exception"},
    {507019, "aicpu datadump response error"},
    {507020, "aicpu model operate response error"},
    {507021, "profiling error"},
    {507022, "ipc error"},
    {507023, "model abort normal"},
    {507024, "kernel unregistering"},
    {507025, "ringbuffer not init"},
    {507026, "ringbuffer no data"},
    {507027, "kernel lookup error"},
    {507028, "kernel register duplicate"},
    {507029, "debug register failed"},
    {507030, "debug unregister failed"},
    {507031, "label not in current context"},
    {507032, "program register num use out"},
    {507033, "device setup error"},
    {507034, "vector core timeout"},
    {507035, "vector core exception"},
    {507036, "vector core trap exception"},
    {507037, "cdq alloc batch abnormal"},
    {507038, "can not change die mode"},
    {507039, "single die mode can not set die"},
    {507040, "invalid die id"},
    {507041, "die mode not set"},
    {507042, "aic trap read overflow"},
    {507043, "aic trap write overflow"},
    {507044, "aiv trap read overflow"},
    {507045, "aiv trap write overflow"},
    {507046, "stream sync time out"},
    {507899, "drv internal error"},
    {507900, "aicpu internal error"},
    {507901, "hdc disconnect"}
};
}

bool RuntimeApi::CheckRtResult(rtError_t result, const string &apiName) const
{
    if (result == 0) {
        DEBUG_LOG("Runtime API call %s() success.", apiName.c_str());
        return true;
    }

    auto it = RT_ERROR_MAP.find(static_cast<int32_t>(result));
    std::string errorInfo = "";
    if (it != RT_ERROR_MAP.end()) {
        errorInfo = "error description: " + it->second;
    }

    ERROR_LOG("Runtime API call %s() failed. error code: %d %s", apiName.c_str(), result, errorInfo.c_str());
    return false;
}

int RuntimeApi::GetMagic(const string& magic) const
{
    if (magic == "RT_DEV_BINARY_MAGIC_ELF_AIVEC") {
        return rtDevBinaryMagicElfAivec;
    }
    if (magic == "RT_DEV_BINARY_MAGIC_ELF_AICUBE") {
        return rtDevBinaryMagicElfAicube;
    }
    return rtDevBinaryMagicElf;
}

rtError_t RuntimeApi::RtSetDevice(int32_t device) const
{
    return rtSetDevice(device);
}

rtError_t RuntimeApi::RtDeviceReset(int32_t device) const
{
    return rtDeviceReset(device);
}

rtError_t RuntimeApi::RtMalloc(void **devPtr, uint64_t size, rtMemType_t type, const uint16_t moduleId) const
{
    return rtMalloc(devPtr, size, type, moduleId);
}

rtError_t RuntimeApi::RtFree(void *devPtr) const
{
    return rtFree(devPtr);
}

rtError_t RuntimeApi::RtMallocHost(void **hostPtr, uint64_t size, const uint16_t moduleId) const
{
    return rtMallocHost(hostPtr, size, moduleId);
}

rtError_t RuntimeApi::RtFreeHost(void *hostPtr) const
{
    return rtFreeHost(hostPtr);
}

rtError_t RuntimeApi::RtMemcpy(void *dst, uint64_t destMax, const void *src, uint64_t count,
                               rtMemcpyKind_t kind) const
{
    return rtMemcpy(dst, destMax, src, count, kind);
}

rtError_t RuntimeApi::RtStreamCreate(rtStream_t *stream, int32_t priority) const
{
    return rtStreamCreate(stream, priority);
}

rtError_t RuntimeApi::RtStreamSynchronize(rtStream_t stream) const
{
    return rtStreamSynchronize(stream);
}

rtError_t RuntimeApi::RtStreamDestroy(rtStream_t stream) const
{
    return rtStreamDestroy(stream);
}

rtError_t RuntimeApi::RtDevBinaryRegister(const rtDevBinary_t *bin, void **handle) const
{
    return rtDevBinaryRegister(bin, handle);
}

rtError_t RuntimeApi::RtDevBinaryUnRegister(void *handle) const
{
    return rtDevBinaryUnRegister(handle);
}

rtError_t RuntimeApi::RtRegisterAllKernel(const rtDevBinary_t *bin, void **handle) const
{
    return rtRegisterAllKernel(bin, handle);
}

rtError_t RuntimeApi::RtFunctionRegister(void *binHandle, const void *stubFunc, const char *stubName,
                                         const void *devFunc, uint32_t funcMode) const
{
    return rtFunctionRegister(binHandle, stubFunc, stubName, devFunc, funcMode);
}

rtError_t RuntimeApi::RtKernelLaunch(const void *stubFunc, uint32_t blockDim, void *args, uint32_t argsSize,
                                     rtStream_t stream) const
{
    return rtKernelLaunch(stubFunc, blockDim, args, argsSize, nullptr, stream);
}

rtError_t RuntimeApi::RtKernelLaunchWithHandleV2(void *hdl, const uint64_t tilingKey, uint32_t blockDim,
                                                 rtArgsEx_t *argsInfo, rtSmDesc_t *smDesc, rtStream_t stm,
                                                 const rtTaskCfgInfo_t *cfgInfo) const
{
    return rtKernelLaunchWithHandleV2(hdl, tilingKey, blockDim, argsInfo, smDesc, stm, cfgInfo);
}

rtError_t RuntimeApi::RtGetC2cCtrlAddr(uint64_t *addr, uint32_t *fftsLen) const
{
    return rtGetC2cCtrlAddr(addr, fftsLen);
}
