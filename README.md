# CP Library

本地 C++23 算法竞赛模板库。开发时通过 `#include <CP/all.h>` 使用模块化模板，提交前由展开器分析源码中实际使用的符号，只展开需要的模块及其依赖，并复制到剪贴板。

## 1. 快速开始

```cpp
#include <bits/stdc++.h>
#include <CP/all.h>

void solve() {
    int n, m;
    std::cin >> n >> m;

    DSU dsu(n + 1);
    while (m--) {
        int u, v;
        std::cin >> u >> v;
        dsu.merge(u, v);
    }
}

int main() {
    io();

    int T = 1;
    std::cin >> T;
    while (T--) solve();

    return 0;
}
```

本地编译：

```powershell
g++ cpp.cpp -std=c++23 -O2 -Wall -Wextra -I D:\Vscode_CPP\.cpp_system\CP\include -o D:\Vscode_CPP\.output_exe\cpp.exe
```

展开、编译检查并复制到剪贴板：

```powershell
cpcopy cpp.cpp
```

在线评测机不存在 `CP/all.h`，提交前必须使用 `cpcopy`，或者手动运行展开器生成单文件代码。

## 2. 目录结构

```text
CP/
├─ include/CP/
│  ├─ all.h
│  ├─ all.hpp
│  ├─ types.hpp
│  ├─ basic.hpp
│  ├─ fastio.hpp
│  ├─ math/
│  ├─ ds/
│  ├─ flow/
│  ├─ graph/
│  ├─ string/
│  └─ geometry/
├─ tools/
│  ├─ bundle.py
│  ├─ cpcopy.ps1
│  └─ test.ps1
├─ library.json
└─ README.md
```

`library.json` 保存模块的头文件、触发符号和依赖关系。`bundle.py` 根据它决定提交代码中应展开哪些模块。

## 3. 通用约定

### 3.1 C++ 版本

所有模板按照 C++23 编译：

```powershell
g++ -std=c++23
```

### 3.2 下标与构造长度

数据结构和网络流统一使用 `1..n` 的点或数组下标。接收整数长度的构造函数传入的是容器大小 `n + 1`：

```cpp
DSU dsu(n + 1);
Fenwick<i64> fenwick(n + 1);
LazySegmentTree<Info, Tag> segmentTree(n + 1);

MaxFlow<i64> maxFlow(n + 1);
MinCostFlow<i64> minCostFlow(n + 1);
StoerWagner stoerWagner(n + 1);
GomoryHuTree<i64> gomoryHuTree(n + 1);

TwoSat twoSat(n + 1);
SCC scc(n + 1);
ECC ecc(n + 1);
VCC vcc(n + 1);
BlockCutTree blockCutTree(n + 1);
SegtGraph::Graph graph(n + 1);

HopcroftKarp hopcroftKarp(leftSize + 1, rightSize + 1);
Hungarian hungarian(leftSize + 1, rightSize + 1);
KM<i64> km(n + 1);
```

接收数组的模板要求数组本身也是 1-indexed，大小为 `n + 1`，其中 `values[0]` 不参与维护：

```cpp
std::vector<Info> values(n + 1);
LazySegmentTree<Info, Tag> segmentTree(values);

std::vector<int> a(n + 1);
SparseTable<int, Max> sparseTable(a);
```

模板默认假设 `n >= 1`、下标和区间合法，不额外检查边界。

### 3.3 头文件使用方式

通常统一包含：

```cpp
#include <bits/stdc++.h>
#include <CP/all.h>
```

也可以只包含单个模块：

```cpp
#include <CP/ds/dsu.hpp>
#include <CP/geometry/point.hpp>
```

使用 `bits/stdc++.h` 时，展开器不会重复输出模板内部已经被覆盖的标准库头文件。

## 4. 展开与复制

### 4.1 `cpcopy`

```powershell
cpcopy cpp.cpp
```

执行流程：

1. 读取已保存的源文件。
2. 检测源文件实际使用的 CP 符号。
3. 展开对应模块及依赖。
4. 使用 C++23 编译展开后的代码。
5. 编译成功后复制到剪贴板。

指定展开文件位置：

```powershell
cpcopy cpp.cpp -Output submission.cpp
```

跳过编译检查：

```powershell
cpcopy cpp.cpp -NoCompile
```

`cpcopy` 读取磁盘文件，不读取 VS Code 中尚未保存的编辑器缓冲区，执行前应先保存文件。

### 4.2 直接使用展开器

```powershell
python D:\Vscode_CPP\.cpp_system\CP\tools\bundle.py cpp.cpp -o submission.cpp --explain
```

`--explain` 会打印每个模块被选中的原因。

查看所有模块和触发符号：

```powershell
python D:\Vscode_CPP\.cpp_system\CP\tools\bundle.py --list
```

静态扫描无法识别宏拼接产生的符号时，可以在源文件中显式指定模块：

```cpp
// CP: require dsu, fenwick
```

也可以从命令行指定：

```powershell
python D:\Vscode_CPP\.cpp_system\CP\tools\bundle.py cpp.cpp -o submission.cpp --require dsu,fenwick
```

## 5. 基础模块

### 5.1 整数与浮点别名

头文件：`CP/types.hpp`

| 名称 | 类型 |
|---|---|
| `i32` | `int` |
| `i64` | `long long` |
| `i128` | `__int128_t` |
| `u32` | `unsigned int` |
| `u64` | `unsigned long long` |
| `u128` | `__uint128_t` |
| `f32` | `double` |
| `f64` | `long double` |

整数无穷大变量模板：

```cpp
template <typename T>
inline constexpr T inf = std::numeric_limits<T>::max() >> 2;
```

例如 `inf<int>`、`inf<i64>`，适用于支持右移运算的整数类型。

### 5.2 标准流加速

头文件：`CP/basic.hpp`

```cpp
void io();
```

等价于：

