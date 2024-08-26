# 海思 Hi2821 SDK 项目

[English Version](README.md) | [中文版](README.zh.md)


## 概述

本项目是海思 Hi2821 芯片的 SDK，旨在通过 Docker 容器进行开发和构建，使用 Visual Studio Code (VSCode) 作为集成开发环境。该项目支持在 Linux（原生或 WSL2 虚拟机中运行）环境中进行开发，并通过 `build.py` 脚本进行构建。

## 目录结构

```plaintext
.
├── analyzerJson
│   ├── cfg
│   ├── funcstack.json
│   └── memoryDetails.json
├── application
│   ├── bs21
│   ├── CMakeLists.txt
│   ├── demo
│   ├── Kconfig
│   ├── samples
│   └── sle_keyboard
├── build
│   ├── cmake
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   ├── config
│   ├── script
│   └── toolchains
├── build.py
├── CMakeLists.txt
├── config.in
├── drivers
│   ├── chips
│   ├── CMakeLists.txt
│   ├── drivers
│   └── Kconfig
├── include
│   ├── cfbb_version.h
│   ├── CMakeLists.txt
│   ├── common_def.h
│   ├── driver
│   ├── errcode.h
│   └── middleware
├── interim_binary
│   └── bs21
├── kernel
│   ├── CMakeLists.txt
│   ├── Kconfig
│   ├── liteos
│   ├── non_os
│   └── osal
├── middleware
│   ├── chips
│   ├── CMakeLists.txt
│   ├── Kconfig
│   ├── services
│   └── utils
├── open_source
│   ├── 7-zip-lzma-sdk
│   ├── CMakeLists.txt
│   ├── GmSSL3.0
│   ├── libboundscheck
│   └── mbedtls
├── output
│   └── bs21
├── protocol
│   ├── bt
│   ├── cat1
│   ├── CMakeLists.txt
│   ├── glp
│   ├── Kconfig
│   └── nfc
├── sdk.code-workspace
├── test
│   ├── CMakeLists.txt
│   ├── common
│   ├── Kconfig
│   └── platform
├── tools
│   ├── bin
│   └── pkg
└── vendor
    ├── CMakeLists.txt
    └── segger
```

## 先决条件

- **Linux 环境**：你可以使用原生 Linux 系统或 WSL2 虚拟机。
- **Docker**：确保你的系统已安装 Docker。
- **Visual Studio Code (VSCode)**：在你的系统上安装 VSCode。
- **Dev Containers 插件**：在 VSCode 中安装 Dev Containers 插件。

## 设置和使用步骤

### 1. 安装 Docker

根据你所使用的 Linux 发行版或 WSL2 设置，参考 Docker 官方安装指南进行安装。

### 2. 连接 VSCode 至 Linux 环境

如果使用 WSL2 虚拟机，使用 Remote - WSL 插件将 VSCode 连接到虚拟机。如果进行远程开发，你也可以通过 SSH 将 VSCode 连接到 Linux 环境。

### 3. 安装 Dev Containers 插件

打开 VSCode 并安装 Dev Containers 插件：

1. 按 `Ctrl + Shift + P`。
2. 输入 `Extensions: Install Extensions`。
3. 搜索 `Dev Containers` 并安装。

### 4. 在 Dev 容器中打开项目

1. 按 `Ctrl + Shift + P`。
2. 输入 `Dev Containers: Reopen in Container`。
3. 选择你的容器配置以在 Docker 容器中打开项目。

### 5. 构建项目

使用 `build.py` 脚本来构建项目：

```bash
python3 build.py
```

该脚本将处理必要的步骤以编译并准备项目进行部署。

