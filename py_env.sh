#!/bin/bash

# 检查并安装必要的工具和依赖
sudo apt-get update
sudo apt-get install -y build-essential git gcc-arm-none-eabi 



# 更新子模块
git submodule update --init --recursive