```cpp
std::cin.tie(nullptr)->std::ios_base::sync_with_stdio(false);
```

使用标准流时在 `main` 开头调用一次：

```cpp
int main() {
    io();
    std::cin >> x;
    std::cout << x << '\n';
}
```

### 5.3 `IOS` 快读快写

头文件：`CP/fastio.hpp`

```cpp
IOS::cin >> x >> s;
IOS::cout << x << ' ' << s << IOS::endl;
```

公开接口：

```cpp
IOS::cin
IOS::cout
IOS::IO
IOS::endl
```

支持读取 `char`、`char *`、`std::string`、整数和浮点数；支持输出字符、C 字符串、`std::string`、普通整数和浮点数。

指定浮点输出格式时使用 `std::format`：

```cpp
IOS::cout << std::format("{:.8f}", value) << IOS::endl;
```

`IOS` 没有定义 `cin`、`cout`、`endl` 宏，不会改写 `std::cin` 等名称。

不要交替使用 `std::cin` 和 `IOS::cin` 读取同一个输入。`IOS::cin` 使用独立的 `fread` 缓冲区，即使标准流没有关闭同步，也可能预读后续数据。建议整份程序选择以下任意一套：

```cpp
io();
std::cin >> x;
std::cout << x << '\n';
```

或：

```cpp
IOS::cin >> x;
IOS::cout << x << IOS::endl;
```

## 6. 数学与模运算

### 6.1 Math Base

头文件：`CP/math/base.hpp`

取模快速幂：

```cpp
i64 power(i64 base, i64 exp, i64 mod);
```

支持负底数，返回 `[0, mod)` 内的结果。要求 `exp >= 0`、`mod > 0`，乘法中间值使用 `i128`。

展开器按参数数量识别：三参数 `power(base, exp, mod)` 展开 `math_base`。

整数向下和向上取整：

```cpp
template <typename T, typename U>
constexpr auto Floor(T a, U b);

template <typename T, typename U>
constexpr auto Ceil(T a, U b);
```

返回实数 `a / b` 的向下或向上取整，支持 `i64`、`i128` 以及二者混用，分子和分母均可为正数或负数，要求 `b != 0`。返回类型由两个参数的除法结果类型决定。

扩展欧几里得：

```cpp
i64 exgcd(i64 a, i64 b, i64 &x, i64 &y);
```

返回 `gcd(a, b)`，并令 `a * x + b * y = gcd(a, b)`。

正整数二元一次不定方程：

```cpp
auto exgcdFunction(i64 a, i64 b, i64 w);
```

求 `a * x + b * y = w` 的正整数解，要求 `a > 0`、`b > 0` 且 `w` 能被 `gcd(a, b)` 整除。返回：

```cpp
(status, count, xmin, xmax, ymin, ymax, dx, dy)
```

- `status == 2`：存在正整数解；`count` 是解数，`x` 从 `xmin` 到 `xmax` 每次增加 `dx`，对应的 `y` 从 `ymax` 到 `ymin` 每次减少 `dy`。
- `status == 1`：不存在正整数解；`count` 和无意义的区间字段为 `-1`，`xmin`、`ymin` 保留越过正整数边界时的参考值。

扩展中国剩余定理：

```cpp
i64 excrt(const std::vector<std::pair<i64, i64>> &equations);
```

`equations` 使用 1-indexed，`equations[i] = {residue, modulus}` 表示 `x ≡ residue (mod modulus)`。有解时返回最小非负解，无解返回 `-1`；要求合并后的模数能放入 `i64`。

高斯消元：

```cpp
template <class T>
auto gauss(std::vector<std::vector<T>> function);
```

增广矩阵使用 1-indexed：第 `1..n` 行有效，第 `1..m-1` 列是变量系数，第 `m` 列是常数项，行列 `0` 不使用。`T` 应为浮点类型。返回 `(status, answer)`：

| status | 含义 |
|---:|---|
| `0` | 无解，`answer` 为空 |
| `1` | 唯一解 |
| `2` | 无穷多解，`answer` 返回一组令自由变量为 `0` 的解 |

浮点判零使用全局常量 `EPS = 1e-8`。

### 6.2 素数判定与整数分解

头文件：`CP/math/prime.hpp`

```cpp
bool MillerRabin(u64 n);
u64 PollardRho(u64 n);
std::vector<u64> factorize(u64 n);
```

`MillerRabin(n)` 对完整 `u64` 范围进行确定性素数判定。`PollardRho(n)` 对合数返回一个非平凡因子，对素数返回自身。`factorize(n)` 返回带重数且从小到大排列的全部质因子；`n <= 1` 时返回空数组。

```cpp
auto factors = factorize(360);
// factors = {2, 2, 2, 3, 3, 5}
```

模乘使用 `u128`，Pollard--Rho 使用 Brent 循环与批量 `gcd`。

### 6.3 Montgomery 模整数

头文件：`CP/math/modint.hpp`

泛型快速幂：

```cpp
template <class T>
constexpr T power(T base, i64 exp);
```

`exp` 应为非负整数，`T` 需要支持乘法和从整数 `1` 构造。

展开器按参数数量识别：二参数 `power(base, exp)` 展开 `modint`。

```cpp
template <u32 P>
struct DynModInt;
```

模数必须是奇数且小于 `2^31`。

底层 Montgomery 约减接口：

```cpp
struct Montgomery {
    u32 m;
    u32 ninv;
    u32 r2;

    explicit Montgomery(u32 modulus = 1);
    u32 reduce(u64 value) const;
    u32 mul(u32 a, u32 b) const;
    u32 to_mont(u32 value) const;
    u32 from_mont(u32 value) const;
};
```

一般代码直接使用 `DynModInt` 或 `Z`，不需要手动操作 Montgomery 表示。

默认模数及别名：

```cpp
constexpr u32 P = 998'244'353;
using Z = DynModInt<P>;
```

基本使用：

