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

#include "utils/Protocol.h"
#include "utils/Singleton.h"

// 设置各个工具的特性配置
// 通过通信从工具端配置用于开启哪些劫持功能
template <typename ConfigT>
class ConfigManager : public Singleton<ConfigManager<ConfigT>, false> {
public:
    friend class Singleton<ConfigManager<ConfigT>, false>;

    ConfigT const &GetConfig() const { return config_; }

private:
    ConfigT config_ {};
};


template <>
class ConfigManager<SanitizerConfig> : public Singleton<ConfigManager<SanitizerConfig>, false> {
public:
    friend class Singleton<ConfigManager<SanitizerConfig>, false>;

    SanitizerConfig const &GetConfig() const;

private:
    ConfigManager() {}
};

using SanitizerConfigManager = ConfigManager<SanitizerConfig>;
