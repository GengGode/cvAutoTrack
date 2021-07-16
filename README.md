# GenshinImpact AutoTrack DLL

一个通过opencv图像匹配算法，从原神客户端中获取角色在地图上的位置的DLL动态链接库。

[![GitHub version](https://badge.fury.io/gh/GengGode%2FGenshinImpact_AutoTrack_DLL.svg)](https://badge.fury.io/gh/GengGode%2FGenshinImpact_AutoTrack_DLL) [![Build status](https://ci.appveyor.com/api/projects/status/1q2jfn373bc15raa?svg=true)](https://ci.appveyor.com/project/GengGode/genshinimpact-autotrack-dll) ![convention](https://img.shields.io/badge/convention-__cdecl-orange.svg) ![platform](https://img.shields.io/badge/platform-Windows-blue.svg) ![](https://img.shields.io/badge/cpu-AMD64-purple.svg)

## 如何使用

1. 下载编译好的动态链接库。
   - https://pan.baidu.com/s/1y-lgkGiyIJPa3_y0aRlOnQ（提取码：e7zv）
   - https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/releases/
2. 装载动态链接库后，根据[函数手册](#函数手册)对相关函数进行调用或封装。
3. 部分语言的调用可参见 `impl` 文件夹内的调用示例。

## 注意事项

- 目前只编译了 Windows x64 平台上的动态链接库（.dll），对于 Windows x86 或其它平台，如 Linux 平台（.so），需要下载源代码另行编译。
- Windows 规定 64位 进程/DLL与 32位 进程/DLL之间不能相互调用/加载，因此调用动态链接库前，请务必确保调用进程是 **64位进程** 。
- 本项目借游戏画面的窗口截图进行图像处理算法以实现所有功能，其不会对游戏内存进行读写，因而不会有封号的风险，但效果也因此具备一定的局限。
- 项目仅在有限的条件下测试过，如需排查错误，强烈建议按照以下描述进行环境配置。
  - 原神客户端 > 右上角派蒙 > 设置 > 抗锯齿，设置为 `SMAA`
  - 原神客户端 > 右上角派蒙 > 设置 > 分辨率，设置为 `1920x1080`

## 对于开发者

- 码云上的仓库：[https://gitee.com/Yu_Sui_Xian/yuanshen-auto-tracking-dll](https://gitee.com/Yu_Sui_Xian/yuanshen-auto-tracking-dll)


## 项目结构

- **cvAutoTrack**，dll工程
- **cvAT_dllTest**，C++下调用dll的测试工程
- **impl**，部分语言的调用示例。


# 函数手册

## init

```C++
bool init();
```

初始化运行环境。

### 返回值

- `true` 表示初始化完毕。当前处在 **已初始化** 状态。
- `false` 表示初始化失败。当前处在 **未初始化** 状态。

### 说明

部分函数需要初始化后才能调用，具体见 [函数手册](#函数手册) 其它函数。

经过简单测试，初始化的过程大概会持续1-10秒，内存占用峰值约为 1 GB，之后稳定在 270 MB 左右。

已经处在 **已初始化** 状态则不会进行任何操作。



## uninit

```C++
bool uninit();
```

卸载初始化时所占用的内存。

### 返回值

- `true` 表示卸载成功，所有需要初始化后才能调用的函数此时不能再被调用。当前处在 **未初始化** 状态。
- `false` 表示卸载失败。当前处在 **已初始化** 状态。

### 说明

已经处在 **未初始化** 状态则不会进行任何操作。



## GetLastErr

```C++
int GetLastErr();
```

获取最后设置的错误码。

### 返回值

返回一个整数，表示错误码。错误码含义如下：

| 错误码 | 含义                                     | 可能的原因 |
| ------ | ---------------------------------------- | ---------- |
| `0`    | 执行成功/正常退出。                      |            |
| `1`    | 未初始化（当前处在 **未初始化** 状态）。 |            |
| `2`    | 未能找到原神窗口句柄。                   |            |
| `3`    | 窗口画面为空。                           |            |
| `4`    | 未能匹配到特征点。                       |            |
| `5`    | 原神小地图区域为空或者区域长宽小于60px。 |            |
| `6`    | 未能匹配到派蒙。                         |            |
| `7`    | 特征点数组访问越界，是个bug。            |            |
| `8`    | 未能在UID区域检测到有效UID。             |            |
| `9`    | 提取小箭头特征误差过大。                 |            |
| `10`   | 无效句柄或指定句柄所指向窗口不存在。     |            |
| `11`   | 未能取到小箭头区域。                     |            |
| `12`   | 窗口句柄失效。                           |            |

### 说明

由于函数成功执行后可能不会将错误码设为 `0` ，以及一些函数不会设置错误码，因此取出的错误码不一定代表前一条函数产生/引发的错误。



## SetHandle

```C++
bool SetHandle(
    long long int handle
);
```

设置原神客户端的窗口句柄。

### 参数

- `handle` 原神客户端的窗口句柄。如果提供非零值，则程序不再自动获取窗口句柄，如果提供 `0`，则自动获取。

### 返回值

- `true` 表示提供的值是窗口句柄。
- `false` 表示提供的值并不是窗口句柄。

### 说明

某些原神数据的获取是基于原神客户端的，因此需要一个客户端的窗口句柄。默认情况下此句柄由程序自动获取，但有可能会因为编码问题或其它未知的原因而获取不到，此时就需要手动指定客户端的窗口句柄。

注意，不论本函数返回何值，`handle` 只要不是 `0`，就会被设置为窗口句柄，并禁止自动获取窗口句柄。



## GetTransform

```C++
bool GetTransform(
    float &x,
    float &y,
    float &a
);
```

获取当前人物所在位置以及角度（箭头朝向）。

### 参数

- `x` 获取到的人物的x轴坐标位置。
- `y` 获取到的人物的y轴坐标位置。
- `a` 获取到的角度。

### 返回值

- `true` 表示获取成功。
- `false` 表示获取失败，此时三个参数均不会被改变。

### 说明

无论成功与否都会设置 `LastErr` 值。调用 [`GetLastErr()`](#GetLastErr) 可以获取错误码。



## GetPosition

```C++
bool GetPosition(
    double &x,
    double &y
);
```

获取当前人物所在位置。

### 参数

- `x` 获取到的人物的x轴坐标位置。
- `y` 获取到的人物的y轴坐标位置。

### 返回值

- `true` 表示坐标位置获取成功。
- `false` 表示坐标位置获取失败，此时两个参数均不会被改变。

### 说明

无论成功与否都会设置 `LastErr` 值。调用 [`GetLastErr()`](#GetLastErr) 可以获取错误码。

调用本函数前，如果没有初始化过，或最后一次初始化返回失败，则需要调用 [`init()`](#init) 函数进行初始化。



## GetDirection

```C++
bool GetDirection(
    double &a
);
```

获取当前角度（箭头朝向）。

### 参数

- `a` 获取到的角度。

### 返回值

- `true` 表示获取成功。
- `false` 表示获取失败，此时参数不会被改变。

### 说明

无论成功与否都会设置 `LastErr` 值。调用 [`GetLastErr()`](#GetLastErr) 可以获取错误码。



## GetUID

```C++
bool GetUID(
    int &uid
);
```

获取在屏幕右下角显示的玩家的UID。

### 参数

- `uid` 获取到的玩家UID。

### 返回值

- `true` 表示获取成功。
- `false` 表示获取失败，此时参数不会被改变。

### 说明

无论成功与否都会设置 `LastErr` 值。调用 [`GetLastErr()`](#GetLastErr) 可以获取错误码。
