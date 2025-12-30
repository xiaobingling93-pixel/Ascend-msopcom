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

// 这个文件本身需要与被劫持的对象一样

#ifndef __ASCEND_HAL_H__
#define __ASCEND_HAL_H__
#include <cstdint>
#include "ascend_hal/AscendHalOrigin.h"
#ifdef __cplusplus
extern "C" {
#endif

drvError_t halGetSocVersion(uint32_t devId, char *socVersion, uint32_t len);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __ASCEND_HAL_H__
