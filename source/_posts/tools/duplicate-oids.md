---
title: duplicate_oids
date: 2016-06-24 11:46:33
tags: [PG_src,tools,duplicate_oids]
categories: [PG_src,tools]
---

> src/include/catalog下脚本duplicate_oids用来找到重复定义的OID

脚本解析如下：
```perl
#!/usr/bin/perl
 
use strict;
use warnings;
 
BEGIN
{
    # 获取当前目录pg_开头及indexing，toasting头文件
    @ARGV = (glob("pg_*.h"), qw(indexing.h toasting.h));
}
 
my %oidcounts;

while (<>) #循环读取@ARGV中所指定的文件，每次获得一行，如果没指定文件，读取标准输入
{
    next if /^CATALOG\(.*BKI_BOOTSTRAP/; #匹配到continue
    next
      unless /^DATA\(insert *OID *= *(\d+)/ #没匹配到continue
          || /^CATALOG\([^,]*, *(\d+).*BKI_ROWTYPE_OID\((\d+)\)/
          || /^CATALOG\([^,]*, *(\d+)/
          || /^DECLARE_INDEX\([^,]*, *(\d+)/
          || /^DECLARE_UNIQUE_INDEX\([^,]*, *(\d+)/
          || /^DECLARE_TOAST\([^,]*, *(\d+), *(\d+)/;
    $oidcounts{$1}++; #hash的value值加1
    $oidcounts{$2}++ if $2; #外部表一次插入两个OID
}
 
my $found = 0;
 
#将重复的额oid打印出来
foreach my $oid (sort { $a <=> $b } keys %oidcounts)
{
    next unless $oidcounts{$oid} > 1;
    $found = 1;
    print "$oid\n";
}
 
#返回值
exit $found;
```
