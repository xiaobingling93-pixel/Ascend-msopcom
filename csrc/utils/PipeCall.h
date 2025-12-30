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


#ifndef __UTILS_PIPECALL_H__
#define __UTILS_PIPECALL_H__

#include <vector>
#include <string>

/** 使用同步的方式执行外部命令
 * @param cmd 外部命令和参数
 * @param output 命令的标准输出
 * @param input 命令的标准输入
 */
bool PipeCall(std::vector<std::string> const &cmd,
              std::string &output,
              std::string const &input = std::string());

#endif // __UTILS_PIPECALL_H__
