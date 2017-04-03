# beebox
A wrapper for libuv with old C++(03)  and sigsolt

## 介绍

Beebox使用C++（03）借助sigslot对libuv的Loop、Thread、TCP&UDP Socket等功能进行了封装。
加上File、Date、SQLite3、Utils等常用功能封装，同时内置了一个简单的HTTP服务器。
以此形成了一个简单的开发框架，适合嵌入式开发。
支持Windows、Linux平台。

## 构建

Linux：

    $ cd build
    $ ./make.sh

在lib目录下生成libbeebox.a
 
## Introduction

Beebox uses old C++(03), a wrapper for libuv (Loop, Thread, TCP & UDP Socket etc.) based on sigslot.
Include File, Date, SQLite3, Utils and other usefull funcions, even offered a simple HTTP Server.
And forming a simple framework for embedded development.
Support Windows, Linux.

## Build

Linux：

    $ cd build
    $ ./make.sh

It will get libbeebox.a under lib/
