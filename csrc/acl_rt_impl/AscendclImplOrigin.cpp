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

#include "AscendclImplOrigin.h"
#include "core/FunctionLoader.h"
#include "core/HijackedLayerManager.h"
#include "utils/InjectLogger.h"
#define LOAD_FUNCTION_BODY(soName, funcName, ...)                       \
    {                                                                   \
        /* 屏蔽 runtime 接口劫持，防止因劫持接口内调用了 runtime 接口污染记录信息 */ \
        LayerGuard guard(HijackedLayerManager::Instance(), #funcName); \
        FUNC_BODY(soName, funcName, Origin, ACL_ERROR_INTERNAL_ERROR, __VA_ARGS__); \
    }

void AscendclImplOriginCtor()
{
    REGISTER_LIBRARY("acl_rt_impl");
    REGISTER_FUNCTION("acl_rt_impl", aclrtSetDeviceImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtGetDeviceImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtResetDeviceForceImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtGetSocNameImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtMallocImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtFreeImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtMemsetImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtMemcpyImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtBinaryLoadFromFileImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtCreateBinaryImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtBinaryLoadImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtBinaryUnLoadImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtDestroyBinaryImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtBinaryGetFunctionImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtBinaryGetFunctionByEntryImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsInitImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsInitByUserMemImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsGetMemSizeImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsGetHandleMemSizeImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsAppendImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsAppendPlaceHolderImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsGetPlaceHolderBufferImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsParaUpdateImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtKernelArgsFinalizeImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtLaunchKernelWithConfigImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtLaunchKernelImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtGetFunctionAddrImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtSynchronizeStreamWithTimeoutImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtSynchronizeStreamImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtMemcpyAsyncImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtMallocHostImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtFreeHostImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtCtxGetCurrentDefaultStreamImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclmdlRICaptureBeginImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclmdlRICaptureEndImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclmdlRIDestroyImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclmdlRIExecuteAsyncImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtCmoAsyncImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtLaunchKernelWithHostArgsImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtBinaryLoadFromDataImpl);
    REGISTER_FUNCTION("acl_rt_impl", aclrtGetFunctionAttributeImpl);
}

aclError aclrtSetDeviceImplOrigin(int32_t deviceId)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtSetDeviceImpl, deviceId);
}

aclError aclrtGetDeviceImplOrigin(int32_t *deviceId)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtGetDeviceImpl, deviceId);
}

aclError aclrtResetDeviceForceImplOrigin(int32_t deviceId)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtResetDeviceForceImpl, deviceId);
}

const char *aclrtGetSocNameImplOrigin()
{
    using FuncType = decltype(&aclrtGetSocNameImpl);
    static FuncType func = nullptr;
    if (func == nullptr) {
        func = reinterpret_cast<FuncType>(GET_FUNCTION("acl_rt_impl", "aclrtGetSocNameImpl"));
        if (func == nullptr) {
            return nullptr;
        }
    }
    return func();
}

aclError aclrtMallocImplOrigin(void **devPtr, size_t size, aclrtMemMallocPolicy policy)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtMallocImpl, devPtr, size, policy);
}

aclError aclrtFreeImplOrigin(void *devPtr)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtFreeImpl, devPtr);
}

aclError aclrtMemsetImplOrigin(void *devPtr, size_t maxCount, int32_t value, size_t count)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtMemsetImpl, devPtr, maxCount, value, count);
}

aclError aclrtMemcpyImplOrigin(void *dst, size_t destMax, const void *src, size_t count, aclrtMemcpyKind kind)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtMemcpyImpl, dst, destMax, src, count, kind);
}

aclError aclrtBinaryLoadFromFileImplOrigin(const char* binPath, aclrtBinaryLoadOptions *options, aclrtBinHandle *binHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtBinaryLoadFromFileImpl, binPath, options, binHandle);
}

aclrtBinary aclrtCreateBinaryImplOrigin(const void *data, size_t dataLen)
{
    FUNC_BODY("acl_rt_impl", aclrtCreateBinaryImpl, Origin, nullptr, data, dataLen);
}

aclError aclrtBinaryLoadImplOrigin(const aclrtBinary binary, aclrtBinHandle *binHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtBinaryLoadImpl, binary, binHandle);
}

aclError aclrtBinaryUnLoadImplOrigin(aclrtBinHandle binHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtBinaryUnLoadImpl, binHandle);
}

aclError aclrtDestroyBinaryImplOrigin(aclrtBinary binary)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtDestroyBinaryImpl, binary);
}

aclError aclrtBinaryGetFunctionImplOrigin(const aclrtBinHandle binHandle, const char *kernelName, aclrtFuncHandle *funcHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtBinaryGetFunctionImpl, binHandle, kernelName, funcHandle);
}

aclError aclrtBinaryGetFunctionByEntryImplOrigin(aclrtBinHandle binHandle, uint64_t funcEntry, aclrtFuncHandle *funcHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtBinaryGetFunctionByEntryImpl, binHandle, funcEntry, funcHandle);
}

aclError aclrtKernelArgsInitImplOrigin(aclrtFuncHandle funcHandle, aclrtArgsHandle *argsHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsInitImpl, funcHandle, argsHandle);
}

aclError aclrtKernelArgsInitByUserMemImplOrigin(aclrtFuncHandle funcHandle, aclrtArgsHandle argsHandle, void *userHostMem, size_t actualArgsSize)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsInitByUserMemImpl, funcHandle, argsHandle, userHostMem, actualArgsSize);
}

