#include <iostream>
//#include <cvAutoTrack.h>
#include "../../../cvAutoTrack/include/cvAutoTrack.h"

#include <Windows.h>

#include <vector>
#include <format>

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

void Run_GetAll()
{
	double x, y, a, r;
	int mapId,uid;
	std::string mapType;
	if (GetAllInfo(x, y, mapId, a, r, uid))
	{
		switch (mapId) {
		case 0:mapType = "提瓦特大陆"; break;
		case 1:mapType = "渊下宫"; break;
		case 2:mapType = "地下矿区"; break;
		}
		std::cout << std::format(R"(
全部信息：
地区:{}
坐标:x = {:6.2f}; y = {:6.2f}
朝向:角色 = {:4.2f}; 相机 = {:4.2f}
UID:{:d}
----------------
)",mapType,x,y,a,r,uid);
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
		std::cout << "坐标         : " << " " << x2 << " " << y2 << " " << mapid << "\n";
	}
	else
	{
		std::cout << "错误码       : " << " \n" << GetLastErr() << " " << "\n";
	}
}

void Run_GetVersion()
{
	char* ver = new char[100];
	verison(ver);
	std::cout << ver <<std::endl;
	delete[] ver;
}

int RUN(bool is_off_capture=false, bool is_only_capture=false, int frame_rate=0)
{
	return 0;
}
int Run()
{
	std::ios::sync_with_stdio(false);
	system("chcp 65001");
	
	// 调用循环
	while (1)
	{
		// 显示菜单
		std::cout << 
R"(
1. 设置Dx截图
2. 设置Bitblt截图
3. 获取坐标和角度
4. 获取坐标
5. 获取角度
6. 获取视角朝向
7. 获取当前UID
8. 获取当前神瞳Json
9. 截图
10.可视化调试【Debug模式】
=====================
-1. 获取版本号
0. 退出
请输入选项: 
)";
		int option = 0;
		std::cin >> option;
		std::cout << "\n";
		switch (option)
		{
		case 1:
			Run_SetDx();
			system("pause");
			break;
		case 2:
			Run_SetBit();
			system("pause");
			break;
		case 3:
			Run_GetTrans();
			system("pause");
			break;
		case 4:
			Run_GetPosit();
			system("pause");
			break;
		case 5:
			Run_GetDir();
			system("pause");
			break;
		case 6:
			Run_GetRot();
			system("pause");
			break;
		case 7:
			Run_GetUID();
			system("pause");
			break;
		case 8:
			Run_GetStars();
			system("pause");
			break;
		case 9:
			Run_Capture();
			system("pause");
			break;
		case 10:
			while (1)
			{
				// 30ms 内检测到ECS键就退出
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
					break;
				}
				Sleep(100);
				Run_GetAll();
			}
			break;
		case -1:
			Run_GetVersion();
			system("pause");
			break;
		case 0:
			return 0;
			break;
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
