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
            rect.x = map.cols - rect.width;
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

        // 如果列表长度小于3，直接返回
        if (list.size() <= 3)
        {
            return list;
        }
        // 计算列表中的平均值
        double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size();
        // 计算标准差
        double accum = 0.0;
        std::for_each(list.begin(), list.end(), [&](const double d) { accum += (d - mean) * (d - mean); });
        double stdev = sqrt(accum / (list.size() - 1));

        // 清洗离群点
        // std::ranges::copy_if(list, std::back_inserter(valid_list), [&](const double d)
        //     { return abs(d - mean) < 0.382 * stdev; });
        // c++17
        for (auto& d : list)
            if (abs(d - mean) < 0.382 * stdev)
                valid_list.push_back(d);
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
        res = res - 90; // 从屏幕空间左侧水平线为0度转到竖直向上为0度
        if (res < -180.0)
            res = res + 360;
        return res;
    }

    cv::Point2d transform(cv::Point2d pos, cv::Point2d origin, double scale)
    {
        return cv::Point2d((pos - origin) * scale);
    }

    cv::Point2d transform(cv::Point2d pos, cv::Rect2d inRect, cv::Rect2d outRect)
    {
        // 输入>输出的缩放
        cv::Point2d scale = cv::Point2d(outRect.width / inRect.width, outRect.height / inRect.height);
        // 输入>输出的平移
        cv::Rect2d scaled_rect = cv::Rect2d(inRect.x * scale.x, inRect.y * scale.y, inRect.width * scale.x, inRect.height * scale.y);
        cv::Point2d translate = cv::Point2d(outRect.x - scaled_rect.x, outRect.y - scaled_rect.y);
        // 坐标换算
        return cv::Point2d(pos.x * scale.x, pos.y * scale.y) + translate;
    }
    cv::Point2d transform(const cv::Point2d& pos, const cv::Mat& m)
    {
        cv::Mat p = (cv::Mat_<double>(3, 1) << pos.x, pos.y, 1);
        cv::Mat res = m * p;
        return cv::Point2d(res.at<double>(0, 0) / res.at<double>(2, 0), res.at<double>(1, 0) / res.at<double>(2, 0));
    }

    // 注册表读取
    bool getRegValue_REG_SZ(HKEY root, std::wstring item, std::wstring key, std::string& ret, int max_length)
    {
        HKEY hKey;
        long lRes = RegOpenKeyExW(root, item.c_str(), 0, KEY_READ, &hKey);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }
        wchar_t* lpData = new wchar_t[max_length];
        DWORD dwType = REG_SZ;
        DWORD dwSize = max_length;

        lRes = RegGetValueW(hKey, NULL, key.c_str(), RRF_RT_REG_SZ, &dwType, lpData, &dwSize);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            delete[] lpData;
            return false;
        }

        char* lpDataA = new char[max_length];
        size_t lpDataALen;
        DWORD isSuccess;
        isSuccess = wcstombs_s(&lpDataALen, lpDataA, max_length, lpData, max_length - 1);
        if (isSuccess == ERROR_SUCCESS)
            ret = lpDataA;
        else
        {
            RegCloseKey(hKey);
            delete[] lpData;
            delete[] lpDataA;
            return false;
        }
        RegCloseKey(hKey);
        delete[] lpData;
        delete[] lpDataA;
        return true;
    }

    bool getRegValue_DWORD(HKEY root, std::wstring item, std::wstring key, int& ret)
    {
        HKEY hKey;
        long lRes = RegOpenKeyExW(root, item.c_str(), 0, KEY_READ, &hKey);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }
        DWORD lpData;
        DWORD dwType = REG_DWORD;
        DWORD dwSize = sizeof(DWORD);

        lRes = RegGetValueW(hKey, NULL, key.c_str(), RRF_RT_REG_DWORD, &dwType, &lpData, &dwSize);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }

        ret = lpData;
        RegCloseKey(hKey);
        return true;
    }

    std::mt19937 create_random_engine()
    {
        std::random_device random_device;
        std::vector<std::uint_least32_t> v(10);
        std::generate(v.begin(), v.end(), std::ref(random_device));
        std::seed_seq seed(v.begin(), v.end());
        return std::mt19937(seed);
    }

    std::vector<unsigned int> create_random_array(const size_t size, const unsigned int rand_min, const unsigned int rand_max)
    {
        assert(rand_min <= rand_max);
        assert(size <= static_cast<size_t>(rand_max - rand_min + 1));

        // メルセンヌ・ツイスタ作成
        // 翻译：创建梅森旋转器
        auto random_engine = create_random_engine();
        std::uniform_int_distribution<unsigned int> uniform_int_distribution(rand_min, rand_max);

        // sizeより少し大きくランダム数列(重複あり)を作成する
        // 翻译：比size稍大的随机数列（可能有重复项）
        const auto make_size = static_cast<size_t>(size * 1.2);

        // vがsizeになるまで繰り返す
        // 翻译：重复直到v的大小为size
        std::vector<unsigned int> v;
        v.reserve(size);
        while (v.size() != size)
        {
            // ランダム整数列を順に追加(重複がある可能性がある)
            // 翻译：按顺序添加随机整数列（可能有重复项）
            while (v.size() < make_size)
            {
                v.push_back(uniform_int_distribution(random_engine));
            }

            // ソートして重複を除く -> 重複が除かれた数列の末尾のイテレータがunique_endに入る
            // 翻译：排序并删除重复项->唯一末尾的迭代器进入unique_end
            std::sort(v.begin(), v.end());
            auto unique_end = std::unique(v.begin(), v.end());

            // vのサイズが大きすぎたら，sizeまでのイテレータに変えておく
            // 翻译：如果v的大小太大，则将其更改为迭代器大小
            if (size < static_cast<size_t>(std::distance(v.begin(), unique_end)))
            {
                unique_end = std::next(v.begin(), size);
            }

            // 重複部分から最後までを削除する
            // 翻译：删除重复部分到最后
            v.erase(unique_end, v.end());
        }

        // 昇順になっているのでシャッフル
        // 翻译：因为它是升序的，所以洗牌
        std::shuffle(v.begin(), v.end(), random_engine);

        return v;
    }
    void normalize(const std::vector<cv::KeyPoint>& keypts, std::vector<cv::Point2d>& normalized_pts, cv::Mat& transform)
    {
        double mean_x = 0;
        double mean_y = 0;
        const auto num_keypts = keypts.size();

        normalized_pts.resize(num_keypts);

        for (const auto& keypt : keypts)
        {
            mean_x += keypt.pt.x;
            mean_y += keypt.pt.y;
        }
        mean_x = mean_x / num_keypts;
        mean_y = mean_y / num_keypts;

        double mean_l1_dev_x = 0;
        double mean_l1_dev_y = 0;

        for (unsigned int index = 0; index < num_keypts; ++index)
        {
            normalized_pts.at(index).x = keypts.at(index).pt.x - mean_x;
            normalized_pts.at(index).y = keypts.at(index).pt.y - mean_y;

            mean_l1_dev_x += std::abs(normalized_pts.at(index).x);
            mean_l1_dev_y += std::abs(normalized_pts.at(index).y);
        }

        mean_l1_dev_x = mean_l1_dev_x / num_keypts;
        mean_l1_dev_y = mean_l1_dev_y / num_keypts;

        const double mean_l1_dev_x_inv = static_cast<double>(1.0) / mean_l1_dev_x;
        const double mean_l1_dev_y_inv = static_cast<double>(1.0) / mean_l1_dev_y;

        for (auto& normalized_pt : normalized_pts)
        {
            normalized_pt.x *= mean_l1_dev_x_inv;
            normalized_pt.y *= mean_l1_dev_y_inv;
        }

        transform = cv::Mat::eye(3, 3, CV_64F);
        transform.at<double>(0, 0) = mean_l1_dev_x_inv;
        transform.at<double>(1, 1) = mean_l1_dev_y_inv;
        transform.at<double>(0, 2) = -mean_x * mean_l1_dev_x_inv;
        transform.at<double>(1, 2) = -mean_y * mean_l1_dev_y_inv;

        // transform(0, 0) = mean_l1_dev_x_inv;
        // transform(1, 1) = mean_l1_dev_y_inv;
        // transform(0, 2) = -mean_x * mean_l1_dev_x_inv;
        // transform(1, 2) = -mean_y * mean_l1_dev_y_inv;
    }

} // namespace TianLi::Utils
