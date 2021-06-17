// Test3Project1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Test2Project1.h>
#include <opencv.hpp>
int main()
{
	
    std::cout << "Hello World!\n";
	fnTest2Project1();
	GGenshinImpactMatch m;

	GPoint p = m.getPosition();
	
	GSize k=GSize(1, 2);
	int a[2] = {0};
	k.toArray(a);
	std::cout << k.width << " " << k.height << std::endl;
	std::cout << p.x <<" "<< p.y << std::endl;
	k.height = 12;
	std::cout << a[0] << " " << a[1] << std::endl;
	
	for (int i = 0; i < m.getMaxCount(); i++) 
	{
		cv::imshow("123", *(cv::Mat*)m.getSource(i));
		cv::waitKey(1000);
	}
	cv::Mat mat= cv::Mat(200,200,CV_8UC3,cv::Scalar(100, 0, 200));
	m.setSource(&mat);

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
