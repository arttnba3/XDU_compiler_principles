# 西安电子科技大学 - 编译原理大作业 by arttnba3

由 cmake 构建的跨平台开箱即用的大作业，使用 tkinter 进行绘图

## 食用指南

~~cmake大家都用过吧，这里就不赘叙了~~

>  为了让你的代码目录看起来没有那么混乱，笔者这里推荐新建一个目录来存放 cmake 生成的东西
>
> 以下 shell 命令 Windows / Linux 平台通用

首先创建一个新文件夹 `build`，我们要在这里面完成项目构建

```shell
mkdir build
cd build
```

然后使用 cmake 自动进行构建

```shell
cmake ..
```

### Compile for Windows

> 对于 Windows 系统，你需要提前安装好 visual studio

cmake 构建完成之后会在我们创建的 `build` 目录下生成一个 visual studio project，如下：

![image.png](https://i.loli.net/2021/11/19/ps4eVBAFRn2hwzD.png)

然后直接双击 `A3DrawLang.sln`，用 visual studio 打开即可

需要注意的是这里我们需要**手动设置启动项为 A3DrawLang**，在左边的解决方案管理器中选中 `A3DrawLang`，然后点击 `项目`，点击 `设为启动项目`

![image.png](https://i.loli.net/2021/11/19/Z8HFTE2ekj1ioNs.png)

### Compile for Linux

> 对于 Linux 系统，你需要提前安装好 gcc 组件及 makefile

make 构建完成之后会在我们创建的 `build` 目录下复制一份源码并生成对应的 makefile 文件，如下：

![image.png](https://i.loli.net/2021/11/19/mVhdKBqIg1tfQji.png)

直接打开 shell 输入：

```shell
make
```

之后一个热腾腾的可执行文件 `A3DrawLang` 就会出现在当前目录下

### Running

命令执行格式如下，你需要手动指定代码源文件路径作为第一个参数：

```shell
./A3compiler [object file] [opt]
```

对我们输入的绘图代码源文件编译成功之后会在当前目录下生成 `draw.py` 文件，你可以选择直接运行也可以不运行，默认是直接运行

```shell
./A3DrawLang example.a3lang 
Start to parse...
Done.
Interpretation done. run it now? [Y/n]
n
Result saved in ./drawer.py
```

当你指定 `opt` 参数为 test 时，会逐个输出文件中读到的 token

```shell
./A3DrawLang ./example.a3lang test
Testing Token...
token 0: letme: ROT type: 2 val: 0 func ptr: 0
token 1: letme: IS type: 3 val: 0 func ptr: 0
token 2: letme: 0 type: 20 val: 0 func ptr: 0
token 3: letme: NULL type: 10 val: 0 func ptr: 0
...
token 174: letme: NULL type: 12 val: 0 func ptr: 0
token 175: letme: NULL type: 10 val: 0 func ptr: 0
```

#### _*for visual studio_

如果你不想看黑乎乎的 shell 界面，想直接用 visual studio 来运行，你需要额外指定源文件路径作为参数

选中 `项目->属性`

![image.png](https://i.loli.net/2021/11/19/Qwi5GcpRyhkC8YS.png)

在 `配置属性->调试` 中设置 `命令参数`，输入的代码源文件应当同样放在 `build` 目录下

![image.png](https://i.loli.net/2021/11/19/ixgZ8H3tyPXDhqL.png)

#### example

测试例程如下：

```a3lang
rot is 0;
origin is (50, 400);
scale is (2, 1);
for T from 0 to 300 step 0.01 draw (t, 0);
for T from 0 to 300 step 0.01 draw (0, -t);
for T from 0 to 300 step 0.01 draw (t, -t);
scale is (2, 0.1);
for T from 0 to 55 step 0.01 draw (t, -(t*t));
scale is (10, 5);
for T from 0 to 60 step 0.01 draw (t, -sqrt(t));
scale is (20, 0.1);
for T from 0 to 8 step 0.01 draw (t, -exp(t));
scale is (2, 20);
for T from 0 to 300 step 0.01 draw (t, -ln(t));
```

效果如下图所示：

> Linux

![image.png](https://i.loli.net/2021/11/19/QzG4sg1TpUFtjOo.png)

>  Windows

![image.png](https://i.loli.net/2021/11/19/FUHZf7XhaWKcPMN.png)

## 实验报告

什么？👴连代码都白给了实验报告还想让👴帮你写？能不能爪巴嗷

srds，👴还是大发善心帮你✍了一份：[https://github.com/arttnba3/compiler_principles/report.pdf](https://github.com/arttnba3/compiler_principles/report.pdf)

