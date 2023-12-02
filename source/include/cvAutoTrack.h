#pragma once
#ifdef CVAUTOTRACK_EXPORTS
#define CVAUTOTRACK_PORT __declspec(dllexport)
#else
#define CVAUTOTRACK_PORT __declspec(dllimport)
#endif
#define CVAUTOTRACK_CALL __stdcall
#define CVAUTOTRACK_API CVAUTOTRACK_PORT CVAUTOTRACK_CALL

extern "C" bool CVAUTOTRACK_API InitResource();
extern "C" bool CVAUTOTRACK_API UnInitResource();

extern "C" bool CVAUTOTRACK_API StartServer();
extern "C" bool CVAUTOTRACK_API StopServer();
extern "C" bool CVAUTOTRACK_API SetServerInterval(int interval_ms);
extern "C" bool CVAUTOTRACK_API SetServerCallback(void (*callback)(const char *json_buff, int buff_size));

extern "C" bool CVAUTOTRACK_API SetDisableFileLog();
extern "C" bool CVAUTOTRACK_API SetEnableFileLog();
extern "C" bool CVAUTOTRACK_API SetUseBitbltCaptureMode();
extern "C" bool CVAUTOTRACK_API SetUseGraphicsCaptureMode();
extern "C" bool CVAUTOTRACK_API SetUseDwmCaptureMode();
extern "C" bool CVAUTOTRACK_API SetUseLocalImageCaptureMode();
extern "C" bool CVAUTOTRACK_API SetScreenSourceCallback(void (*callback)(const char *image_encode_data, int image_data_size));
extern "C" bool CVAUTOTRACK_API SetScreenSourceCallbackEx(void (*callback)(const char *image_data, int image_width, int image_height, int image_channels));
extern "C" bool CVAUTOTRACK_API SetScreenSourceImage(const char *image_encode_data, int image_data_size);
extern "C" bool CVAUTOTRACK_API SetScreenSourceImageEx(const char *image_data, int image_width, int image_height, int image_channels);
extern "C" bool CVAUTOTRACK_API SetScreenClientRectCallback(void (*callback)(int &x, int &y, int &width, int &height));
extern "C" bool CVAUTOTRACK_API SetLogFilePath(const char *path);
extern "C" bool CVAUTOTRACK_API SetLogFileName(const char *name);
extern "C" bool CVAUTOTRACK_API SetTrackCachePath(const char *path);
extern "C" bool CVAUTOTRACK_API SetTrackCacheName(const char *name);
extern "C" bool CVAUTOTRACK_API SetHandle(long long int handle);
extern "C" bool CVAUTOTRACK_API SetWorldCenter(double x, double y);
extern "C" bool CVAUTOTRACK_API SetWorldScale(double scale);

extern "C" bool CVAUTOTRACK_API ImportMapBlock(int uuid, const char *image_encode_data, int image_data_size);
extern "C" bool CVAUTOTRACK_API ImportMapBlockData(int uuid, const char *image_data, int image_width, int image_height, int image_channels);
extern "C" bool CVAUTOTRACK_API ImportMapBlockCenter(int uuid, int x, int y);
extern "C" bool CVAUTOTRACK_API ImportMapBlockRelativeCenter(int uuid, int parent_uuid, int x, int y);
extern "C" bool CVAUTOTRACK_API ImportMapBlockCenterScale(int uuid, int x, int y, double scale);
extern "C" bool CVAUTOTRACK_API ImportMapBlockRelativeCenterScale(int uuid, int parent_uuid, int x, int y, double scale);

extern "C" bool CVAUTOTRACK_API GetTransformOfMap(double &x, double &y, double &a, int &mapId);
extern "C" bool CVAUTOTRACK_API GetPositionOfMap(double &x, double &y, int &mapId);
extern "C" bool CVAUTOTRACK_API GetDirection(double &a);
extern "C" bool CVAUTOTRACK_API GetRotation(double &a);
extern "C" bool CVAUTOTRACK_API GetStar(double &x, double &y, bool &isEnd);
extern "C" bool CVAUTOTRACK_API GetStarJson(char *json_buff, int buff_size);
extern "C" bool CVAUTOTRACK_API GetUID(int &uid);
extern "C" bool CVAUTOTRACK_API GetAllInfo(double &x, double &y, int &mapId, double &a, double &r, int &uid);

extern "C" bool CVAUTOTRACK_API GetInfoLoadPicture(const char *path, int &uid, double &x, double &y, double &a);
extern "C" bool CVAUTOTRACK_API GetInfoLoadVideo(const char *path, const char *out_path);

extern "C" bool CVAUTOTRACK_API DebugCapture();
extern "C" bool CVAUTOTRACK_API DebugCapturePath(const char *path);

extern "C" int CVAUTOTRACK_API GetLastErr();
extern "C" int CVAUTOTRACK_API GetLastErrMsg(char *msg_buff, int buff_size);
extern "C" int CVAUTOTRACK_API GetLastErrJson(char *json_buff, int buff_size);

extern "C" bool CVAUTOTRACK_API GetCompileVersion(char *version_buff, int buff_size);
extern "C" bool CVAUTOTRACK_API GetCompileTime(char *time_buff, int buff_size);

extern "C" bool CVAUTOTRACK_API GetMapIsEmbedded();
/// @brief 
/// @param doc_buff 
/// @param buff_size 
/// @return 
extern "C" bool CVAUTOTRACK_API GetHelpDoc(char *doc_buff, int buff_size);
