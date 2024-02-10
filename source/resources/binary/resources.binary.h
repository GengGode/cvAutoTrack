#pragma once

namespace TianLi::Resources::Binary
{
    struct binary_resources
    {
        const unsigned char* data;
        size_t size;
    };
    int get_image_count();
    const char* get_image_name(int image_id);
    binary_resources get_image_binary(const char* name);
} // namespace TianLi::Resources::Binary