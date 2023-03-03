#include "pch.h"
#include "Utils.h"
#include <algorithm>
#include <numeric>

namespace TianLi::Utils
{
	double dis(cv::Point2d p)
	{
		return sqrt(p.x * p.x + p.y * p.y);
	}
	
	std::vector<double> extract_valid(std::vector<double> list)
	{
		std::vector<double> valid_list;

		if (list.size() <= 3)
		{
			return list;
		}
		
		double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size(); 
		
		double accum = 0.0;
		std::for_each(list.begin(), list.end(), [&](const double d) { accum += (d - mean) * (d - mean); });
		
		double stdev = sqrt(accum / (list.size() - 1));
		
		std::ranges::copy_if(list, std::back_inserter(valid_list), [&](const double d) { return abs(d - mean) < 0.382 * stdev; });
		return valid_list;
	}
	
	double stdev(std::vector<double> list)
	{
		double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size();

		double accum = 0.0;
		std::for_each(list.begin(), list.end(), [&](const double d) { accum += (d - mean) * (d - mean); });

		return sqrt(accum / (list.size() - 1));
	}
	double stdev_abs(std::vector<double> list)
	{
		double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size();

		double accum = 0.0;
		std::for_each(list.begin(), list.end(), [&](const double d) { accum += (abs(d - mean)) * (abs(d - mean)); });

		return accum / (list.size() - 1);
	}

	std::vector<cv::Point2d> extract_valid(std::vector<cv::Point2d> list)
	{
		std::vector<cv::Point2d> valid_list;

		if (list.size() <= 3)
		{
			return list;
		}

		std::vector<double> x_list;
		std::vector<double> y_list;
		for (auto point : list)
		{
			x_list.push_back(point.x);
			y_list.push_back(point.y);
		}

		std::vector<double> x_valid_list;
		std::vector<double> y_valid_list;
		
		//double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size();
		double x_mean = std::accumulate(x_list.begin(), x_list.end(), 0.0) / x_list.size();
		double y_mean = std::accumulate(y_list.begin(), y_list.end(), 0.0) / y_list.size();
		
		double x_accum = 0.0;
		std::for_each(x_list.begin(), x_list.end(), [&](const double d) {x_accum += (d - x_mean) * (d - x_mean); });
		double y_accum = 0.0;
		std::for_each(y_list.begin(), y_list.end(), [&](const double d) {y_accum += (d - y_mean) * (d - y_mean); });
		
		double x_stdev = sqrt(x_accum / (x_list.size() - 1));
		double y_stdev = sqrt(y_accum / (y_list.size() - 1));

		double param = 1.0;
		if (list.size() > 100)
		{
			param = 0.382;
		}
		else if (list.size() > 50)
		{
			param = 0.618;
		}

		int valid_count = 0;
		for (auto& point : list)
		{
			if (abs(point.x - x_mean) < param * x_stdev && abs(point.y - y_mean) < param * y_stdev)
			{
				x_valid_list.push_back(point.x);
				y_valid_list.push_back(point.y);
				valid_count = valid_count + 1;
			}
		}
		
		for (int i = 0; i < valid_count; i++)
		{
			valid_list.push_back(cv::Point2d(x_valid_list[i], y_valid_list[i]));
		}
		return valid_list;
	}

	void remove_invalid(std::vector<KeyPoint> keypoints, double scale, std::vector<double>& x_list, std::vector<double>& y_list)
	{
		for (int i = 0; i < keypoints.size(); i++)
		{
			auto mini_keypoint = keypoints[i].query;
			auto map_keypoint = keypoints[i].train;

			auto diff_pos = mini_keypoint * scale + map_keypoint;

			x_list.push_back(diff_pos.x);
			y_list.push_back(diff_pos.y);
		}
	}
	
	
	cv::Point2d SPC(std::vector<double> lisx, std::vector<double> lisy)
	{
		cv::Point2d pos;
		double meanx = std::accumulate(lisx.begin(), lisx.end(), 0.0) / lisx.size();
		double meany = std::accumulate(lisy.begin(), lisy.end(), 0.0) / lisy.size();
		double x = meanx;
		double y = meany;
		if (lisx.size() > 3 && lisy.size() > 3)
		{
			double accumx = 0.0;
			double accumy = 0.0;
			for (int i = 0; i < (lisx.size() > lisy.size() ? lisy.size() : lisx.size()); i++)
			{
				accumx += (lisx[i] - meanx) * (lisx[i] - meanx);
				accumy += (lisy[i] - meany) * (lisy[i] - meany);
			}

			double stdevx = sqrt(accumx / (lisx.size() - 1)); //标准差
			double stdevy = sqrt(accumy / (lisy.size() - 1)); //标准差

			double sumx = 0;
			double sumy = 0;
			double numx = 0;
			double numy = 0;
			for (int i = 0; i < (lisx.size() > lisy.size() ? lisy.size() : lisx.size()); i++)
			{
				if (abs(lisx[i] - meanx) < 1 * stdevx)
				{
					sumx += lisx[i];
					numx++;
				}

				if (abs(lisy[i] - meany) < 1 * stdevy)
				{
					sumy += lisy[i];
					numy++;
				}
			}
			x = sumx / numx;
			y = sumy / numy;
			pos = cv::Point2d(x, y);
		}
		else
		{
			pos = cv::Point2d(x, y);
		}
		return pos;
	}

	int getMaxID(double lis[], int len)
	{
		int maxId = 0;
		for (int i = 1; i < len; i++)
		{
			if (lis[i] > lis[maxId])
			{
				maxId = i;
			}
		}
		return maxId;
	}

