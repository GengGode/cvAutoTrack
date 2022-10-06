#pragma once
#ifdef CVAUTOTRACK_EXPORTS
#define CVAUTOTRACK_PORT __declspec(dllexport)
#else
#define CVAUTOTRACK_PORT __declspec(dllimport)
#endif
#define CVAUTOTRACK_CALL  
//__stdcall
#define CVAUTOTRACK_API CVAUTOTRACK_PORT CVAUTOTRACK_CALL


extern "C" CVAUTOTRACK_API bool verison(char* versionBuff);

extern "C" CVAUTOTRACK_API bool init();
extern "C" CVAUTOTRACK_API bool uninit();

extern "C" CVAUTOTRACK_API bool startServe();
extern "C" CVAUTOTRACK_API bool stopServe();

extern "C" CVAUTOTRACK_API bool SetUseBitbltCaptureMode();
extern "C" CVAUTOTRACK_API bool SetUseDx11CaptureMode();

extern "C" CVAUTOTRACK_API bool SetHandle(long long int handle);
extern "C" CVAUTOTRACK_API bool SetWorldCenter(double x, double y);
extern "C" CVAUTOTRACK_API bool SetWorldScale(double scale);
extern "C" CVAUTOTRACK_API bool GetTransformOfMap(double& x, double& y, double& a, int& mapId);
extern "C" CVAUTOTRACK_API bool GetPositionOfMap(double& x,double& y,int& mapId);
extern "C" CVAUTOTRACK_API bool GetDirection(double& a);
extern "C" CVAUTOTRACK_API bool GetRotation(double& a);
extern "C" CVAUTOTRACK_API bool GetStar(double &x, double &y, bool &isEnd);
extern "C" CVAUTOTRACK_API bool GetStarJson(char *jsonBuff);
extern "C" CVAUTOTRACK_API bool GetUID(int &uid);

extern "C" CVAUTOTRACK_API bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);
extern "C" CVAUTOTRACK_API bool GetInfoLoadVideo(char* path, char* pathOutFile);

extern "C" CVAUTOTRACK_API bool DebugCapture();

extern "C" CVAUTOTRACK_API int  GetLastErr();
extern "C" CVAUTOTRACK_API const char* GetLastErrStr();
extern "C" CVAUTOTRACK_API int GetLastErrMsg(char* msg_buff,int buff_size);



