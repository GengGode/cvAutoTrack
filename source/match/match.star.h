#pragma once
#include "match.include.h"
#include "utils/Utils.h"
#include "resources/Resources.h"

struct star_calculation_config
{
	bool is_on_city = false;
	bool error = false;
	error_info err = { 0,"" };
};

void star_calculation(cv::Mat& giStarRef,char* jsonBuff, star_calculation_config& config)
{
	Resources& res = Resources::getInstance();

	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	std::vector<cv::Point2d> pos;
	double scale = 1.3;

	matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
	minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
	if (maxVal > 0.66)
	{
		isLoopMatch = true;
		pos.emplace_back(cv::Point2d(maxLoc) -
			cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
			cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
	}

	while (isLoopMatch)
	{
		giStarRef(cv::Rect(maxLoc.x, maxLoc.y, res.StarTemplate.cols, res.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
		matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isLoopMatch = false;
		}
		else
		{
			pos.emplace_back(cv::Point2d(maxLoc) -
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
				cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
		}

		MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
		}

	if (config.is_on_city)
	{
		scale = 0.8667;
	}


	sprintf_s(jsonBuff, 1024, "{\"n\": %d ,\"list\":[", static_cast<int>(pos.size()));//[123,12],[123,53]]}")
	for (int i = 0; i < pos.size(); i++)
	{
		char buff[99];
		if (i == 0)
		{
			sprintf_s(buff, 99, "[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
		}
		else
		{
			sprintf_s(buff, 99, ",[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
		}
		strncat_s(jsonBuff, 1024, buff, 99);
	}
	strncat_s(jsonBuff, 1024, "]}", 3);

}
