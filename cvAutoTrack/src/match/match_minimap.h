#pragma once
#include "type/MatchType.h"

namespace TianLi::Match
{
	bool match_minimap_cailb(const GenshinScreen& genshin_screen, GenshinMinimapCailb& out_genshin_minimap_cailb);

	bool splite_minimap(const GenshinScreen& genshin_screen, GenshinMinimap& out_genshin_minimap);
}