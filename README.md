this Chinese, Click to [English](https://github.com/GengGode/cvAutoTrack/blob/master/doc/README_en.md) 
# GenshinImpact AutoTrack DLL

通过图像匹配算法，从原神客户端中获取角色在地图上的位置的DLL动态链接库

[![GitHub version](https://badge.fury.io/gh/GengGode%2FcvAutoTrack.svg)](https://badge.fury.io/gh/GengGode%2FcvAutoTrack) [![dev-window-dynamic](https://github.com/GengGode/cvAutoTrack/actions/workflows/dev-window-dynamic.yml/badge.svg)](https://github.com/GengGode/cvAutoTrack/actions/workflows/dev-window-dynamic.yml)![convention](https://img.shields.io/badge/convention-__stdcall-orange.svg) ![platform](https://img.shields.io/badge/platform-Windows-blue.svg) ![](https://img.shields.io/badge/cpu-AMD64-purple.svg)

## 注意事项

- 本项目借游戏画面的窗口截图进行图像处理算法以实现所有功能，其不会对游戏内存进行读写，因而不会有封号的风险，但效果也因此具备一定的局限。
- 项目仅在有限的条件下测试过，如需排查错误，强烈建议按照以下描述进行环境配置。
  - 原神客户端 > 右上角派蒙 > 设置 > 抗锯齿，设置为 `SMAA`
  - 原神客户端 > 右上角派蒙 > 设置 > 其他 > 小地图锁定，设置为 `锁定方向`

# 介绍 

- [x] [空荧酒馆原神地图手动更新/测试教程](doc/kyjg.md)
- [x] 联系 QQ群[717355752](https://jq.qq.com/?_wv=1027&k=YvH6TsM4)

## 新版本计划 7.x

- [ ] 添加对秘境中的角度和视野识别
- [x] 实现无透明通道下的视野识别
- [x] 实现日志可控制不输出
- [x] 添加定位的惯性加速度导航算法（见 [giAlgorithmVerification](https://github.com/GengGode/giAlgorithmVerification) 库）
- [ ] 添加定位的自动校准算法
- [ ] 支持任意地图区域
- [ ] 支持传入地图而非内嵌
- [x] 可选的嵌入预计算结果
- [x] 更高的匹配精度
- [x] 使用二进制文件而不是xml序列化追踪特征点
- [x] 不再内嵌预计算结果，首次运行将会在运行目录生成缓存xml
- [x] 目前支持任意分辨率，以及手柄模式，但暂不支持小地图设置的【跟随视角】仅支持【锁定方向】

# 如何使用

1. 下载编译好的动态链接库。
   - https://github.com/GengGode/cvAutoTrack/releases/
2. 装载动态链接库后，根据[函数目录](#函数目录)对相关函数进行调用或封装。
3. 部分语言的调用可参见 [impl](impl) 文件夹内的调用示例。
4. 由于默认接口输出的是[天理坐标模型](doc/天理坐标模型.md)的坐标，所以使用者需要根据自身地图坐标系与天理坐标模型之间的映射关系，设置世界中心以及缩放系数或者后期手动换算。

# 如何编译(即将转向cmake构建)

1. 环境需求，[Visual Studio 2019 +](https://visualstudio.microsoft.com/zh-hans/vs/) 、[Opencv 4.5.0 static lib zip](https://github.com/GengGode/opencv450Release/releases/download/v1.0/Release.zip)
2. 安装 Visual Studio 2019 +
3. 创建 Opencv 库的环境变量，若不使用环境变量则需要手动修改 cvAutoTrack 项目的`附加包含目录`和`附加库目录`

    ``` $OpenCvDir = C:\projects\opencv\ ```
4. 下载 Opencv 4.5.0 static lib zip 并解压到任意目录

    ``` 7z x ./*.zip -y -o%OpenCvDir% ```
5. Clone 代码

   ``` Git Clone https://github.com/GengGode/cvAutoTrack ```

6. 进入项目文件夹解压资源文件

    ``` cd .\cvAutoTrack | 7z -x resource.zip ```
7. 编译项目

    ``` msbuild cvAutoTrack.sln ```

# 对于开发者

## 项目结构

- **cvAutoTrack**，dll工程
- **doc**，文档及部分插图
- **impl**，部分语言的调用示例
    - **Cpp/TEST_cvAutoTrack_Cpp** C++调用
    - **CSharp/TEST_cvAutoTrack_CSharp** C#调用
    - **Python** Python调用


# [函数目录](doc/函数目录.md) 

## [天理坐标模型](doc/%E5%A4%A9%E7%90%86%E5%9D%90%E6%A0%87%E6%A8%A1%E5%9E%8B.md)

## 错误处理
- ###  [运行时错误](doc/%E6%97%A0%E6%B3%95%E8%BF%90%E8%A1%8C.md) 
- ### [编译、链接、打包过程中的异常及错误]()

# 特别感谢 
   - [Opencv](https://opencv.org/)
   - [空荧酒馆-原神地图](https://yuanshen.site/docs/)
