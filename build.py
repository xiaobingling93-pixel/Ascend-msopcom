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

import argparse
import logging
import multiprocessing
import os
import subprocess
import sys
import traceback
from pathlib import Path

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


class BuildManager:
    def __init__(self):
        self.project_root = Path(__file__).resolve().parent
        self.build_parallelism = multiprocessing.cpu_count()
        argument_parser = argparse.ArgumentParser(description='Build script with optional testing')
        argument_parser.add_argument('command', nargs='*', default=[],
                                     choices=[[], 'local', 'test'],
                                     help='Command to execute (python build.py [ |local|test])')
        argument_parser.add_argument('-r', '--revision',
                                     help='Build with specific revision or tag')
        self.parsed_arguments = argument_parser.parse_args()

    def _execute_command(self, command_sequence, timeout_seconds=36000, cwd=None):
        """执行外部命令，失败时抛出异常"""
        logging.info("Running: %s", " ".join(command_sequence))
        subprocess.run(command_sequence, timeout=timeout_seconds, check=True, cwd=cwd)

    def run(self):
        os.chdir(self.project_root)

        # 非 local 场景时按需更新代码；local 场景不更新代码只使用本地代码
        if 'local' not in self.parsed_arguments.command:
            from download_dependencies import DependencyManager
            DependencyManager(self.parsed_arguments).run()

        if 'test' in self.parsed_arguments.command:
            # -------------------- 单元测试 --------------------
            build_dir = self.project_root / "build_ut"
            test_dir = build_dir / "test"
            build_dir.mkdir(exist_ok=True)
            os.chdir(build_dir)

            self._execute_command(["cmake", "..", "-DBUILD_TESTS=ON", "-DCMAKE_BUILD_TYPE=Debug"])
            self._execute_command(["make", "-j", str(self.build_parallelism), "injectionTest"])
            os.environ['LD_LIBRARY_PATH'] = str(test_dir / "stub") + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')
            self._execute_command(["./injectionTest"], cwd=str(test_dir))

        else:
            # -------------------- 产品构建 --------------------
            build_dir = self.project_root / "build"
            build_dir.mkdir(exist_ok=True)
            os.chdir(build_dir)

            self._execute_command(["cmake", ".."])
            self._execute_command(["make", "-j", str(self.build_parallelism)])
            self._execute_command(["make", "install"])  # 安装步骤


if __name__ == "__main__":
    try:
        BuildManager().run()
    except Exception:
        logging.error(f"Unexpected error: {traceback.format_exc()}")
        sys.exit(1)