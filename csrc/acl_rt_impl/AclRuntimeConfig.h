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


#ifndef __ACL_RUNTIME_CONFIG_H__
#define __ACL_RUNTIME_CONFIG_H__

#include <dlfcn.h>
#include <string>
#include <sys/stat.h>

#include "utils/Singleton.h"
#include "utils/FileSystem.h"

class AclRuntimeConfig : public Singleton<AclRuntimeConfig, false> {
public:
    static const std::string &Name() {
        auto &inst = AclRuntimeConfig::Instance();
        if (!inst.soName_.empty()) {
            return inst.soName_;
        }
        inst.soName_ = "acl_rt_impl";
        // check so valid
        std::string soPath = GetEnv("ASCEND_HOME_PATH");
        if (soPath.empty()) {
          return inst.soName_;
        }
        soPath = soPath + "/lib64/libacl_rt_impl.so";
        if (!IsExist(soPath)) {
            inst.soName_ = "ascendcl_impl";
        }
        return inst.soName_;
    }
protected:
    std::string soName_;
};

inline const std::string &AclRuntimeLibName()
{
    return AclRuntimeConfig::Name();
}

#endif // __RUNTIME_CONFIG_H__
