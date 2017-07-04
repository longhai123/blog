---
title: PG增加一个系统表

date: 2016-06-24 14:28:47

tags: [PG_src,kernel,add_system_catalog]

categories: [PG_src,kernel]
---
摘要
> pg内核增加一个系统表

1、新增：src/include/catalog/pg_demo_lh.h

```1C
/*-------------------------------------------------------------------------
 *
 * pg_demo_lh.h
 *
 * add a catalog table.
 *
 * src/include/catalog/pg_demo_lh.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_DEMO_LH_H
#define PG_DEMO_LH_H

#include "catalog/genbki.h"

/* ----------------
 *              pg_demo_lh definition.  cpp turns this into
 *              typedef struct FormData_pg_demo_lh
 * ----------------
 */
#define DemoLhRelationId      7000

CATALOG(pg_demo_lh,7000)
{
        int32 id;
} FormData_pg_demo_lh;


/* ----------------
 *              Form_pg_demo_lh corresponds to a pointer to a tuple with
 *              the format of pg_demo_lh relation.
 * ----------------
 */
typedef FormData_pg_demo_lh *Form_pg_demo_lh;

/* ----------------
 *              compiler constants for pg_demo_lh
 * ----------------
 */

#define Natts_pg_demo_lh                                1
#define Anum_pg_demo_lh_id                              1

#endif   /* PG_DEMO_LH_H */
```
<!-- more -->

2、src/backend/catalog/Makefile增加：

```1C
POSTGRES_BKI_SRCS = $(......
    pg_demo_lh.h \
)
```

3、src/include/catalog/indexing.h增加：
```1C
DECLARE_UNIQUE_INDEX(pg_demo_lh_oid_index, 8000, on pg_demo_lh using btree(oid oid_ops));
#define DemoLhOidIndexId  8000
```
4、编译安装后查看结果：

```Bash
postgres=# \dS
                      List of relations
   Schema   |              Name               | Type  | Owner 
------------+---------------------------------+-------+-------
 pg_catalog | pg_aggregate                    | table | srdb
 ......
 pg_catalog | pg_demo_lh                      | table | srdb
 ......

postgres=# \d pg_demo_lh
Table "pg_catalog.pg_demo_lh"
 Column |  Type   | Modifiers 
--------+---------+-----------
 id     | integer | not null
Indexes:
    "pg_demo_lh_oid_index" UNIQUE, btree (oid)
```
