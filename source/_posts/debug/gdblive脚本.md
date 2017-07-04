---
title: gdblive脚本
date: 2017-04-03 21:34(Monday)
tags: [PG_src,gdb,shell]
categories: [PG_src,gdb,shell]
---

大神Bruce Momjian的原脚本：[gdblive](https://wiki.postgresql.org/wiki/Gdblive_script)

```sh
#!/bin/sh

# Usage: gdblive [ arguments to grep output of ps ]

cd $HOME

# tee /dev/tty is for user to see the set of procs considered
if [ $# -eq 0 ]
then
    PROCS=`ps auxww | \
        grep postgres: | \
        grep -v -e 'grep' \
        -e 'postgres: stats' \
        -e 'postgres: writer' \
        -e 'postgres: wal writer' \
        -e 'postgres: checkpointer' \
        -e 'postgres: archiver' \
        -e 'postgres: logger' 
        -e 'postgres: autovacuum' \
        -e 'postgres: bgworker' | \
        tee /dev/tty | \
        awk '{if($1=="'$USER'") {print $2}}'`
else
    PROCS=`ps auxww | \
        grep postgres: | \
        grep -v -e 'grep postgres:' \
        -e 'postgres: stats' \
        -e 'postgres: writer' \
        -e 'postgres: wal writer' 
        -e 'postgres: checkpointer' 
        -e 'postgres: archiver' 
        -e 'postgres: logger' 
        -e 'postgres: autovacuum' \
        -e 'postgres: bgworker' | \
        grep $@ | \
        tee /dev/tty | \
        awk '{if($1=="'$USER'") {print $2}}'`
fi

if [ `echo "$PROCS" | wc -w` -eq 1 ]
then
    exec gdb $PGINSTROOT/bin/postgres -silent "$PROCS"
else
    exec gdb $PGINSTROOT/bin/postgres -silent
fi
```

做了两点修改：
- grep -v -e 'grep postgres:'改成了 grep -v -e 'grep'，因为有的环境grep是有--color选项的（应该可以设置为无）：grep --color=auto postgres:
- awk '{print $2}'改成了 awk '{if($1=="'$USER'") {print $2}}'，因为同一台机器上可能有许多用户都开启了pg并且有客户端连接，不根据用户排除的话，可能有多个选项导致出错

其他说明：
只适用于当前用户仅开启一个客户端情况，有多个客户端连接调试不适用