aclError aclrtKernelArgsGetMemSizeImplOrigin(aclrtFuncHandle funcHandle, size_t userArgsSize, size_t *actualArgsSize)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsGetMemSizeImpl, funcHandle, userArgsSize, actualArgsSize);
}

aclError aclrtKernelArgsGetHandleMemSizeImplOrigin(aclrtFuncHandle funcHandle, size_t *memSize)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsGetHandleMemSizeImpl, funcHandle, memSize);
}

aclError aclrtKernelArgsAppendImplOrigin(aclrtArgsHandle argsHandle, void *param, size_t paramSize, aclrtParamHandle *paramHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsAppendImpl, argsHandle, param, paramSize, paramHandle);
}

aclError aclrtKernelArgsAppendPlaceHolderImplOrigin(aclrtArgsHandle argsHandle, aclrtParamHandle *paramHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsAppendPlaceHolderImpl, argsHandle, paramHandle);
}

aclError aclrtKernelArgsGetPlaceHolderBufferImplOrigin(aclrtArgsHandle argsHandle, aclrtParamHandle paramHandle, size_t dataSize, void **bufferAddr)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsGetPlaceHolderBufferImpl, argsHandle, paramHandle, dataSize, bufferAddr);
}

aclError aclrtKernelArgsParaUpdateImplOrigin(aclrtArgsHandle argsHandle, aclrtParamHandle paramHandle, void *param, size_t paramSize)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsParaUpdateImpl, argsHandle, paramHandle, param, paramSize);
}

aclError aclrtKernelArgsFinalizeImplOrigin(aclrtArgsHandle argsHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtKernelArgsFinalizeImpl, argsHandle);
}

aclError aclrtLaunchKernelWithConfigImplOrigin(aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream, aclrtLaunchKernelCfg *cfg, aclrtArgsHandle argsHandle, void *reserve)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtLaunchKernelWithConfigImpl, funcHandle, blockDim, stream, cfg, argsHandle, reserve);
}

aclError aclrtLaunchKernelImplOrigin(aclrtFuncHandle funcHandle, uint32_t blockDim, const void *argsData, size_t argsSize, aclrtStream stream)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtLaunchKernelImpl, funcHandle, blockDim, argsData, argsSize, stream);
}

aclError aclrtLaunchKernelWithHostArgsImplOrigin(aclrtFuncHandle funcHandle, uint32_t blockDim, aclrtStream stream,
    aclrtLaunchKernelCfg *cfg, void *hostArgs, size_t argsSize, aclrtPlaceHolderInfo *placeHolderArray, size_t placeHolderNum)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtLaunchKernelWithHostArgsImpl, funcHandle, blockDim, stream, cfg, hostArgs,
                       argsSize, placeHolderArray, placeHolderNum);
}

aclError aclrtBinaryLoadFromDataImplOrigin(const void *data, size_t length, const aclrtBinaryLoadOptions *options, aclrtBinHandle *binHandle)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtBinaryLoadFromDataImpl, data, length, options, binHandle);
}

aclError aclmdlRICaptureBeginImplOrigin(aclrtStream stream, aclmdlRICaptureMode mode)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclmdlRICaptureBeginImpl, stream, mode);
}

aclError aclmdlRICaptureEndImplOrigin(aclrtStream stream, aclmdlRI *modelRI)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclmdlRICaptureEndImpl, stream, modelRI);
}

aclError aclmdlRIDestroyImplOrigin(aclmdlRI modelRI)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclmdlRIDestroyImpl, modelRI);
}

aclError aclmdlRIExecuteAsyncImplOrigin(aclmdlRI modelRI, aclrtStream stream)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclmdlRIExecuteAsyncImpl, modelRI, stream);
}

aclError aclrtSynchronizeStreamWithTimeoutImplOrigin(aclrtStream stream, int32_t timeout)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtSynchronizeStreamWithTimeoutImpl, stream, timeout);
}

aclError aclrtSynchronizeStreamImplOrigin(aclrtStream stream)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtSynchronizeStreamImpl, stream);
}

aclError aclrtMemcpyAsyncImplOrigin(void *dst, size_t destMax, const void *src, size_t count,
                                    aclrtMemcpyKind kind, aclrtStream stream)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtMemcpyAsyncImpl, dst, destMax, src, count, kind, stream);
}

aclError aclrtMallocHostImplOrigin(void **hostPtr, size_t size)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtMallocHostImpl, hostPtr, size);
}

aclError aclrtFreeHostImplOrigin(void *hostPtr)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtFreeHostImpl, hostPtr);
}

aclError aclrtCtxGetCurrentDefaultStreamImplOrigin(aclrtStream *stream)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtCtxGetCurrentDefaultStreamImpl, stream);
}

aclError aclrtGetFunctionAddrImplOrigin(aclrtFuncHandle funcHandle, void **aicAddr, void **aivAddr)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtGetFunctionAddrImpl, funcHandle, aicAddr, aivAddr);
}

aclError aclrtCmoAsyncImplOrigin(void *src, size_t size, aclrtCmoType cmoType, aclrtStream stream)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtCmoAsyncImpl, src, size, cmoType, stream);
}

aclError aclrtGetFunctionAttributeImplOrigin(aclrtFuncHandle funcHandle, aclrtFuncAttribute attr, int64_t *value)
{
    LOAD_FUNCTION_BODY("acl_rt_impl", aclrtGetFunctionAttributeImpl, funcHandle, attr, value);
}
