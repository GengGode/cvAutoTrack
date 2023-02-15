#include "pch.h"
#include "Match.h"
#include "resources/Resources.h"
#include "surf/SurfMatch.h"
#include "../filter/kalman/Kalman.h"

// #include <filesystem>

void TianLi::Match::get_avatar_position(const GenshinMinimap& genshin_minimap, GenshinAvatarPosition& out_genshin_position)
{
	static SurfMatch surf_match;
	static bool is_init = false;
	if (genshin_minimap.is_run_init_start == true)
	{
		if (is_init)return;
		Resources::getInstance().install();
		
		if(out_genshin_position.config.pos_filter == nullptr)
		{
			out_genshin_position.config.pos_filter = new Kalman();
		}
		
		std::vector<cv::KeyPoint> gi_map_keypoints;
		cv::Mat gi_map_descriptors;

		surf_match.setMap(Resources::getInstance().MapTemplate);
		get_map_keypoint(gi_map_keypoints, gi_map_descriptors);
		surf_match.Init(gi_map_keypoints, gi_map_descriptors);

		is_init = true;
		return;
	}

	if (genshin_minimap.is_run_uninit_start == true)
	{
		surf_match.UnInit();
		Resources::getInstance().release();
		if (out_genshin_position.config.pos_filter)
		{
			delete out_genshin_position.config.pos_filter;
		}
		is_init = false;
		return;
	}

	if (genshin_minimap.config.is_find_paimon == false)
	{
		return;
	}

	if (genshin_minimap.img_minimap.empty())
	{
		return;
	}

	surf_match.setMiniMap(genshin_minimap.img_minimap);

	surf_match.match();
	
	out_genshin_position.position = surf_match.getLocalPos();
	out_genshin_position.config.is_continuity = surf_match.isContinuity;
	out_genshin_position.config.is_coveying = surf_match.isCoveying;
	out_genshin_position.config.is_on_city = surf_match.isOnCity;

	if (out_genshin_position.config.is_use_filter)
	{
		cv::Point2d pos = out_genshin_position.position;
		cv::Point2d filt_pos;
		if (out_genshin_position.config.is_coveying || out_genshin_position.config.is_continuity == false)
		{
			filt_pos = out_genshin_position.config.pos_filter->re_init_filterting(pos);
		}
		else
		{
			filt_pos = out_genshin_position.config.pos_filter->filterting(pos);
		}
		out_genshin_position.position = filt_pos;
	}
	
	if (surf_match.is_success_match)
	{
		out_genshin_position.config.img_last_match_minimap = genshin_minimap.img_minimap.clone();
		out_genshin_position.config.is_exist_last_match_minimap = true;
	}
	else
	{
		out_genshin_position.config.is_exist_last_match_minimap = false;
	}
}