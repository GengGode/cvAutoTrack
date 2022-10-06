#pragma once
#include "type/MatchType.h"

namespace TianLi::Match
{
	GenshinHandle func_get_handle(HWND& in);

	void get_genshin_handle(GenshinHandle& genshin_handle);

	void update_genshin_handle(const HWND& old_handle, GenshinHandle& out_genshin_handle);

	void get_genshin_screen(const GenshinHandle& genshin_handle, GenshinScreen& out_genshin_screen);

	bool check_paimon(const GenshinScreen& genshin_screen, GenshinPaimon& out_genshin_paimon);

	bool match_minimap_cailb(const GenshinScreen& genshin_screen, GenshinMinimapCailb& out_genshin_minimap_cailb);

	bool splite_minimap(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap);
}