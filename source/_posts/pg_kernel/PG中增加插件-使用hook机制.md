---
title: PG中增加插件-使用hook机制
date: 2016-06-24 15:20:45
tags: [PG_src,contrib,PG_add_contrib,hook]
categories: [PG_src,contrib]
---

本篇介绍PG利用hook机制添加插件。
> PG源码是最好的学习资料，大家有兴趣可以看一下contrib目录下的插件。

**实现：在用户brother登录验证成功后，显示信息Welcome to the world of PostgreSQL!**

## 1. 源码增加
> 在contrib目录下创建brother目录，新增brother.c文件和Makefile文件。

<!-- more -->

***brother.c:***
```1C
/*-------------------------------------------------------------------------
 *
 * brother.c
 *    welcome to the world of PostgreSQL
 *
 *    contrib/brother/brother.c
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "libpq/auth.h"
#include "miscadmin.h"
#include "utils/guc.h"

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

static ClientAuthentication_hook_type pre_ClientAuthentication_hook = NULL;

static
void welcome_to_pg(Port *port, int status)
{
    if(status == STATUS_OK && (strcmp(port->user_name, "brother") == 0))
        printf("Welcome to the world of PostgreSQL!\n");
}

void
_PG_init(void)
{
        pre_ClientAuthentication_hook = ClientAuthentication_hook;
        ClientAuthentication_hook = welcome_to_pg;
}

void
_PG_fini(void)
{
        ClientAuthentication_hook = pre_ClientAuthentication_hook;
}
```

***Makefile:***
``` Makefile
# contrib/brother/Makefile
 
MODULE_big = brother
OBJS = brother.o
 
# uncomment the following two lines to enable cracklib support
# PG_CPPFLAGS = -DUSE_CRACKLIB '-DCRACKLIB_DICTPATH="/usr/lib/cracklib_dict"'
# SHLIB_LINK = -lcrack
 
ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/brother
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

```
## 2. 编译并安装

```Bash
[longhai@localhost brother]$ ls
brother.c  Makefile
[longhai@localhost brother]$ make
gcc -Wall -Wmissing-prototypes -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -g -O2 -fpic -I. -I. -I../../src/include -D_GNU_SOURCE   -c -o brother.o brother.c
gcc -Wall -Wmissing-prototypes -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -g -O2 -fpic -shared -o brother.so brother.o -L../../src/port -L../../src/common -Wl,--as-needed -Wl,-rpath,'/home/longhai/pg952/lib',--enable-new-dtags  
[longhai@localhost brother]$ make install
/bin/mkdir -p '/home/longhai/pg952/lib/postgresql'
/bin/install -c -m 755  brother.so '/home/longhai/pg952/lib/postgresql/brother.so'
[longhai@localhost brother]$
```

## 3. 文件安装位置

```bash
[longhai@localhost postgresql]$ pwd
/home/longhai/pg952/lib/postgresql
[longhai@localhost postgresql]$ ls -lrt brother.so 
-rwxr-xr-x. 1 longhai longhai 23401 May  2 20:50 brother.so
[longhai@localhost postgresql]$
```

## 4. 在postgresql.conf中配置

> shared_preload_libraries = 'brother'

## 5. 运行，查看效果

```bash
[longhai@localhost ~]$ pg_ctl start
server starting
[longhai@localhost ~]$ LOG:  database system was shut down at 2016-05-03 00:41:35 PDT
LOG:  MultiXact member wraparound protections are now enabled
LOG:  database system is ready to accept connections
LOG:  autovacuum launcher started

[longhai@localhost ~]$ psql
psql (9.5.2)
Type "help" for help.

postgres=# create user brother;
CREATE ROLE
postgres=# \q
[longhai@localhost ~]$ psql postgres brother
Welcome to the world of PostgreSQL!
psql (9.5.2)
Type "help" for help.

postgres=>
```
