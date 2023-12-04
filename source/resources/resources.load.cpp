#include "pch.h"
#include "resources.load.h"
#include "binary/resources.binary.h"

cv::Mat binary_to_mat(const unsigned char* image_binary, size_t binary_size)
{
	auto image_array = cv::Mat(1, static_cast<int>(binary_size), CV_8UC1, (void*)image_binary);
	return cv::imdecode(image_array, cv::IMREAD_UNCHANGED);
}

cv::Mat TianLi::Resources::Load::load_image(std::string name)
{
	auto image_binary = TianLi::Resources::Binary::get_image_binary(name.c_str());
	return binary_to_mat(image_binary.data, image_binary.size);
}
