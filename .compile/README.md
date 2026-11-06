# 编译与对拍

本目录提供 `compilecpp`、`compilepy`、`brutecheck`。命令入口为 `commands.ps1`，实现位于 `commands.py`；模板自动展开与复制见 [CP README](../CP/README.md)。

## 加载

需要 Windows、Python 3.10 或更新版本，以及支持 C++23 的 `g++`。Python 打包另需在当前 Python 环境安装 PyInstaller。

```powershell
. "D:\Vscode_CPP\.cpp_system\.compile\commands.ps1"
```

可将上面的加载语句加入 `$PROFILE`。PowerShell 5.1 可以加载命令；下方包含 `&&` 的写法需要 PowerShell 7。

## C++ 编译

```powershell
compilecpp a.cpp -o a
compilecpp a.cpp -o a && ./a
```

使用 `-std=c++23 -O2 -Wall -Wextra`，自动添加相邻 `CP/include` 的搜索路径。编译器取 `CXX` 环境变量，未设置时使用 `g++`。这里只编译本地源码；准备提交时用 `cpcopy` 展开模板。

## Python 打包

```powershell
compilepy a.py -o a
compilepy a.py -o a && ./a
```

使用当前 `python` 环境的 PyInstaller 生成单文件控制台 EXE，保留标准输入输出。它用于打包，不提供原生代码性能优化；额外数据文件和动态导入不由这个简化命令单独配置。

两个编译命令都支持 `-o` / `--output`。`-o a` 输出 `a.exe`；未指定时使用源文件名，在执行命令的当前目录生成 EXE。路径含空格时用引号包住。构建失败会报错，`&&` 后面的命令不执行；旧 EXE 在新构建成功后才替换，中间文件自动清理。

## 对拍

```powershell
brutecheck bte.cpp std.cpp gen.cpp
brutecheck bte.cpp std.cpp gen.cpp -Count 2000 -Seed 1 -Timeout 3
brutecheck bte.cpp std.cpp gen.cpp -Count 0
brutecheck bte.cpp std.cpp gen.cpp -Exact
```

参数顺序为待测程序、参考程序、生成器，三者都是 C++ 源文件，使用与 `compilecpp` 相同的参数编译。

| 参数 | 默认值 | 含义 |
|---|---|---|
| `-Count` / `-n` / `--count` | `1000` | 组数；`0` 表示持续运行，Ctrl+C 停止 |
| `-Seed` / `--seed` | `1` | 第一组种子，后续逐组加一 |
| `-Timeout` / `--timeout` | `3` | 每个程序每组的超时秒数，必须为正数 |
| `-Exact` / `--exact` | 关闭 | 开启后按字节严格比较输出 |
| `-o` / `--output` | `.output_cph/brutecheck` | 失败记录的父目录，相对于执行命令的目录 |

默认按空白分隔后的内容比较，忽略空格和换行差异，不做浮点误差判断，也不使用自定义 checker。程序使用执行命令时的工作目录，请通过标准输入输出交换数据。

生成器通过 `argv[1]` 接收十进制种子，标准输入为空。生成器应据此初始化随机数，以便生成不同数据并复现失败：

```cpp
#include <bits/stdc++.h>

int main(int argc, char *argv[]) {
    std::mt19937_64 rng(argc > 1 ? std::stoull(argv[1]) : 1);
    std::cout << rng() % 100 + 1 << '\n';
    return 0;
}
```

答案不同、程序非零退出或超时都会停止对拍并返回失败。在输出目录的 `failure-...` 子目录保存：

- `input.txt`：本组输入。
- `actual.txt`、`expected.txt`：待测输出、参考输出。
- `candidate.stderr.txt`、`reference.stderr.txt`、`generator.stderr.txt`：各程序错误输出。
- `report.json`：失败原因、组号、种子、源码路径、工作目录、限时和耗时。

生成器或参考程序先失败时，后续未运行程序的输出文件为空。成功对拍只使用临时目录，不保留测试 EXE。
