---
title: PostgreSQL中hook使用
date: 2016-06-24 15:14:47
tags: [PG_src,contrib,use_hook]
categories: [PG_src,contrib]
---

可参考博客：
> http://blog.endpoint.com/2010/05/using-postgresql-hooks.html
> http://my.oschina.net/Suregogo/blog/312848
> http://www.cnblogs.com/gaojian/p/3259147.html


## 经常使用的Hook汇总

| Hook | Initial release |
| --- | --- |
| check_password_hook | 9.0 |
| ClientAuthentication_hook | 9.1 |
| ExecutorStart_hook | 8.4 |
| ExecutorRun_hook | 8.4 |
| ExecutorFinish | 8.4 |
| ExecutorEnd_hook | 8.4 |
| ExecutorCheckPerms_hook | 9.1 |
| ProcessUtility_hook | 9.0 |

<!-- more -->

| Hook | Initial release | Used in |
| --- | :-: | :-- |
| explain_get_index_name_hook | 8.3 |  |
| ExplainOneQuery_hook | 8.3 | IndexAdvisor |
| fmgr_hook | 9.1 | sepgsql |
| get_attavgwidth_hook | 8.4 |  |
| get_index_stats_hook | 8.4 |  |
| get_relation_info_hook | 8.3 | plantuner |
| get_relation_stats_hook | 8.4 |  |
| join_search_hook | 8.3 | saio |
| needs_fmgr_hook | 9.1 | sepgsql |
| object_access_hook | 9.1 | sepgsql |
| planner_hook | 8.3 | planinstr |
| shmem_startup_hook | 8.4 | pg_stat_statements |

## Hook工作原理分析

*   PG中hook如何工作
Hooks存在一个全局函数指针；并初始为NULL；当PG想使用一个hook时，检查这个全局函数指针，假如被设置，则执行函数。

*   如何设置函数指针
hook函数定义在共享库中；在加载的时候，PG调用共享库中的_PG_init()函数；_PG_init函数设置函数指针（一般会把原来的指针保存）。

*   如何清除函数指针
在卸载时，PG调用共享库中的_PG_fini()函数；_PG_fini函数需要重新设置函数指针（通常将原来的指针恢复）。

## 实例分析

### 1 ClientAuthentication_hook

*   定义

```1C
/* src/include/libpq/auth.h */
/* Hook for plugins to get control in ClientAuthentication() */
typedef void (*ClientAuthentication_hook_type) (Port *, int);
extern PGDLLIMPORT ClientAuthentication_hook_type ClientAuthentication_hook;
```

*   设置

```1c
/* src/backend/libpq/auth.c */
/*
 * This hook allows plugins to get control following client authentication,
 * but before the user has been informed about the results.  It could be used
 * to record login events, insert a delay after failed authentication, etc.
 */
ClientAuthentication_hook_type ClientAuthentication_hook = NULL;
```

*   检查和执行

```1c
/* src/backend/libpq/auth.c */

/*
 * Client authentication starts here.  If there is an error, this
 * function does not return and the backend process is terminated.
 */
void
ClientAuthentication(Port *port)
{
    //...
    if (ClientAuthentication_hook)
        (*ClientAuthentication_hook) (port, status);
    //...
}
```

*   说明

控制：客户端认证之后，通知用户之前

用处：记录登录事件；认证失败后插入一段延迟

*   用到此hook的模块

auth_delay；sepgsql；connection_limits

*   ClientAuthentication_hook函数介绍

两个参数 f(Port *port, int status)

Port结构位于src/include/libpq/libpq-be.h，部分字段如下

```1c
typedef struct Port
{
    pgsocket    sock;           /* File descriptor */
    bool        noblock;        /* is the socket in non-blocking mode? */
    ProtocolVersion proto;      /* FE/BE protocol version */
    SockAddr    laddr;          /* local addr (postmaster) */
    SockAddr    raddr;          /* remote addr (client) */
    char       *remote_host;    /* name (or ip addr) of remote host */
    char       *remote_hostname;/* name (not ip addr) of remote host, if
                                 * available */
    char       *remote_port;    /* text rep of remote port */
    char       *database_name;
    char       *user_name;
    char       *cmdline_options;
    List       *guc_options;
} Port;
```

Status为状态代码--STATUS_ERROR，STATUS_OK等

*   实例

==**写一个具体使用案例：假如某个文件存在则禁止连接。**==

需要两个函数：

1.  安装hook

2.  检查文件可用性，决定允许或拒绝连接

添加contrib/my_client_auth目录

添加文件：Makefile my_client_auth.c

```1c
/* -------------------------------------------------------------------------
 *
 * my_client_auth.c
 *
 * Copyright (C) 2010-2011, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *      contrib/my_client_auth/my_client_auth.c
 *
 * -------------------------------------------------------------------------
 */
#include <fcntl.h>
#include <sys/stat.h>
#include "postgres.h"
#include "fmgr.h"

#include "libpq/auth.h"

PG_MODULE_MAGIC;

void        _PG_init(void);

/* Original Hook */
static ClientAuthentication_hook_type next_client_auth_hook = NULL;

/*
 * Check authentication
 */
static void
my_client_auth(Port *port, int status)
{
    struct stat buf;
    if (next_client_auth_hook)
        (*next_client_auth_hook) (port, status);
    if (status != STATUS_OK)
            return;
    if(!stat("/tmp/connection.stopped", &buf))
        ereport(FATAL, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("Connection not authorized!!")));
}

/*
 * Module Load Callback
 */
void
_PG_init(void)
{
    /* Install Hooks */
    next_client_auth_hook = ClientAuthentication_hook;
    ClientAuthentication_hook = my_client_auth;
}
```
添加Makefile文件

```Makefile
#contrib/my_client_auth/Makefile

MODULES = my_client_auth
PGFILEDESC = "my_client_auth - Writing My ClientAuthentication_hook"

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/my_client_auth
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

```

使用 make&&make install

data/postgres.conf配置文件汇总添加：

> shared_preload_libraries = 'my_client_auth'

重启数据库

/tmp下创建connection.stopped文件

psql连接报错：

> psql: FATAL: Connection not authorized!!