# cpp-zoo
A collection of C++ syntax examples, from basic typedef struct to advanced CRTP and template metaprogramming.

> **从基础语法到模板元编程的实战演练场**  
> A collection of minimal, self-contained C++ examples demonstrating core concepts, idioms, and advanced patterns.

本仓库旨在通过**最小可运行代码片段 (Minimal Working Examples)**，直观地展示 C++ 的关键语法特性。从基础的 `struct` 别名到高级的 **CRTP (奇异递归模板模式)**，每一段代码都经过精心打磨，适合快速查阅、学习或作为教学素材。

## 🚀 核心亮点

- **🔍 零开销抽象**：深入理解编译期多态与运行时多态的区别。
- **🧩 模式解析**：拆解复杂的设计模式（如 CRTP, PIMPL, RAII）。
- **📝 最佳实践**：展示现代 C++ (C++11/14/17/20) 的推荐写法。
- **⚡ 即拷即用**：每个文件都是独立的 `.cpp` 或 `.h`，可直接编译运行。

---

## 📚 目录索引 (Catalog)

### 1. 基础基石 (Basics)
| 文件名 | 描述 | 关键概念 |
| :--- | :--- | :--- |
| `01_typedef_struct.cpp` | 结构体别名详解 | `typedef`, `struct`, 类型别名 |
| `02_const_correctness.cpp` | const 的正确使用姿势 | `const_cast`, 成员函数 const |
| `03_initialization.cpp` | 初始化列表与统一初始化 | `{}`, 构造函数委托 |

### 2. 面向对象 (OOP)
| 文件名 | 描述 | 关键概念 |
| :--- | :--- | :--- |
| `10_virtual_dispatch.cpp` | 虚函数表机制演示 | `vptr`, `vtable`, 动态绑定 |
| `11_inheritance_rules.cpp` | 继承访问控制与菱形继承 | `public/protected/private`, 虚继承 |

### 3. 高级模板与元编程 (Advanced Templates)
| 文件名 | 描述 | 关键概念 |
| :--- | :--- | :--- |
| [`crtp_pattern.cpp`](crtp_pattern.cpp) | **奇异递归模板模式** | 静态多态, 编译期接口, Mixins |
| `21_sfinae_basics.cpp` | SFINAE 基础入门 | `enable_if`, 重载决议 |
| `22_variadic_templates.cpp` | 可变参数模板 | 折叠表达式 (C++17), 递归展开 |

> 💡 **重点推荐**: 查看 [`20_crtp_pattern.cpp`](./20_crtp_pattern.cpp) 了解如何用模板实现零开销的多态。

---

## 🛠️ 如何编译与运行

本仓库代码需要支持 **C++11** 或更高版本的编译器（推荐 **C++17** 或 **C++20**）。

### 使用 g++ / clang++

```bash
# 编译单个示例 (例如 CRTP)
g++ -std=c++17 -O2 -Wall -Wextra 20_crtp_pattern.cpp -o crtp_demo

# 运行
./crtp_demo
