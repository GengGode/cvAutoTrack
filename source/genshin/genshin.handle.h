#pragma once
#include "match/type/MatchType.h"

namespace TianLi::Genshin
{
	GenshinHandle func_get_handle(HWND& in);

	void get_genshin_handle(GenshinHandle& genshin_handle);

	void update_genshin_handle(const HWND& old_handle, GenshinHandle& out_genshin_handle);
}
