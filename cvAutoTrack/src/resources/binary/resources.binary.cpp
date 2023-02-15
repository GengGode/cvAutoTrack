#include "pch.h"
#include "resources.binary.h"
#include "image/resources.binary.image.h"

TianLi::Resources::Binary::binary_resources TianLi::Resources::Binary::get_image_binary(const char* name)
{
	for (int i = 0; i < sizeof(Image::image_list); i++)
	{
		if (strcmp(name, Image::image_name[i]) == 0)
		{
			return { Image::image_list[i], Image::image_size[i] };
		}
	}
	return {};
}