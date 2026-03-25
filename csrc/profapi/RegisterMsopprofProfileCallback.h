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


#ifndef MSOPT_REGISTER_MSOPPROF_FUNC_H
#define MSOPT_REGISTER_MSOPPROF_FUNC_H

#include <map>
#include "include/thirdparty/prof.h"
#include "ProfOriginal.h"

int32_t MsprofCompactInfoReportCallbackImpl(uint32_t agingFlag, const VOID_PTR data, uint32_t len);

int32_t MsprofReportAdditionalInfoCallbackImpl(uint32_t agingFlag, const VOID_PTR data, uint32_t len);

class RegisterMsopprofProfileCallback {
public:
    static RegisterMsopprofProfileCallback *Instance();
    ~RegisterMsopprofProfileCallback();
    void RegisterFuncMsprof();
private:
    std::map<uint32_t, VOID_PTR> callBackFuncMap;
    bool register_ = false;
};

#endif // MSOPT_REGISTER_MSOPPROF_FUNC_H