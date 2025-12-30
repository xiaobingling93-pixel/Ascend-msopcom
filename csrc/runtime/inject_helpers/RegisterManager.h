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

#pragma once

#include <unordered_map>
#include <mutex>
#include "acl.h"
#include "runtime.h"
#include "RegisterContext.h"
#include "utils/Singleton.h"

/**
 * @brief 根据 magic str 解析出 magic 值
 */
bool ParseMagicStr(std::string const &magicStr, uint32_t &magic);

/**
 * @brief 从 kernel 配套的 json 文件中读取 magic 字符串
 */
bool ReadMagicFromKernelJson(std::string const &jsonPath, std::string &magicStr);

/**
 * 对Kernel二进制做增删查的管理.
 * 依赖elf的解析功能函数.
 * 一个kernel二进制信息对应一个binHandle, rt和acrt接口都是一样有个对应的binHandle
 */
class RegisterManager : public Singleton<RegisterManager, false> {
public:
    RegisterContextSP CreateContext(const char *binPath, aclrtBinHandle binHandle, uint32_t magic,
                                    aclrtBinaryLoadOptions *options = nullptr);

    RegisterContextSP CreateContext(aclrtBinary bin, aclrtBinHandle binHandle, uint32_t magic, aclrtBinaryLoadOptions *options = nullptr);

    RegisterContextSP CreateContext(const void *data, size_t length,
        aclrtBinHandle binHandle, uint32_t magic, aclrtBinaryLoadOptions *options = nullptr);

    void CacheElfData(aclrtBinary bin, const char *data, size_t dataLen);

    RegisterContextSP GetContext(void *key);

#if defined (__BUILD_TESTS__)
    void Clear()
    {
        contexts_.clear();
        tempEflData_.clear();
    }
#endif
private:
    std::mutex mtx_;
    std::unordered_map<void *, RegisterContextSP> contexts_;
    std::unordered_map<aclrtBinary, std::vector<char>> tempEflData_;
    size_t currentRegId_{0};
};
