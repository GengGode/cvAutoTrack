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

        if (list.size() <= 3)
        {
            return list;
        }

        double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size();

        double accum = 0.0;
        std::for_each(list.begin(), list.end(), [&](const double d)
            { accum += (d - mean) * (d - mean); });

        double stdev = sqrt(accum / (list.size() - 1));

        std::ranges::copy_if(list, std::back_inserter(valid_list), [&](const double d)
            { return abs(d - mean) < 0.382 * stdev; });
        return valid_list;
    }

    double stdev(std::vector<double> list)
    {
        double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size();

        double accum = 0.0;
        std::for_each(list.begin(), list.end(), [&](const double d)
            { accum += (d - mean) * (d - mean); });

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
        std::for_each(list.begin(), list.end(), [&](const double d)
            { accum += (abs(d - mean)) * (abs(d - mean)); });

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

        // double mean = std::accumulate(list.begin(), list.end(), 0.0) / list.size_r();
        double x_mean = std::accumulate(x_list.begin(), x_list.end(), 0.0) / x_list.size();
        double y_mean = std::accumulate(y_list.begin(), y_list.end(), 0.0) / y_list.size();

        double x_accum = 0.0;
        std::for_each(x_list.begin(), x_list.end(), [&](const double d)
            { x_accum += (d - x_mean) * (d - x_mean); });
        double y_accum = 0.0;
        std::for_each(y_list.begin(), y_list.end(), [&](const double d)
            { y_accum += (d - y_mean) * (d - y_mean); });

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

    bool SPC(std::vector<double> lisx, std::vector<double> lisy, cv::Point2d& out)
    {
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

            double stdevx = sqrt(accumx / (lisx.size() - 1)); // 标准差
            double stdevy = sqrt(accumy / (lisy.size() - 1)); // 标准差

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
            out = cv::Point2d(x, y);
        }
        else
        {
            out = cv::Point2d();
            return 0;
        }
        return true;
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

    cv::Point2d TransferAxes(cv::Point2d pos, cv::Point2d origin, double scale)
    {
        return cv::Point2d((pos - origin) * scale);
    }

    cv::Point2d TransferAxes_inv(cv::Point2d pos, cv::Point2d origin, double scale)
    {
        return cv::Point2d(pos / scale + origin);
    }

    std::pair<cv::Point2d, int> ConvertSpecialMapsPosition(double x, double y)
    {
        std::multimap<std::string, std::pair<cv::Rect2i, cv::Rect2i>> mappers{
            { "渊下宫", { cv::Rect2i(-3968, 2037, 1600, 1600),cv::Rect2i(211, 267, 2133, 2133) }},
            { "地下层岩", { cv::Rect2i(-2368, 2837, 1000, 800),cv::Rect2i(369,584,1333,1066)} },
            { "蒙德城", { cv::Rect2i(2432, -2163, 800, 600),cv::Rect2i(560, -1800, 267, 200)} },
            { "望舒客栈", { cv::Rect2i(2832, -3763, 400, 400),cv::Rect2i(-28, -955, 133, 133)} },
            { "璃月港", { cv::Rect2i(1532, -963, 900, 1100),cv::Rect2i(-68, -288, 300, 367)} },
            { "离岛", { cv::Rect2i(2632, -363, 600, 500),cv::Rect2i(2349, 1037, 200, 167)} },
            { "稻妻城", { cv::Rect2i(1232, 137, 700, 900),cv::Rect2i(2732, 1370, 233, 299)} },
            { "须弥城", { cv::Rect2i(1932, -2163, 500, 600),cv::Rect2i(-1501, -230, 166, 200)} },
            { "奥摩斯港", { cv::Rect2i(2432, -1563, 800, 1200),cv::Rect2i(-1401, 570, 267, 400)} },
            { "枫丹庭", { cv::Rect2i(2132, -3363, 1100, 1200),cv::Rect2i(-2602, -2596, 367, 400)} },
            { "歌剧院", { cv::Rect2i(1432, -3363, 700, 1200),cv::Rect2i(-2068, -2630, 233, 400)} },
            { "荒海遗迹 · 地下一层",{cv::Rect2i(1861,5210,240,248), cv::Rect2i(2659,912,140,145)} },
            { "荒海遗迹 · 地下二层",{cv::Rect2i(1861,5458,240,232), cv::Rect2i(2659,922,139,136)} },
            { "荒海遗迹 · 地下二层",{cv::Rect2i(1205,4330,340,280), cv::Rect2i(2717,817,199,164)} },
            { "鹤观遗迹 · 上层",{cv::Rect2i(-727,3838,424,312), cv::Rect2i(1712,3039,247,182)} },
            { "鹤观遗迹 · 下层",{cv::Rect2i(-3127,5974,108,104), cv::Rect2i(1797,3118,63,61)} },
            { "蕈菌点亮的洞窟小径",{cv::Rect2i(77,5270,268,332), cv::Rect2i(-1277,-579,157,194)} },
            { "掣电树的篱阴",{cv::Rect2i(469,4582,284,324), cv::Rect2i(-1112,-524,166,190)} },
            { "茸蕈窟 · 南面洞窟",{cv::Rect2i(1861,4314,256,240), cv::Rect2i(-1525,-391,150,140)} },
            { "茸蕈窟 · 北面地下遗迹",{cv::Rect2i(841,4070,360,332), cv::Rect2i(-1591,-542,211,194)} },
            { "无郁夷摩 · 隐匿之地",{cv::Rect2i(77,3838,392,340), cv::Rect2i(-1326,-763,230,199)} },
            { "旧窟暗道",{cv::Rect2i(-1195,6042,328,180), cv::Rect2i(-1367,454,191,105)} },
            { "迪弗旧窟",{cv::Rect2i(1561,3934,300,284), cv::Rect2i(-1431,517,174,166)} },
            { "降诸魔山 · 水封洞窟",{cv::Rect2i(-1559,4326,352,464), cv::Rect2i(-1139,209,206,271)} },
            { "降诸魔山 · 巨像之下",{cv::Rect2i(-1195,5622,440,420), cv::Rect2i(-1194,495,258,246)} },
            { "迷失洞窟",{cv::Rect2i(77,4178,384,376), cv::Rect2i(-1533,288,223,219)} },
            { "山中洞窟 · 一层",{cv::Rect2i(1205,3838,356,292), cv::Rect2i(-1759,56,207,171)} },
            { "山中洞窟 · 二层",{cv::Rect2i(1561,4646,276,252), cv::Rect2i(-1719,77,162,147)} },
            { "山中洞窟 · 三层",{cv::Rect2i(-303,5018,256,348), cv::Rect2i(-1829,-24,149,203)} },
            { "山中洞窟 · 四层",{cv::Rect2i(841,3838,364,232), cv::Rect2i(-1767,78,212,136)} },
            { "翠翎恐蕈的巢窟",{cv::Rect2i(-303,4638,276,380), cv::Rect2i(-2125,-369,162,222)} },
            { "桓那兰那 · 南侧小径",{cv::Rect2i(841,5750,324,260), cv::Rect2i(-1809,-239,190,152)} },
            { "废墟之下的蜿蜒暗道",{cv::Rect2i(-727,4150,424,240), cv::Rect2i(-1967,619,248,140)} },
            { "雨的尽头",{cv::Rect2i(841,5418,324,332), cv::Rect2i(-1784,619,190,194)} },
            { "通往觉树之堂的水道",{cv::Rect2i(-727,4390,420,336), cv::Rect2i(-2073,232,244,197)} },
            { "须罗蕈的藏身地",{cv::Rect2i(-303,4246,316,392), cv::Rect2i(-1213,-124,185,229)} },
            { "须罗蕈的受困处",{cv::Rect2i(-727,5138,268,404), cv::Rect2i(-1181,-265,157,236)} },
            { "往昔的桓那兰那 · 沙中故里",{cv::Rect2i(1205,4130,348,200), cv::Rect2i(-2111,-69,204,117)} },
            { "往昔的桓那兰那 · 失梦的国土",{cv::Rect2i(-1195,3838,468,416), cv::Rect2i(-2206,-66,271,244)} },
            { "舍身步道 · 遗迹上层",{cv::Rect2i(-3543,5174,556,524), cv::Rect2i(-2538,807,326,307)} },
            { "舍身步道 · 遗迹下层",{cv::Rect2i(1861,4050,256,264), cv::Rect2i(-2373,955,150,154)} },
            { "活力之家 · 地下黯道",{cv::Rect2i(77,4930,272,340), cv::Rect2i(-2194,940,159,199)} },
            { "无相之草 · 盘踞之地",{cv::Rect2i(1861,3838,272,212), cv::Rect2i(-2146,1003,159,124)} },
            { "秘仪圣殿 · 门厅",{cv::Rect2i(2397,3838,180,180), cv::Rect2i(-2781,962,105,105)} },
            { "秘仪圣殿 · 地下区域",{cv::Rect2i(469,5134,212,280), cv::Rect2i(-2885,927,124,164)} },
            { "秘仪圣殿 · 地下深处",{cv::Rect2i(-3691,5676,100,88), cv::Rect2i(-2880,1027,59,51)} },
            { "秘仪圣殿 · 密室",{cv::Rect2i(-1867,6066,140,88), cv::Rect2i(-2868,989,82,51)} },
            { "赤王陵 · 主厅",{cv::Rect2i(-3691,5448,104,128), cv::Rect2i(-3050,975,61,75)} },
            { "赤王陵 · 中层",{cv::Rect2i(1561,4434,288,212), cv::Rect2i(-3070,950,168,124)} },
            { "赤王陵 · 底层",{cv::Rect2i(2397,4018,160,148), cv::Rect2i(-3059,976,92,86)} },
            { "赤王陵西北 · 岩窟上层",{cv::Rect2i(-1559,5658,240,412), cv::Rect2i(-3308,746,140,241)} },
            { "赤王陵西北 · 岩窟中层",{cv::Rect2i(-2211,4274,620,596), cv::Rect2i(-3330,765,363,349)} },
            { "赤王陵西北 · 岩窟下层",{cv::Rect2i(-1559,4790,336,440), cv::Rect2i(-3239,846,197,257)} },
            { "赤王陵西南 · 岩窟区域",{cv::Rect2i(-1559,3838,364,488), cv::Rect2i(-3265,1049,213,286)} },
            { "赤王陵西南 · 岩窟区域",{cv::Rect2i(-3967,5406,268,420), cv::Rect2i(-3102,971,157,246)} },
            { "赤王陵西南 · 中层通道",{cv::Rect2i(469,5886,172,212), cv::Rect2i(-3169,1061,100,124)} },
            { "赤王陵西南 · 下层遗迹",{cv::Rect2i(1561,5802,220,188), cv::Rect2i(-3199,1115,129,110)} },
            { "赤王陵东部 · 权杖厅",{cv::Rect2i(-3967,3838,424,880), cv::Rect2i(-3053,783,248,515)} },
            { "赤王陵东部 · 岩窟区域",{cv::Rect2i(469,5654,204,232), cv::Rect2i(-2960,1057,119,136)} },
            { "赤王陵南部 · 地下遗迹",{cv::Rect2i(1561,3838,300,96), cv::Rect2i(-3043,1353,175,56)} },
            { "赤王陵西南 · 地下遗迹",{cv::Rect2i(-1867,5758,156,308), cv::Rect2i(-3298,1174,92,180)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(1861,4554,252,192), cv::Rect2i(-2459,1235,147,112)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(1861,5898,168,200), cv::Rect2i(-2315,1343,98,117)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(469,4906,284,228), cv::Rect2i(-2396,1430,166,133)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(1205,4610,332,212), cv::Rect2i(-2535,1318,194,123)} },
            { "落谷山内 · 洞窟上层",{cv::Rect2i(-2479,5858,164,368), cv::Rect2i(-2537,1349,96,215)} },
            { "图特摩斯的秘密据点",{cv::Rect2i(469,4210,296,372), cv::Rect2i(-2666,1349,173,218)} },
            { "逾渊地墟 · 上层",{cv::Rect2i(-2211,3838,652,436), cv::Rect2i(-3762,-760,382,255)} },
            { "逾渊地墟 · 下层",{cv::Rect2i(-1195,4254,468,260), cv::Rect2i(-3508,-695,274,152)} },
            { "阻勒隘 · 地下洞窟",{cv::Rect2i(-2979,5342,700,516), cv::Rect2i(-3852,-848,410,301)} },
            { "精石铜城 · 上层",{cv::Rect2i(-3543,5698,416,540), cv::Rect2i(-3645,-1109,242,315)} },
            { "精石铜城 · 下层",{cv::Rect2i(1561,5430,224,188), cv::Rect2i(-3650,-937,131,110)} },
            { "遗忘之路",{cv::Rect2i(-2979,5858,500,348), cv::Rect2i(-3307,-1236,292,202)} },
            { "甘露池",{cv::Rect2i(2133,5118,196,196), cv::Rect2i(-3265,-644,115,115)} },
            { "万种母树",{cv::Rect2i(2133,4550,244,240), cv::Rect2i(-3181,-774,142,140)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(77,5890,208,284), cv::Rect2i(-3153,527,122,166)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(-303,5366,180,300), cv::Rect2i(-3073,632,106,175)} },
            { "居尔城墟神殿 · 下层",{cv::Rect2i(77,5602,208,288), cv::Rect2i(-3071,595,121,168)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(2133,4950,196,168), cv::Rect2i(-3229,566,115,98)} },
            { "居尔城墟神殿 · 下层",{cv::Rect2i(1561,5174,228,256), cv::Rect2i(-3232,536,133,150)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(2133,4790,204,160), cv::Rect2i(-3166,649,119,93)} },
            { "阿佩普的行宫花园",{cv::Rect2i(-303,3838,380,408), cv::Rect2i(-3178,-356,222,239)} },
            { "镇灵监牢",{cv::Rect2i(77,4554,292,376), cv::Rect2i(-3165,220,171,220)} },
            { "永恒绿洲",{cv::Rect2i(-2979,3838,768,784), cv::Rect2i(-3013,-87,450,459)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(-2211,5286,508,472), cv::Rect2i(-3355,-3,298,276)} },
            { "达马山遗迹 · 上层",{cv::Rect2i(469,3838,372,372), cv::Rect2i(-2791,43,218,218)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(841,4706,348,356), cv::Rect2i(-2800,56,204,208)} },
            { "达马山遗迹 · 上层",{cv::Rect2i(-1195,4514,460,380), cv::Rect2i(-3075,-222,269,222)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(469,5414,212,240), cv::Rect2i(-3025,-229,124,140)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(-2979,4622,712,720), cv::Rect2i(-3119,-145,417,420)} },
            { "沙虫隧道 · 上方通路",{cv::Rect2i(1205,5350,312,288), cv::Rect2i(-2473,162,183,168)} },
            { "沙虫隧道 · 上层",{cv::Rect2i(-3543,4646,560,528), cv::Rect2i(-2608,-13,328,309)} },
            { "沙虫隧道 · 中层",{cv::Rect2i(-3967,5826,248,412), cv::Rect2i(-2441,-13,145,241)} },
            { "沙虫隧道 · 下层",{cv::Rect2i(-303,5666,156,236), cv::Rect2i(-2426,-13,91,136)} },
            { "白淞镇",{cv::Rect2i(841,5062,332,356), cv::Rect2i(-2322,-2105,194,208)} },
            { "「铁甲熔火帝皇御座」",{cv::Rect2i(1561,4898,228,276), cv::Rect2i(-2408,-1716,133,161)} },
            { "厄里那斯内部 · 北区",{cv::Rect2i(-3543,3838,564,808), cv::Rect2i(-2777,-2108,331,473)} },
            { "厄里那斯内部 · 南区",{cv::Rect2i(-727,4726,348,412), cv::Rect2i(-2654,-1840,204,241)} },
            { "厄里那斯内部 · 南区",{cv::Rect2i(-1195,4894,452,304), cv::Rect2i(-2772,-1821,264,178)} },
            { "炽热的洞窟 · 下层",{cv::Rect2i(1205,4822,332,252), cv::Rect2i(-2594,-1897,194,147)} },
            { "炽热的洞窟 · 上层",{cv::Rect2i(2397,4266,160,160), cv::Rect2i(-2592,-1897,94,93)} },
            { "沉船残骸",{cv::Rect2i(2133,4310,248,240), cv::Rect2i(-2511,-1913,145,140)} },
            { "安眠处",{cv::Rect2i(-1559,5230,316,428), cv::Rect2i(-2780,-2654,185,249)} },
            { "高塔尽头的回旋长廊",{cv::Rect2i(1561,5618,224,184), cv::Rect2i(-2681,-2676,131,107)} },
            { "跳出回旋长廊之后",{cv::Rect2i(-727,5542,220,380), cv::Rect2i(-2711,-2719,128,222)} },
            { "灰河",{cv::Rect2i(1205,5638,312,252), cv::Rect2i(-2510,-2478,182,147)} },
            { "水下城墟 · 外围区域",{cv::Rect2i(1561,4218,292,216), cv::Rect2i(-2482,-2351,170,126)} },
            { "水下城墟 · 中心区域",{cv::Rect2i(841,4402,360,304), cv::Rect2i(-2367,-2331,211,178)} },
            { "水下城墟 · 北部区域",{cv::Rect2i(-3967,4718,276,688), cv::Rect2i(-2310,-2628,161,403)} },
            { "梅洛彼得堡 · 接待处",{cv::Rect2i(2133,5314,160,192), cv::Rect2i(-2448,-2981,92,112)} },
            { "梅洛彼得堡 · 监舍区",{cv::Rect2i(-727,5922,192,316), cv::Rect2i(-2627,-3064,113,185)} },
            { "梅洛彼得堡 · 监舍区",{cv::Rect2i(1205,5890,308,292), cv::Rect2i(-2590,-3062,180,171)} },
            { "梅洛彼得堡 · 管理区",{cv::Rect2i(1205,5074,320,276), cv::Rect2i(-2563,-3017,186,161)} },
            { "「千年珍珠骏麟王庭」",{cv::Rect2i(-2211,4870,568,416), cv::Rect2i(-2790,-3318,333,244)} },
            { "水下岩道",{cv::Rect2i(-1195,5198,440,424), cv::Rect2i(-2571,-3237,256,248)} },
            { "水下岩道",{cv::Rect2i(-3691,4718,140,368), cv::Rect2i(-2396,-3026,82,215)} },
            { "奎瑟尔的发条工坊 · 入口",{cv::Rect2i(469,6098,172,124), cv::Rect2i(-2417,-3321,100,73)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(2133,5506,152,176), cv::Rect2i(-2412,-3315,89,102)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(-3691,5302,104,146), cv::Rect2i(-2435,-3304,59,83)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(-3691,5576,104,100), cv::Rect2i(-2466,-3309,60,58)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(2133,5682,148,176), cv::Rect2i(-2397,-3283,87,103)} },
            { "奎瑟尔的发条工坊 · 下层",{cv::Rect2i(-3127,5698,136,156), cv::Rect2i(-2472,-3316,78,91)} },
            { "奎瑟尔的发条工坊 · 下层",{cv::Rect2i(1861,5690,180,208), cv::Rect2i(-2464,-3265,105,121)} },
            { "奎瑟尔的发条工坊 · 下层",{cv::Rect2i(-3127,5854,124,120), cv::Rect2i(-2399,-3245,72,70)} },
            { "水下洞窟",{cv::Rect2i(-2211,5758,344,472), cv::Rect2i(-2798,-3067,201,276)} },
            { "梅洛彼得堡 · 生产区上层",{cv::Rect2i(1561,5990,212,172), cv::Rect2i(-2529,-3017,124,100)} },
            { "梅洛彼得堡 · 生产区下层",{cv::Rect2i(1861,4746,252,212), cv::Rect2i(-2552,-3041,147,124)} },
            { "梅洛彼得堡 · 废弃生产区",{cv::Rect2i(2133,4098,260,212), cv::Rect2i(-2590,-3113,152,124)} },
            { "梅洛彼得堡 · 废弃生产区",{cv::Rect2i(2133,3838,264,260), cv::Rect2i(-2596,-3214,154,152)} },
            { "废弃实验室 · 上方水域",{cv::Rect2i(-1559,6070,224,152), cv::Rect2i(-2006,-2995,131,89)} },
            { "废弃实验室 · 遗址",{cv::Rect2i(-3691,5086,124,216), cv::Rect2i(-1938,-2970,73,126)} },
            { "废弃实验室 · 遗址",{cv::Rect2i(1861,4958,252,252), cv::Rect2i(-2038,-2988,148,147)} },
            { "废弃实验室 · 遗址下层",{cv::Rect2i(841,6010,324,200), cv::Rect2i(-2130,-3012,189,117)} },
            { "布拉维的藏书室",{cv::Rect2i(1861,6098,168,124), cv::Rect2i(-2151,-2885,97,73)} },
            { "水文数据中枢处理站",{cv::Rect2i(-303,5902,144,208), cv::Rect2i(-2156,-3353,85,121)} },
        };

        int id = 0;
        cv::Point2i center = { 3967, 3962 };
        for (auto [key, value] : mappers)
        {
            auto srcRect = value.first + center;
            auto dstRect = value.second + center;

            if (key == "渊下宫" or key == "地下层岩")
            {
                dstRect -= center;
            }

            if (srcRect.contains(cv::Point2d(x, y)))
            {
                if (key == "渊下宫")
                    id = 1;
                else if (key == "地下层岩")
                    id = 2;
                else
                    id = 0;
                cv::Point2d dstPoint(
                    ((double)dstRect.width / srcRect.width) * (x - srcRect.x) + dstRect.x,
                    ((double)dstRect.height / srcRect.height) * (y - srcRect.y) + dstRect.y);
                return { dstPoint,id };
            }
        }
        return { cv::Point(x, y), 0 };
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
                                   img_object.rows / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.y},
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

    // 注册表读取
    bool getRegValue_REG_SZ(HKEY root, std::wstring item, std::wstring key, std::string& ret, int max_length)
    {
        HKEY hKey;
        long lRes = RegOpenKeyEx(root, item.c_str(), 0, KEY_READ, &hKey);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }
        wchar_t* lpData = new wchar_t[max_length];
        DWORD dwType = REG_SZ;
        DWORD dwSize = max_length;

        lRes = RegGetValue(hKey, NULL, key.c_str(), RRF_RT_REG_SZ, &dwType, lpData, &dwSize);
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
        long lRes = RegOpenKeyEx(root, item.c_str(), 0, KEY_READ, &hKey);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }
        DWORD lpData;
        DWORD dwType = REG_DWORD;
        DWORD dwSize = sizeof(DWORD);

        lRes = RegGetValue(hKey, NULL, key.c_str(), RRF_RT_REG_DWORD, &dwType, &lpData, &dwSize);
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