```cpp
Z a = 10;
Z b = -3;
Z c = a * b + 7;

std::cout << c << '\n';
```

公开接口：

```cpp
DynModInt();
DynModInt(integer);

static void setMod(u32 modulus);
static constexpr u32 mod();
constexpr u32 val() const;

DynModInt inv() const;

operator +=, -=, *=, /=
operator +, -, *, /
operator ++, --
operator ==, <=>
operator >>, <<
```

除法通过逆元实现，除数必须与模数互质。

运行时设置模数：

```cpp
using M = DynModInt<0>;
M::setMod(modulus);
M x = value;
```

同一个 `DynModInt<P>` 类型共享模数状态。

### 6.4 FFT

头文件：`CP/math/fft.hpp`

复数类型：

```cpp
struct Cp {
    f64 x, y;
};
```

支持复数的 `+`、`-`、`*`。

变换接口：

```cpp
void FFT(std::vector<Cp> &a, int flag);
```

- `a.size()` 必须是 2 的幂。
- `flag = 1` 表示正变换，`flag = -1` 表示逆变换。
- 逆变换不会自动除以数组长度。

整数卷积：

```cpp
std::vector<i64> mul(std::vector<Cp> &a, std::vector<Cp> &b);
```

返回长度为 `a.size() + b.size() - 1` 的整数卷积。函数会扩充并修改 `a`、`b`。结果使用 `llround` 四舍五入，适用于卷积结果没有超出浮点精度范围的情况；大系数应拆系数或使用 NTT。

### 6.5 多项式

头文件：`CP/math/poly.hpp`

NTT 接口：

```cpp
template <u32 P>
void dft(std::vector<DynModInt<P>> &a);

template <u32 P>
void idft(std::vector<DynModInt<P>> &a);
```

数组长度必须是 2 的幂并整除 `DynModInt<P>::mod() - 1`。`idft` 会完成逆变换和长度归一化。

多项式类型固定使用 `Z = DynModInt<998244353>`，系数从常数项开始按 `0..size-1` 保存：

```cpp
struct Poly : public std::vector<Z>;
```

构造方式：

```cpp
Poly();
Poly(int size);
Poly(int size, Z value);
Poly(std::vector<Z> values);
Poly{a0, a1, a2};
```

这里的整数参数是多项式系数数量，不使用 `n + 1` 约定。

公开运算：

```cpp
Poly shift(int k) const;
Poly trunc(int size) const;

Poly deriv() const;
Poly integr() const;
Poly inv(int size) const;
Poly log(int size) const;
Poly exp(int size) const;
Poly pow(int exponent, int size) const;
Poly sqrt(int size) const;
Poly mulT(Poly b) const;

std::vector<Z> eval(std::vector<Z> points) const;
```

同时支持多项式 `+ - *`、一元负号、乘除标量及对应复合赋值。

- 乘法较短时使用朴素卷积，较长且模数支持当前变换长度时使用 NTT。
- `inv(size)` 要求常数项非零。
- `log(size)` 要求常数项为 `1`。
- `exp(size)` 要求常数项为 `0`。
- `pow(exponent, size)` 要求指数为非负整数。
- `sqrt(size)` 当前要求常数项为 `1`，返回常数项为 `1` 的平方根。
- `eval(points)` 返回多项式在各点的值。

### 6.6 ModComb

头文件：`CP/math/modcomb.hpp`

使用现有 `Z` 预处理阶乘、逆阶乘和逆元：

```cpp
Comb combination(maximum);

Z factorial = combination.fac(n);
Z inverseFactorial = combination.invfac(n);
Z inverse = combination.inv(n);
Z value = combination.binom(n, m);
```

也可以直接使用全局对象：

```cpp
Z value = comb.binom(n, m);
```

公开接口：

```cpp
Comb();
explicit Comb(int maximum);

void reset();
void init(int maximum);

Z fac(int n);
Z invfac(int n);
Z inv(int n);
Z binom(int n, int m);
```

构造参数和 `init` 参数表示预处理到的最大值，不使用 `n + 1` 约定。查询会按需扩充缓存，但最多预处理到 `Z::mod() - 1`。要求当前模数为质数。`binom` 在 `n < m`、`m < 0` 或 `n >= Z::mod()` 时返回 `0`。

如果调用 `Z::setMod` 改变模数，原缓存立即失效，继续查询前必须调用 `reset()`。

### 6.7 Matrix

头文件：`CP/math/matrix.hpp`

```cpp
Matrix<> matrix(n);
matrix[i][j] = value;

Matrix<i64> sum = a + b;
Matrix<i64> product = a * b;
Matrix<i64> result = Matrix<i64>::Power(matrix, exponent);
```

模意义下求逆：

```cpp
Matrix<Z> matrix(n);
auto inverse = matrix.inv();

if (inverse) {
    Matrix<Z> identity = matrix * *inverse;
}
```

`inv()` 使用 Gauss--Jordan 消元，可逆时返回逆矩阵，奇异矩阵返回 `std::nullopt`。元素类型必须支持四则运算并构成域；用于模意义时要求模数为质数。

当前模板维护 `n × n` 方阵，行列均使用 `0..n-1`，构造参数就是矩阵阶数，不使用 `n + 1` 约定。支持 `[]`、`+`、`*`、`+=`、`*=`、非负整数快速幂和逆矩阵。矩阵乘法和求逆复杂度均为 `O(n^3)`。

## 7. 数据结构

### 7.1 DSU

头文件：`CP/ds/dsu.hpp`

```cpp
DSU dsu(n + 1);
```

公开接口：

```cpp
int find(int x);
bool same(int x, int y);
void merge(int x, int y);
int size(int x);
```

示例：

```cpp
DSU dsu(n + 1);
dsu.merge(1, 2);

if (dsu.same(1, 2)) {
    std::cout << dsu.size(1) << '\n';
}
```

`find` 使用路径压缩。当前 `merge` 将 `x` 的根连接到 `y` 的根，不按大小合并。

