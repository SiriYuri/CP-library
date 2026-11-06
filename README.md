# CP Library

Windows 下的 C++23 算法竞赛模板与本地命令工具。

## 目录

```text
.cpp_system/
├─ README.md
├─ CP/
│  ├─ README.md
│  ├─ include/CP/       # 现有算法模板
│  ├─ library.json      # 模块、触发符号与依赖
│  └─ tools/
│     ├─ bundle.py      # 按需展开为单文件
│     └─ cpcopy.ps1     # 展开、编译检查、复制
└─ .compile/
   ├─ README.md
   ├─ commands.ps1      # PowerShell 命令入口
   └─ commands.py       # 编译、Python 打包与对拍
```

- [模板与自动展开](CP/README.md)
- [编译与对拍](.compile/README.md)

## 环境

- Windows、PowerShell 7，使用 `&&` 串联编译与运行。
- 支持 C++23 的 `g++`，编译参数为 `-std=c++23 -O2 -Wall -Wextra`。
- Python 3.10 或更新版本。
- 使用 `compilepy` 时，当前 Python 环境需要安装 PyInstaller。

```powershell
python -m pip install pyinstaller
```

## 加载命令

以下以本地目录 `D:\Vscode_CPP\.cpp_system` 为例。首次使用时执行；也可以加入自己的 PowerShell `$PROFILE`，让新终端自动加载。

```powershell
. "D:\Vscode_CPP\.cpp_system\.compile\commands.ps1"

function cpcopy {
    & "D:\Vscode_CPP\.cpp_system\CP\tools\cpcopy.ps1" @args
}
```

`g++` 与 `python` 应可直接从终端调用。`compilecpp`、`brutecheck` 也支持通过 `CXX` 环境变量指定编译器。

## 常用命令

在解题源码所在目录执行：

```powershell
compilecpp a.cpp -o a && ./a
compilepy a.py -o a && ./a
brutecheck bte.cpp std.cpp gen.cpp
cpcopy a.cpp
```

本地开发使用 `#include <CP/all.h>`；提交评测前，用 `cpcopy` 将实际使用的模板展开为单文件。先保存编辑器中的源码再执行命令。

源码、脚本与文档纳入版本控制；IntelliSense 缓存、EXE、构建临时文件和对拍结果不上传。
