# GenshinImpact_AutoTrack_DLL

能够从原神中获取角色在地图上的位置，通过opencv的图像匹配算法

# 接口

bool init();

初始化函数，初始化之后才能调用 GetTransforn 函数，整个过程大概会持续1-10s，内存占用峰值1GB，之后稳定占用270MB左右。

bool GetTransforn(float &x, float &y, float &a);

获取当前所在位置以及箭头朝向，返回True为成功得到数据，返回False为未成功匹配到位置，调用 GetlastErr 获取错误码查看细节，此时数据不会被改变。

bool GetUID(int &uid);

获取当前UID，返回True为成功得到UID，返回False为未成功获取UID，调用 GetlastErr 获取错误码查看细节，此时数据不会被改变。

int GetLastErr();

返回最后一次错误的错误码，如果此前没有错误，将返回0。

bool uninit();

卸载初始化时所占用的内存，此后需要再次初始化才能继续调用 GetTransforn 函数。

# 错误码

error_code = 1;//未初始化

error_code = 2;//未能找到原神窗口句柄

error_code = 3;//窗口画面为空

error_code = 4;//未能匹配到特征点

error_code = 5;//原神小地图区域为空或者区域长宽小于60px

error_code = 6;//未能匹配到派蒙