### 7.2 Fenwick

头文件：`CP/ds/fenwick.hpp`

```cpp
Fenwick<T> fenwick(n + 1);
```

公开接口：

```cpp
void add(int x, T value);
T prefixSum(int x) const;
T rangeSum(int l, int r) const;
```

示例：

```cpp
Fenwick<i64> fenwick(n + 1);
fenwick.add(3, 10);

i64 prefix = fenwick.prefixSum(5);
i64 range = fenwick.rangeSum(2, 5);
```

单点加和前缀/区间求和复杂度均为 `O(log n)`。

### 7.3 LazySegmentTree

头文件：`CP/ds/lazysegmenttree.hpp`

```cpp
template <class Info, class Tag>
struct LazySegmentTree;
```

构造与初始化：

```cpp
LazySegmentTree<Info, Tag> tree(n + 1, initialValue);
LazySegmentTree<Info, Tag> tree(values);

tree.init(n + 1, initialValue);
tree.init(values);
```

`values` 必须大小为 `n + 1`，`values[0]` 不使用。

公开操作：

```cpp
void modify(int x, const Info &value);
void rangeModify(int l, int r, const Tag &value);
Info rangeQuery(int l, int r);

template <class F>
int findFirst(int l, int r, F pred);

template <class F>
int findLast(int l, int r, F pred);
```

`Info` 和 `Tag` 必须提供：

```cpp
Info operator+(const Info &lhs, const Info &rhs);
void Info::apply(const Tag &value);
void Tag::apply(const Tag &value);
```

同时要求：

- `Info()` 是区间合并的单位元。
- `Tag()` 是空标记。
- `Tag::apply(newTag)` 表示在旧标记之后复合新标记。
- `Info::apply(tag)` 将标记作用到整个节点信息。

区间加、区间和示例：

```cpp
struct Tag {
    i64 add = 0;

    void apply(const Tag &value) {
        add += value.add;
    }
};

struct Info {
    i64 sum = 0;
    int len = 0;

    void apply(const Tag &value) {
        sum += value.add * len;
    }
};

Info operator+(const Info &lhs, const Info &rhs) {
    return {lhs.sum + rhs.sum, lhs.len + rhs.len};
}

std::vector<Info> values(n + 1);
for (int i = 1; i <= n; ++i) values[i] = {a[i], 1};

LazySegmentTree<Info, Tag> tree(values);
tree.rangeModify(2, 5, Tag{3});
std::cout << tree.rangeQuery(1, n).sum << '\n';
```

`findFirst/findLast` 使用 `pred(info)` 剪枝。谓词必须满足：如果某个区间内可能存在答案，则该区间的聚合信息上 `pred` 必须返回 `true`。

### 7.4 SparseTable

头文件：`CP/ds/sparsetable.hpp`

```cpp
template <class T, class Op>
struct SparseTable;
```

`Op` 必须是可调用对象：

```cpp
T Op::operator()(const T &x, const T &y) const;
```

构造和查询：

```cpp
SparseTable<T, Op> table(values, operation);
table.init(values);
T answer = table.query(l, r);
```

当前查询使用两个可能重叠的长度为 `2^k` 的区间，因此 `Op` 应满足结合律和幂等律，适用于 `min`、`max`、`gcd`，不适用于普通区间求和。

示例：

```cpp
struct Max {
    int operator()(int x, int y) const {
        return std::max(x, y);
    }
};

std::vector<int> a(n + 1);
SparseTable<int, Max> table(a);
int maximum = table.query(l, r);
```

预处理复杂度 `O(n log n)`，查询复杂度 `O(1)`。

### 7.5 DynHeap

头文件：`CP/ds/dynheap.hpp`

```cpp
DynHeap<T, Compare> heap(maximumInsertCount + 1);
```

左偏树实现的可并堆。默认 `Compare = std::less<T>`，即小根堆；使用 `std::greater<T>` 时为大根堆。每次 `insert` 创建一个独立的单元素堆并返回永久编号，容量按整个程序的 `insert` 总次数确定，删除不会复用编号。

公开接口：

```cpp
int insert(const T &value);
bool merge(int x, int y);
std::optional<std::pair<T, int>> top(int x);
std::optional<std::pair<T, int>> pop(int x);
bool erased(int x) const;
int size(int x);
```

`x` 和 `y` 是插入时返回的元素编号。`merge(x,y)` 合并两个编号当前所在的堆；如果任一编号已删除或已经属于同一个堆，则返回 `false`。`top/pop` 返回 `(极值, 编号)`；传入编号已删除时返回 `std::nullopt`。数值相同时优先选择编号更小的元素。

```cpp
DynHeap<int> heap(n + 1);

for (int i = 1; i <= n; i++) {
    int value;
    std::cin >> value;
    heap.insert(value);
}

heap.merge(x, y);

if (auto result = heap.pop(x)) {
    std::cout << result->first << '\n';
} else {
    std::cout << -1 << '\n';
}
```

`insert` 为 `O(1)`，`merge` 和 `pop` 为 `O(log n)`，`top`、`size` 的并查集查找均摊接近 `O(1)`。

## 8. 网络流与割

所有网络流模板使用点 `1..n`，构造时传 `n + 1`。

### 8.1 MaxFlow

头文件：`CP/flow/maxflow.hpp`

Dinic 最大流：

```cpp
MaxFlow<i64> graph(n + 1);
graph.addEdge(u, v, capacity);
i64 answer = graph.flow(source, sink);
```

公开接口：

```cpp
MaxFlow();
explicit MaxFlow(int _n);
void init(int _n);

void addEdge(int u, int v, T cap);
T flow(int s, int t);
std::vector<bool> minCut() const;
std::vector<Edge> edges() const;
```

`addEdge` 添加有向边。无向容量边通常需要添加两个方向：

```cpp
graph.addEdge(u, v, cap);
graph.addEdge(v, u, cap);
```

