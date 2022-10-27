#include "pch.h"
#include "DiffMatch.h"

DiffMatch::DiffMatch()
{
}

DiffMatch::~DiffMatch()
{
}

void DiffMatch::setInitMap(cv::Mat minimap_last_mat)
{
	_minimap_last_mat = minimap_last_mat;
}

void DiffMatch::setMiniMap(cv::Mat minimap_now_mat)
{
    _minimap_now_mat = minimap_now_mat;
}

void DiffMatch::match()
{
    if (_minimap_last_mat.empty())
    {
        _minimap_last_mat = _minimap_now_mat;
		return;
    }
	
	if (_minimap_last_mat.size != _minimap_now_mat.size)
	{
		_minimap_last_mat = _minimap_now_mat;
		return;
	}
	
	cv::Mat minimap_last_mat32;
	cv::Mat minimap_now_mat32;
	cv::cvtColor(_minimap_now_mat, minimap_now_mat32, cv::COLOR_RGBA2GRAY);
	cv::cvtColor(_minimap_last_mat, minimap_last_mat32, cv::COLOR_RGBA2GRAY);

	minimap_now_mat32.convertTo(minimap_now_mat32, CV_32FC1);
	minimap_last_mat32.convertTo(minimap_last_mat32, CV_32FC1);
	
	// 利用 phaseCorrelate 方法计算两帧之间小地图的平移
	auto diff_pos = cv::phaseCorrelate(minimap_now_mat32, minimap_last_mat32);
	// TODO: 数据需要-0.5，待研究 
	diff_pos.x -= 0.5;
	diff_pos.y -= 0.5;
	
	_minimap_last_mat = _minimap_now_mat;
	
	_diff_pos = diff_pos;
}

cv::Point2d DiffMatch::match(cv::Mat minimap_now_mat)
{
    setMiniMap(minimap_now_mat);
    match();
    return getDiffPos();
}

cv::Point2d DiffMatch::getDiffPos()
{
    return _diff_pos;
}
