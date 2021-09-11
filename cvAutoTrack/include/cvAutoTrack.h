#pragma once


#ifdef CVAUTOTRACK_EXPORTS
#define CVAUTOTRACK_PORT __declspec(dllexport)
#else
#define CVAUTOTRACK_PORT __declspec(dllimport)
#endif

#define CVAUTOTRACK_CALL __stdcall
#define CVAUTOTRACK_API CVAUTOTRACK_PORT CVAUTOTRACK_CALL

extern "C" {
	// 这是已导出的函数。
	bool CVAUTOTRACK_API init();
	bool CVAUTOTRACK_API uninit();
	bool CVAUTOTRACK_API SetHandle(long long int handle);
	bool CVAUTOTRACK_API SetWorldCenter(double x, double y);
	bool CVAUTOTRACK_API SetWorldScale(double scale);
	bool CVAUTOTRACK_API GetTransform(float& x, float& y, float& a);
	bool CVAUTOTRACK_API GetPosition(double& x, double& y);
	bool CVAUTOTRACK_API GetDirection(double& a);
	bool CVAUTOTRACK_API GetRotation(double& a);
	// bool CVAUTOTRACK_API GetStar(double &x, double &y, bool &isEnd);
	bool CVAUTOTRACK_API GetStar(char* jsonBuff);
	bool CVAUTOTRACK_API GetUID(int& uid);

	bool CVAUTOTRACK_API GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a);
	bool CVAUTOTRACK_API GetInfoLoadVideo(char* path, char* pathOutFile);

	int CVAUTOTRACK_API GetLastErr();
	CVAUTOTRACK_PORT const char* CVAUTOTRACK_CALL GetLastErrStr();

	bool CVAUTOTRACK_API startServe();
	bool CVAUTOTRACK_API stopServe();

#ifdef _DEBUG

	bool CVAUTOTRACK_API test(char* str);

	//void testLocalVideo(std::string path);

#endif

}