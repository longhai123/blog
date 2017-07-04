---
title: win7_64下postgresql编译
date: 2016-06-24 14:44:45
tags: [PG_src,kernel,win_compile]
categories: [PG_src,kernel]
---
>摘要
对于windows用户来说（linux用户也可以，毕竟vs是个好工具，毕竟很多人的工作环境都是win+linux虚拟机），要研究pg内核，首先必须掌握windows编译吧，所以提供给初学者，至于大牛们，请绕道而行，或者不吝赐教！

1、flex&bison安装
本着我自己去找官网什么的老是打不开啊神马的，然后下源码什么的安装还是linux玩的溜，所以直接把我的打包放上来，解压就能用（如果自己去找找网站研究一下其实收获会更大），不要放到Program Files或者Program Files (x86)路径下啊，有空格的话会有各种问题。参考路径：D:\GnuWin32，然后把环境变量加上（这个不知道的就百度一下咯）。
检验方式直接上图算了：
![flex_bison_version.jpg](/images/flex_bison_version.jpg)
http://pan.baidu.com/s/1wUfTW

<!-- more -->

2、perl安装
perl的官网还是很好找的，百度就能找到。但是经过我的试验哈，我现在写这篇文章的时候5.22版本貌似不好使，编译的时候会出现很多的宏未找到的错误，所以推荐5.20的版本，但是我在写这篇文章的时候呢5.20版本的win64位官网是不能下载的（点背），所以其他地方找了一个，网盘也奉上算了。
http://pan.baidu.com/s/1dDdlhXR
打开cmd，然后perl --version可以查看安装是否正确。

干货：假如安装的第一个perl版本编译的时候出现那个很多宏找不到的错误，好，更换perl版本，这时候记得在编译之前使用clean.bat把原来编译出来的一些东西清除掉，免得下次还是会报错。（本人也一度没有注意这个问题东东，perl版本从5.22-5.18-5.20，我估计5.18也好使，留给有心人去验证了哈。）

3、vs2008安装
本人绝对是个好人，直接安装包再来，免去您老到处寻找的痛苦：
http://pan.baidu.com/s/1c0sgqPi
安装时选择“完全”吧，作为一个开发人员，电脑至少得说的过去吧，资源浪费在学习工具上是值得的。电脑实在带不起的话，果断砸了，换新的！

4、编译
![vs_compile.jpg](/images/vs_compile.jpg)
打开如图命令提示，cd /d E:\postgresql-9.4.4\src\tools\msvc进入目录，perl build.pl DEBUG进行编译

1.Unable to determine Visual Studio version: The nmake version could not be determined. at src/tools/msvc/Mkvcbuild.pm line 63.
提示找不到vs版本，解决方案：
E:\postgresql-9.4.4\src\tools\msvc\VSObjectFactory.pm中CreateSolution函数第二行直接添加return new VS2008Solution(@_);【其中的2008根据具体情况而定，你要是安装的vs2010，这个填写2010】
![find_vs_version1.jpg](/images/find_vs_version1.jpg)
E:\postgresql-9.4.4\src\tools\msvc\Mkvcbuild.pm修改如下：
![find_vs_version2.jpg](/images/find_vs_version2.jpg)

2.忘了截图了，真是个悲剧，偷个懒就不恢复错误环境截出现问题的图了，反正就是很多的宏找不到的问题。譬如这样的，出现50多个这个的错误吧：TRACE_POSTGRESQL_MULTIXACT_CHECKPOINT_DONE
这个问题呢，就是perl版本问题，更换perl版本，记得clean.bat一下先！（上面已经说过了，重要的事情说了第二遍！）
请耐心等待。。。
好了，成功一般都是报11个警告（pg944，等亲爱的你看到这篇文章的时候可能版本又不一样了，警告个数不同了也不一定，可以不管），这时候所有的vs工程也都编译出来啦，熟悉了吧。看见sln应该安心了呀~
![compile_success.jpg](/images/compile_success.jpg)

5、安装
简单：
install.bat 目的地
例如 
![win_install_pg.jpg](/images/win_install_pg.jpg)
最后再说一遍，clean.bat这个还是可以经常试试的，在编译过程中假如出错了的话（重要的事情说三遍！这是写这篇文章时下比较流行的网络语，有可能你看的时候早就没人记得这句话了，时光荏苒啊）。