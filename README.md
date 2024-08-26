# hi2821_sdk

## Overview

This project is structured to be developed and built within a Docker container, utilizing Visual Studio Code (VSCode) for an integrated development environment. The project is compatible with Linux (either native or within a WSL2 virtual machine) and can be built using a `build.py` script.

## Directory Structure

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

## Prerequisites

- **Linux Environment**: You can use either a native Linux installation or a WSL2 virtual machine.
- **Docker**: Ensure Docker is installed on your system.
- **Visual Studio Code (VSCode)**: Install VSCode on your system.
- **Dev Containers Extension**: Install the Dev Containers extension in VSCode.

## Setup and Usage

### 1. Install Docker

Follow the official Docker installation guide for your Linux distribution or WSL2 setup.

### 2. Connect VSCode to Linux Environment

If you are using a WSL2 virtual machine, connect VSCode to it via the Remote - WSL extension. For remote development, you can also connect VSCode to a Linux environment using SSH.

### 3. Install Dev Containers Extension

Open VSCode and install the Dev Containers extension:

1. Press `Ctrl + Shift + P`.
2. Type `Extensions: Install Extensions`.
3. Search for `Dev Containers` and install it.

### 4. Open the Project in a Dev Container

1. Press `Ctrl + Shift + P`.
2. Type `Dev Containers: Reopen in Container`.
3. Select your container configuration to open the project in a Docker container.

### 5. Build the Project

Use the `build.py` script to build the project:

```bash
python3 build.py
```

This script will handle the necessary steps to compile and prepare the project for deployment.

