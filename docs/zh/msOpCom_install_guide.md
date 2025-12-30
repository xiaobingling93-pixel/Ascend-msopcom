# MindStudio Ops Common安装指南

# 安装说明
MindStudio Ops Common（算子工具基础组件，msOpCom）是算子工具基础组件，对于昇腾产品工具而言，在开发维测环境下，需要将运行时进行改变，以支持"隐式"行为的隐藏。本文主要介绍msOpCom工具的安装方法。  


# 安装前准备
## 更新依赖子仓代码

为了确保代码能够下载成功，需提前在环境中配置git仓库的用户名和秘密信息，方式如下：
配置git存储用户密码，并通过git submodule来下载.gitmodules中的子仓，在下载过程中可能会提示需要输入用户名和密码，输入后git会记住授权信息，后续就不会再需要输入用户名和密码了：
```
git config --global credential.helper store
git submodule update --init --recursive --depth=1
```

# 安装步骤
## 项目构建
开始构建之前，需要确保已安装编译器bisheng，并且其可执行文件所在路径在环境变量$PATH中(如果已安装cann算子工具包，可在工具包安装路径下执行source set_env.sh).

可以通过如下命令构建：
```
mkdir build
cd build
cmake ..
make -j8 && make install
```
也可以通过一键式脚本来执行：
```
python build.py
```
注：如果本地更改了依赖子仓中的代码，不想构建过程中执行更新动作，可以执行python build.py local


# UT测试
```
mkdir build_ut
cd build_ut
cmake .. –DBUILD_TEST=on
make -j8 injectionTest
export LD_LIBRARY_PATH=$PWD/test/stub:$LD_LIBRARY_PATH
./test/injectionTest
```
也可以通过一键式脚本来执行：
```
python build.py test
```
