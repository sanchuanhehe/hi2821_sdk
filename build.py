#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    build system entry, receive param & start to build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# ============================================================================
"""
接收参数列表及解释:
    -c:
        clean后编译
    -j:
        -j<num>, 以num线程数编译
        默认为机器最大线程数
    -def=:
        -def=XXX,YYY,ZZZ=x,...  向本次编译target中添加XXX、YYY、ZZZ=x编译宏
        可使用-def=-:XXX 来屏蔽XXX宏
        可使用-def=-:ZZZ=x 来添加或者修改ZZZ宏
    -component=:
        -component=XXX,YYY,...  仅编译XXX,YYY组件
    -ninja:
        使用ninja生成中间文件
        默认使用Unix makefile
    -[release/debug]:
        debug:     在生成反汇编文件时信息更加全面但也更耗时
        release:   在生成反汇编文件时节省时间
        默认为debug
    -dump:
        输出target的编译参数
    -nhso:
        不更新HSO数据库
    -out_libs:
        -out_libs=file_path, 不再链接成elf, 转而将所有.a打包成一个大的.a
    others:
        作为匹配编译target_names的关键字
"""
import os
import sys
from distutils.spawn import find_executable

# 获取当前路径
current_dir = os.path.dirname(os.path.realpath(__file__))

sys.dont_write_bytecode = True
root_dir = os.path.split(os.path.realpath(__file__))[0]
sys.path.append(os.path.join(root_dir, 'build', 'config'))
sys.path.append(os.path.join(root_dir, 'build', 'script'))

from cmake_builder import CMakeBuilder
from generate_clangd_config import generate_clangd_config

def check_environment():
    if not find_executable("cmake"):
        print("cmake is not installed or not added to system path.")
    if not find_executable("ninja") and not find_executable("make"):
        print("make/ninja is not installed or not added to system path.")

check_environment()

builder = CMakeBuilder(sys.argv)

builder.build()

# 定义函数递归查找 compile_commands.json 文件，并返回时间戳最新的一个
def find_compile_commands(root_dir):
    compile_commands_paths = []
    for dirpath, dirnames, filenames in os.walk(root_dir):
        if 'compile_commands.json' in filenames:
            compile_commands_paths.append(os.path.join(dirpath, 'compile_commands.json'))
    
    if not compile_commands_paths:
        return None
    
    # 找到修改时间最新的文件
    latest_compile_commands = max(compile_commands_paths, key=os.path.getmtime)
    return latest_compile_commands

# 查找 compile_commands.json 文件路径
compilation_database_path = find_compile_commands(current_dir)

if not compilation_database_path:
    print("Error: compile_commands.json not found in any directory.")
    sys.exit(1)

# 调用函数生成 .clangd 文件 sysroot_path 为 tools/bin/compiler/riscv/cc_riscv32_musl_b090/cc_riscv32_musl_fp/sysroot
generate_clangd_config(output_path='.clangd', compilation_database_path=compilation_database_path ,sysroot_path = os.path.join(root_dir, 'tools', 'bin', 'compiler', 'riscv', 'cc_riscv32_musl_b090', 'cc_riscv32_musl_fp', 'sysroot'))
