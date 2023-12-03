#pragma once
#include "match.include.h"
#include "utils/Utils.h"
#include "resources/Resources.h"

struct uid_calculation_config 
{
	bool error = false;
	error_info err = { 0,"" };
	
};

void uid_calculation(cv::Mat& giUIDRef,int& uid, uid_calculation_config config)
{
	Resources& res = Resources::getInstance();

	int _uid = 0;
	int _NumBit[9] = { 0 };

	int bitCount = 1;
	cv::Mat matchTmp;
	cv::Mat Roi;
	cv::Mat checkUID = res.UID;

#ifdef _DEBUG
	//if (checkUID.rows > Roi.rows)
	//{
	//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
	//}
#endif
	giUIDRef.copyTo(Roi);

	if (checkUID.rows > Roi.rows)
	{
		cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
	}

	cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	//寻找最佳匹配位置
	cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
	if (maxVal > 0.75)
	{
		int x_uid_ = maxLoc.x + checkUID.cols + 7;
		int y_uid_ = 0;
		double tmplis[10] = { 0 };
		int tmplisx[10] = { 0 };
		for (int p = 8; p >= 0; p--)
		{
			_NumBit[p] = 0;
			for (int i = 0; i < 10; i++)
			{
				cv::Rect r(x_uid_, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
				if (x_uid_ + r.width > giUIDRef.cols)
				{
					r = cv::Rect(giUIDRef.cols - res.UIDnumber[i].cols - 2, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);
				}

				cv::Mat numCheckUID = res.UIDnumber[i];
				Roi = giUIDRef(r);

				cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

				double minVali, maxVali;
				cv::Point minLoci, maxLoci;
				//寻找最佳匹配位置
				cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

				tmplis[i] = maxVali;
				tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
				if (maxVali > 0.91)
				{
					_NumBit[p] = i;
					x_uid_ = x_uid_ + maxLoci.x + numCheckUID.cols - 1;
					break;
				}
				if (i == 10 - 1)
				{
					_NumBit[p] = TianLi::Utils::getMaxID(tmplis, 10);
					x_uid_ = x_uid_ + tmplisx[_NumBit[p]];
				}
			}
		}
	}
	_uid = 0;
	for (int i = 0; i < 9; i++)
	{
		_uid += _NumBit[i] * bitCount;
		bitCount = bitCount * 10;
	}
	if (_uid == 0)
	{
		config.error = true;
		config.err = { 8,"未能在UID区域检测到有效UID" };
		return;
	}
	uid = _uid;
}