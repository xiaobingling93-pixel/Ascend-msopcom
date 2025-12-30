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

#include <string>
#include <gtest/gtest.h>

TEST(RuntimeApi, rt_malloc_memcpy_and_free_success)
{
    RuntimeApi api;
    void *addr = nullptr;
    void *hostAddr = nullptr;
    constexpr uint64_t mallocSize = 0x100;

    ASSERT_EQ(api.RtMalloc(&addr, mallocSize, RT_MEMORY_DEFAULT, 0U), RT_ERROR_NONE);
    ASSERT_EQ(api.RtMallocHost(&hostAddr, mallocSize, 0U), RT_ERROR_NONE);
    ASSERT_EQ(api.RtMemcpy(addr, mallocSize, hostAddr, mallocSize,
        RT_MEMCPY_HOST_TO_DEVICE), RT_ERROR_NONE);
    ASSERT_EQ(api.RtFreeHost(hostAddr), RT_ERROR_NONE);
    ASSERT_EQ(api.RtFree(addr), RT_ERROR_NONE);
}

TEST(RuntimeApi, rt_set_and_reset_device_success)
{
    RuntimeApi api;
    constexpr int32_t devId = 1;

    ASSERT_EQ(api.RtSetDevice(devId), RT_ERROR_NONE);
    ASSERT_EQ(api.RtDeviceReset(devId), RT_ERROR_NONE);
}

TEST(RuntimeApi, rt_create_sync_and_destroy_stream_success)
{
    RuntimeApi api;
    rtStream_t stream;

    ASSERT_EQ(api.RtStreamCreate(&stream, 0), RT_ERROR_NONE);
    ASSERT_EQ(api.RtStreamSynchronize(stream), RT_ERROR_NONE);
    ASSERT_EQ(api.RtStreamDestroy(stream), RT_ERROR_NONE);
}

TEST(RuntimeApi, rt_register_and_kernel_launch_success)
{
    RuntimeApi api;
    rtDevBinary_t bin;
    void *handle = nullptr;
    void *stubFunc = nullptr;
    std::string stubName = "";
    void *devFunc = nullptr;
    constexpr uint32_t blockDim = 1;
    void *args = nullptr;
    rtStream_t stream;

    ASSERT_EQ(api.RtDevBinaryRegister(&bin, &handle), RT_ERROR_NONE);
    ASSERT_EQ(api.RtFunctionRegister(handle, stubFunc, stubName.c_str(), devFunc, 0U), RT_ERROR_NONE);
    ASSERT_EQ(api.RtKernelLaunch(stubFunc, blockDim, args, 1U, stream), RT_ERROR_NONE);
    ASSERT_EQ(api.RtDevBinaryUnRegister(&handle), RT_ERROR_NONE);
}

TEST(RuntimeApi, rt_get_c2c_ctrl_addr_success)
{
    RuntimeApi api;
    ASSERT_EQ(api.RtGetC2cCtrlAddr(nullptr, nullptr), RT_ERROR_NONE);
}

TEST(RuntimeApi, check_result_success)
{
    const std::string apiName = "rtMalloc";
    RuntimeApi api;
    ASSERT_EQ(api.CheckRtResult(RT_ERROR_NONE, apiName), true);
}

TEST(RuntimeApi, check_result_fail_and_print_error_str)
{
    const std::string apiName = "rtMalloc";
    RuntimeApi api;
    constexpr int32_t errorCode = 107000;
    ASSERT_EQ(api.CheckRtResult(static_cast<rtError_t>(errorCode), apiName), false);
}

TEST(RuntimeApi, check_result_fail_and_cannot_find_error_str)
{
    const std::string apiName = "rtMalloc";
    RuntimeApi api;
    constexpr int32_t errorCode = 1;
    ASSERT_EQ(api.CheckRtResult(static_cast<rtError_t>(errorCode), apiName), false);
}

TEST(RuntimeApi, get_magic_return_aivec)
{
    const std::string magic = "RT_DEV_BINARY_MAGIC_ELF_AIVEC";
    RuntimeApi api;
    ASSERT_EQ(api.GetMagic(magic), rtDevBinaryMagicElfAivec);
}

TEST(RuntimeApi, get_magic_return_aicube)
{
    const std::string magic = "RT_DEV_BINARY_MAGIC_ELF_AICUBE";
    RuntimeApi api;
    ASSERT_EQ(api.GetMagic(magic), rtDevBinaryMagicElfAicube);
}

TEST(RuntimeApi, get_magic_return_elf)
{
    const std::string magic = "";
    RuntimeApi api;
    ASSERT_EQ(api.GetMagic(magic), rtDevBinaryMagicElf);
}
