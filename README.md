# GenshinImpact_AutoTrack_DLL

能够从原神中获取角色在地图上的位置，通过opencv的图像匹配算法。

[![Build status](https://ci.appveyor.com/api/projects/status/1q2jfn373bc15raa?svg=true)](https://ci.appveyor.com/project/GengGode/genshinimpact-autotrack-dll)

# 目录

+ ## 介绍

+ ## 接口

+ ## 错误码

# 介绍

cvAutoTrack dll工程

cvAT_dllTest C++下调用dll的测试工程

### 国内访问
| type | link |
| --- | --- |
| 国内访问 gitee | [https://gitee.com/Yu_Sui_Xian/yuanshen-auto-tracking-dll](https://gitee.com/Yu_Sui_Xian/yuanshen-auto-tracking-dll) |

### 下载链接

| type | link |
| :---: | :---: |
|国内 |链接：https://pan.baidu.com/s/1y-lgkGiyIJPa3_y0aRlOnQ 提取码：e7zv |
|国外 |链接：https://github.com/GengGode/GenshinImpact_AutoTrack_DLL/releases/ |



# 接口

> `bool init();`

初始化函数，初始化之后才能调用 GetPosition 函数。GetTransform 会自动调用初始化，不需要手动初始化。

整个过程大概会持续1-10s，内存占用峰值1GB，之后稳定占用270MB左右。

> `bool SetHandle(long long int handle);`

设置原神窗口句柄，当由于编码问题无法自动获得原神窗口句柄时，可以通过该函数手动设置原神窗口句柄。

当传入0时则恢复自动获取模式。

> `bool GetTransform(float &x, float &y, float &a);`

获取当前所在位置以及箭头朝向，返回True为成功得到数据，返回False为未成功匹配到位置。

调用 GetlastErr 获取错误码查看细节，此时数据不会被改变。（不需要初始化）

> `bool GetPosition(double &x, double &y);`

获取当前位置，返回True为成功得到数据，返回False为未成功匹配到位置。

调用 GetlastErr 获取错误码查看细节，此时数据不会被改变。（需要手动初始化）

> `bool GetDirection(double &a);`

获取当前角度，返回True为成功得到数据，返回False为未成功匹配到角度。

调用 GetlastErr 获取错误码查看细节，此时数据不会被改变。（不需要初始化）

> `bool GetUID(int &uid);`

获取当前UID，返回True为成功得到UID，返回False为未成功获取UID。

调用 GetlastErr 获取错误码查看细节，此时数据不会被改变。（不需要初始化）

> `int GetLastErr();`

返回最后一次错误的错误码，如果此前没有错误，将返回0。

> `bool uninit();`

卸载初始化时所占用的内存，此后需要再次初始化才能继续调用 GetPosition 函数。

# 错误码

| 错误码 | 意义 |
| :---: | :---: |
| 0 | 正常退出 |
| 1 | 未初始化 |
| 2 | 未能找到原神窗口句柄 |
| 3 | 窗口画面为空|
| 4 | 未能匹配到特征点|
| 5 | 原神小地图区域为空或者区域长宽小于60px |
| 6 | 未能匹配到派蒙 |
| 7 | 特征点数组访问越界，是个bug |
| 8 | 未能在UID区域检测到有效UID |
| 9 | 提取小箭头特征误差过大 |
| 10 | 无效句柄或指定句柄所指向窗口不存在 |
| 11 | 未能取到小箭头区域 |
| 12 | 窗口句柄失效 |
