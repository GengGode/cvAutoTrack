#include "pch.h"
#include "Utils.h"

namespace TianLi::Utils
{
	cv::Mat get_some_map(const cv::Mat& map, const cv::Point& pos, int size_r)
	{
		cv::Rect rect(pos.x - size_r, pos.y - size_r, size_r + size_r, size_r + size_r);
		if (rect.x < 0)
		{
			rect.x = 0;
		}
		if (rect.y < 0)
		{
			rect.y = 0;
		}
		if (rect.x + rect.width > map.cols)
		{
			rect.x = map.cols -  rect.width;
		}
		if (rect.y + rect.height > map.rows)
		{
			rect.y = map.rows - rect.height;
		}
		return map(rect);
	}
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
	cv::Mat crop_border(const cv::Mat& mat, double border)
	{
		int crop_size = static_cast<int>((mat.rows + mat.cols) * 0.5 * border);
		return mat(cv::Rect(crop_size, crop_size, mat.cols - crop_size * 2, mat.rows - crop_size * 2));
	}
	double stdev(std::vector<cv::Point2d> list)
	{
		std::vector<double> x_list(list.size());
		std::vector<double> y_list(list.size());
		for (int i = 0; i < list.size(); i++)
		{
			x_list[i] = list[i].x;
			y_list[i] = list[i].y;
		}
		return (stdev(x_list) + stdev(y_list)) / 2;
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

		//double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size_r();
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

	void remove_invalid(std::vector<MatchKeyPoint> keypoints, double scale, std::vector<double>& x_list, std::vector<double>& y_list)
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

	cv::Point2d TransferAxes(cv::Point2d pos, cv::Point2d origin, double scale)
	{
		return cv::Point2d((pos - origin) * scale);
	}

	std::pair<cv::Point2d, int> getSpecialMapPosition(double x, double y)
	{
		// 地下层岩
	  cv::Rect2d rect_DiXiaCengYan(0, 0, 1700, 1700);
		rect_DiXiaCengYan -= cv::Point2d(340, 565);
		// 渊下宫
		const cv::Rect2d rect_YuanXiaGong(0, 5543, 2400, 2401);
		const std::array<cv::Rect, 2> rectList = {rect_YuanXiaGong,rect_DiXiaCengYan };
		for (int i = 0; i < rectList.size(); i++)
		{
			if (rectList[i].contains(cv::Point2d(x, y)))
			{
				return std::make_pair(cv::Point2d(x - rectList[i] .x,y-rectList[i] .y), i+1);
			}
		}
		return std::make_pair(cv::Point2d(x, y), 0);
	}

	void draw_good_matches(const cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<cv::DMatch>& good_matches)
	{
		cv::Mat img_matches, imgmap, imgminmap;
		drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	}

	namespace CalcMatch
	{
		void calc_good_matches_show(const cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, std::vector<MatchKeyPoint>& good_keypoints)
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
						{keypoint_scene[KNN_m[i][0].trainIdx].pt.x, keypoint_scene[KNN_m[i][0].trainIdx].pt.y} });
				}
			}
#ifdef _DEBUG
			draw_good_matches(img_scene, keypoint_scene, img_object, keypoint_object, good_matches);
#endif
		}
	}

	void calc_good_matches(const cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, std::vector<TianLi::Utils::MatchKeyPoint>& good_keypoints)
	{
		CalcMatch::calc_good_matches_show(img_scene, keypoint_scene, img_object, keypoint_object, KNN_m, ratio_thresh, good_keypoints);
	}
	

	//注册表读取
	bool getRegValue_REG_SZ(HKEY root, std::wstring Item, std::wstring Key, std::string& ret, size_t maxLength)
	{
		HKEY hKey;
		long lRes = RegOpenKeyEx(root, Item.c_str(), 0, KEY_READ, &hKey);
		if (lRes != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}
		wchar_t* lpData = new wchar_t[maxLength];
		DWORD dwType = REG_SZ;
		DWORD dwSize = maxLength;

		lRes = RegGetValue(hKey, NULL, Key.c_str(), RRF_RT_REG_SZ, &dwType, lpData, &dwSize);
		if (lRes != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			delete[] lpData;
			return false;
		}

		char* lpDataA = new char[maxLength];
		size_t  lpDataALen;
		DWORD isSuccess;
		isSuccess = wcstombs_s(&lpDataALen,lpDataA,maxLength, lpData, maxLength-1);
		if (isSuccess == ERROR_SUCCESS)
			ret = lpDataA;
		else
		{
			delete[] lpData;
			delete[] lpDataA;
			return false;
		}
		RegCloseKey(hKey);
		delete[] lpData;
		delete[] lpDataA;
		return true;
	}

	bool getRegValue_DWORD(HKEY root, std::wstring Item, std::wstring Key, int& ret)
	{
		HKEY hKey;
		long lRes = RegOpenKeyEx(root, Item.c_str(), 0, KEY_READ, &hKey);
		if (lRes != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}
		DWORD lpData;
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);

		lRes = RegGetValue(hKey, NULL, Key.c_str(), RRF_RT_REG_DWORD, &dwType, &lpData, &dwSize);
		if (lRes != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}

		ret = lpData;
		RegCloseKey(hKey);
		return true;
	}
}