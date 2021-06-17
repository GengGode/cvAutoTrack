# GenshinImpact_AutoTrack_DLL

能够从原神中获取角色在地图上的位置，通过opencv的图像匹配算法

# 接口

extern "C" __declspec(dllexport) bool __stdcall init();
extern "C" __declspec(dllexport) bool __stdcall GetTransforn(float &x, float &y, float &a);
extern "C" __declspec(dllexport) bool __stdcall GetUID(int &uid);
extern "C" __declspec(dllexport) int __stdcall GetLastErr();
extern "C" __declspec(dllexport) bool __stdcall uninit();

# 错误码

error_code = 1;//未初始化
error_code = 2;//未能找到原神窗口句柄
error_code = 3;//窗口画面为空
error_code = 4;//未能匹配到特征点
error_code = 5;//原神小地图区域为空或者区域长宽小于60px
error_code = 6;//未能匹配到派蒙
