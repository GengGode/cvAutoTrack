// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CVAUTOTRACK_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CVAUTOTRACK_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef CVAUTOTRACK_EXPORTS
#define CVAUTOTRACK_API __declspec(dllexport)
#else
#define CVAUTOTRACK_API __declspec(dllimport)
#endif

// 这是已导出的函数。
extern "C" CVAUTOTRACK_API bool init();
extern "C" CVAUTOTRACK_API bool SetHandle(long long int handle);
extern "C" CVAUTOTRACK_API bool SetWorldCenter(double x, double y);
extern "C" CVAUTOTRACK_API bool SetWorldScale(double scale);
extern "C" CVAUTOTRACK_API bool GetTransform(float &x, float &y, float &a);
extern "C" CVAUTOTRACK_API bool GetPosition(double &x, double &y);
extern "C" CVAUTOTRACK_API bool GetDirection(double &a);
extern "C" CVAUTOTRACK_API bool GetUID(int &uid);

extern "C" CVAUTOTRACK_API bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);

extern "C" CVAUTOTRACK_API int GetLastErr();
extern "C" CVAUTOTRACK_API const char* GetLastErrStr();
extern "C" CVAUTOTRACK_API bool uninit();

#ifdef _DEBUG

	void testLocalVideo(std::string path);

#endif