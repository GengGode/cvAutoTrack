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
            { "奥摩斯港", { cv::Rect2i(2432, -1563, 800, 1200),cv::Rect2i(-1401, 570, 267, 400)} },
            { "枫丹庭", { cv::Rect2i(2132, -3363, 1100, 1200),cv::Rect2i(-2602, -2596, 367, 400)} },
            { "歌剧院", { cv::Rect2i(1432, -3363, 700, 1200),cv::Rect2i(-2068, -2630, 233, 400)} },
            { "荒海遗迹 · 地下一层",{cv::Rect2i(1861,5210,240,248), cv::Rect2i(2637,889,186,190)} },
            { "荒海遗迹 · 地下二层",{cv::Rect2i(1861,5458,240,232), cv::Rect2i(2637,899,184,181)} },
            { "荒海遗迹 · 地下二层",{cv::Rect2i(1205,4330,340,280), cv::Rect2i(2695,795,244,209)} },
            { "鹤观遗迹 · 上层",{cv::Rect2i(-727,3838,424,312), cv::Rect2i(1690,3017,292,227)} },
            { "鹤观遗迹 · 下层",{cv::Rect2i(-3127,5974,108,104), cv::Rect2i(1774,3096,108,106)} },
            { "蕈菌点亮的洞窟小径",{cv::Rect2i(77,5270,268,332), cv::Rect2i(-1300,-602,202,240)} },
            { "掣电树的篱阴",{cv::Rect2i(469,4582,284,324), cv::Rect2i(-1134,-547,212,235)} },
            { "茸蕈窟 · 南面洞窟",{cv::Rect2i(1861,4314,256,240), cv::Rect2i(-1547,-414,195,186)} },
            { "茸蕈窟 · 北面地下遗迹",{cv::Rect2i(841,4070,360,332), cv::Rect2i(-1614,-564,256,240)} },
            { "无郁夷摩 · 隐匿之地",{cv::Rect2i(77,3838,392,340), cv::Rect2i(-1348,-786,275,244)} },
            { "旧窟暗道",{cv::Rect2i(-1195,6042,328,180), cv::Rect2i(-1389,431,237,150)} },
            { "迪弗旧窟",{cv::Rect2i(1561,3934,300,284), cv::Rect2i(-1454,494,219,212)} },
            { "降诸魔山 · 水封洞窟",{cv::Rect2i(-1559,4326,352,464), cv::Rect2i(-1162,186,251,316)} },
            { "降诸魔山 · 巨像之下",{cv::Rect2i(-1195,5622,440,420), cv::Rect2i(-1217,473,303,291)} },
            { "迷失洞窟",{cv::Rect2i(77,4178,384,376), cv::Rect2i(-1555,266,269,264)} },
            { "山中洞窟 · 一层",{cv::Rect2i(1205,3838,356,292), cv::Rect2i(-1782,34,252,216)} },
            { "山中洞窟 · 二层",{cv::Rect2i(1561,4646,276,252), cv::Rect2i(-1741,54,207,193)} },
            { "山中洞窟 · 三层",{cv::Rect2i(-303,5018,256,348), cv::Rect2i(-1851,-47,194,248)} },
            { "山中洞窟 · 四层",{cv::Rect2i(841,3838,364,232), cv::Rect2i(-1790,56,257,181)} },
            { "翠翎恐蕈的巢窟",{cv::Rect2i(-303,4638,276,380), cv::Rect2i(-2147,-392,207,268)} },
            { "桓那兰那 · 南侧小径",{cv::Rect2i(841,5750,324,260), cv::Rect2i(-1832,-262,235,197)} },
            { "废墟之下的蜿蜒暗道",{cv::Rect2i(-727,4150,424,240), cv::Rect2i(-1990,596,293,186)} },
            { "雨的尽头",{cv::Rect2i(841,5418,324,332), cv::Rect2i(-1806,596,235,239)} },
            { "通往觉树之堂的水道",{cv::Rect2i(-727,4390,420,336), cv::Rect2i(-2095,209,290,242)} },
            { "须罗蕈的藏身地",{cv::Rect2i(-303,4246,316,392), cv::Rect2i(-1235,-147,230,274)} },
            { "须罗蕈的受困处",{cv::Rect2i(-727,5138,268,404), cv::Rect2i(-1204,-288,202,281)} },
            { "往昔的桓那兰那 · 沙中故里",{cv::Rect2i(1205,4130,348,200), cv::Rect2i(-2133,-92,249,162)} },
            { "往昔的桓那兰那 · 失梦的国土",{cv::Rect2i(-1195,3838,468,416), cv::Rect2i(-2229,-88,316,289)} },
            { "舍身步道 · 遗迹上层",{cv::Rect2i(-3543,5174,556,524), cv::Rect2i(-2560,784,371,352)} },
            { "舍身步道 · 遗迹下层",{cv::Rect2i(1861,4050,256,264), cv::Rect2i(-2395,933,195,200)} },
            { "活力之家 · 地下黯道",{cv::Rect2i(77,4930,272,340), cv::Rect2i(-2216,918,204,244)} },
            { "无相之草 · 盘踞之地",{cv::Rect2i(1861,3838,272,212), cv::Rect2i(-2168,980,204,169)} },
            { "秘仪圣殿 · 门厅",{cv::Rect2i(2397,3838,180,180), cv::Rect2i(-2804,940,150,150)} },
            { "秘仪圣殿 · 地下区域",{cv::Rect2i(469,5134,212,280), cv::Rect2i(-2908,905,169,209)} },
            { "秘仪圣殿 · 地下深处",{cv::Rect2i(-3691,5676,100,88), cv::Rect2i(-2903,1005,104,96)} },
            { "秘仪圣殿 · 密室",{cv::Rect2i(-1867,6066,140,88), cv::Rect2i(-2891,966,127,96)} },
            { "赤王陵 · 主厅",{cv::Rect2i(-3691,5448,104,128), cv::Rect2i(-3072,953,106,120)} },
            { "赤王陵 · 中层",{cv::Rect2i(1561,4434,288,212), cv::Rect2i(-3093,927,213,169)} },
            { "赤王陵 · 底层",{cv::Rect2i(2397,4018,160,148), cv::Rect2i(-3081,954,138,132)} },
            { "赤王陵西北 · 岩窟上层",{cv::Rect2i(-1559,5658,240,412), cv::Rect2i(-3330,723,186,286)} },
            { "赤王陵西北 · 岩窟中层",{cv::Rect2i(-2211,4274,620,596), cv::Rect2i(-3353,743,408,394)} },
            { "赤王陵西北 · 岩窟下层",{cv::Rect2i(-1559,4790,336,440), cv::Rect2i(-3261,824,242,302)} },
            { "赤王陵西南 · 岩窟区域",{cv::Rect2i(-1559,3838,364,488), cv::Rect2i(-3288,1026,259,331)} },
            { "赤王陵西南 · 岩窟区域",{cv::Rect2i(-3967,5406,268,420), cv::Rect2i(-3125,948,202,291)} },
            { "赤王陵西南 · 中层通道",{cv::Rect2i(469,5886,172,212), cv::Rect2i(-3192,1039,146,169)} },
            { "赤王陵西南 · 下层遗迹",{cv::Rect2i(1561,5802,220,188), cv::Rect2i(-3221,1092,174,155)} },
            { "赤王陵东部 · 权杖厅",{cv::Rect2i(-3967,3838,424,880), cv::Rect2i(-3075,761,294,560)} },
            { "赤王陵东部 · 岩窟区域",{cv::Rect2i(469,5654,204,232), cv::Rect2i(-2983,1035,165,181)} },
            { "赤王陵南部 · 地下遗迹",{cv::Rect2i(1561,3838,300,96), cv::Rect2i(-3065,1330,221,101)} },
            { "赤王陵西南 · 地下遗迹",{cv::Rect2i(-1867,5758,156,308), cv::Rect2i(-3321,1151,137,226)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(1861,4554,252,192), cv::Rect2i(-2482,1213,193,157)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(1861,5898,168,200), cv::Rect2i(-2338,1321,143,162)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(469,4906,284,228), cv::Rect2i(-2418,1408,212,179)} },
            { "落谷山内 · 洞窟下层",{cv::Rect2i(1205,4610,332,212), cv::Rect2i(-2557,1295,240,168)} },
            { "落谷山内 · 洞窟上层",{cv::Rect2i(-2479,5858,164,368), cv::Rect2i(-2559,1326,141,260)} },
            { "图特摩斯的秘密据点",{cv::Rect2i(469,4210,296,372), cv::Rect2i(-2689,1326,219,263)} },
            { "逾渊地墟 · 上层",{cv::Rect2i(-2211,3838,652,436), cv::Rect2i(-3785,-783,427,300)} },
            { "逾渊地墟 · 下层",{cv::Rect2i(-1195,4254,468,260), cv::Rect2i(-3531,-718,319,197)} },
            { "阻勒隘 · 地下洞窟",{cv::Rect2i(-2979,5342,700,516), cv::Rect2i(-3874,-870,455,346)} },
            { "精石铜城 · 上层",{cv::Rect2i(-3543,5698,416,540), cv::Rect2i(-3668,-1132,288,360)} },
            { "精石铜城 · 下层",{cv::Rect2i(1561,5430,224,188), cv::Rect2i(-3672,-960,176,155)} },
            { "遗忘之路",{cv::Rect2i(-2979,5858,500,348), cv::Rect2i(-3330,-1259,337,248)} },
            { "甘露池",{cv::Rect2i(2133,5118,196,196), cv::Rect2i(-3288,-666,160,160)} },
            { "万种母树",{cv::Rect2i(2133,4550,244,240), cv::Rect2i(-3204,-797,187,186)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(77,5890,208,284), cv::Rect2i(-3175,505,167,212)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(-303,5366,180,300), cv::Rect2i(-3096,609,151,221)} },
            { "居尔城墟神殿 · 下层",{cv::Rect2i(77,5602,208,288), cv::Rect2i(-3093,572,167,214)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(2133,4950,196,168), cv::Rect2i(-3251,543,160,143)} },
            { "居尔城墟神殿 · 下层",{cv::Rect2i(1561,5174,228,256), cv::Rect2i(-3255,513,179,195)} },
            { "居尔城墟神殿 · 上层",{cv::Rect2i(2133,4790,204,160), cv::Rect2i(-3188,627,165,139)} },
            { "阿佩普的行宫花园",{cv::Rect2i(-303,3838,380,408), cv::Rect2i(-3201,-378,268,284)} },
            { "镇灵监牢",{cv::Rect2i(77,4554,292,376), cv::Rect2i(-3187,197,216,266)} },
            { "永恒绿洲",{cv::Rect2i(-2979,3838,768,784), cv::Rect2i(-3035,-110,495,504)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(-2211,5286,508,472), cv::Rect2i(-3378,-26,343,321)} },
            { "达马山遗迹 · 上层",{cv::Rect2i(469,3838,372,372), cv::Rect2i(-2814,21,263,263)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(841,4706,348,356), cv::Rect2i(-2822,33,249,253)} },
            { "达马山遗迹 · 上层",{cv::Rect2i(-1195,4514,460,380), cv::Rect2i(-3098,-245,315,268)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(469,5414,212,240), cv::Rect2i(-3047,-252,169,186)} },
            { "达马山遗迹 · 下层",{cv::Rect2i(-2979,4622,712,720), cv::Rect2i(-3142,-168,462,465)} },
            { "沙虫隧道 · 上方通路",{cv::Rect2i(1205,5350,312,288), cv::Rect2i(-2496,139,228,214)} },
            { "沙虫隧道 · 上层",{cv::Rect2i(-3543,4646,560,528), cv::Rect2i(-2631,-36,373,354)} },
            { "沙虫隧道 · 中层",{cv::Rect2i(-3967,5826,248,412), cv::Rect2i(-2464,-35,190,287)} },
            { "沙虫隧道 · 下层",{cv::Rect2i(-303,5666,156,236), cv::Rect2i(-2448,-35,136,181)} },
            { "白淞镇",{cv::Rect2i(841,5062,332,356), cv::Rect2i(-2345,-2128,240,253)} },
            { "「铁甲熔火帝皇御座」",{cv::Rect2i(1561,4898,228,276), cv::Rect2i(-2431,-1739,179,207)} },
            { "厄里那斯内部 · 北区",{cv::Rect2i(-3543,3838,564,808), cv::Rect2i(-2799,-2131,376,518)} },
            { "厄里那斯内部 · 南区",{cv::Rect2i(-727,4726,348,412), cv::Rect2i(-2676,-1863,249,287)} },
            { "厄里那斯内部 · 南区",{cv::Rect2i(-1195,4894,452,304), cv::Rect2i(-2794,-1844,309,223)} },
            { "炽热的洞窟 · 下层",{cv::Rect2i(1205,4822,332,252), cv::Rect2i(-2616,-1919,239,193)} },
            { "炽热的洞窟 · 上层",{cv::Rect2i(2397,4266,160,160), cv::Rect2i(-2615,-1920,139,139)} },
            { "沉船残骸",{cv::Rect2i(2133,4310,248,240), cv::Rect2i(-2534,-1935,190,186)} },
            { "安眠处",{cv::Rect2i(-1559,5230,316,428), cv::Rect2i(-2803,-2676,230,294)} },
            { "高塔尽头的回旋长廊",{cv::Rect2i(1561,5618,224,184), cv::Rect2i(-2703,-2698,176,153)} },
            { "跳出回旋长廊之后",{cv::Rect2i(-727,5542,220,380), cv::Rect2i(-2733,-2742,174,268)} },
            { "灰河",{cv::Rect2i(1205,5638,312,252), cv::Rect2i(-2533,-2501,227,193)} },
            { "水下城墟 · 外围区域",{cv::Rect2i(1561,4218,292,216), cv::Rect2i(-2505,-2374,215,172)} },
            { "水下城墟 · 中心区域",{cv::Rect2i(841,4402,360,304), cv::Rect2i(-2390,-2354,256,223)} },
            { "水下城墟 · 北部区域",{cv::Rect2i(-3967,4718,276,688), cv::Rect2i(-2333,-2651,207,448)} },
            { "梅洛彼得堡 · 接待处",{cv::Rect2i(2133,5314,160,192), cv::Rect2i(-2471,-3003,138,157)} },
            { "梅洛彼得堡 · 监舍区",{cv::Rect2i(-727,5922,192,316), cv::Rect2i(-2649,-3087,158,230)} },
            { "梅洛彼得堡 · 监舍区",{cv::Rect2i(1205,5890,308,292), cv::Rect2i(-2613,-3085,226,216)} },
            { "梅洛彼得堡 · 管理区",{cv::Rect2i(1205,5074,320,276), cv::Rect2i(-2585,-3039,232,207)} },
            { "「千年珍珠骏麟王庭」",{cv::Rect2i(-2211,4870,568,416), cv::Rect2i(-2813,-3340,378,289)} },
            { "水下岩道",{cv::Rect2i(-1195,5198,440,424), cv::Rect2i(-2594,-3259,301,293)} },
            { "水下岩道",{cv::Rect2i(-3691,4718,140,368), cv::Rect2i(-2418,-3048,127,260)} },
            { "奎瑟尔的发条工坊 · 入口",{cv::Rect2i(469,6098,172,124), cv::Rect2i(-2439,-3343,146,118)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(2133,5506,152,176), cv::Rect2i(-2434,-3337,134,147)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(-3691,5302,104,146), cv::Rect2i(-2458,-3327,104,129)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(-3691,5576,104,100), cv::Rect2i(-2488,-3332,106,103)} },
            { "奎瑟尔的发条工坊 · 中层",{cv::Rect2i(2133,5682,148,176), cv::Rect2i(-2420,-3305,132,148)} },
            { "奎瑟尔的发条工坊 · 下层",{cv::Rect2i(-3127,5698,136,156), cv::Rect2i(-2495,-3339,123,136)} },
            { "奎瑟尔的发条工坊 · 下层",{cv::Rect2i(1861,5690,180,208), cv::Rect2i(-2486,-3288,150,167)} },
            { "奎瑟尔的发条工坊 · 下层",{cv::Rect2i(-3127,5854,124,120), cv::Rect2i(-2422,-3268,118,115)} },
            { "水下洞窟",{cv::Rect2i(-2211,5758,344,472), cv::Rect2i(-2820,-3089,247,321)} },
            { "梅洛彼得堡 · 生产区上层",{cv::Rect2i(1561,5990,212,172), cv::Rect2i(-2551,-3040,169,146)} },
            { "梅洛彼得堡 · 生产区下层",{cv::Rect2i(1861,4746,252,212), cv::Rect2i(-2575,-3063,192,169)} },
            { "梅洛彼得堡 · 废弃生产区",{cv::Rect2i(2133,4098,260,212), cv::Rect2i(-2612,-3135,197,169)} },
            { "梅洛彼得堡 · 废弃生产区",{cv::Rect2i(2133,3838,264,260), cv::Rect2i(-2619,-3236,199,197)} },
            { "废弃实验室 · 上方水域",{cv::Rect2i(-1559,6070,224,152), cv::Rect2i(-2028,-3017,176,134)} },
            { "废弃实验室 · 遗址",{cv::Rect2i(-3691,5086,124,216), cv::Rect2i(-1961,-2992,118,172)} },
            { "废弃实验室 · 遗址",{cv::Rect2i(1861,4958,252,252), cv::Rect2i(-2061,-3010,193,193)} },
            { "废弃实验室 · 遗址下层",{cv::Rect2i(841,6010,324,200), cv::Rect2i(-2153,-3034,234,162)} },
            { "布拉维的藏书室",{cv::Rect2i(1861,6098,168,124), cv::Rect2i(-2173,-2908,143,118)} },
            { "水文数据中枢处理站",{cv::Rect2i(-303,5902,144,208), cv::Rect2i(-2179,-3376,130,167)} },
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
