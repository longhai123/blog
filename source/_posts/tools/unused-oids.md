
title: unused_oids
date: 2016-06-24 11:02:39
tags: [PG_src,tools,unused_oids]
categories: [PG_src,tools]

---
> src/include/catalog下unused_oids，可以显示未使用的oid

首先上运行效果，有个直观认识：

![](http://static.oschina.net/uploads/space/2015/1214/174118_VxvM_2505138.png)

<!-- more -->

> 简要说明：从src/include/catalog目录下的pg_*.h toasting.h indexing.h中获取所有已经使用的oid，最后将2--FistBootstrapObjectId-1（即10000-1）之间的未使用的id打印出来。其中涉及管道符使用，正则表达式，更有许多有用的linux小命令，譬如grep（egrep），sed，tr，sort，uniq，awk，都是一些常用的命令（所以对于初学者这个shell学习价值还蛮高）。

逐行简单解释如下：

```Bash
# 一堆注释，自己阅读，有助于英文提升！
AWK="awk" # 定义变量awk

# Get FirstBootstrapObjectId from access/transam.h
FIRSTOBJECTID=`grep '#define[   ]*FirstBootstrapObjectId' ../access/transam.h | $AWK '{ print $3 }'`
export FIRSTOBJECTID # 从../access/transam.h文件中找到FirstBootstrapObjectId并取到它的值
 
# this part (down to the uniq step) should match the duplicate_oids script
# note: we exclude BKI_BOOTSTRAP relations since they are expected to have
# matching DATA lines in pg_class.h and pg_type.h
# 将CATALOG(.*BKI_BOOTSTRAP这一行去掉是因为在pg_class.h和pg_type.h中DATA行肯定会匹配到这些OID
 
cat pg_*.h toasting.h indexing.h | \ #从这些文件中查找
egrep -v -e '^CATALOG\(.*BKI_BOOTSTRAP' | \ #去除CATALOG开头后面接(BKI_BOOTSTRAP的行
sed -n  -e 's/^DATA(insert *OID *= *\([0-9][0-9]*\).*$/\1/p' \
        -e 's/^CATALOG([^,]*, *\([0-9][0-9]*\).*BKI_ROWTYPE_OID(\([0-9][0-9]*\)).*$/\1,\2/p' \
        -e 's/^CATALOG([^,]*, *\([0-9][0-9]*\).*$/\1/p' \
        -e 's/^DECLARE_INDEX([^,]*, *\([0-9][0-9]*\).*$/\1/p' \
        -e 's/^DECLARE_UNIQUE_INDEX([^,]*, *\([0-9][0-9]*\).*$/\1/p' \
        -e 's/^DECLARE_TOAST([^,]*, *\([0-9][0-9]*\), *\([0-9][0-9]*\).*$/\1,\2/p' | \
        # 根据正则表达式匹配，并将行替换为oid(,oid)形式，其他无关字符去掉
tr ',' '\n' | \ # 将，替换为换行符
sort -n | \ # 对结果按数值排序
uniq | \ # 去掉临近的重复行
$AWK ' # 以下将上述内容使用awk用漂亮的方式显示出来（单独的直接显示，连续的用 x - y 方式，有多漂亮。。。）
BEGIN {
        last = 0;
}
/^[0-9]/ {
        if ($1 > last + 1) {
                if ($1 > last + 2) {
                        print last + 1, "-", $1 - 1;
                } else {
                        print last + 1;
                }
        }
        last = $1;
}
END {
        print last + 1, "-", ENVIRON["FIRSTOBJECTID"]-1;

```

参考：
[linux sed](http://www.cnblogs.com/ggjucheng/archive/2013/01/13/2856901.html)  
[linux grep](http://www.cnblogs.com/ggjucheng/archive/2013/01/13/2856896.html)  
[linux awk](http://www.cnblogs.com/ggjucheng/archive/2013/01/13/2858470.html)  
[linux sort+uniq](http://www.cnblogs.com/ggjucheng/archive/2013/01/13/2858385.html)  
[OID源码分析by德哥](http://blog.163.com/digoal@126/blog/static/163877040201571885354927)  