`minCut()` 应在 `flow()` 完成后调用，返回最终残量网络中从源点可达的点集。

Dinic 在一般图上的常用最坏复杂度为 `O(n^2 m)`。

公开边结构：

```cpp
struct Edge {
    int from;
    int to;
    T cap;
    T flow;
};
```

`edges()` 返回所有原始有向边的初始容量和当前流量。

### 8.2 MinCostFlow

头文件：`CP/flow/mincostflow.hpp`

```cpp
MinCostFlow<i64> graph(n + 1);
graph.addEdge(u, v, capacity, cost);
auto [maxFlow, minCost] = graph.flow(source, sink);
```

公开接口：

```cpp
MinCostFlow();
explicit MinCostFlow(int _n);
void init(int _n);

void addEdge(int u, int v, T cap, T cost);
std::pair<T, T> flow(int s, int t);
std::vector<Edge> edges() const;
```

`flow` 计算最小费用最大流，没有单独限制目标流量的参数。

公开边结构：

```cpp
struct Edge {
    int from;
    int to;
    T cap;
    T cost;
    T flow;
};
```

初始边允许负费用，但要求从源点可达的初始残量网络不存在负费用环。第一轮使用优先队列重复松弛来建立势能：实际数据通常较快，但负边情况下第一轮没有标准 Dijkstra 的最坏复杂度保证。若初始图是 DAG，可改用拓扑最短路初始化势能；若需要严格的一般图复杂度，可使用 Bellman–Ford 初始化势能。

初始约化费用非负时，逐次最短增广路的常用复杂度记为 `O(F m log n)`，其中 `F` 是整数总流量上界。

### 8.3 StoerWagner

头文件：`CP/flow/stoerwagner.hpp`

无向图全局最小割：

```cpp
StoerWagner graph(n + 1);
graph.addEdge(u, v, weight);
i64 answer = graph.minCut();
```

公开接口：

```cpp
explicit StoerWagner(int _n);
void addEdge(int u, int v, i64 weight);
i64 minCut();
```

支持重边，权值会累加。`minCut()` 会收缩并修改内部图，通常每个对象只调用一次。复杂度为 `O(n^3)`。

### 8.4 GomoryHuTree

头文件：`CP/flow/gomoryhutree.hpp`

无向图任意两点最小割树：

```cpp
GomoryHuTree<i64> tree(n + 1);
tree.addEdge(u, v, weight);
tree.build();

i64 answer = tree.query(u, v);
```

公开接口：

```cpp
explicit GomoryHuTree(int _n);
void addEdge(int u, int v, T weight);
void build();
T query(int u, int v) const;
```

`addEdge` 已按无向边处理，不需要手动添加反向边。所有边添加完成后调用一次 `build()`，随后可以多次 `query()`。`query(u, v)` 返回原无向图中 `u` 与 `v` 的最小割值。

当前实现预处理复杂度为 `O(n^3 m)`，单次查询复杂度为 `O(log n)`。

## 9. 图论

图论模板统一使用 `1..n` 编号，构造时传入容器大小 `n + 1`。

### 9.1 TwoSat

头文件：`CP/graph/twosat.hpp`

```cpp
TwoSat twoSat(n + 1);
twoSat.add(u, f, v, g);
bool possible = twoSat.work();
```

`add(u, f, v, g)` 添加条件 `(x_u == f) or (x_v == g)`。`work()` 返回是否有解；有解时 `ans[i]` 是变量 `i` 的一组取值。复杂度为 `O(n + m)`。

### 9.2 HopcroftKarp

头文件：`CP/graph/bipartite.hpp`

```cpp
HopcroftKarp graph(leftSize + 1, rightSize + 1);
graph.addEdge(u, v);
int matching = graph.work();
auto edges = graph.get();
```

左右点分别使用 `1..leftSize` 和 `1..rightSize`。`work()` 返回最大匹配大小，`get()` 返回匹配边，复杂度为 `O(sqrt(V) E)`。同一对象通常只调用一次 `work()`。

### 9.3 KM

头文件：`CP/graph/bipartite.hpp`

```cpp
KM<i64> graph(n + 1);
graph.addEdge(x, y, weight);
i64 answer = graph.work();
auto edges = graph.get();
```

求两侧大小均为 `n` 的最大权完美匹配，复杂度为 `O(n^3)`。重边保留最大权值。原图必须存在完美匹配；否则需要加入权值为 `0` 的虚拟点或虚拟边，并从结果中去掉它们。

### 9.4 Hungarian

头文件：`CP/graph/hungarian.hpp`

```cpp
Hungarian graph(leftSize + 1, rightSize + 1);
graph.addEdge(u, v);
int matching = graph.work();
auto edges = graph.edges();
```

使用增广路求二分图最大匹配，左右点均为 1-indexed，复杂度为 `O(VE)`。

### 9.5 SCC

头文件：`CP/graph/scc.hpp`

```cpp
SCC graph(n + 1);
graph.add(u, v);
auto [count, edge, belong, size] = graph.work();
```

`add` 添加有向边。返回强连通分量数量、缩点后的有向图、每个原点所属分量和各分量大小。分量编号为 `1..count`，缩点图可能包含重边。复杂度为 `O(n + m)`。

### 9.6 ECC

头文件：`CP/graph/ecc.hpp`

```cpp
ECC graph(n + 1);
graph.add(u, v);
auto [count, tree, id, size] = graph.work();
```

`add` 添加无向边，支持重边。返回边双连通分量数量、缩点图、原点所属分量和分量大小。公开成员 `bridge` 保存 DFS 方向下发现的桥；缩点图的每条桥会保存两个方向。

### 9.7 VCC

头文件：`CP/graph/vcc.hpp`

```cpp
VCC graph(n + 1);
graph.add(u, v);
graph.work();
```

