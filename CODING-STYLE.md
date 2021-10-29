# 代码风格规范

仓库中有 `.clang-format`，大部分代码风格的问题可以使用 Clang Format 这个工具解决。以下是附加规范：

* 文件名使用小写和下划线。头文件后缀名为 `.h`，源代码文件后缀名为 `.c`。

* 注释使用双斜杠 `//` 或者 `/** ... */`。注释首字母不大写。每一句话后面需要有句点（`.`）。

```c
// one line comment.

/* one line comment. */

// multi
// line
// comment.

/**
 * multi
 * line
 * comment. Multi line comment.
 */
```

* 结构体使用 `typedef struct { ... } StructName;`，枚举类型使用 `typedef enum { ... } EnumName;`。

* 变量名、函数名使用 `snake_case`，类型名、结构体名、枚举类型名使用 `CamelCase`。

```c
void hello_world() {
    usize some_count = 0;
}

typedef struct {
    i32 a;
} HelloWorld;

typedef HelloWorld AnotherOne;
```

* 如非名称过长，请尽量避免使用缩写。

* 无论什么名称，尽量避免使用双下划线（`__`），包括开头和结尾。

```c
#define __CORE_AHA_H__  // NO
#define _CORE_AHA_H_    // YES
```

* 希望对其它编译单元隐藏的函数、变量，请在名称前加上下划线（`_`）。

```c
static bool _hidden;
static INLINE _helper_function();
```

* 没有用到的函数参数，请使用 `(void)arg_name`。

```c
i64 add(i64 a, i64 b, bool flag) {
    (void)flag;
    return a + b;
}
```

* 使用头文件时，请使用尖括号 `<...>`。无论是不是在同一个文件夹下。

```c
#include "some_header.h"          // NO
#include <package/some_header.h>  // YES
```

* 头文件的第一行使用 `#pragma once`。

* 避免直接使用 `int`、`short`、`unsigned`、`long long`、`size_t` 等类型，使用 `bool`、`i16`、`u32`、`i64`、`usize` 等。

```c
int is_okay();   // NO
bool is_okay();  // YES
```

* 尽量避免使用 `0`、`1` 表示布尔值，而是使用 `false`、`true`。

* 如果 `typedef` 只用到了一次，可以不用。

```c
// NO
typedef void (*SomeHandler)(void);

typedef struct {
    SomeHandler handler;
} Context;

// YES
typedef struct {
    void (*handler)(void);
} Context;
```

* 函数和函数指针的申明中请带上参数名称。

```c
// NO
i32 foo(i32);
typedef struct {
    void (*func)(i32);
} Interface;

// YES
i32 foo(i32 value);
typedef struct {
    void (*func)(i32 value);
} Interface;
```

* 函数指针中若没有需要传入的参数，请填入 `void`。

```c
typedef int (*VoidFunc)(void);
```

* 所有 `inline` 函数请使用 `static INLINE` 或者 `static ALWAYS_INLINE`。

```c
inline i64 add(i64 a, i64 b);         // NO
static INLINE i64 add(i64 a, i64 b);  // YES
```

* 如果希望函数不被内联优化，请使用 `NO_INLINE`。

```c
NO_INLINE i64 complicated_add(i64 a, i64 b);
```

* 不会返回的函数（如 `panic`、`fork`），请在前面标注 `NO_RETURN`。

```c
NO_RETURN void panic();

NO_RETURN void panic() {
    while (1) {}
}
```

* 对于一个模块（如内存分配器），建议将所有全局变量放到一个结构体内。如果不需要被其它模块引用，请标记为 `static`。

```c
// NO
SpinLock mem_lock;
usize mem_size;

// YES
typedef struct {
    SpinLock lock;
    usize size;
} MemoryContext;

static MemoryContext ctx;
```

* 请使用 `asm volatile` 而不是 `asm`。

* 请尽可能消除所有的编译器警告，而不是忽略它们。

欢迎对当前的代码风格提出意见。
