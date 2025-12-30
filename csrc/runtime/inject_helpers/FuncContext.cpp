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

#include "FuncContext.h"

#include <cinttypes>

#include "utils/InjectLogger.h"
#include "acl_rt_impl/AscendclImplOrigin.h"

using namespace std;

namespace {
KernelType AclrtKernelTypeTrans(aclrtKernelType kernelType)
{
    switch (kernelType) {
        case aclrtKernelType::ACL_KERNEL_TYPE_AICORE:
        case aclrtKernelType::ACL_KERNEL_TYPE_MIX:
            return KernelType::MIX;
        case aclrtKernelType::ACL_KERNEL_TYPE_VECTOR:
            return KernelType::AIVEC;
        case aclrtKernelType::ACL_KERNEL_TYPE_CUBE:
            return KernelType::AICUBE;
        case aclrtKernelType::ACL_KERNEL_TYPE_AICPU:
            return KernelType::AICPU;
        default:
            return KernelType::AIVEC;
    }
}
} // namespace

uint64_t FuncContext::GetStartPC() const
{
    void *aicAddr{nullptr};
    void *aivAddr{nullptr};
    uint64_t pcStartAddr;
    auto ret = aclrtGetFunctionAddrImplOrigin(funcHandle_, &aicAddr, &aivAddr);
    if (ret != 0) {
        WARN_LOG("aclrtGetFunctionAddrImpl failed. ret=%d\n", ret);
        return 0;
    }
    if (aicAddr) {
        pcStartAddr = reinterpret_cast<uint64_t>(aicAddr);
    } else if (aivAddr) {
        pcStartAddr = reinterpret_cast<uint64_t>(aivAddr);
    } else {
        ERROR_LOG("aclrtGetFunctionAddrImpl get addr all zero");
        return 0;
    }
    uint64_t kernelOffset = regCtx_->GetKernelOffsetByName(kernelName_);
    if (pcStartAddr < kernelOffset) {
        ERROR_LOG("pc start addr %#" PRIx64 " smaller than kernel offset %#" PRIx64, pcStartAddr, kernelOffset);
        return 0;
    }
    pcStartAddr -= kernelOffset;
    return pcStartAddr;
}

KernelType FuncContext::GetKernelType(const std::string &kernelName) const
{
    if (regCtx_->KernelSymbolNameIsMix(kernelName)) {
        return KernelType::MIX;
    }
    aclrtKernelType kernelType{};
    int64_t value;
    auto ret = aclrtGetFunctionAttributeImplOrigin(funcHandle_, aclrtFuncAttribute::ACL_FUNC_ATTR_KERNEL_TYPE, &value);
    if (ret != ACL_SUCCESS) {
        ERROR_LOG("get function kernel type failed, ret=%u", static_cast<uint32_t>(ret));
        return KernelType::INVALID;
    }
    kernelType = static_cast<aclrtKernelType>(value);
    return AclrtKernelTypeTrans(kernelType);
}