点双连通分量保存在 `component[1..count]`，`isCut[u]` 表示 `u` 是否为割点。孤立点单独形成一个点双。

### 9.8 BlockCutTree

头文件：`CP/graph/blockcuttree.hpp`

```cpp
BlockCutTree graph(n + 1);
graph.addEdge(u, v);
auto [blockCount, edges] = graph.work();
```

原点编号保持 `1..n`，每个点双对应一个方点，编号为 `n + 1..n + blockCount`。返回方点数量和圆方树的无向边；每条边在结果中只出现一次。输入允许重边。

### 9.9 SegtGraph

头文件：`CP/graph/segtgraph.hpp`

```cpp
SegtGraph::Graph graph(n + 1);

graph.addPointToPoint(u, v, weight);
graph.addPointToSeg(u, l, r, weight);
graph.addSegToPoint(l, r, v, weight);

int node = graph.get(position);
const auto &edge = graph.edge;
```

公开接口：

```cpp
void addPointToPoint(int u, int v, i64 weight = 1);
void addPointToSeg(int u, int l, int r, i64 weight = 1);
void addSegToPoint(int l, int r, int v, i64 weight = 1);
int get(int position) const;
```

`get(position)` 返回原点在优化图中的实际节点编号。`edge` 是构造后图的带权邻接表，可以直接交给 Dijkstra 等算法。每次点到区间或区间到点连边增加 `O(log n)` 条边。

### 9.10 BlossomTree

头文件：`CP/graph/blossomtree.hpp`

一般图最大权匹配：

```cpp
BlossomTree<i64> blossom(n + 1);

std::vector<std::tuple<int, int, i64>> edges;
edges.emplace_back(u, v, weight);

auto [maximumWeight, match] = blossom.work(edges);
```

公开接口：

```cpp
template <typename T = i64>
struct BlossomTree;

explicit BlossomTree(int _n);

std::pair<T, std::vector<int>> work(
    const std::vector<std::tuple<int, int, T>> &edges
);
```

点编号为 `1..n`，构造时传 `n + 1`。`match[u]` 是与 `u` 匹配的点，未匹配时为 `0`。允许重边，同一对端点只保留最大权值；非正权边不会进入最终匹配。算法允许点不匹配，返回所有匹配中的最大总权值，复杂度为 `O(n^3)`，空间复杂度为 `O(n^2)`。

`T` 应使用带符号整数类型。算法依赖精确的相等判断和除以 `2`，不适合直接使用浮点数。

## 10. 平面几何

几何模块使用全局函数和类型，不放在命名空间内。

通用约定：

- `Pi`、`Li`、`Ci` 使用 `i64`。
- `Pd`、`Ld`、`Cd` 使用 `f64 = long double`。
- 浮点判断使用 `EPS = 1e-8`、`Equal` 和 `Sign`。
- `Point` 的排序与相等运算使用精确比较，不使用 `EPS`。
- 整数点积、叉积和平方距离可能溢出，大坐标时使用更宽类型。
- 没有特别说明时，多边形顶点不重复首点。

### 10.1 Geometry Base

头文件：`CP/geometry/base.hpp`

常量：

```cpp
PI
EPS
```

浮点判断：

```cpp
template <class T, class S>
bool Equal(T x, S y);

template <class T>
int Sign(T x);
```

`Sign(x)` 返回 `-1`、`0`、`1`。

精确分数：

```cpp
template <class T>
struct Frac {
    T x, y;

    Frac(T numerator = 0, T denominator = 1);
    explicit operator f64() const;
};

using Fi = Frac<i64>;
```

支持 `+ - * /`、复合赋值、负号、`< > == !=` 和输出。`T` 必须是有符号整数，分母必须非零；运算过程中仍可能发生整数溢出。

方程求根：

```cpp
std::tuple<int, f64, f64> QuadraticRoots(f64 a, f64 b, f64 c);
```

兼容一次方程。返回实根个数和两个根；一个实根时两个根值相同。

### 10.2 Point 与平面向量

头文件：`CP/geometry/point.hpp`

```cpp
template <class T>
struct Point {
    T x, y;
};

using Pi = Point<i64>;
using Pd = Point<f64>;
```

支持：

```cpp
Point + Point
Point - Point
Point + scalar
Point - scalar
Point * scalar
scalar * Point
Point / scalar
-Point
```

同时支持字典序比较和 `std::istream/std::ostream`。

向量接口：

```cpp
T Dot(Point<T> a, Point<T> b);
T Dot(Point<T> p1, Point<T> p2, Point<T> p0);

T Cross(Point<T> a, Point<T> b);
T Cross(Point<T> p1, Point<T> p2, Point<T> p0);
```

三点版本表示 `(p1 - p0)` 与 `(p2 - p0)` 的点积或叉积。

距离与变换：

```cpp
f64 Distance(T x1, T y1, T x2, T y2);
f64 Distance(Point<T> a, Point<T> b);

T Ex_Distance(T x1, T y1, T x2, T y2);
T Ex_Distance(Point<T> a, Point<T> b);

T Manhattan_Distance(Point<T> a, Point<T> b);

Pd Standardize(Pd vector);
f64 ToDeg(f64 radians);
f64 ToArc(f64 degrees);
f64 CosinesRule(f64 a, f64 b, f64 c);
f64 VectorAngle(Pd a, Pd b, bool absolute = true);
Pd Rotate(Pd p, Pd center, f64 radians, bool clockwise = false);
Point<T> Rotate90(Point<T> p, Point<T> center = { }, bool clockwise = false);
```

- `Ex_Distance` 返回距离平方。
- `Standardize` 要求非零向量。
- `VectorAngle` 默认返回无符号夹角；`absolute=false` 时返回从 `a` 到 `b` 的有向角。
- `Rotate` 默认逆时针旋转。

### 10.3 Line 与线段

头文件：`CP/geometry/line.hpp`

```cpp
template <class T>
struct Line {
    Point<T> a, b;
};

using Li = Line<i64>;
using Ld = Line<f64>;
```

