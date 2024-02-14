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
    // string alloc
    struct CVAUTOTRACK_API cvat_string;
    typedef struct cvat_string* cvat_string_ptr;

    // string alloc and free
    CVAUTOTRACK_API cvat_string_ptr alloc_string();
    CVAUTOTRACK_API int get_string_length(cvat_string_ptr str);
    CVAUTOTRACK_API int get_string_context(cvat_string_ptr str, char* buffer, int buffer_size);
    CVAUTOTRACK_API void free_string(cvat_string_ptr str);

#ifdef explicit_link
// typedef std::shared_ptr<cvat_string_ptr> (*alloc_string_t)();
// typedef int (*get_string_length_t)(cvat_string_ptr str);
// typedef int (*get_string_context_t)(cvat_string_ptr str, char *buffer, int buffer_size);
// typedef void (*free_string_t)(cvat_string_ptr str);

// LoadFunction(alloc_string);
// LoadFunction(get_string_length);
// LoadFunction(get_string_context);
// LoadFunction(free_string);
#endif // explicit_link

    struct CVAUTOTRACK_API cvat_error;
    typedef struct cvat_error* cvat_error_ptr;

    // error alloc and free
    CVAUTOTRACK_API cvat_error_ptr alloc_error();
    CVAUTOTRACK_API cvat_string_ptr get_error_what(cvat_error_ptr err);
    CVAUTOTRACK_API cvat_string_ptr get_error_msg(cvat_error_ptr err);
    CVAUTOTRACK_API cvat_string_ptr get_error_file(cvat_error_ptr err);
    CVAUTOTRACK_API int get_error_line(cvat_error_ptr err);
    CVAUTOTRACK_API int get_error_column(cvat_error_ptr err);
    CVAUTOTRACK_API int get_error_code(cvat_error_ptr err);
    CVAUTOTRACK_API void free_error(cvat_error_ptr err);

    // error code
    CVAUTOTRACK_API int get_last_error();
    CVAUTOTRACK_API int get_error_define_count();
    CVAUTOTRACK_API int get_error_define(int index, cvat_string_ptr result);

    struct CVAUTOTRACK_API cvat_resource;
    struct CVAUTOTRACK_API cvat_capture;
    struct CVAUTOTRACK_API cvat_tracker;
    struct CVAUTOTRACK_API cvat_percepter;
    struct CVAUTOTRACK_API cvat_server;
    struct CVAUTOTRACK_API cvat_logger;

    typedef cvat_resource* cvat_resource_t;
    typedef cvat_capture* cvat_capture_t;
    typedef cvat_tracker* cvat_tracker_t;
    typedef cvat_percepter* cvat_percepter_t;
    typedef cvat_server* cvat_server_t;
    typedef cvat_logger* cvat_logger_t;
    typedef cvat_error* cvat_error_t;

    /// @brief 创建日志
    /// @return 日志指针
    cvat_logger_t CVAUTOTRACK_API CreateLogger();
    /// @brief 销毁日志
    /// @param logger 日志指针
    void CVAUTOTRACK_API DestroyLogger(cvat_logger_t logger);
    /// @brief 创建错误
    /// @return 错误指针
    cvat_error_t CVAUTOTRACK_API CreateError();
    /// @brief 销毁错误
    /// @param error 错误指针
    void CVAUTOTRACK_API DestroyError(cvat_error_t error);

    /// @brief 创建资源
    /// @return 资源指针
    cvat_resource_t CVAUTOTRACK_API CreateResource();
    /// @brief 销毁资源
    /// @param resource 资源指针
    void CVAUTOTRACK_API DestroyResource(cvat_resource_t resource);
    /// @brief 创建采集器
    /// @return 采集器指针
    cvat_capture_t CVAUTOTRACK_API CreateCapture();
    /// @brief 销毁采集器
    /// @param capture 采集器指针
    void CVAUTOTRACK_API DestroyCapture(cvat_capture_t capture);
    /// @brief 创建追踪器
    /// @param resource 资源指针
    /// @param capture 采集器指针
    /// @return 追踪器指针
    cvat_tracker_t CVAUTOTRACK_API CreateTracker(cvat_resource_t resource, cvat_capture_t capture);
    /// @brief 销毁追踪器
    /// @param tracker 追踪器指针
    void CVAUTOTRACK_API DestroyTracker(cvat_tracker_t tracker);
    /// @brief 创建感知器
    /// @param resource 资源指针
    /// @param capture 采集器指针
    /// @return 感知器指针
    cvat_percepter_t CVAUTOTRACK_API CreatePercepter(cvat_resource_t resource, cvat_capture_t capture);
    /// @brief 销毁感知器
    /// @param percepter 感知器指针
    void CVAUTOTRACK_API DestroyPercepter(cvat_percepter_t percepter);
    /// @brief 创建服务
    /// @param tracker 追踪器指针
    /// @param percepter 感知器指针
    /// @return 服务指针
    cvat_server_t CVAUTOTRACK_API CreateServer(cvat_tracker_t tracker, cvat_percepter_t percepter);
    /// @brief 销毁服务
    /// @param server 服务指针
    void CVAUTOTRACK_API DestroyServer(cvat_server_t server);

    bool CVAUTOTRACK_API ResourceBeginLoad(cvat_resource_t resource, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceEndLoad(cvat_resource_t resource, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadConfig(cvat_resource_t resource, const char* path, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadCatch(cvat_resource_t resource, const char* path, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadMap(cvat_resource_t resource, const char* path, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadMapBlockFromFile(cvat_resource_t resource, int uuid, const char* path, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadMapBlockFromData(cvat_resource_t resource, int uuid, const char* data, int width, int height, int channels, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadMapBlockCenter(cvat_resource_t resource, int uuid, int x, int y, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadMapBlockRelativeCenter(cvat_resource_t resource, int uuid, int parent_uuid, int x, int y, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadMapBlockCenterScale(cvat_resource_t resource, int uuid, int x, int y, double scale, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceLoadMapBlockRelativeCenterScale(cvat_resource_t resource, int uuid, int parent_uuid, int x, int y, double scale, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceInitailize(cvat_resource_t resource, cvat_error_t error);
    bool CVAUTOTRACK_API ResourceUnInitailize(cvat_resource_t resource, cvat_error_t error);

    bool CVAUTOTRACK_API CaptureCreateWindowCapture(cvat_capture_t capture, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureCreateWindowGraphicsCapture(cvat_capture_t capture, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureCreateWindowDwmCapture(cvat_capture_t capture, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureCreateLocalPictureCapture(cvat_capture_t capture, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureCreateLocalVideoCapture(cvat_capture_t capture, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetHandle(cvat_capture_t capture, long long int handle, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetHandleName(cvat_capture_t capture, const char* name, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetHandleNameEx(cvat_capture_t capture, const char* name, const char* class_name, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetHandleCallback(cvat_capture_t capture, long long int (*callback)(), cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetSourceCallback(cvat_capture_t capture, void (*callback)(const char* image_encode_data, int& image_data_size), cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetSourceCallbackEx(cvat_capture_t capture, void (*callback)(const char* image_data, int& image_width, int& image_height, int& image_channels), cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetSourceImageFromFile(cvat_capture_t capture, const char* image_encode_data, int image_data_size, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetSourceImageFromData(cvat_capture_t capture, const char* image_data, int image_width, int image_height, int image_channels, cvat_error_t error);
    bool CVAUTOTRACK_API CaptureSetClientRectCallback(cvat_capture_t capture, void (*callback)(int& x, int& y, int& width, int& height), cvat_error_t error);

#ifdef __cplusplus
}
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

    bool CVAUTOTRACK_API GetTransformOfMap(double &x, double &y, double &a, int &mapId);
    bool CVAUTOTRACK_API GetPositionOfMap(double &x, double &y, int &mapId);
    bool CVAUTOTRACK_API GetDirection(double &a);
    bool CVAUTOTRACK_API GetRotation(double &a);
    bool CVAUTOTRACK_API GetUID(int &uid);
    bool CVAUTOTRACK_API GetAllInfo(double &x, double &y, int &mapId, double &a, double &r, int &uid);
    bool CVAUTOTRACK_API DebugCapture();
    bool CVAUTOTRACK_API DebugCapturePath(const char* path);
    int CVAUTOTRACK_API GetLastErr();
    int CVAUTOTRACK_API GetLastErrMsg(char *msg_buff, int buff_size);
    int CVAUTOTRACK_API GetLastErrJson(char *json_buff, int buff_size);

    bool CVAUTOTRACK_API GetCompileVersion(char *version_buff, int buff_size);
    bool CVAUTOTRACK_API GetCompileTime(char* time_buff, int buff_size);
    bool CVAUTOTRACK_API GetCoreModulePath(char* path_buff, int buff_size);

#ifdef __cplusplus
}
#endif

#ifdef explicit_link
    #if defined(_WIN32) || defined(_WIN64) || defined(_WIN128) || defined(__CYGWIN__)
        #include <Windows.h>
    #else
        #include <dlfcn.h>
    #endif

    #if defined(_WIN32) || defined(_WIN64) || defined(_WIN128) || defined(__CYGWIN__)
        #define LibraryHandle HMODULE
        #define cvat_load(path) LoadLibraryA(path)
        #define cvat_free(handle) FreeLibrary(handle)
        #define get_proc(handle, name) GetProcAddress(handle, name)
    #else
        #define LibraryHandle void*
        #define cvat_load(path) dlopen(path, RTLD_LAZY)
        #define cvat_free(handle) dlclose(handle)
        #define get_proc(handle, name) dlsym(handle, name)
    #endif

    #include <functional>
    #include <memory>
    #include <string>

    #define maroc_concatenate(a, b) maroc_concatenate_1(a, b)
    #define maroc_concatenate_1(a, b) maroc_concatenate_2(a, b)
    #define maroc_concatenate_2(a, b) a##b
    #define maroc_expand(x) x
    #define maroc_for_each_0(pred, ...)
    #define maroc_for_each_1(pred, n, x, ...) pred(x, 0)
    #define maroc_for_each_2(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_1(pred, 1, __VA_ARGS__))
    #define maroc_for_each_3(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_2(pred, 2, __VA_ARGS__))
    #define maroc_for_each_4(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_3(pred, 3, __VA_ARGS__))
    #define maroc_for_each_5(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_4(pred, 4, __VA_ARGS__))
    #define maroc_for_each_6(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_5(pred, 5, __VA_ARGS__))
    #define maroc_for_each_7(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_6(pred, 6, __VA_ARGS__))
    #define maroc_for_each_8(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_7(pred, 7, __VA_ARGS__))
    #define maroc_for_each_9(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_8(pred, 8, __VA_ARGS__))
    #define maroc_for_each_10(pred, n, x, ...) pred(x, n) maroc_expand(maroc_for_each_9(pred, 9, __VA_ARGS__))
    #define maroc_args_count(...) maroc_expand(maroc_arg_count_1(0, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
    #define maroc_arg_count_1(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, n, ...) n
    #define maroc_for_each_(n, pred, ...) maroc_expand(maroc_concatenate(maroc_for_each_, n)(pred, n, __VA_ARGS__))
    #define maroc_for_each(pred, ...) maroc_expand(maroc_for_each_(maroc_expand(maroc_args_count(__VA_ARGS__)), pred, __VA_ARGS__))
    #define comma_0()
    #define comma_1() ,
    #define comma_2() ,
    #define comma_3() ,
    #define comma_4() ,
    #define comma_5() ,
    #define comma_6() ,
    #define comma_7() ,
    #define comma_8() ,
    #define comma_9() ,

typedef long long int long_long_int;
typedef const char* const_char_ptr;
typedef char* char_ptr;
typedef bool& bool_ref;
typedef int& int_ref;
typedef double& double_ref;
    #define type_null_
    #define type_null_bool
    #define type_null_int
    #define type_null_double
    #define type_null_bool_ref
    #define type_null_int_ref
    #define type_null_double_ref
    #define type_null_long_long_int
    #define type_null_const_char_ptr
    #define type_null_char_ptr
    #define only_name(v, n) type_null_##v comma_##n()
    #define bind_call(name, ...)                                               \
        name(__VA_ARGS__)                                                      \
        {                                                                      \
            auto func = (decltype(&::name))get_proc(lib, #name); \
            if (func == nullptr)                                               \
                return false;                                                  \
            return func(maroc_for_each(only_name, __VA_ARGS__));               \
        }

 template <typename T> auto GetFunction(const std::string& name)
{
    return reinterpret_cast<T>(GetProcAddress(get_global_handle(), name.c_str()));
}
    #define LoadFunction(name) GetFunction<decltype(&name)>(#name)
    #define LoadFunctionEx(name) (decltype(&name))GetProcAddress(get_global_handle(), #name)

struct cvAutoTrack
{
    LibraryHandle lib;
    bool bind_call(InitResource);
    bool bind_call(UnInitResource);
    bool bind_call(SetCoreCachePath, const_char_ptr path);
    bool bind_call(GetCoreCachePath, char_ptr path_buff, int buff_size);
    bool bind_call(SetWorldCenter, double x, double y);
    bool bind_call(SetWorldScale, double scale);
    bool bind_call(GetTransformOfMap, double_ref x, double_ref y, double_ref a, int_ref mapId);
    bool bind_call(GetPositionOfMap, double_ref x, double_ref y, int_ref mapId);
    bool bind_call(GetDirection, double_ref a);
    bool bind_call(GetRotation, double_ref a);
    bool bind_call(GetUID, int_ref uid);
    bool bind_call(GetAllInfo, double_ref x, double_ref y, int_ref mapId, double_ref a, double_ref r, int_ref uid);
    bool bind_call(DebugCapture);
    int bind_call(GetLastErr);
    int bind_call(GetLastErrMsg, char_ptr msg_buff, int buff_size);
    int bind_call(GetLastErrJson, char_ptr json_buff, int buff_size);
    bool bind_call(GetCompileVersion, char_ptr version_buff, int buff_size);
    bool bind_call(GetCompileTime, char_ptr time_buff, int buff_size);
    bool bind_call(GetCoreModulePath, char_ptr time_buff, int buff_size);

    #define GetFunction(name) name##_func = _GetFunction<decltype(&::name)>(#name)

    cvAutoTrack(const std::string& path = "cvAutoTrack.dll")
    {
        lib = cvat_load(path.c_str());
    }

    ~cvAutoTrack()
    {
        if (lib != nullptr)
            cvat_free(lib);
    }

    template <typename T> T _GetFunction(std::string name)
    {
        auto func = (T)get_proc(lib, name.c_str());
        if (func == nullptr)
            return nullptr;
        return func;
    }
};

#endif // explicit_link
#endif // CVAUTOTRACE_H
