---
title: vim插件使用（一）--使用vundle管理插件.md
date: 2017-04-16 17:16:52
tags: vim
---

相比sublime text2等现代编辑器，Vim缺乏默认的插件管理器，所有插件的文件都散布在\~/.vim下的几个文件夹中，配置vim的过程,就是在网上不停的搜插件，拷贝到~/.vim下，发现更新，要重新下载重新拷贝，想要删除某个不需要插件，更是要小心翼翼的不要删错。配置出顺手的Vim, 需要极大的耐心和运气。
当然vim社区的活力是不容怀疑的，没有枪没有炮大神们自己造。以前使用过tpope的pathogen，使用git submodule和pathogen,管理插件已经大大简化。而今天介绍的vundle,更是把git操作整合进去，用户需要做的只是去Github上找到自己想要的插件的名字，安装，更新和卸载都可有vundle来完成了。虽然去发现一个好的插件仍然是一个上下求索的过程，但是用户已经可以从安装配置的繁琐过程解脱了。
Vundle的具体介绍查看[vim.org](http://www.vim.org/scripts/script.php?script_id=3458),或者[github repo](https://github.com/gmarik/vundle)

## 安装和配置

**下载**

    git clone https://github.com/gmarik/vundle.git ~/.vim/bundle/vundle

**在.vimrc 中添加bundle的配置**

```
set nocompatible                " be iMproved
filetype off                    " required!
set rtp+=~/.vim/bundle/vundle/
call vundle#rc()
" let Vundle manage Vundle
Bundle 'gmarik/vundle'
"my Bundle here:
"
" original repos on github
Bundle 'kien/ctrlp.vim'
Bundle 'sukima/xmledit'
Bundle 'sjl/gundo.vim'
Bundle 'jiangmiao/auto-pairs'
Bundle 'klen/python-mode'
Bundle 'Valloric/ListToggle'
Bundle 'SirVer/ultisnips'
Bundle 'Valloric/YouCompleteMe'
Bundle 'scrooloose/syntastic'
Bundle 't9md/vim-quickhl'
" Bundle 'Lokaltog/vim-powerline'
Bundle 'scrooloose/nerdcommenter'
"..................................
" vim-scripts repos
Bundle 'YankRing.vim'
Bundle 'vcscommand.vim'
Bundle 'ShowPairs'
Bundle 'SudoEdit.vim'
Bundle 'EasyGrep'
Bundle 'VOoM'
Bundle 'VimIM'
"..................................
" non github repos
" Bundle 'git://git.wincent.com/command-t.git'
"......................................
filetype plugin indent on
```

bundle分为三类：
- 在Github vim-scripts 用户下的repos,只需要写出repos名称
- 在Github其他用户下的repos, 需要写出”用户名/repos名”
- 不在Github上的插件，需要写出git全路径

## 使用

- 打开一个vim, 运行:BundleInstall
- 在命令行运行 vim +BundleInstall +qall

安装完成后插件就能用了

**其它常用命令：**

- 更新插件:BundleUpdate
- 清除不再使用的插件:BundleClean,
- 列出所有插件:BundleList
- 查找插件:BundleSearch

出自：http://zuyunfei.com/2013/04/12/killer-plugin-of-vim-vundle