位置关系：

```cpp
bool Online(Point<T> a, Point<T> b, Point<T> c);
bool Online(Point<T> p, Line<T> line);
bool Left(Point<T> p, Line<T> line);
bool SameSide(Point<T> p1, Point<T> p2, Line<T> line);
bool DifferentSide(Point<T> p1, Point<T> p2, Line<T> line);
bool Parallel(Line<T> l1, Line<T> l2);
bool Vertical(Line<T> l1, Line<T> l2);
```

`Online(a, b, c)` 判断 `a` 是否在直线 `bc` 上。`Left(p, line)` 的方向由 `line.a -> line.b` 决定。

直线与线段操作：

```cpp
Pd LineIntersection(Ld l1, Ld l2);
std::pair<Pd, f64> Project(Pd p, Ld line);
bool Onsegment(Point<T> p, Line<T> segment, bool endPointIncluded = true);
std::pair<Pd, f64> PointToSegment(Pd p, Ld segment);
Ld Median(Ld segment);
bool SegmentIntersect(Line<T> l1, Line<T> l2);
```

- `LineIntersection` 调用前必须确认两直线不平行。
- `Project` 返回垂足和点到直线距离，直线不能退化为一点。
- `PointToSegment` 返回线段上的最近点和距离。
- `Median` 返回线段中垂线，线段不能退化为一点。
- `SegmentIntersect` 判断两闭线段是否相交。

带交点信息的线段相交：

```cpp
std::tuple<int, Pd, Pd> SegmentIntersection(Ld l1, Ld l2);
```

返回类型：

| type | 含义 |
|---:|---|
| `0` | 无交点 |
| `1` | 一个严格内部交点 |
| `2` | 共线且重叠为线段，后两个值是重叠端点 |
| `3` | 端点相交，或共线重叠为一个点 |

直线表示转换：

```cpp
f64 Slope(Ld line);
Frac<T> FracSlope(Point<T> a, Point<T> b);
std::array<T, 3> GeneralLine(Line<T> line);
Ld GeneralLine(f64 A, f64 B, f64 C);
```

- `Slope` 要求非竖直直线。
- `FracSlope` 用整数分数表示精确斜率，要求 `a.x != b.x`。
- 数组版本返回 `A, B, C`，表示 `Ax + By = C`。
- 浮点版本把 `Ax + By = C` 转换为两点式，要求 `A`、`B` 不同时为零。

### 10.4 Circle

头文件：`CP/geometry/circle.hpp`

```cpp
template <class T>
struct Circle {
    Point<T> O;
    T R;
};

using Ci = Circle<i64>;
using Cd = Circle<f64>;
```

圆和点：

```cpp
Pd CircumferencePoint(Cd circle, f64 radians);
std::pair<Pd, f64> PointToCircle(Pd p, Cd circle);
std::vector<Pd> PointCircleTangents(Pd p, Cd circle);
```

- `CircumferencePoint` 返回圆上指定极角的点。
- `PointToCircle` 返回圆周上的最近点和距离。
- `PointCircleTangents` 返回点到圆的切点，圆内、圆上、圆外分别返回 `0/1/2` 个点。

交点：

```cpp
std::tuple<int, Pd, Pd> LineCircleIntersection(Ld line, Cd circle);
std::tuple<int, Pd, Pd> SegmentCircleIntersection(Ld segment, Cd circle);
std::tuple<int, Pd, Pd> CircleCircleIntersection(Cd c1, Cd c2);
```

直线圆和线段圆返回 `0/1/2` 个交点。`LineCircleIntersection` 要求直线不退化。

两圆相交返回：

| type | 含义 |
|---:|---|
| `-1` | 两圆重合 |
| `0` | 无交点 |
| `1` | 相切 |
| `2` | 两个交点 |

其他接口：

```cpp
f64 CircleIntersectionArea(Cd c1, Cd c2);
std::pair<bool, Cd> CircleFromThreePoints(Pd a, Pd b, Pd c);
std::pair<int, std::vector<std::pair<Pd, Pd>>> CommonTangents(Cd c1, Cd c2);
```

- `CircleFromThreePoints` 在三点共线时返回 `{false, {}}`。
- `CommonTangents` 返回公切线数量和每条切线在两个圆上的切点；重合圆返回数量 `-1`。
- 圆相关函数默认半径非负。

### 10.5 Polygon

头文件：`CP/geometry/polygon.hpp`

凸包：

```cpp
std::vector<Point<T>> Convex(std::vector<Point<T>> points);
T Diameter(const std::vector<Point<T>> &convex);
int PointInConvex(Point<T> p, const std::vector<Point<T>> &convex);
std::vector<Point<T>> MinkowskiSum(std::vector<Point<T>> a, std::vector<Point<T>> b);
```

- `Convex` 返回逆时针严格凸包，去重并删除边上的共线中间点，不重复首点。
- `Diameter` 返回凸包直径平方，输入应是逆时针凸包。
- `PointInConvex` 要求逆时针严格凸包，返回 `0/1/2` 表示外部/边界/内部。
- `MinkowskiSum` 会先对两个输入求严格凸包。

面积和三角形中心：

```cpp
f64 TriangleArea(Pd a, Pd b, Pd c);
f64 PolygonArea(const std::vector<Point<T>> &polygon, bool absolute = true);
std::pair<bool, Pd> Circumcenter(Pd a, Pd b, Pd c);
Pd Incenter(Pd a, Pd b, Pd c);
Point<T> Centroid(Point<T> a, Point<T> b, Point<T> c);
```

- `PolygonArea(..., false)` 返回有向面积，逆时针为正。
- `Circumcenter` 在三点共线时返回 `false`。
- `Incenter` 要求非退化三角形。
- 整数类型的 `Centroid` 会发生除法截断。

多边形包含关系：

