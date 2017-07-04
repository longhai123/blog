---

title: Next主题显示问题

date: 2016-11-08 20:06:34

tags:

---
今天重新部署，本地显示正确，但是部署到服务器却显示不正常。

通过寻找资料，找到症结。

原因是 GitHub Pages 过滤掉了 source/vendors 目录的访问。

<!-- more -->

解决方案：

```
首先修改source/vendors为source/lib，然后修改_config.yml,
将 _internal: vendors修改为_internal:lib。
然后修改next底下所有引用source/vendors路径为source/lib。
这些地方可以通过文件查找找出来。
主要集中在这几个文件中。
1. Hexo\themes\next\.bowerrc 
2. Hexo\themes\next\.gitignore 
3. Hexo\themes\next\.javascript_ignore 
4. Hexo\themes\next\bower.json 。
修改完毕后，刷新重新g一遍就ok啦。
```

有人在github里面提了issue

link在这里：[Hexo Issue](https://github.com/iissnan/hexo-theme-next/issues/1214)
