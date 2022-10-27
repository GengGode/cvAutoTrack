#include <iostream>
//#include <cvAutoTrack.h>
#include "../../../cvAutoTrack/include/cvAutoTrack.h"

#include <Windows.h>

#include <vector>

int TEST()
{
	char version_buff[256] = { 0 };

	if (GetCompileVersion(version_buff,256))
	{
		std::cout << "版本号       : " << " " << version_buff << " " << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
	}
	
	char version_time_buff[256] = {0};
	
	if (GetCompileTime(version_time_buff, 256))
	{
		std::cout << "编译时间     : " << " " << version_time_buff << " " << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
	}
	
	std::cout << "测试完成\n";
	return 0;
}

int TEST_init_and_uninit()
{
	std::cout << "测试 init 与 uninit\n";

	init();

	Sleep(1000);

	uninit();

	Sleep(1000);

	init();

	Sleep(1000);

	uninit();

	Sleep(1000);

	init();

	Sleep(1000);

	uninit();

	Sleep(1000);

	std::cout << "测试完成\n";
	return 0;
}

void Run_SetDx()
{
	//设置Dx截图
	if (SetUseDx11CaptureMode())
	{
		std::cout << "设置Dx截图成功" << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}
void Run_SetBit()
{
	//设置Bitblt截图
	if (SetUseBitbltCaptureMode())
	{
		std::cout << "设置Bitblt截图成功" << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}
void Run_GetTrans()
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
		std::cout << "错误码 : " << " \n" << GetLastErr() << " " << "\n";
	}
}
void Run_GetDir()
{
	double a2 = 0;
	if (GetDirection(a2))
	{
		std::cout << "角度         : " << " " << a2 << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}
void Run_GetRot()
{
	double aa2 = 0;
	if (GetRotation(aa2))
	{
		std::cout << "视角朝向     : " << " " << aa2 << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}
void Run_GetUID()
{
	int uid = 0;
	if (GetUID(uid))
	{
		std::cout << "当前UID      : " << " " << uid << " " << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}
void Run_GetStars()
{
	char buff[1024] = { 0 };
	if (GetStarJson(buff))
	{
		//坐标需要映射 p + AvatarPos
		std::cout << "当前神瞳Json : " << buff << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}
void Run_Capture()
{
	// 设置Dx截图
	if (DebugCapture())
	{
		std::cout << "截图成功" << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}

void Run_GetPosit()
{
	int mapid = 0;
	double x2 = 0;
	double y2 = 0;
	if (GetPositionOfMap(x2, y2, mapid))
	{
		int mapid = 0;
		GetPositionOfMap(x2, y2, mapid);
		std::cout << "坐标         : " << " " << x2 << " " << y2 << " " << mapid << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}

int RUN(bool is_off_capture=false, bool is_only_capture=false, int frame_rate=0)
{
	return 0;
}
int Run()
{
	std::ios::sync_with_stdio(false);
	system("chcp 65001");
	

	// 准备变量
	
	// 调用循环
	while (1)
	{
		// 显示菜单
		std::cout << "1. 设置Dx截图\n";
		std::cout << "2. 设置Bitblt截图\n";
		std::cout << "3. 获取坐标和角度\n";
		std::cout << "4. 获取角度\n";
		std::cout << "5. 获取视角朝向\n";
		std::cout << "6. 获取当前UID\n";
		std::cout << "7. 获取当前神瞳Json\n";
		std::cout << "8. 截图\n";
		std::cout << "9. 获取坐标\n";
		std::cout << "0. 退出\n";
		std::cout << "请输入选项: ";
		int option = 0;
		std::cin >> option;
		std::cout << "\n";
		switch (option)
		{
		case 1:
			Run_SetDx();
			break;
		case 2:
			Run_SetBit();
			break;
		case 3:
			Run_GetTrans();
			break;
		case 4:
			Run_GetDir();
			break;
		case 5:
			Run_GetRot();
			break;
		case 6:
			Run_GetUID();
			break;
		case 7:
			Run_GetStars();
			break;
		case 8:
			Run_Capture();
			break;
		case 9:
			Run_GetPosit();
			break;
		case 0:
			return 0;
		default:
			break;
		}
		// 推送io流缓存
		std::cout << std::flush;

		Sleep(30);
		system("cls");
	}
	return 0;
}
void HELP()
{
	/*
	-help : show help text
	-test : run test
	-capture : set capture param
		[--off] : off capture
		[--only] : set run only a capture
		[--t int] : set capture frame rate
	*/
	std::cout << "-help      : show help text\n";
	std::cout << "-test      : run test\n";
	std::cout << "-capture   : set capture param\n";
	std::cout << "	[--off]  : off capture\n";
	std::cout << "	[--only] : set run only a capture\n";
	std::cout << "	[--t int] : set capture frame rate\n";
}
int main(int argc, char* argv[])
{
	std::vector<std::string> args;
	for (int i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}
	
	//if (argc > 1 && args[1] == "-help")
	//{
	//	HELP();
	//}
	//else if (argc > 1 && args[1] == "-test")
	//{
	//	TEST();
	//}
	//else if (argc > 1 && args[1] == "-capture")
	//{
	//	HELP();
	//	return 0;
	//}
	//else if (argc > 1 && args[1] == "-capture")
	//{
	//	bool is_off_capture = false;
	//	bool is_only_capture = false;
	//	int frame_rate = 0;
	//	
	//	if (argc > 2 && args[2] == "--off")
	//	{
	//		is_off_capture = true;
	//		// 删除已处理的
	//		
	//	}
	//	else if (argc > 2 && args[2] == "--only")
	//	{
	//		
	//	}
	//	else
	//	{
	//		
	//	}
	//}
	//else
	//{
	//	HELP();
	//	return 0;
	//}
		
	
	
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
