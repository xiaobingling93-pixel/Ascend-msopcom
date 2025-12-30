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

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "acl.h"
#include "runtime.h"
#include "ArgsContext.h"

/*
 * 管理rtArgsEx_t结构的kernel入参信息
 */
class ArgsExContext : public ArgsContext {
public:
    // 由调用者保证传入的指针非空
    explicit ArgsExContext(rtArgsEx_t *argsInfo) : ArgsContext(), argsInfo_(*argsInfo) {}

    bool ExpandArgs(void *param, size_t paramSize, uint32_t &paramOffset) override;

    bool Save(const std::string &outputPath, DumperContext &config, OpMemInfo &memInfo, bool isSink) override;

    ArgsContextSP Clone(void) const override;
private:
    rtArgsEx_t argsInfo_;
};
