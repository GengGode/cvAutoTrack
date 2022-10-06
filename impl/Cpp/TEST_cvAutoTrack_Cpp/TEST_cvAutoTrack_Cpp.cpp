#include <iostream>
//#include <cvAutoTrack.h>
#include "../../../cvAutoTrack/include/cvAutoTrack.h"

#include <Windows.h>

#include <vector>

void Test_video()
{
	// 静态方法调用
	// 初始化
	//init();
	// 准备变量
	float x = 0;
	float y = 0;
	float a = 0;
	double x2 = 0;
	double y2 = 0;
	double a2 = 0;
	double aa2 = 0;
	int uid = 0;
	// 调用循环

	std::vector<std::vector<double>> his;
	char path[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Picture/001.png" };
	char pathV[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.mp4" };

	char pathTxt[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.json" };

	//char pathTxt[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.txt" };
	/*GetInfoLoadVideo(pathV, pathTxt);
	std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";*/

	if (init())
	{
		//	Sleep(2000);
	}
	//uninit();
	//Sleep(1000);

	FILE* fptr = NULL;
	fopen_s(&fptr, "./Output.txt", "w+");


	//SetWorldScale(0.666667);
	if (GetInfoLoadPicture(path, uid, x2, y2, a2))
	{
		std::cout << "Now Coor and Angle: " << " " << uid << " " << " " << x2 << " " << y2 << " " << a2 << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
	}
	//SetWorldScale(1.0);
	if (GetInfoLoadPicture(path, uid, x2, y2, a2))
	{
		std::cout << "Now Coor and Angle: " << " " << uid << " " << " " << x2 << " " << y2 << " " << a2 << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
	}
	char buff[1024] = { 0 };
#ifdef _DEBUG
	if (GetStarJson(buff))
	{
		//坐标需要映射 p * 1.33 + AvatarPos
		std::cout << buff << "\n";
	}
#endif

}


int TEST()
{
	double x = 0;
	double y = 0;
	double a = 0;
	int map_id = 0;

	if (GetTransformOfMap(x, y, a, map_id))
	{
		std::cout << "坐标和角度   : " << " " << map_id << x << " " << y << " " << a << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}

	return 0;
}

int Run()
{


	std::ios::sync_with_stdio(false);

	FILE* fptr = NULL;
	fopen_s(&fptr, "./Output.txt", "w+");

	// 准备变量
	double x = 0;
	double y = 0;
	double a = 0;
	int map_id = 0;
	double x2 = 0;
	double y2 = 0;
	double a2 = 0;
	double aa2 = 0;
	int uid = 0;
	char buff[1024] = { 0 };

	// 调用循环
	while (1)
	{
		// 设置Dx截图
		/*if (SetUseDx11CaptureMode())
		{
			std::cout << "设置Dx截图成功" << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		}*/
#pragma warning(suppress : 4996)
		if (GetTransformOfMap(x, y, a, map_id))
		{
			std::cout << "坐标和角度   : " << " " << map_id << x << " " << y << " " << a << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		}
		//#pragma warning(suppress : 4996)
		//		if (GetPosition(x2, y2))
		//		{
		//			int mapid = 0;
		//			GetPositionOfMap(x2, y2, mapid);
		//			std::cout << "坐标         : " << " " << x2 << " " << y2 << " " << mapid << "\n";
		//		}
		//		else
		//		{
		//			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		//		}
		//		if (GetDirection(a2))
		//		{
		//			std::cout << "角度         : " << " " << a2 << "\n";
		//		}
		//		else
		//		{
		//			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		//		}
		//		if (GetRotation(aa2))
		//		{
		//			std::cout << "视角朝向     : " << " " << aa2 << "\n";
		//		}
		//		else
		//		{
		//			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		//		}
		if (GetUID(uid))
		{
			std::cout << "当前UID      : " << " " << uid << " " << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		}
		if (GetStarJson(buff))
		{
			//坐标需要映射 p + AvatarPos
			std::cout << "当前神瞳Json : " << buff << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		}
		// 设置Dx截图
		if (DebugCapture())
		{
			std::cout << "截图成功" << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
		}

		// 推送io流缓存
		std::cout << std::flush;

		Sleep(30);
		system("cls");
	}
	fclose(fptr);
	return 0;
}

int main(int argc, char* argv[])
{
	// 如果输入参数 -test 就执行测试
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		return TEST();
	}
	else
	{
		// 否则执行正常的程序
		return Run();
	}
}