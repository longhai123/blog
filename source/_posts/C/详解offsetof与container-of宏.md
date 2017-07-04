---
title: 详解offsetof与container_of宏.md
date: 2017-05-16 09:51:42
tags: C
---

## 前言

关于offsetof与container_of宏,以前见过,同事也分享过,但是一直没有太过深究.

这次面试,被问到之后并没有很好的对答如流,回来也好好总结了一下,也试着分享一下.

## offsetof宏

定义:

```
#define offsetof(TYPE, MEMBER) (size_t)&(((TYPE*)0)->MEMBER)
```
抽丝剥茧:
```
(TYPE *)0 //将地址0强转为TYPE类型的指针
((TYPE *)0)->MEMBER //引用TYPE类型的MEMBER成员
&(((TYPE *)0)->MEMBER) //获取MEMBER成员你的地址
(size_t)&(((TYPE *)0)->MEMBER) //上述地址转化为size_t类型
```
总结:
> 获取MEMBER变量相对于结构体TYPE的偏移量

## container_of宏

定义:
```
#define container_of(ptr, TYPE, MEMBER) ({\
    const typeof(((TYPE *)0)->MEMBER) *__ptr = (ptr);\
    (TYPE *)((char *)__ptr - offsetof(TYPE, MEMBER));})
```
抽丝剥茧:
```
((TYPE *)0)->MEMBER //引用TYPE类型的MEMBER成员
typeof(((TYPE *)0)->MEMBER) //获取MENBER的类型
const typeof(((TYPE *)0)->MEMBER) *__ptr = (ptr) //ptr保存到上述类型的指针__ptr中
(TYPE *)((char *)__ptr - offsetof(TYPE, MEMBER)) //通过上述地址减去偏移量得到type的起始地址
```
总结:
> 通过结构体某个成员的地址得到整个结构体的起始地址

说明:
> 使用\__ptr的目的是在编译期间进行类型检测(第一句，赋值时如果类型不匹配会报告警)，
保证传入的成员地址与成员类型是匹配的，而在运行期间则和忽略中间变量__ptr是一样的。
