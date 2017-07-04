---
title: PG中增加插件-使用C函数
date: 2016-06-24 15:25:47
tags: [PG_src,contrib,PG_add_contrib,use_C_function]
categories: [PG_src,contrib]
---

本篇介绍PG使用C函数添加插件。
> PG源码是最好的学习资料，大家有兴趣可以看一下contrib目录下的插件。

## 1. 源码增加

> 在contrib下建立目录userid，建立C文件userid.c，建立Makefile文件，建立两个SQL文件，userid--1.0.sql，userid--unpackaged--1.0.sql，以及一个control文件，userid.control。

<!-- more -->

* userid.c：

```1C
/*-------------------------------------------------------------------------
 *
 * userid.c
 *    display current session user oid
 *
 *    contrib/userid/userid.c
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "libpq/auth.h"
#include "utils/guc.h"
#include "miscadmin.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(get_current_user_id);

Datum
get_current_user_id(PG_FUNCTION_ARGS)
{
    PG_RETURN_OID(GetSessionUserId());
}
```
* Makefile：

```Makefile
# contrib/userid/Makefile

MODULE_big = userid
OBJS = userid.o

EXTENSION = userid
DATA = userid--1.0.sql userid--unpackaged--1.0.sql

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/userid
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
```
* userid--1.0.sql【在create extension时加载的SQL文件，会执行SQL文件中的sql】

```SQL
/* contrib/userid/userid--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION userid" to load this file. \quit

-- Register the function.
CREATE SCHEMA brother;
CREATE FUNCTION get_current_userid()
RETURNS OID
AS 'MODULE_PATHNAME', 'get_current_user_id'
LANGUAGE C;
```

* userid--unpackaged--1.0.sql【CREATE EXTENSION userid FROM unpackaged时调用的SQL文件，这个文件是在更新9.1之前版本的插件函数时使用的，后面会介绍用法。】

```SQL
/* contrib/userid/userid--unpackaged--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION userid FROM unpackaged" to load this file. \quit

ALTER EXTENSION userid ADD schema brother;
ALTER EXTENSION userid ADD function get_current_userid();
```

* userid.control【在create extension加载的文件，根据这个文件，PG去寻找对应的sql文件】

```bash
# userid extension
comment = 'display current session user oid'
default_version = '1.0'
module_pathname = '$libdir/userid'
relocatable = true
```

## 2. 编译安装

```bash
[longhai@localhost userid]$ make
gcc -Wall -Wmissing-prototypes -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -g -O2 -fpic -I. -I. -I../../src/include -D_GNU_SOURCE   -c -o userid.o userid.c
gcc -Wall -Wmissing-prototypes -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -g -O2 -fpic -shared -o userid.so userid.o -L../../src/port -L../../src/common -Wl,--as-needed -Wl,-rpath,'/home/longhai/pg952/lib',--enable-new-dtags  
[longhai@localhost userid]$ make install
/bin/mkdir -p '/home/longhai/pg952/lib/postgresql'
/bin/mkdir -p '/home/longhai/pg952/share/postgresql/extension'
/bin/mkdir -p '/home/longhai/pg952/share/postgresql/extension'
/bin/install -c -m 755  userid.so '/home/longhai/pg952/lib/postgresql/userid.so'
/bin/install -c -m 644 ./userid.control '/home/longhai/pg952/share/postgresql/extension/'
/bin/install -c -m 644 ./userid--1.0.sql ./userid--unpackaged--1.0.sql  '/home/longhai/pg952/share/postgresql/extension/'
```


## 3. 文件安装位置
```bash
[longhai@localhost postgresql]$ pwd
/home/longhai/pg952/share/postgresql
[longhai@localhost postgresql]$ cd extension/
[longhai@localhost extension]$ ls
plpgsql--1.0.sql  plpgsql.control  plpgsql--unpackaged--1.0.sql  userid--1.0.sql  userid.control  userid--unpackaged--1.0.sql
[longhai@localhost extension]$ pwd
/home/longhai/pg952/share/postgresql/extension
```

## 4. 配置
```bash
[longhai@localhost extension]$ psql
psql (9.5.2)
Type "help" for help.

postgres=# create extension userid ;
CREATE EXTENSION
postgres=# select * from pg_extension ;
 extname | extowner | extnamespace | extrelocatable | extversion | extconfig | extcondition 
---------+----------+--------------+----------------+------------+-----------+--------------
 plpgsql |       10 |           11 | f              | 1.0        |           | 
 userid  |       10 |         2200 | t              | 1.0        |           | 
(2 rows)

postgres=#
```

## 5. 运行，查看效果 

```bash
postgres=# select SESSION_USER;
 session_user
--------------
 longhai
(1 row)

postgres=# select oid from pg_authid where rolname = 'longhai';
 oid
-----
  10
(1 row)

postgres=# select get_cu

postgres=# select get_current_userid();
 get_current_userid
--------------------
                 10
(1 row)

postgres=# \dn
  List of schemas
  Name   |  Owner
---------+---------
 brother | longhai
 public  | longhai
(2 rows)

postgres=#
```

## 6. unpackaged脚本说明

对于PostgreSQL9.1之前的插件，都是通过执行SQL语言创建。创建的对象（函数或者模式等）没有和extension进行关联。
而unpackaged脚本就是增加这种关联（或者说把插件创建的对象都加入到extension？）。
增加关联之后使用drop extension就可以删除所有插件创建的对象。
对于9.1之前的版本，创建插件的过程模拟为：

```bash
postgres=# create schema brother;
CREATE SCHEMA
postgres=# CREATE FUNCTION get_current_userid()
postgres-# RETURNS OID
postgres-# AS '$libdir/userid', 'get_current_user_id'
postgres-# LANGUAGE C;
CREATE FUNCTION
postgres=# select get_cu

postgres=# select get_current_userid();
 get_current_userid
--------------------
                 10
(1 row)

postgres=#
```

此时数据库中没有userid的extension

```bash
postgres=# select * from pg_extension ;
 extname | extowner | extnamespace | extrelocatable | extversion | extconfig | extcondition 
---------+----------+--------------+----------------+------------+-----------+--------------
 plpgsql |       10 |           11 | f              | 1.0        |           | 
(1 row)
```

使用unpackaged，将对象与extension关联：
```bash
postgres=# create extension userid with schema public from unpackaged;
CREATE EXTENSION
postgres=# select * from pg_extension ;
 extname | extowner | extnamespace | extrelocatable | extversion | extconfig | extcondition 
---------+----------+--------------+----------------+------------+-----------+--------------
 plpgsql |       10 |           11 | f              | 1.0        |           | 
 userid  |       10 |         2200 | t              | 1.0        |           | 
(2 rows)
```

关联关系如下：
```bash
postgres=# select oid from pg_proc where proname = 'get_current_userid';
  oid
-------
 16406
(1 row)

postgres=# select oid from pg_namespace where nspname = 'brother';
  oid
-------
 16405
(1 row)

postgres=# select oid from pg_extension where extname = 'userid';
  oid
-------
 16407
(1 row)

postgres=# select * from pg_depend where refobjid = 16407;
 classid | objid | objsubid | refclassid | refobjid | refobjsubid | deptype 
---------+-------+----------+------------+----------+-------------+---------
    2615 | 16405 |        0 |       3079 |    16407 |           0 | e
    1255 | 16406 |        0 |       3079 |    16407 |           0 | e
(2 rows)

postgres=#
```

drop extension时，同时会删除插件中SQL建立的对象的原因就是pg_depend表中记录。

## 7. create extension时调用函数

有兴趣可以研究**recordDependencyOnCurrentExtension**函数