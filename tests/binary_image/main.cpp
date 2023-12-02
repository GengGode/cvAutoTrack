#include <opencv2/opencv.hpp>
#include "resources/binary/resources.binary.h"

int main(int argc, char **argv)
{
    using namespace TianLi::Resources::Binary;

    auto image_count = get_image_count();
    for (auto i = 0; i < image_count; i++)
    {
        auto image_name = get_image_name(i);
        auto image_binary = get_image_binary(image_name);
        
        std::vector<uchar> image_binary_vector(image_binary.data, image_binary.data + image_binary.size);
        auto image = cv::imdecode(image_binary_vector, cv::IMREAD_UNCHANGED);
        std::cout << "image_name: " << image_name << std::endl;
    }
}