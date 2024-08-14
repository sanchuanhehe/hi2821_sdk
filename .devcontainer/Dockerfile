# 使用 Ubuntu 20.04 作为基础镜像
FROM ubuntu:20.04

# 设置非交互模式以避免时间区选择等提示
ENV DEBIAN_FRONTEND=noninteractive

# 更新系统并安装必要的工具和依赖
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    cmake \
    python3 \
    python3-pip \
    python3-setuptools \
    wget \
    build-essential \
    sudo \
    curl \
    git \
    bash \
    ninja-build \
    unzip

# 配置shell为bash
RUN echo "dash dash/sh boolean false" | debconf-set-selections && \
    dpkg-reconfigure -p critical dash

# 从 LLVM 官方下载最新的 clangd
RUN CLANGD_VERSION=18.1.3 && \
    wget https://github.com/clangd/clangd/releases/download/$CLANGD_VERSION/clangd-linux-$CLANGD_VERSION.zip && \
    unzip clangd-linux-$CLANGD_VERSION.zip && \
    cp -r clangd_$CLANGD_VERSION/bin/* /usr/local/bin/ && \
    cp -r clangd_$CLANGD_VERSION/lib/* /usr/local/lib/ && \
    rm -rf clangd-linux-$CLANGD_VERSION.zip clangd_$CLANGD_VERSION

# 安装kconfiglib，版本大于14.1.0
RUN pip3 install "kconfiglib>=14.1.0"

# 安装其他Python依赖包，版本大于2.21
RUN pip3 install "pycparser>=2.21"

# 安装PYYAML
RUN pip3 install pyyaml

# 设置环境变量export LC_ALL=C.UTF-8
ENV LC_ALL=C.UTF-8
