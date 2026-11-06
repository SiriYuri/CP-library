# 算法模板与自动展开

本目录保存当前可用的 C++23 模板。`include/CP/all.h` 是统一入口，`library.json` 登记模块、触发符号和依赖，`tools` 中的脚本负责生成可提交的单文件代码。

本地编译、Python 打包与对拍命令见 [`.compile` README](../.compile/README.md)。

## 使用模板

```cpp
#include <bits/stdc++.h>
#include <CP/all.h>

int main() {
    std::cin.tie(nullptr)->std::ios_base::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;
    DSU dsu(n + 1);
    while (m--) {
        int u, v;
        std::cin >> u >> v;
        dsu.merge(u, v);
    }
    std::cout << dsu.size(1) << '\n';
    return 0;
}
```

也可以单独包含模块，例如 `#include <CP/string/kmp.hpp>`。使用统一入口时会在本地编译全部现有模板；提交展开时仅输出被选中的模块和依赖。

## 现有模块

当前共 **29 个模块**。

| 分类 | 模块 | 头文件 | 主要触发符号 |
|---|---|---|---|
| 基础 | `types` | [types.hpp](include/CP/types.hpp) | `i32`, `i64`, `u32`, `u64`, `i128`, `u128` 等 |
| 基础 | `fastio` | [fastio.hpp](include/CP/fastio.hpp) | `IOS` |
| 数学 | `math_base` | [math/mathbase.hpp](include/CP/math/mathbase.hpp) | `power`, `floor`, `ceil` |
| 数学 | `sieve` | [math/sieve.hpp](include/CP/math/sieve.hpp) | `sieve`, `minp`, `primes` |
| 数学 | `prime` | [math/prime.hpp](include/CP/math/prime.hpp) | `millerRabin`, `pollardRho`, `factorize` |
| 数学 | `modint` | [math/modint.hpp](include/CP/math/modint.hpp) | `Z`, `DynModInt`, `Montgomery`, `power` |
| 数学 | `modcomb` | [math/modcomb.hpp](include/CP/math/modcomb.hpp) | `Comb`, `comb` |
| 数学 | `fft` | [math/fft.hpp](include/CP/math/fft.hpp) | `Cp`, `fft` |
| 数学 | `poly` | [math/poly.hpp](include/CP/math/poly.hpp) | `Poly`, `dft`, `idft`, `proot`, `findPrimitiveRoot` |
| 数学 | `matrix` | [math/matrix.hpp](include/CP/math/matrix.hpp) | `Matrix` |
| 数据结构 | `dsu` | [ds/dsu.hpp](include/CP/ds/dsu.hpp) | `DSU` |
| 数据结构 | `fenwick` | [ds/fenwick.hpp](include/CP/ds/fenwick.hpp) | `Fenwick` |
| 数据结构 | `lazysegmenttree` | [ds/lazysegmenttree.hpp](include/CP/ds/lazysegmenttree.hpp) | `LazySegmentTree` |
| 数据结构 | `sparsetable` | [ds/sparsetable.hpp](include/CP/ds/sparsetable.hpp) | `SparseTable` |
| 字符串 | `kmp` | [string/kmp.hpp](include/CP/string/kmp.hpp) | `kmp` |
| 字符串 | `manacher` | [string/manacher.hpp](include/CP/string/manacher.hpp) | `manacher` |
| 字符串 | `suffixarray` | [string/suffixarray.hpp](include/CP/string/suffixarray.hpp) | `SuffixArray` |
| 网络流 | `maxflow` | [flow/maxflow.hpp](include/CP/flow/maxflow.hpp) | `MaxFlow` |
| 网络流 | `mincostflow` | [flow/mincostflow.hpp](include/CP/flow/mincostflow.hpp) | `MinCostFlow` |
| 图论 | `twosat` | [graph/twosat.hpp](include/CP/graph/twosat.hpp) | `TwoSat` |
| 图论 | `scc` | [graph/scc.hpp](include/CP/graph/scc.hpp) | `SCC` |
| 图论 | `ecc` | [graph/ecc.hpp](include/CP/graph/ecc.hpp) | `ECC` |
| 图论 | `vcc` | [graph/vcc.hpp](include/CP/graph/vcc.hpp) | `VCC` |
| 几何 | `geometry_base` | [geometry/base.hpp](include/CP/geometry/base.hpp) | `PI`, `equal`, `sign`, `Frac`, `Fi`, `quadraticRoots` |
| 几何 | `geometry_point` | [geometry/point.hpp](include/CP/geometry/point.hpp) | `Point`, `Pi`, `Pd`, `dot`, `cross`, `distance` 等 |
| 几何 | `geometry_line` | [geometry/line.hpp](include/CP/geometry/line.hpp) | `Line`, `Li`, `Ld`, `onLine`, `left`, `sameSide` 等 |
| 几何 | `geometry_circle` | [geometry/circle.hpp](include/CP/geometry/circle.hpp) | `Circle`, `Ci`, `Cd`, `circumferencePoint`, `pointToCircle`, `lineCircleIntersection` 等 |
| 几何 | `geometry_polygon` | [geometry/polygon.hpp](include/CP/geometry/polygon.hpp) | `convex`, `diameter`, `triangleArea`, `polygonArea`, `circumcenter`, `incenter` 等 |
| 几何 | `geometry_halfplane` | [geometry/halfplane.hpp](include/CP/geometry/halfplane.hpp) | `inHalfPlane`, `halfPlaneIntersection` |

上表根据当前 `library.json` 和实际头文件整理。完整触发符号列表可用以下命令查看：

```powershell
python D:\Vscode_CPP\.cpp_system\CP\tools\bundle.py --list
```

## 使用约定

