#pragma once
#include "paimon/resources.binary.image.paimon.h"
#include "cailb/resources.binary.image.cailb.h"
#include "star/resources.binary.image.star.h"
#include "uid/resources.binary.image.uid_.h"
#include "uid/resources.binary.image.uid0.h"
#include "uid/resources.binary.image.uid1.h"
#include "uid/resources.binary.image.uid2.h"
#include "uid/resources.binary.image.uid3.h"
#include "uid/resources.binary.image.uid4.h"
#include "uid/resources.binary.image.uid5.h"
#include "uid/resources.binary.image.uid6.h"
#include "uid/resources.binary.image.uid7.h"
#include "uid/resources.binary.image.uid8.h"
#include "uid/resources.binary.image.uid9.h"

namespace TianLi::Resources::Binary::Image
{
	const unsigned char* image_list[] =
	{
		Png::cailb,
		Png::paimon,
		Bmp::star,
		Bmp::uid_,
		Bmp::uid0,
		Bmp::uid1,
		Bmp::uid2,
		Bmp::uid3,
		Bmp::uid4,
		Bmp::uid5,
		Bmp::uid6,
		Bmp::uid7,
		Bmp::uid8,
		Bmp::uid9
	};
	const size_t image_size[]=
	{
		sizeof(Png::cailb),
		sizeof(Png::paimon),
		sizeof(Bmp::star),
		sizeof(Bmp::uid_),
		sizeof(Bmp::uid0),
		sizeof(Bmp::uid1),
		sizeof(Bmp::uid2),
		sizeof(Bmp::uid3),
		sizeof(Bmp::uid4),
		sizeof(Bmp::uid5),
		sizeof(Bmp::uid6),
		sizeof(Bmp::uid7),
		sizeof(Bmp::uid8),
		sizeof(Bmp::uid9)
	};
	const char* image_name[] =
	{
		"cailb",
		"paimon",
		"star",
		"uid_",
		"uid0",
		"uid1",
		"uid2",
		"uid3",
		"uid4",
		"uid5",
		"uid6",
		"uid7",
		"uid8",
		"uid9"
	};
}