```cpp
int PointInPolygon(Point<T> p, const std::vector<Point<T>> &polygon);
bool SegmentInPolygon(Ld segment, const std::vector<Pd> &polygon, bool boundaryIncluded = true);
```

输入必须是简单多边形。`PointInPolygon` 返回 `0/1/2` 表示外部/边界/内部。

格点多边形：

```cpp
i64 BoundaryLatticePoints(const std::vector<Pi> &polygon);
i64 InteriorLatticePoints(const std::vector<Pi> &polygon);
```

`InteriorLatticePoints` 使用 Pick 定理。

矩形判断：

```cpp
int RectangleType(std::vector<Point<T>> points);
```

只接受四个整数点，返回 `0/1/2` 表示其他图形/矩形/正方形。

### 10.6 Half Plane

头文件：`CP/geometry/halfplane.hpp`

```cpp
bool InHalfPlane(Pd p, Ld line, bool boundaryIncluded = true);
std::vector<Pd> HalfPlaneIntersection(std::vector<Ld> lines);
```

半平面由有向直线 `a -> b` 的左侧定义。每条直线的两个端点必须不同。`HalfPlaneIntersection` 返回有界交多边形；空集或无界交返回空数组。

单位正方形对应的四个半平面：

```cpp
std::vector<Ld> lines = {
    {{0, 0}, {1, 0}},
    {{1, 0}, {1, 1}},
    {{1, 1}, {0, 1}},
    {{0, 1}, {0, 0}}
};

std::vector<Pd> polygon = HalfPlaneIntersection(lines);
```

## 11. 字符串

### 11.1 SuffixArray

头文件：`CP/string/suffixarray.hpp`

```cpp
SuffixArray suffixArray(s);
```

公开成员：

```cpp
int n;
std::vector<int> sa;
std::vector<int> rk;
std::vector<int> lc;
```

含义：

- `sa[i]`：排名为 `i` 的后缀起点。
- `rk[i]`：从位置 `i` 开始的后缀排名。
- `lc[i]`：后缀 `sa[i]` 与前一个后缀 `sa[i - 1]` 的最长公共前缀长度，`lc[0] = 0`。

所有位置和排名均使用 `0..n-1`，与其他 1-indexed 数据结构不同。输入字符串不能为空。

示例：

```cpp
std::string s = "banana";
SuffixArray suffixArray(s);

for (int position : suffixArray.sa) {
    std::cout << position << ' ' << s.substr(position) << '\n';
}
```

构造复杂度为 `O(n log n)`，LCP 数组使用 Kasai 算法在线性时间内计算。

## 12. 模块与触发符号

| 模块名 | 主要触发符号 |
|---|---|
| `types` | `i32`, `i64`, `i128`, `u32`, `u64`, `u128`, `f32`, `f64`, `EPS`, `inf` |
| `basic` | `io` |
| `fastio` | `IOS` |
| `math_base` | `power`, `Floor`, `Ceil`, `exgcd`, `excrt`, `gauss` 等 |
| `prime` | `MillerRabin`, `PollardRho`, `factorize` |
| `modint` | `Z`, `DynModInt`, `Montgomery`, `power` |
| `modcomb` | `Comb`, `comb` |
| `fft` | `Cp`, `FFT` |
| `poly` | `Poly`, `dft`, `idft`, `primitiveRoot` |
| `matrix` | `Matrix` |
| `dsu` | `DSU` |
| `dynheap` | `DynHeap` |
| `fenwick` | `Fenwick` |
| `lazysegmenttree` | `LazySegmentTree` |
| `sparsetable` | `SparseTable` |
| `suffixarray` | `SuffixArray` |
| `maxflow` | `MaxFlow` |
| `mincostflow` | `MinCostFlow` |
| `stoerwagner` | `StoerWagner` |
| `gomoryhutree` | `GomoryHuTree` |
| `twosat` | `TwoSat` |
| `bipartite` | `HopcroftKarp`, `KM` |
| `hungarian` | `Hungarian` |
| `scc` | `SCC` |
| `ecc` | `ECC` |
| `vcc` | `VCC` |
| `blossomtree` | `BlossomTree` |
| `blockcuttree` | `BlockCutTree` |
| `segtgraph` | `SegtGraph` |
| `geometry_base` | `PI`, `EPS`, `Equal`, `Sign`, `Frac`, `QuadraticRoots` |
| `geometry_point` | `Point`, `Pi`, `Pd`, `Dot`, `Cross`, `Distance`, `Rotate` 等 |
| `geometry_line` | `Line`, `Li`, `Ld`, `LineIntersection`, `SegmentIntersection` 等 |
| `geometry_circle` | `Circle`, `Ci`, `Cd` 及圆相关函数 |
| `geometry_polygon` | `Convex`, `PolygonArea`, `PointInPolygon`, `MinkowskiSum` 等 |
| `geometry_halfplane` | `InHalfPlane`, `HalfPlaneIntersection` |

完整列表可以随时通过以下命令查看：

```powershell
python D:\Vscode_CPP\.cpp_system\CP\tools\bundle.py --list
```

## 13. 新增模块

以添加 `SegTree` 为例。

创建头文件：

```cpp
#pragma once

#include <vector>

struct SegTree {
};
```

在 `include/CP/all.hpp` 中加入：

```cpp
#include "CP/ds/segtree.hpp"
```

在 `library.json` 的 `modules` 中登记：

```json
"segtree": {
  "header": "include/CP/ds/segtree.hpp",
  "symbols": ["SegTree"],
  "depends": []
}
```

- `symbols` 填源代码中能够直接搜索到的公开标识符。
- `depends` 填其他 CP 模块名，不依赖其他模块时使用空数组。
- 头文件依赖应保持单向，避免循环依赖。

完成后检查：

```powershell
python D:\Vscode_CPP\.cpp_system\CP\tools\bundle.py --list
```

再准备一个使用新符号的最小程序，分别验证本地编译和展开后编译。
