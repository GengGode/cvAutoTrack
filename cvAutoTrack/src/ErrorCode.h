#pragma once
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#define ERR_SUCCESS					0		//执行成功 / 正常退出。
#define ERR_UNINITIALIZED			1		//未初始化。
#define ERR_HWND_NO_FOUND			2		//未能找到原神窗口句柄。
#define ERR_SCREEN_EMPTY			3		//窗口画面为空。
#define ERR_SURF_KEYPOINT_NO		4		//未能匹配到特征点。
#define ERR_MINIMAP_TOO_SMALL		5		//原神小地图区域为空或者区域长宽小于60px。
#define ERR_PAIMON_NO_FOUND			6		//未能匹配到派蒙。
#define ERR_KEYPOINT_OUT_OF_INDEX	7		//特征点数组访问越界，是个bug。
#define ERR_UIDRECT_INVALID			8		//未能在UID区域检测到有效UID。
#define ERR_AVATARKEYPOINT_ERROR	9		//提取小箭头特征误差过大。
#define ERR_HWND_INVALID			10		//无效句柄或指定句柄所指向窗口不存在。
#define ERR_AVATARRECT_UNFIND		11		//未能取到小箭头区域。
#define ERR_GIHANDLE_NULL			12 		//窗口句柄为空。
#define ERR_GIHANDLE_INVALID		13 		//窗口句柄失效。


class ErrorCode
{
	FILE * fptr ;
private:
	ErrorCode();

public:
	~ErrorCode();
	ErrorCode(const ErrorCode&) = delete;
	ErrorCode& operator=(const ErrorCode&) = delete;
	static ErrorCode& getInstance();
	ErrorCode& operator=(const int& code);
	operator int();

	friend ostream & operator<<(ostream & os, const ErrorCode & err);

public:
	void setError(int code);
	int getLastError();
	string getLastErrorMsg();

private:
	int errorCode=0;
	vector<int> errCodeList;
	vector<string> errCodeMsg;

private:
	void push(int code);
};
