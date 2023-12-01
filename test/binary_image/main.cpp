#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "resources/binary/resources.binary.h"

namespace TianLi::Resources::Binary
{
    TEST(Binary, BinaryImage)
    {
        auto image_count = get_image_count();
        ASSERT_NE(image_count, 1);
        for (auto i = 0; i < image_count; i++)
        {
            auto image_name = get_image_name(i);
            auto image_binary = get_image_binary(image_name);
            ASSERT_NE(image_binary.data, nullptr);
            ASSERT_NE(image_binary.size, 0);
            std::vector<uchar> image_binary_vector(image_binary.data, image_binary.data + image_binary.size);
            ASSERT_EQ(image_binary_vector.size(), image_binary.size);
            auto image = cv::imdecode(image_binary_vector, cv::IMREAD_UNCHANGED);
            ASSERT_NE(image.data, nullptr);
            ASSERT_NE(image.rows, 0);
            ASSERT_NE(image.cols, 0);
            ASSERT_NE(image.channels(), 0);
            ASSERT_NE(image.type(), 0);
            ASSERT_NE(image.elemSize(), 0);
        }
    }
} // namespace Resources
