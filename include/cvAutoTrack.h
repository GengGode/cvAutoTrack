#pragma once
#ifdef CVAUTOTRACK_EXPORTS
#define CVAUTOTRACK_PORT __declspec(dllexport)
#else
#define CVAUTOTRACK_PORT __declspec(dllimport)
#endif
#define CVAUTOTRACK_CALL __stdcall
#define CVAUTOTRACK_API CVAUTOTRACK_PORT CVAUTOTRACK_CALL

/// @brief 【即将删除】旧版兼容，请使用InitResource代替
/// @return 初始化状态
extern "C" bool CVAUTOTRACK_API init();

/// @brief 【即将删除】旧版接口兼容，请使用UnInitResource代替
/// @return 是否卸载成功
extern "C" bool CVAUTOTRACK_API uninit();

#ifdef _DEBUG
/// @brief 【开发保留】加载调试使用的地图图片，以用于可视化点位
/// @brief 正式版本请移除此借口
/// @param path 调试用的地图图片路径
/// @return 
extern "C" bool CVAUTOTRACK_API LoadDbgMapImg(const char* path);
#endif

/// @brief 资源初始化
/// @return 初始化状态
extern "C" bool CVAUTOTRACK_API InitResource();
/// @brief 卸载资源
/// @return 是否卸载成功
extern "C" bool CVAUTOTRACK_API UnInitResource();

/// @brief 启用循环调用服务
/// @return 启用成功
extern "C" bool CVAUTOTRACK_API StartServer();
/// @brief 停止循环调用服务
/// @return 停止成功
extern "C" bool CVAUTOTRACK_API StopServer();
/// @brief 循环调用间隔
/// @param interval_ms 间隔时间
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetServerInterval(int interval_ms);
/// @brief 设置循环调用回调函数
/// @param callback 回调函数
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetServerCallback(void (*callback)(const char* json_buff, int buff_size));

/// @brief 设置禁用日志文件
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetDisableFileLog();
/// @brief 设置启用日志文件
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetEnableFileLog();
/// @brief 设置日志文件路径
/// @param path 路径，如果路径不存在会自动创建，默认为当前目录
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetLogFilePath(const char* path);
/// @brief 设置日志文件名
/// @param name 文件名，默认为cvAutoTrack.log
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetLogFileName(const char* name);
/// @brief 设置使用Bitblt截图模式
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetUseBitbltCaptureMode();
/// @brief 设置使用WidnowsGraphicsCaphics截图模式
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetUseGraphicsCaptureMode();
/// @brief 设置使用Dwm截图模式
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetUseDwmCaptureMode();
/// @brief 设置使用本地图片截图模式
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetUseLocalPictureMode();
/// @brief 设置使用本地图片截图模式
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetUseLocalVideoMode();
/// @brief 设置采集句柄
/// @param handle 句柄，传入0则自动寻找句柄
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetCaptureHandle(long long int handle);
/// @brief 设置采集句柄回调函数
/// @param callback 回调函数
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetCaptureHandleCallback(long long int (*callback)());
/// @brief 设置视窗源回调函数
/// @param callback 回调函数
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetScreenSourceCallback(void (*callback)(const char *image_encode_data, int &image_data_size));
/// @brief 设置视窗源回调函数扩展
/// @param callback 回调函数
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetScreenSourceCallbackEx(void (*callback)(const char *image_data, int &image_width, int &image_height, int &image_channels));
/// @brief 设置视窗源图片
/// @param image_encode_data 图片文件二进制数据
/// @param image_data_size 图片文件二进制数据大小
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetScreenSourceImage(const char* image_encode_data, int image_data_size);
/// @brief 设置视窗源图片扩展
/// @param image_data 图片数据指针
/// @param image_width 图片宽度
/// @param image_height 图片高度
/// @param image_channels 图片通道数
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetScreenSourceImageEx(const char* image_data, int image_width, int image_height, int image_channels);
/// @brief 设置视窗客户区位置回调函数，用于裁剪处理视窗源
/// @param callback 回调函数
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetScreenClientRectCallback(void (*callback)(int& x, int& y, int& width, int& height));

/// @brief 设置缓存路径
/// @param path 现有路径，默认为当前目录
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetTrackCachePath(const char* path);
/// @brief 设置缓存文件名
/// @param name 文件名，默认为cvAutoTrack.Cache
/// @return 找到缓存文件
extern "C" bool CVAUTOTRACK_API SetTrackCacheName(const char* name);
/// @brief 设置世界坐标原点
/// @param x 原点x坐标
/// @param y 原点y坐标
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetWorldCenter(double x, double y);
/// @brief 设置世界坐标缩放
/// @param scale 缩放比例
/// @return 设置成功
extern "C" bool CVAUTOTRACK_API SetWorldScale(double scale);

extern "C" bool CVAUTOTRACK_API ImportMapBlock(int uuid, const char* image_encode_data, int image_data_size);
extern "C" bool CVAUTOTRACK_API ImportMapBlockData(int uuid, const char* image_data, int image_width, int image_height, int image_channels);
extern "C" bool CVAUTOTRACK_API ImportMapBlockCenter(int uuid, int x, int y);
extern "C" bool CVAUTOTRACK_API ImportMapBlockRelativeCenter(int uuid, int parent_uuid, int x, int y);
extern "C" bool CVAUTOTRACK_API ImportMapBlockCenterScale(int uuid, int x, int y, double scale);
extern "C" bool CVAUTOTRACK_API ImportMapBlockRelativeCenterScale(int uuid, int parent_uuid, int x, int y, double scale);

extern "C" bool CVAUTOTRACK_API GetTransformOfMap(double& x, double& y, double& a, int& mapId);
extern "C" bool CVAUTOTRACK_API GetPositionOfMap(double& x, double& y, int& mapId);
extern "C" bool CVAUTOTRACK_API GetDirection(double& a);
extern "C" bool CVAUTOTRACK_API GetRotation(double& a);
extern "C" bool CVAUTOTRACK_API GetStar(double& x, double& y, bool& isEnd);
extern "C" bool CVAUTOTRACK_API GetStarJson(char* json_buff, int buff_size);
extern "C" bool CVAUTOTRACK_API GetUID(int& uid);
extern "C" bool CVAUTOTRACK_API GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid);

extern "C" bool CVAUTOTRACK_API GetInfoLoadPicture(const char* path, int& uid, double& x, double& y, double& a);
extern "C" bool CVAUTOTRACK_API GetInfoLoadVideo(const char* path, const char* out_path);

extern "C" bool CVAUTOTRACK_API DebugCapture();
extern "C" bool CVAUTOTRACK_API DebugCapturePath(const char* path);

extern "C" int CVAUTOTRACK_API GetLastErr();
extern "C" int CVAUTOTRACK_API GetLastErrMsg(char* msg_buff, int buff_size);
extern "C" int CVAUTOTRACK_API GetLastErrJson(char* json_buff, int buff_size);

extern "C" bool CVAUTOTRACK_API GetMapIsEmbedded();
extern "C" bool CVAUTOTRACK_API GetCompileVersion(char* version_buff, int buff_size);
extern "C" bool CVAUTOTRACK_API GetCompileTime(char* time_buff, int buff_size);

/// @brief 
/// @param doc_buff 
/// @param buff_size 
/// @return 
extern "C" bool CVAUTOTRACK_API GetHelpDoc(char* doc_buff, int buff_size);
