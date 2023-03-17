# 西安电子科技大学 - 编译原理大作业 by arttnba3

由 cmake 构建的跨平台开箱即用的大作业，使用 tkinter 进行绘图

> 注：以前写的老版本已经移动到 `old_trash_version/` 下，虽然也能用但是**整个代码本质上就是一坨 shit**，所以笔者找时间重构了一下，**功能上并无差异，只是代码架构有所区别**，请自行按需取用：） 

## 食用指南

本代码通用于 Linux/Windows 平台，但是首先你需要安装一个 [cmake](https://cmake.org/) ：）

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

### I. Use on Windows

> 对于 Windows 系统，你需要提前安装好 visual studio

cmake 构建完成之后会在我们创建的 `build` 目录下生成一个 visual studio project，如下：

![image.png](https://s2.loli.net/2023/03/17/7BfAWjQRXOsLgG2.png)

然后直接双击 `A3DrawLang.sln`，用 visual studio 打开即可

需要注意的是这里我们需要**手动设置启动项为 A3DrawLang**，在左边的解决方案管理器中选中 `A3DrawLang`，然后点击 `项目`，点击 `设为启动项目` 后就能直接运行了

![image.png](https://s2.loli.net/2023/03/17/3ZvkACeoVWw5h9X.png)

运行后一个热腾腾的可执行文件 `A3DrawLang.exe` 就会出现在 `./Debug/` 目录下

### II. Use on Linux

> 对于 Linux 系统，你需要提前安装好 gcc 组件及 makefile

make 构建完成之后会在我们创建的 `build` 目录下复制一份源码并生成对应的 makefile 文件，如下：

![image.png](https://i.loli.net/2021/11/19/mVhdKBqIg1tfQji.png)

直接打开 shell 输入：

```shell
make
```

之后一个热腾腾的可执行文件 `A3DrawLang` 就会出现在当前目录下

### III. Running

我们可以通过附加 `--help` 选项来查看帮助：

```
./A3DrawLang --help
Usage: A3DrawLang [options] file
Options:
  --help                   Display this information.
  -o <file>                Place the output into <file>.
                           If it's not set, it'll be 'drawer.py'.
  --test                   Scan and print the tokens only.

For bug reporting, please issue it on:
https://github.com/arttnba3/XDU_compiler_principles/issues
```

编译器默认以第一个无指定参数作为输入文件，若未指定输出文件则默认将 python 绘图代码输出至 `./drawer.py` 中,完成编译后你可以让编译器直接运行该文件以进行绘图，也可以稍后自行运行该文件，默认是直接运行

```shell
$ ./A3DrawLang ./example.a3lang 
Interpretation done. run it now? [Y/n]

Press enter to exit...

```

当你指定了参数 `--test` 时，会逐个输出文件中读到的 token

```shell
$ ./A3DrawLang ./example.a3lang --test
lexeme: ROT, type: 2, val: 0.000000, fn: (nil)
lexeme: IS, type: 3, val: 0.000000, fn: (nil)
lexeme: 0, type: 20, val: 0.000000, fn: (nil)
lexeme: , type: 10, val: 0.000000, fn: (nil)
lexeme: ORIGIN, type: 0, val: 0.000000, fn: (nil)
lexeme: IS, type: 3, val: 0.000000, fn: (nil)
lexeme: , type: 11, val: 0.000000, fn: (nil)
lexeme: 50, type: 20, val: 50.000000, fn: (nil)
...
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

## ~~实验报告~~

什么？👴连代码都白给了实验报告还想让👴帮你写？能不能爪巴嗷

> 代码框架大改后的新版本的实验报告短时间内估计是没有了（谁tm闲着有时间还重新写一遍这玩意），不过需要注意的是**重构版本的代码虽然也实现了实验所要求的绘图语言，不过所用代码和 ppt 上大有不同**，`old_trash_version/` 下的老版本则更多的是循规蹈矩地按照课上的 ppt 来弄的, 看你自己的需求取用了：）
>
> 以及 `old_trash_version/` 下有老版本的实验报告，👴寻思你也可以照着抄一抄，词法分析的逻辑是基本没变的，不过语法分析阶段会生成独立的 AST，在语义分析时才进行绘图等操作，以及整个项目框架和老版本相比不算大改也算中改，这个就需要你自己去读👴的源码自己想想该怎么写实验报告了