- 数据结构、图论和网络流的点下标通常为 `1..n`，整数构造参数传容器大小 `n + 1`，例如 `DSU(n + 1)`、`Fenwick<i64>(n + 1)`。数组构造的 `LazySegmentTree`、`SparseTable` 同样保留下标 `0`。
- `Matrix` 的构造参数是矩阵阶数，行列下标为 `0..n-1`；字符串下标和多项式系数下标从 `0` 开始。`Comb` 的构造参数是预处理上界。
- 区间查询使用闭区间。调用方应保证下标、区间和算法前提合法。
- `i32/i64/i128`、`u32/u64/u128` 是整数别名；`f32` 为 `double`，`f64` 为 `long double`。`EPS = 1e-8`，`inf<T>` 用于支持右移的整数类型。

## 输入输出

标准流可在 `main` 中自行设置加速，如上方示例。快读快写使用 `IOS`：

```cpp
int n;
IOS::cin >> n;
IOS::cout << n << IOS::endl;
```

`IOS::cin` 使用独立的 `fread` 缓冲，不应与 `std::cin` 交替读取同一份输入。没有改写 `cin`、`cout` 等名称的宏。浮点格式可通过 `std::format` 生成字符串后输出。

## 常用接口

### 线性筛

```cpp
sieve(10);
// primes = {2, 3, 5, 7}
// minp[6] = 2, minp[9] = 3
```

`sieve(n)` 要求 `n >= 0`，以 `O(n)` 时间和空间构建 `minp` 和 `primes`。`minp` 大小为 `n + 1`，保存 `x >= 2` 的最小质因子；存在的下标 `0`、`1` 对应值为 `0`。`primes` 按升序保存素数。每次调用都会重置两者，`n < 2` 时素数列表为空。

### KMP

```cpp
auto [positions, fail] = kmp("ababa", "aba");
// positions = {0, 2}
// fail = {0, 0, 1}
```

`kmp(s, t)` 返回匹配起点和模式串的前缀函数，包含重叠匹配。`fail[i]` 表示 `t[0..i]` 最长相等真前缀与真后缀的长度。空模式串匹配文本的所有边界 `0..s.size()`。时间复杂度为 `O(s.size() + t.size())`。

### Manacher

```cpp
auto r = manacher("abba");
int length = *std::max_element(r.begin(), r.end()) - 1;
// length = 4
```

变换串为 `#s[0]#s[1]#...#`，`r` 长度为 `2 * s.size() + 1`。`r[i]` 包含中心，覆盖变换串闭区间 `[i-r[i]+1, i+r[i]-1]`；对应原串回文长度为 `r[i]-1`，起点为 `(i-r[i]+1)/2`。空串返回 `{1}`，输入可以包含 `#`。时间、空间复杂度均为 `O(s.size())`。

### 后缀数组

`SuffixArray(s)` 要求非空字符串，公开 `sa`、`rk`、`lc`，下标均从 `0` 开始。`sa[i]` 是排名为 `i` 的后缀起点，`rk[i]` 是起点 `i` 的后缀排名，`lc[i]` 是 `sa[i]` 与前一个后缀的最长公共前缀长度，`lc[0] = 0`。

### 数学与图结构

- 三参数 `power(base, exp, mod)` 使用整数模快速幂，要求 `exp >= 0`、`mod > 0`；二参数 `power(base, exp)` 使用模整数模块中的泛型快速幂。
- `millerRabin` 判断 `u64` 素数；`pollardRho` 寻找因子；`factorize` 返回升序、含重数的质因子。
- `DynModInt` 使用 Montgomery 表示，模数为小于 `2^31` 的奇数；除法要求除数可逆。默认别名 `Z` 的模数为 `998244353`。组合数和多项式还需满足各自运算的前提。
- `LazySegmentTree<Info, Tag>` 由调用方提供区间合并、信息更新和标记合成。`SparseTable` 的合并操作需满足重叠查询要求，例如最大值、最小值、GCD。
- `MaxFlow`、`MinCostFlow` 使用 `addEdge` 加边、`flow` 求流；原始边信息通过 `eds()` 获取。最小费用流初始网络应不存在源点可达的负费用环。
- `TwoSat::add(u, f, v, g)` 添加 `(x_u == f) or (x_v == g)`；`work()` 判断有解，解保存在 `ans`。`SCC`、`ECC`、`VCC` 分别处理强连通、边双连通和点双连通分量。
- 几何模块依次提供基础数值、点与向量、直线与线段、圆、多边形及半平面交。各头文件中的声明是具体接口的依据。

## 展开与复制

已加载 `cpcopy` 后：

```powershell
cpcopy a.cpp
cpcopy a.cpp -Output submission.cpp
cpcopy a.cpp -NoCompile
```

默认先展开已保存源码，再用 C++23 编译检查，成功后复制到剪贴板。默认展开文件为系统临时目录中的 `CP-submission.cpp`，检查用 EXE 位于 `D:\Vscode_CPP\.output_exe`。`-Output` 可指定展开文件，`-NoCompile` 跳过编译检查。

只生成单文件，不操作剪贴板：

```powershell
python D:\Vscode_CPP\.cpp_system\CP\tools\bundle.py a.cpp -o submission.cpp --explain
```

`--explain` 显示选中模块的原因。统一入口按源码符号选择模块；直接包含模块头文件也会选中该模块。依赖按顺序展开，相同模块只输出一次，已有 `bits/stdc++.h` 时会减少重复标准库头文件。

需要显式指定模块时：

```cpp
// CP: require sieve, kmp
```

或在命令行添加 `--require sieve,kmp`。展开器使用静态符号扫描，宏拼接产生的符号等情况可用显式指定补充。
