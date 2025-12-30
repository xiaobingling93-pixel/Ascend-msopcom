#!/usr/bin/python3
# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
# This file is part of the MindStudio project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# MindStudio is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
# -------------------------------------------------------------------------

import os
import sys
import logging
import subprocess
import multiprocessing
import argparse


def exec_cmd(cmd):
    result = subprocess.run(cmd, capture_output=False, text=True, timeout=3600)
    if result.returncode != 0:
        logging.error("execute command %s failed, please check the log", " ".join(cmd))
        sys.exit(result.returncode)


def update_submodle(build_test):
    logging.info("============ start download thirdparty code using git submodule ============")
    if build_test:
        exec_cmd(["git", "submodule", "update", "--init", "--recursive", "--depth=1", "--jobs=4"])
    else:
        # 只构建打包时，只需下载构建所需子仓即可
        exec_cmd(["git", "submodule", "update", "--init", "--depth=1", "--jobs=4", "thirdparty/json"])
    logging.info("============ download thirdparty code  done ============")


def execute_build(build_path, cmake_cmd, make_cmd, install_cmd):
    if not os.path.exists(build_path):
        os.makedirs(build_path, mode=0o755)
    os.chdir(build_path)
    exec_cmd(cmake_cmd)
    exec_cmd(make_cmd)
    # 只有构建时才需要安装和打包，测试时不需要
    if install_cmd != "":
        exec_cmd(install_cmd)


def execute_test(build_path, test_cmd):
    os.chdir(build_path)
    if test_cmd != "":
        os.chdir(test_cmd.rsplit('/', 1)[0])
        cmd = "./" + test_cmd.rsplit('/', 1)[1]
        os.environ['LD_LIBRARY_PATH'] = os.getcwd() + "/stub"
        exec_cmd(cmd)


def create_arg_parser():
    parser = argparse.ArgumentParser(description='Build script with optional testing')
    parser.add_argument('command', nargs='*', default=[],
                        choices=[[], 'local', 'test'],
                        help='Command to execute (python build.py [ |local|test])')
    parser.add_argument('-r', '--revision',
                        help="Build with specific revision or tag")
    return parser


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    parser = create_arg_parser()
    args = parser.parse_args()
    current_dir = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))
    os.chdir(current_dir)
    cpu_cores = multiprocessing.cpu_count()

    build_path = os.path.join(current_dir, "build")
    cmake_cmd = ["cmake", ".."]
    make_cmd = ["make", "-j", str(cpu_cores)]
    install_cmd = ["make", "install"]
    test_cmd = ""


    # ut使用单独的目录构建，与build区分开，避免相互影响，并传入对应的参数
    if 'test' in args.command:
        build_path = os.path.join(current_dir, "build_ut")
        cmake_cmd.append("-DBUILD_TESTS=ON")
        make_cmd.append("injectionTest")
        install_cmd = ""
        test_cmd ="./test/injectionTest"

    # 解析入参是否为local，非local场景时按需更新代码；local场景不更新代码只使用本地代码
    if 'local' not in args.command:
        update_submodle('test' in args.command)
    # 执行构建
    execute_build(build_path, cmake_cmd, make_cmd, install_cmd)
    # 执行测试
    execute_test(build_path, test_cmd)
