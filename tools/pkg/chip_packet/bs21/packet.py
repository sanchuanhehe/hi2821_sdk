#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import re

PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)

from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
sys.path.append(os.path.join(SDK_DIR, "build", "script"))

# bs21
def make_all_in_one_packet(pack_style_str):
    # make all in one packet
    if pack_style_str.startswith("pack"):
        bin_dir = os.path.join(SDK_DIR, "output", "package", "bs21", pack_style_str)
        application_dir = bin_dir
        ate_dir = bin_dir
        partition_dir = bin_dir
        nv_dir =  bin_dir
        bs21_fwpkg = os.path.join(bin_dir, "bs21_all_in_one.fwpkg")
        bs21_flashboot_only_fwpkg = os.path.join(bin_dir, "bs21_flashboot_only.fwpkg")
        bs21_loadapp_only_fwpkg = os.path.join(bin_dir, "bs21_loadapp_only.fwpkg")
        bs21_nv_fwpkg = os.path.join(bin_dir, "bs21_nv_only.fwpkg")
    else:
        bin_dir = os.path.join(SDK_DIR, "interim_binary", "bs21", "bin", "boot_bin")
        ate_dir = os.path.join(SDK_DIR, "output", "bs21", "acore", pack_style_str)
        partition_dir = os.path.join(SDK_DIR, "interim_binary", "bs21", "bin", "partition", pack_style_str)
        nv_dir =  os.path.join(SDK_DIR, "interim_binary", "bs21", "bin", "nv", pack_style_str)
        application_dir = os.path.join(SDK_DIR, "output", "bs21", "acore", pack_style_str)
        bs21_fwpkg = os.path.join(SDK_DIR, "output", "bs21", "fwpkg", pack_style_str, "bs21_all_in_one.fwpkg")
        bs21_ide_fwpkg = os.path.join(SDK_DIR, "tools", "pkg", "fwpkg", "bs21", "bs21_all.fwpkg")
        bs21_flashboot_only_fwpkg = os.path.join(SDK_DIR, "output", "bs21", "fwpkg", pack_style_str, "bs21_flashboot_only.fwpkg")
        bs21_loadapp_only_fwpkg = os.path.join(SDK_DIR, "output", "bs21", "fwpkg", pack_style_str, "bs21_loadapp_only.fwpkg")
        bs21_nv_fwpkg = os.path.join(SDK_DIR, "output", "bs21", "fwpkg", pack_style_str, "bs21_nv_only.fwpkg")

    partition = os.path.join(partition_dir, "partition.bin")
    loadboot = os.path.join(bin_dir, "loaderboot_sign.bin")
    flashboot = os.path.join(bin_dir, "flashboot_sign_a.bin")
    flashboot_bak = os.path.join(bin_dir, "flashboot_sign_b.bin")
    nv = os.path.join(nv_dir, "bs21_all_nv.bin")

    loadboot_bx = loadboot + "|0x0|0x0|0"
    partition_bx = partition + "|0x90100000|0x1000|1"

    app = os.path.join(application_dir, "application_sign.bin")
    if ("1M" in pack_style_str) or ("slekey" in pack_style_str):
        flashboot_bx = flashboot + "|0x90101000|0x8000|1"
        flashboot_bak_bx = flashboot_bak + "|0x90109000|0x8000|1"
        app_bx = app + "|0x90111000|0x8F000|1"
        nv_bx =  nv + "|0x901FE000|0x2000|1"
    elif "extern-flash" in pack_style_str or 'extern_flash' in pack_style_str or 'periphera' in pack_style_str:
        flashboot_bx = flashboot + "|0x90101000|0xa000|1"
        flashboot_bak_bx = flashboot_bak + "|0x9010b000|0xa000|1"
        app_bx = app + "|0x90115000|0x69000|1"
        nv_bx =  nv + "|0x9017E000|0x2000|1"
    else:
        flashboot_bx = flashboot + "|0x90101000|0x8000|1"
        flashboot_bak_bx = flashboot_bak + "|0x90109000|0x8000|1"
        app_bx = app + "|0x90111000|0x6D000|1"
        nv_bx =  nv + "|0x9017E000|0x2000|1"
    ate = os.path.join(ate_dir, "ate_sign.bin")
    ate_bx = ate + "|0x90101000|0x10000|1"

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(partition_bx)
        if pack_style_str.startswith("ate"):
            packet_post_agvs.append(ate_bx)
        else:
            packet_post_agvs.append(flashboot_bx)
            packet_post_agvs.append(flashboot_bak_bx)
            packet_post_agvs.append(app_bx)
            packet_post_agvs.append(nv_bx)
        packet_bin(bs21_fwpkg, packet_post_agvs)

        if not pack_style_str.startswith("ate") and not pack_style_str.startswith("pack"):
            packet_post_agvs = list()
            packet_post_agvs.append(loadboot_bx)
            packet_post_agvs.append(partition_bx)
            packet_post_agvs.append(flashboot_bx)
            packet_post_agvs.append(flashboot_bak_bx)
            packet_post_agvs.append(app_bx)
            packet_post_agvs.append(nv_bx)
            packet_bin(bs21_ide_fwpkg, packet_post_agvs)

        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(flashboot_bx)
        packet_post_agvs.append(flashboot_bak_bx)
        packet_bin(bs21_flashboot_only_fwpkg, packet_post_agvs)

        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(app_bx)
        packet_bin(bs21_loadapp_only_fwpkg, packet_post_agvs)

        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(nv_bx)
        packet_bin(bs21_nv_fwpkg, packet_post_agvs)

    except Exception as e:
        print(e)
        exit(-1)


def is_packing_files_exist(soc, pack_style_str):
    """
    判断打包文件是否存在
    :return:
    """
    packing_files = get_packing_files(soc, pack_style_str)
    lost_files = list()
    for f_path in packing_files:
        if not os.path.isfile(f_path):
            lost_files.append(f_path)
    return lost_files

def get_packing_files(soc, pack_style_str):
    """
    直接添加需要打包的文件路径
    :return:
    """

    packing_files = list()
    if pack_style_str.startswith("pack"):
        bin_dir = os.path.join(SDK_DIR, "output", "package", "bs21", pack_style_str)
        if pack_style_str.endswith("ate"):
            ate_path = os.path.join(bin_dir, "ate.bin")
            packing_files.append(ate_path)
        else:
            application_path = os.path.join(bin_dir, "application.bin")
            packing_files.append(application_path)
    else:
        bin_dir = os.path.join(SDK_DIR, "output", "bs21", "acore", pack_style_str)
        if pack_style_str.startswith("ate"):
            ate_path = os.path.join(bin_dir, "ate.bin")
            packing_files.append(ate_path)
        else:
            application_path = os.path.join(bin_dir, "application.bin")
            packing_files.append(application_path)
    return packing_files
