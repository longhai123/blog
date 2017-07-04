---
title: 使用GDB调试PostgreSQL源码
date: 2017-04-03
tags: [PG_src,gdb]
categories: [PG_src,gdb]
---

## 编译阶段

```
./configure --prefix=$HOME/install_srdb --enable-cassert --enable-debug CFLAGS="-ggdb -Og -g3 -fno-omit-frame-pointer"
./configure --prefix=$HOME/pgsql --enable-cassert --enable-debug CFLAGS=-O0
```

- 允许断言将打开许多完整性检查选项
- 允许调试符号将支持使用调试器（例如gdb）来跟踪行为异常的代码
- -ggdb -Og -g3在提供更多的调试信息的同时还有优化作用
- fno-omit-frame-pointer在使用tracing和profiling工具，譬如perf时有用（暂时没用到过）

## 运行阶段

postgres服务器有一个-d选项，其使得详细的信息被日志记录起来（elog或ereport的输出）。-d选项使用一个数，其指定了调试等级。需要注意的是，高调试等级会产生很大的日志文件。当使用pg_ctl启动服务器时，这个选项是不可用的，但是你可以使用-o log_min_messages=debug4或类似的选项来代替。 

## 使用gdb调试

如果postmaster正在运行，并且在一个窗口中启动了psql，那么可以使用

```    
SELECT pg_backend_pid();
```

找出psql所用的postgres进程的PID。使用一个调试器连接至postgres的PID——

```
gdb -p 1234
```

或者对于一个正在运行的gdb，用

```
attach 1234
```

这样的方法。

也可查看[gdblive](https://wiki.postgresql.org/wiki/Gdblive_script)脚本。

你可以在调试器里设置断点，并在此之后在psql会话中发起一个查询。
寻找产生错误或日志消息的位置，在errfinish处设置一个断点。这将会进入所有对所开启日志等级的elog和ereport调用，因此它将会被触发很多次。如果你只对ERROR/FATAL/PANIC感兴趣，那么请对errordata[errordata_stack_depth].elevel >= 20的使用一个[gdb条件断点](http://blog.vinceliu.com/2009/07/gdbs-conditional-breakpoints.html)，或者用errfinish对PANIC、FATAL和ERROR的情况设置一个source-line断点。

```
b errfinish
cond 2  errordata[errordata_stack_depth].elevel >= 20
info b
```

```
2       breakpoint     keep y   0x00000000007ddd7d in errfinish at elog.c:408
	stop only if errordata[errordata_stack_depth].elevel >= 20
```

请注意，不是所有的错误都通过errfinish；特别地，权限检查的异常是单独抛出的。如果你的断点没有被触发，使用git grep获取你的错误文本，并查看它是从哪里产生的。 

如果你正在调试某些在会话启动时的发生的事情，你可以设置PGOPTIONS="-W n"，之后启动psql。这会导致启动延迟n秒，因此你可以用的调试器连接至该进程，设置相应的断点，之后再继续启动过程。
