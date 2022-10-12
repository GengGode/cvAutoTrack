#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

//图片资源 加载类
class Resources
{
private:
	Resources();
public:
	~Resources();

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	static Resources& getInstance();

public:
	cv::Mat PaimonTemplate;
	cv::Mat MinimapCailbTemplate;
	cv::Mat StarTemplate;
	cv::Mat MapTemplate;
	cv::Mat UID;
	cv::Mat UIDnumber[10];
	
	struct XmlPtr
	{
		char* ptr = nullptr;
		int size = 0;
	};
	
	XmlPtr xmlPtr = { nullptr,0 };
public:
	void install();
	void release();
private:
	bool is_installed = false;
};