	int getMinID(double lis[], int len)
	{
		int maxId = 0;
		for (int i = 1; i < len; i++)
		{
			if (lis[i] < lis[maxId])
			{
				maxId = i;
			}
		}
		return maxId;
	}

	std::vector<cv::Point2f> Vector2UnitVector(std::vector<cv::Point2f> pLis)
	{
		double length = 1;
		std::vector<cv::Point2f> res;
		for (int i = 0; i < pLis.size(); i++)
		{
			length = sqrt(pLis[i].x * pLis[i].x + pLis[i].y * pLis[i].y);
			res.emplace_back(cv::Point2f((float)(pLis[i].x / length), (float)(pLis[i].y / length)));
		}
		return res;
	}

	double Line2Angle(cv::Point2f p)
	{
		const double rad2degScale = 180 / CV_PI;
		double res = atan2(-p.y, p.x) * rad2degScale;
		res = res - 90; //从屏幕空间左侧水平线为0度转到竖直向上为0度
		if (res < -180.0)res = res + 360;
		return res;
	}

	cv::Point2d TransferTianLiAxes(cv::Point2d pos, cv::Point2d origin, double scale)
	{
		return cv::Point2d((pos + origin) * scale);
	}

	cv::Point2d TransferUserAxes(cv::Point2d pos, double x, double y, double scale)
	{
		return cv::Point2d((pos.x + x) * scale, (pos.y + y) * scale);
	}

	cv::Point2d TransferTianLiAxes_Tr(cv::Point2d pos, cv::Point2d origin, double scale)
	{
		return cv::Point2d(pos / scale - origin);
	}

	cv::Point2d TransferUserAxes_Tr(cv::Point2d pos, double x, double y, double scale)
	{
		return cv::Point2d(pos.x / scale - x, pos.y / scale - y);
	}

	std::pair<cv::Point2d, int> TransferTianLiAxes(double x, double y)
	{
		const cv::Rect rect_DiXiaCengYan(0, 0, 1250, 1016);
		const cv::Rect rect_YuanXiaGong(0, 5543, 2400, 2401);

		cv::Point2d res;
		int mapId = 0;

		{
			double _x = x;
			double _y = y;
			// 渊下宫
			if (_x > 0 && _x <= 0 + 2400 && _y > 5543 && _y <= 5543 + 2401)
			{
				mapId = 1;
			}
			// 地下层岩
			if (_x > 0 && _x <= 0 + 1250 && _y > 0 && _y <= 0 + 1016)
			{
				mapId = 2;
			}

			switch (mapId)
			{
			case 0:
			{
				res.x = _x;
				res.y = _y;
				break;
			}
			case 1:
			{
				res.x = _x - 0;
				res.y = _y - 5543;
				break;
			}
			case 2:
			{
				res.x = _x - 0;
				res.y = _y - 0;
				break;
			}
			default:
				break;
			}
		}

		return std::make_pair(res, mapId);
	}

	void draw_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<cv::DMatch>& good_matches)
	{
			cv::Mat img_matches, imgmap, imgminmap;
			drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	}
	
	namespace CalcMatch
	{
		namespace Debug
		{
			void calc_good_matches_show(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, double mapScale, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy)
			{
				std::vector<cv::DMatch> good_matches;
				for (size_t i = 0; i < KNN_m.size(); i++)
				{
					if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
					{
						good_matches.push_back(KNN_m[i][0]);
						if (KNN_m[i][0].queryIdx >= keypoint_object.size())
						{
							continue;
						}
						lisx.push_back(((img_object.cols / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.x) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.x));
						lisy.push_back(((img_object.rows / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.y) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.y));
						sumx += lisx.back();
						sumy += lisy.back();
					}
				}
				draw_good_matches(img_scene, keypoint_scene, img_object, keypoint_object, good_matches);
			}
		}

		void calc_good_matches_show(cv::Mat& , std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, double mapScale, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy)
		{
			for (size_t i = 0; i < KNN_m.size(); i++)
			{
				if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
				{
					if (KNN_m[i][0].queryIdx >= keypoint_object.size())
					{
						continue;
					}
					lisx.push_back(((img_object.cols / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.x) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.x));
					lisy.push_back(((img_object.rows / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.y) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.y));
					sumx += lisx.back();
					sumy += lisy.back();
				}
			}
		}
		
		void calc_good_matches_show(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, std::vector<KeyPoint>& good_keypoints)
		{
#ifdef _DEBUG
			std::vector<cv::DMatch> good_matches;
#else
			UNREFERENCED_PARAMETER(img_scene);
#endif
			for (size_t i = 0; i < KNN_m.size(); i++)
			{
				if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
				{
#ifdef _DEBUG
					good_matches.push_back(KNN_m[i][0]);
#endif
					if (KNN_m[i][0].queryIdx >= keypoint_object.size())
					{
						continue;
					}
					good_keypoints.push_back({ {img_object.cols / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.x,
						img_object.rows / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.y },
						{keypoint_scene[KNN_m[i][0].trainIdx].pt.x,keypoint_scene[KNN_m[i][0].trainIdx].pt.y} });
				}
			}
#ifdef _DEBUG
			draw_good_matches(img_scene, keypoint_scene, img_object, keypoint_object, good_matches);
#endif
		}
		
	}

	void calc_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, double mapScale, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy)
	{
		CalcMatch::
#ifdef _DEBUG
			Debug::
#endif
			calc_good_matches_show(img_scene, keypoint_scene, img_object, keypoint_object, KNN_m, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);
	}
	
	void calc_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, std::vector<KeyPoint>& good_keypoints)
	{
		CalcMatch::calc_good_matches_show(img_scene, keypoint_scene, img_object, keypoint_object, KNN_m, ratio_thresh, good_keypoints);
	}
}