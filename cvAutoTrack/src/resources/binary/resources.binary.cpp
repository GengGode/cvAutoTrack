#include "pch.h"
#include "resources.binary.h"
#include "image/resources.binary.image.h"

int TianLi::Resources::Binary::get_image_count()
{
	return sizeof(Image::image_list) / sizeof(Image::image_list[0]);
}
const char *TianLi::Resources::Binary::get_image_name(int image_id)
{
	if (image_id < 0 || image_id >= get_image_count())
	{
		return nullptr;
	}
	return Image::image_name[image_id];
}
TianLi::Resources::Binary::binary_resources TianLi::Resources::Binary::get_image_binary(const char *name)
{
	for (int i = 0; i < sizeof(Image::image_list); i++)
	{
		if (strcmp(name, Image::image_name[i]) == 0)
		{
			return {Image::image_list[i], Image::image_size[i]};
		}
	}
	return {};
}