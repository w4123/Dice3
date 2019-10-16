# Dice V3

[![Build status](https://ci.appveyor.com/api/projects/status/ft7x1jvdam3xb8jc?svg=true)](https://ci.appveyor.com/project/w4123/dice3)

Dice! V3 是重构后的Dice!新版本

实现了部分功能，剩余功能还在编写当中。具体信息可以参考: https://kokona.tech 开发日志一栏

本应用文档: https://docs.kokona.tech (还没写完)


## 使用方式

### 预备

本项目使用 [CMake](https://cmake.org/) 构建，依赖项通过 [vcpkg](https://github.com/Microsoft/vcpkg) 管理。如果你没有使用过这两个工具，请先前往它们的官方网站了解基本用法。

在开始使用之前，请确保你已经安装了 Git，且 `PATH` 中存在 `git` 命令。不需要安装 vcpkg，后面的脚本中会自动安装。

然后确保安装了 **Visual Studio 2017 或 2019**，并勾选「使用 C++ 的桌面开发」，确保安装了 CMake、**MSVC v141 或 v142**、**Windows 10 SDK** 这三个组件。其中，如果系统中已经安装了 CMake，无需再在 VS Installer 中安装，但需要确保命令已添加进 `PATH`。除此之外，vcpkg 还要求安装 VS 的**英文语言包**。

### 下载

```ps1
git clone https://github.com/w4123/Dice3 Dice
cd Dice
git submodule init
git submodule update
```

### 准备构建环境

```ps1
powershell .\scripts\prepare.ps1
```

[`scripts/prepare.ps1`](scripts/prepare.ps1) 脚本会在当前项目目录的 `vcpkg` 子目录中安装 vcpkg，并安装所需依赖。

*注意，如果 PowerShell 提示不让运行脚本，需要先使用管理员权限打开 PowerShell，运行 `Set-ExecutionPolicy Unrestricted`，然后再重新运行上面的脚本；如果 vcpkg 安装依赖时出错，比较常见的问题是网络超时，请适当设置 `HTTP_PROXY` 和 `HTTPS_PROXY`。*

### 构建项目

```ps1
powershell .\scripts\generate.ps1 Debug
powershell .\scripts\build.ps1 Debug
```

上面两条命令分别生成 build 目录和构建项目，将 `Debug` 改为 `Release` 可以构建 release 版本。如果安装了 CMake 还没支持的较新版本 VS，需要先手动进入 VS 2017 或 2019 的 Developer Command Prompt，再执行上面的命令。

如果你使用 VS Code，可以直接运行 task；如果使用 VS，可以直接选择菜单 CMake - 全部生成。

### 安装插件到 酷Q

复制 `build/Debug/Debug`（如果是 release 编译则是 `build/Release/Release`）中和你的 App Id 名字相同的文件夹到 酷Q 的 `dev` 目录下，在 酷Q 中重载应用即可（注意需要开启 酷Q 的开发模式）。

如果不想每次构建后都手动安装插件，可以添加 `scripts/install.ps1` 文件（使用 UTF-16 LE 编码）如下：

```ps1
$coolqRoot = "dir\to\CoolQ" # 注意修改 酷Q 目录

$appId = $args[0]
$libName = $args[1]
$appOutDir = $args[2]

$coolqAppDevDir = "$coolqRoot\dev\$appId"
$dllName = "$libName.dll"
$dllPath = "$appOutDir\$dllName"
$jsonName = "$libName.json"
$jsonPath = "$appOutDir\$jsonName"

Write-Host "正在拷贝插件到 酷Q 应用文件夹……"

New-Item -Path $coolqAppDevDir -ItemType Directory -ErrorAction SilentlyContinue
Copy-Item -Force $dllPath "$coolqAppDevDir\$dllName"
Copy-Item -Force $jsonPath "$coolqAppDevDir\$jsonName"

Write-Host "拷贝完成" -ForegroundColor Green
```

后期生成事件脚本 [`scripts/post_build.ps1`](scripts/post_build.ps1) 发现存在 `scripts/install.ps1` 时会自动运行它，从而将构建出的 DLL 和 JSON 文件安装到 酷Q，此时在 酷Q 悬浮窗快速重启即可。
