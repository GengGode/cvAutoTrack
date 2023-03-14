#include "pch.h"
#include "resources.import.h"

cv::Mat TianLi::Resources::Import::import_mat_from_data(int width, int height, const unsigned char* data, int data_size)
{
	if (data_size != width * height * 4)
	{
		return cv::Mat();
	}
	return cv::Mat(height, width, IMPORT_TYPE, (void*)data, cv::Mat::AUTO_STEP);
}
