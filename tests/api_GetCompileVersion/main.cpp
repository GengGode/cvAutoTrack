#include <iostream>
//#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <cvAutoTrack.h>


int main()
{
	GetCompileVersion(nullptr, 0);
	char version_buff[256] = {0};
	GetCompileVersion(version_buff, 256);
	std::cout << "版本号       : "
			  << " " << version_buff << " "
			  << "\n";
	char version_time_buff[256] = {0};
	GetCompileTime(version_time_buff, 256);
	std::cout << "编译时间     : "
			  << " " << version_time_buff << " "
			  << "\n";
}

