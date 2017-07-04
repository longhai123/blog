---
title: PG增加一个系统用户
date: 2016-06-23 09:50:47
tags: [PG_src,kernel,add_system_user]
categories: [PG_src,kernel]
---
摘要
> pg内核增加一个系统用户

src/include/catalog/pg_authid.h增加一行代码：
```1C
DATA(insert OID = 10 ( "POSTGRES" t t t t t t t -1 _null_ _null_));
DATA(insert OID = 9000 ( "LONGHAI" t t f t t f t -1 _null_ _null_)); /* 增加 */
```
编译安装之后效果：
![add_user.png](/images/add_user.png)

<!-- more -->

其中权限对应宏定义：
```1C
#define Natts_pg_authid                         11
#define Anum_pg_authid_rolname                  1
#define Anum_pg_authid_rolsuper                 2
#define Anum_pg_authid_rolinherit               3
#define Anum_pg_authid_rolcreaterole            4
#define Anum_pg_authid_rolcreatedb              5
#define Anum_pg_authid_rolcanlogin              6
#define Anum_pg_authid_rolreplication           7
#define Anum_pg_authid_rolbypassrls             8
#define Anum_pg_authid_rolconnlimit             9
#define Anum_pg_authid_rolpassword              10
#define Anum_pg_authid_rolvaliduntil            11
```

对应系统表pg_authid 字段

名字 |类型 |描述|
---|---|---|
oid |oid |行标识符(隐藏属性; 必须明确选择) 
rolname |name |角色名称 
rolsuper |bool |角色拥有超级用户权限 
rolinherit| bool |角色自动继承其所属角色的权限 
rolcreaterole| bool |角色可以创建更多角色 
rolcreatedb |bool |角色可以创建数据库 
rolcatupdate |bool |角色可以直接更新系统表。如果没有设置这个字段为真，即使超级用户也不能这么做。  
rolcanlogin |bool |角色可以登录，也就是说，这个角色可以给予会话认证标识符。  
rolreplication| bool |角色是一个复制的角色。也就是说，这个角色可以发起流复制 （参阅第 25.2.5 节）和使用 pg_start_backup和pg_stop_backup 设置/取消设置系统备份模式。  
rolconnlimit |int4 |对于可以登录的角色，限制其最大并发连接数量。-1 表示没有限制。  
rolpassword |text |口令(可能是加密的)；如果没有则为 NULL。如果密码是加密的，该字段将以md5 字符串开始，后面跟着一个32字符的十六进制MD5哈希值。该MD5哈希将是用户的口令加上用户名。 例如，如果用户joe的口令为xyzzy，PostgreSQL 将存储MD5哈希为xyzzyjoe。不遵从这个格式的密码被假设为未加密的。  
rolvaliduntil |timestamptz |口令失效时间(只用于口令认证)；如果没有失效期，则为 null 

仅仅增加用户，还是比较简单！