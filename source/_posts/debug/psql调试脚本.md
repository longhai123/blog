---
title: gdbpsql脚本
date: 2017-04-03 22:14(Monday)
tags: [PG_src,gdb,psql]
categories: [PG_src,gdb,psql]
---

前面对大神的gdblive进行了一点修改，以适应自己的开发环境和需求。

进一步的，有时候有调试psql的需要，所以也写了一个简单脚本进行psql的调试。

```sh
#!/bin/sh

# Usage: gdbpsql
GDB=gdb

cd `pwd`

# tee /dev/tty is for user to see the set of procs considered
PROCS=`ps auxww | grep psql | grep -v grep | grep -v defunct| grep -v $0 |\
        tee /dev/tty | awk '{print $2}'`
if [ -z $PROCS ]
then
	echo 'please startup a psql first'
	exit
fi

exec $GDB -silent -p "$PROCS"
```
