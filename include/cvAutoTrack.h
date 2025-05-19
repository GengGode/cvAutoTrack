#ifndef CVAUTOTRACE_H
#define CVAUTOTRACE_H

#if defined(_WIN32) || defined(_WIN64) || defined(_WIN128) || defined(__CYGWIN__)
    #ifdef CVAUTOTRACK_EXPORTS
        #define CVAUTOTRACK_PORT __declspec(dllexport)
    #else
        #define CVAUTOTRACK_PORT __declspec(dllimport)
    #endif
    #define CVAUTOTRACK_API CVAUTOTRACK_PORT
#elif __GNUC__ >= 4
    #define CVAUTOTRACK_API __attribute__((visibility("default")))
#else
    #define CVAUTOTRACK_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief 【开发保留】加载调试使用的地图图片，以用于可视化点位
    /// @brief 正式版本请移除此借口
    /// @param path 调试用的地图图片路径
    /// @return
    bool CVAUTOTRACK_API DebugLoadMapImagePath(const char* path);

    /// @brief 资源初始化
    /// @return 初始化状态
    bool CVAUTOTRACK_API InitResource();
    /// @brief 卸载资源
    /// @return 是否卸载成功
    bool CVAUTOTRACK_API UnInitResource();

    bool CVAUTOTRACK_API SetCacheConfig(const char* config_file, const char* blocks_dir, const char* config, int config_size);

    bool CVAUTOTRACK_API SetCoreCachePath(const char* path);
    bool CVAUTOTRACK_API GetCoreCachePath(char* path_buff, int buff_size);

    /// @brief 启用循环调用服务
    /// @return 启用成功
    bool CVAUTOTRACK_API StartServer();
    /// @brief 停止循环调用服务
    /// @return 停止成功
    bool CVAUTOTRACK_API StopServer();
    /// @brief 循环调用间隔
    /// @param interval_ms 间隔时间
    /// @return 设置成功
    bool CVAUTOTRACK_API SetServerInterval(int interval_ms);
    /// @brief 设置循环调用回调函数
    /// @param callback 回调函数
    /// @return 设置成功
    bool CVAUTOTRACK_API SetServerCallback(void (*callback)(const char* json_buff, int buff_size));

    /// @brief 设置禁用日志文件
    /// @return 设置成功
    bool CVAUTOTRACK_API SetDisableFileLog();
    /// @brief 设置启用日志文件
    /// @return 设置成功
    bool CVAUTOTRACK_API SetEnableFileLog();
    /// @brief 设置日志文件路径
    /// @param path 路径，如果路径不存在会自动创建，默认为当前目录
    /// @return 设置成功
    bool CVAUTOTRACK_API SetLogFilePath(const char* path);
    /// @brief 设置日志文件名
    /// @param name 文件名，默认为cvAutoTrack.log
    /// @return 设置成功
    bool CVAUTOTRACK_API SetLogFileName(const char* name);
    /// @brief 设置使用Bitblt截图模式
    /// @return 设置成功
    bool CVAUTOTRACK_API SetUseBitbltCaptureMode();
    /// @brief 设置使用WidnowsGraphicsCaphics截图模式
    /// @return 设置成功
    bool CVAUTOTRACK_API SetUseGraphicsCaptureMode();
    /// @brief 设置使用Dwm截图模式
    /// @return 设置成功
    bool CVAUTOTRACK_API SetUseDwmCaptureMode();
    /// @brief 设置使用本地图片截图模式
    /// @return 设置成功
    bool CVAUTOTRACK_API SetUseLocalPictureMode();
    /// @brief 设置使用本地图片截图模式
    /// @return 设置成功
    bool CVAUTOTRACK_API SetUseLocalVideoMode();
    /// @brief 设置采集句柄
    /// @param handle 句柄，传入0则自动寻找句柄
    /// @return 设置成功
    bool CVAUTOTRACK_API SetCaptureHandle(long long int handle);
    /// @brief 设置采集句柄回调函数
    /// @param callback 回调函数
    /// @return 设置成功
    bool CVAUTOTRACK_API SetCaptureHandleCallback(long long int (*callback)());
    /// @brief 设置视窗源回调函数
    /// @param callback 回调函数
    /// @return 设置成功
    bool CVAUTOTRACK_API SetScreenSourceCallback(void (*callback)(const char* image_encode_data, int& image_data_size));
    /// @brief 设置视窗源回调函数扩展
    /// @param callback 回调函数
    /// @return 设置成功
    bool CVAUTOTRACK_API SetScreenSourceCallbackEx(void (*callback)(const char* image_data, int& image_width, int& image_height, int& image_channels));
    /// @brief 设置视窗源图片
    /// @param image_encode_data 图片文件二进制数据
    /// @param image_data_size 图片文件二进制数据大小
    /// @return 设置成功
    bool CVAUTOTRACK_API SetScreenSourceImage(const char* image_encode_data, int image_data_size);
    /// @brief 设置视窗源图片扩展
    /// @param image_data 图片数据指针
    /// @param image_width 图片宽度
    /// @param image_height 图片高度
    /// @param image_channels 图片通道数
    /// @return 设置成功
    bool CVAUTOTRACK_API SetScreenSourceImageEx(const char* image_data, int image_width, int image_height, int image_channels);
    /// @brief 设置视窗客户区位置回调函数，用于裁剪处理视窗源
    /// @param callback 回调函数
    /// @return 设置成功
    bool CVAUTOTRACK_API SetScreenClientRectCallback(void (*callback)(int& x, int& y, int& width, int& height));

    /// @brief 设置缓存路径
    /// @param path 现有路径，默认为当前目录
    /// @return 设置成功
    bool CVAUTOTRACK_API SetTrackCachePath(const char* path);
    /// @brief 设置缓存文件名
    /// @param name 文件名，默认为cvAutoTrack.Cache
    /// @return 找到缓存文件
    bool CVAUTOTRACK_API SetTrackCacheName(const char* name);
    /// @brief 设置世界坐标原点
    /// @param x 原点x坐标
    /// @param y 原点y坐标
    /// @return 设置成功
    bool CVAUTOTRACK_API SetWorldCenter(double x, double y);
    /// @brief 设置世界坐标缩放
    /// @param scale 缩放比例
    /// @return 设置成功
    bool CVAUTOTRACK_API SetWorldScale(double scale);

    bool CVAUTOTRACK_API GetTransformOfMap(double& x, double& y, double& a, int& mapId);
    bool CVAUTOTRACK_API GetPositionOfMap(double& x, double& y, int& mapId);
    bool CVAUTOTRACK_API GetDirection(double& a);
    bool CVAUTOTRACK_API GetRotation(double& a);
    bool CVAUTOTRACK_API GetUID(int& uid);
    bool CVAUTOTRACK_API GetAllInfo(double& x, double& y, int& mapId, double& a, double& r, int& uid);
    bool CVAUTOTRACK_API DebugCapture();
    bool CVAUTOTRACK_API DebugCapturePath(const char* path);
    int CVAUTOTRACK_API GetLastErr();
    int CVAUTOTRACK_API GetLastErrMsg(char* msg_buff, int buff_size);
    int CVAUTOTRACK_API GetLastErrJson(char* json_buff, int buff_size);

    bool CVAUTOTRACK_API GetCompileVersion(char* version_buff, int buff_size);
    bool CVAUTOTRACK_API GetCompileTime(char* time_buff, int buff_size);
    bool CVAUTOTRACK_API GetCoreModulePath(char* path_buff, int buff_size);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    // 定义上下文结构体
    struct cvAutoTrackContextV1
    {
        // 开发保留接口
        bool (*DebugLoadMapImagePath)(const char* path);

        // 资源管理接口
        bool (*InitResource)();
        bool (*UnInitResource)();

        // 缓存配置接口
        bool (*SetCacheConfig)(const char*, const char*, const char*, int);
        bool (*SetCoreCachePath)(const char*);
        bool (*GetCoreCachePath)(char*, int);

        // 服务控制接口
        bool (*StartServer)();
        bool (*StopServer)();
        bool (*SetServerInterval)(int);
        bool (*SetServerCallback)(void (*)(const char*, int));

        // 日志配置接口
        bool (*SetDisableFileLog)();
        bool (*SetEnableFileLog)();
        bool (*SetLogFilePath)(const char*);
        bool (*SetLogFileName)(const char*);

        // 截图模式配置接口
        bool (*SetUseBitbltCaptureMode)();
        bool (*SetUseGraphicsCaptureMode)();
        bool (*SetUseDwmCaptureMode)();
        bool (*SetUseLocalPictureMode)();
        bool (*SetUseLocalVideoMode)();

        // 采集相关接口
        bool (*SetCaptureHandle)(long long);
        bool (*SetCaptureHandleCallback)(long long (*)());
        bool (*SetScreenSourceCallback)(void (*)(const char*, int&));
        bool (*SetScreenSourceCallbackEx)(void (*)(const char*, int&, int&, int&));
        bool (*SetScreenSourceImage)(const char*, int);
        bool (*SetScreenSourceImageEx)(const char*, int, int, int);
        bool (*SetScreenClientRectCallback)(void (*)(int&, int&, int&, int&));

        // 跟踪缓存接口
        bool (*SetTrackCachePath)(const char*);
        bool (*SetTrackCacheName)(const char*);

        // 坐标配置接口
        bool (*SetWorldCenter)(double, double);
        bool (*SetWorldScale)(double);

        // 数据获取接口
        bool (*GetTransformOfMap)(double&, double&, double&, int&);
        bool (*GetPositionOfMap)(double&, double&, int&);
        bool (*GetDirection)(double&);
        bool (*GetRotation)(double&);
        bool (*GetUID)(int&);
        bool (*GetAllInfo)(double&, double&, int&, double&, double&, int&);

        // 调试接口
        bool (*DebugCapture)();
        bool (*DebugCapturePath)(const char*);

        // 错误处理接口
        int (*GetLastErr)();
        int (*GetLastErrMsg)(char*, int);
        int (*GetLastErrJson)(char*, int);

        // 版本信息接口
        bool (*GetCompileVersion)(char*, int);
        bool (*GetCompileTime)(char*, int);
        bool (*GetCoreModulePath)(char*, int);
    };

    // 定义上下文初始化函数
    CVAUTOTRACK_API cvAutoTrackContextV1* create_cvAutoTrack_context_v1();
    // 定义上下文销毁函数
    CVAUTOTRACK_API void destroy_cvAutoTrack_context_v1(cvAutoTrackContextV1* context);

    // 定义上下文结构体
    struct cvAutoTrackContextV2
    {
        // 开发保留接口
        bool (*Call)(const char* command, const char* json_buff, int json_size, char* result_buff, int result_size);

        // 数据获取接口
        bool (*GetTransformOfMap)(double&, double&, double&, int&);
        bool (*GetPositionOfMap)(double&, double&, int&);
        bool (*GetDirection)(double&);
        bool (*GetRotation)(double&);
        bool (*GetUID)(int&);
        bool (*GetAllInfo)(double&, double&, int&, double&, double&, int&);

        // 错误处理接口
        int (*GetLastErr)();
        int (*GetLastErrMsg)(char*, int);

        // 版本信息接口
        bool (*GetCompileVersion)(char*, int);
        bool (*GetCompileTime)(char*, int);
        bool (*GetCoreModulePath)(char*, int);
    };

    // 定义上下文初始化函数
    CVAUTOTRACK_API cvAutoTrackContextV2* create_cvAutoTrack_context_v2();
    // 定义上下文销毁函数
    CVAUTOTRACK_API void destroy_cvAutoTrack_context_v2(cvAutoTrackContextV2* context);

#ifdef __cplusplus
}
#endif
#endif // CVAUTOTRACE_H
