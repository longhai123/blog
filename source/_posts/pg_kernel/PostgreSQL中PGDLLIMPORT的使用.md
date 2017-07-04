---
title: PostgreSQL中PGDLLIMPORT的使用
date: 2017-05-19 16:38:15
tags: pg_kernel
---

在PostgreSQL源码中，随处可见PGDLLIMPORT的使用，笔者曾经使用过，当时对其含义是
没有太多认知的。经过一番研究，多少有点了解，本着分享原则，本文尝试解释其含义。

## 定义

> src/include/port/win32.h

```
#ifdef BUILDING_DLL
#define PGDLLIMPORT __declspec (dllexport)
#else                           /* not BUILDING_DLL */
#define PGDLLIMPORT __declspec (dllimport)
#endif
```

## 含义

首先，PGDLLIMPORT在非windows平台，是不做任何事情的。

在Windows平台, PGDLLIMPORT的含义根据在编译PostgreSQL主程序还是扩展插件，有不同的含义：

- 假设BUILDING_DLL被定义，PGDLLIMPORT被定义为__declspec(dllexport)，即会从 PostgreSQL的server二进制文件中导出符号（变量、函数）

- 假如BUILDING_DLL未被定义，PGDLLIMPORT被定义为__declspec(dllimport)，表明 此符号是被导入的，即引用自PostgreSQL。

需要注意的是，仅仅在编译posres.exe的时候才会定义BUILDING_DLL。

所以在postgres主程序中使用，代表输出符号；在插件代码中使用，代表导入符号。

## 示例

结合PostgreSQL中的hook使用给出举例说明。

[PostgreSQL中hook使用](http://www.longhaiqwe.com/2016/06/24/src/PostgreSQL%E4%B8%ADhook%E4%BD%BF%E7%94%A8/)

在上面这篇文章中，在src/include/libpq/auth.h中对ClientAuthentication_hook的定义使用了PGDLLIMPORT, 代表导出此hook；

在我新增的插件中（my_client_auth.c）使用：#include "libpq/auth.h"，说明此hook定义是从其他地方导入。

之所以需要使用PGDLLIMPORT，涉及到windows中动态库的链接个工作机制（[how DLL linkage works on win32/PE](https://msdn.microsoft.com/en-us/library/a90k134d.aspx)）。

## 总结

**在windows下，假如在extension中需要用到postgres主程序中的变量或函数，必须使用PGDLLIMPORT声明，并且在扩展中也用PGDLLIMPORT声明。**
