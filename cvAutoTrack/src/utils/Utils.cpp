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
           {"UI_Map_LayeredMap_3110101.png",{cv::Rect2i(1350,6412,240,248), cv::Rect2i(2659,912,140,145)}},
            {"UI_Map_LayeredMap_3110102.png",{cv::Rect2i(-719,4737,438,410), cv::Rect2i(2659,817,257,240)}},
            {"UI_Map_LayeredMap_3180101.png",{cv::Rect2i(-719,5535,424,312), cv::Rect2i(1712,3039,247,182)}},
            {"UI_Map_LayeredMap_3180102.png",{cv::Rect2i(-2231,4646,108,104), cv::Rect2i(1797,3118,63,61)}},
            {"UI_Map_LayeredMap_3350401.png",{cv::Rect2i(195,6042,160,100), cv::Rect2i(-2070,-2334,93,58)}},
            {"UI_Map_LayeredMap_3350301.png",{cv::Rect2i(2016,5838,292,216), cv::Rect2i(-2482,-2351,170,126)}},
            {"UI_Map_LayeredMap_3350302.png",{cv::Rect2i(1646,4386,360,304), cv::Rect2i(-2367,-2331,211,178)}},
            {"UI_Map_LayeredMap_3350303.png",{cv::Rect2i(-3967,5581,276,688), cv::Rect2i(-2310,-2628,161,403)}},
            {"UI_Map_LayeredMap_3350102.png",{cv::Rect2i(-2671,6379,316,428), cv::Rect2i(-2780,-2654,185,249)}},
            {"UI_Map_LayeredMap_3350103.png",{cv::Rect2i(971,5918,224,184), cv::Rect2i(-2681,-2676,131,107)}},
            {"UI_Map_LayeredMap_3350106.png",{cv::Rect2i(-273,5326,220,380), cv::Rect2i(-2711,-2719,128,222)}},
            {"UI_Map_LayeredMap_3340303.png",{cv::Rect2i(-2671,3838,564,808), cv::Rect2i(-2777,-2108,331,473)}},
            {"UI_Map_LayeredMap_3340305.png",{cv::Rect2i(-1339,5902,550,412), cv::Rect2i(-2772,-1840,322,241)}},
            {"UI_Map_LayeredMap_3350201.png",{cv::Rect2i(2016,5050,312,252), cv::Rect2i(-2510,-2478,182,147)}},
            {"UI_Map_LayeredMap_3330201.png",{cv::Rect2i(1646,5022,332,356), cv::Rect2i(-2322,-2105,194,208)}},
            {"UI_Map_LayeredMap_3340402.png",{cv::Rect2i(195,6142,160,160), cv::Rect2i(-2592,-1897,94,93)}},
            {"UI_Map_LayeredMap_3340401.png",{cv::Rect2i(2016,4330,332,252), cv::Rect2i(-2594,-1897,194,147)}},
            {"UI_Map_LayeredMap_3340501.png",{cv::Rect2i(1350,6172,248,240), cv::Rect2i(-2511,-1913,145,140)}},
            {"UI_Map_LayeredMap_3330301.png",{cv::Rect2i(971,5454,228,276), cv::Rect2i(-2408,-1716,133,161)}},
            {"UI_Map_LayeredMap_3360601.png",{cv::Rect2i(-3043,5527,344,472), cv::Rect2i(-2798,-3067,201,276)}},
            {"UI_Map_LayeredMap_3360301.png",{cv::Rect2i(-2671,4646,440,729), cv::Rect2i(-2571,-3237,257,426)}},
            {"UI_Map_LayeredMap_3360201.png",{cv::Rect2i(-1339,4434,568,416), cv::Rect2i(-2790,-3318,333,244)}},
            {"UI_Map_LayeredMap_3360101.png",{cv::Rect2i(195,5850,160,192), cv::Rect2i(-2448,-2981,92,112)}},
            {"UI_Map_LayeredMap_3360103.png",{cv::Rect2i(1646,3838,370,316), cv::Rect2i(-2627,-3064,217,185)}},
            {"UI_Map_LayeredMap_3360104.png",{cv::Rect2i(1646,6406,320,276), cv::Rect2i(-2563,-3017,186,161)}},
            {"UI_Map_LayeredMap_3360701.png",{cv::Rect2i(971,6290,212,172), cv::Rect2i(-2529,-3017,124,100)}},
            {"UI_Map_LayeredMap_3360702.png",{cv::Rect2i(1350,5960,252,212), cv::Rect2i(-2552,-3041,147,124)}},
            {"UI_Map_LayeredMap_3360704.png",{cv::Rect2i(195,4178,271,384), cv::Rect2i(-2596,-3214,158,225)}},
            {"UI_Map_LayeredMap_3360501.png",{cv::Rect2i(-1339,6702,172,124), cv::Rect2i(-2417,-3321,100,73)}},
            {"UI_Map_LayeredMap_3360503.png",{cv::Rect2i(1350,5226,264,230), cv::Rect2i(-2466,-3315,155,135)}},
            {"UI_Map_LayeredMap_3360507.png",{cv::Rect2i(587,5266,248,296), cv::Rect2i(-2472,-3316,145,173)}},
            {"UI_Map_LayeredMap_3370101.png",{cv::Rect2i(-3967,6681,224,152), cv::Rect2i(-2006,-2995,131,89)}},
            {"UI_Map_LayeredMap_3370102.png",{cv::Rect2i(2016,5586,295,252), cv::Rect2i(-2038,-2988,173,147)}},
            {"UI_Map_LayeredMap_3370104.png",{cv::Rect2i(1646,5946,324,200), cv::Rect2i(-2130,-3012,189,117)}},
            {"UI_Map_LayeredMap_3370201.png",{cv::Rect2i(-1167,6702,168,124), cv::Rect2i(-2151,-2885,97,73)}},
            {"UI_Map_LayeredMap_3370301.png",{cv::Rect2i(-3691,5581,144,208), cv::Rect2i(-2156,-3353,85,121)}},
            {"UI_Map_LayeredMap_3390501.png",{cv::Rect2i(-273,6250,148,148), cv::Rect2i(-1864,-1921,86,86)}},
            {"UI_Map_LayeredMap_3340107.png",{cv::Rect2i(195,5642,160,208), cv::Rect2i(-1787,-2772,93,121)}},
            {"UI_Map_LayeredMap_3380101.png",{cv::Rect2i(-3543,5527,500,808), cv::Rect2i(-1814,-2808,293,473)}},
            {"UI_Map_LayeredMap_3380104.png",{cv::Rect2i(971,3838,379,380), cv::Rect2i(-1783,-2519,222,221)}},
            {"UI_Map_LayeredMap_3380108.png",{cv::Rect2i(-719,5147,436,388), cv::Rect2i(-1959,-2682,255,227)}},
            {"UI_Map_LayeredMap_3380301.png",{cv::Rect2i(1646,5378,332,236), cv::Rect2i(-1595,-2657,194,138)}},
            {"UI_Map_LayeredMap_3390102.png",{cv::Rect2i(-3243,6683,196,148), cv::Rect2i(-1955,-2167,115,86)}},
            {"UI_Map_LayeredMap_3390103.png",{cv::Rect2i(195,4910,180,300), cv::Rect2i(-1962,-2220,105,175)}},
            {"UI_Map_LayeredMap_3390104.png",{cv::Rect2i(587,5562,248,292), cv::Rect2i(-2055,-2216,145,171)}},
            {"UI_Map_LayeredMap_3390201.png",{cv::Rect2i(971,4594,276,332), cv::Rect2i(-1805,-2089,161,194)}},
            {"UI_Map_LayeredMap_3390202.png",{cv::Rect2i(195,5210,172,220), cv::Rect2i(-1757,-2141,100,128)}},
            {"UI_Map_LayeredMap_3390301.png",{cv::Rect2i(971,6462,172,180), cv::Rect2i(-2102,-1841,100,105)}},
            {"UI_Map_LayeredMap_3390302.png",{cv::Rect2i(1350,4766,272,248), cv::Rect2i(-2192,-1809,158,145)}},
            {"UI_Map_LayeredMap_3390401.png",{cv::Rect2i(1350,6660,180,140), cv::Rect2i(-1906,-1807,105,82)}},
            {"UI_Map_LayeredMap_3390402.png",{cv::Rect2i(971,5178,272,276), cv::Rect2i(-1851,-1829,159,161)}},
            {"UI_Map_LayeredMap_3380201.png",{cv::Rect2i(-3691,5789,128,148), cv::Rect2i(-1618,-2392,74,85)}},
            {"UI_Map_LayeredMap_3380202.png",{cv::Rect2i(1350,4534,280,232), cv::Rect2i(-1616,-2391,163,135)}},
            {"UI_Map_LayeredMap_3380203.png",{cv::Rect2i(2016,6506,236,240), cv::Rect2i(-1545,-2407,136,139)}},
            {"UI_Map_LayeredMap_3260101.png",{cv::Rect2i(-1339,5378,556,524), cv::Rect2i(-2538,807,326,307)}},
            {"UI_Map_LayeredMap_3260102.png",{cv::Rect2i(1350,5456,256,264), cv::Rect2i(-2373,955,150,154)}},
            {"UI_Map_LayeredMap_3270101.png",{cv::Rect2i(587,6562,180,180), cv::Rect2i(-2781,962,105,105)}},
            {"UI_Map_LayeredMap_3270102.png",{cv::Rect2i(587,5854,212,280), cv::Rect2i(-2885,927,124,164)}},
            {"UI_Map_LayeredMap_3270103.png",{cv::Rect2i(-3691,6065,100,88), cv::Rect2i(-2880,1027,59,51)}},
            {"UI_Map_LayeredMap_3270104.png",{cv::Rect2i(-999,6702,140,88), cv::Rect2i(-2868,989,82,51)}},
            {"UI_Map_LayeredMap_3270501.png",{cv::Rect2i(-3967,4701,424,880), cv::Rect2i(-3053,783,248,515)}},
            {"UI_Map_LayeredMap_3270502.png",{cv::Rect2i(587,6134,204,232), cv::Rect2i(-2960,1057,119,136)}},
            {"UI_Map_LayeredMap_3270201.png",{cv::Rect2i(-3691,5937,104,128), cv::Rect2i(-3050,975,61,75)}},
            {"UI_Map_LayeredMap_3270202.png",{cv::Rect2i(2016,6054,288,212), cv::Rect2i(-3070,950,168,124)}},
            {"UI_Map_LayeredMap_3270203.png",{cv::Rect2i(195,6302,160,148), cv::Rect2i(-3059,976,92,86)}},
            {"UI_Map_LayeredMap_3270401.png",{cv::Rect2i(-2107,5574,546,622), cv::Rect2i(-3265,971,320,363)}},
            {"UI_Map_LayeredMap_3270403.png",{cv::Rect2i(195,5430,172,212), cv::Rect2i(-3169,1061,100,124)}},
            {"UI_Map_LayeredMap_3270404.png",{cv::Rect2i(971,6102,220,188), cv::Rect2i(-3199,1115,129,110)}},
            {"UI_Map_LayeredMap_3270301.png",{cv::Rect2i(-3967,6269,240,412), cv::Rect2i(-3308,746,140,241)}},
            {"UI_Map_LayeredMap_3270302.png",{cv::Rect2i(-1339,3838,620,596), cv::Rect2i(-3330,765,363,349)}},
            {"UI_Map_LayeredMap_3270303.png",{cv::Rect2i(-3043,5999,336,440), cv::Rect2i(-3239,846,197,257)}},
            {"UI_Map_LayeredMap_3280201.png",{cv::Rect2i(1350,3838,296,372), cv::Rect2i(-2666,1349,173,218)}},
            {"UI_Map_LayeredMap_3280105.png",{cv::Rect2i(-471,6423,164,368), cv::Rect2i(-2537,1349,96,215)}},
            {"UI_Map_LayeredMap_3280103.png",{cv::Rect2i(-2107,6196,542,561), cv::Rect2i(-2535,1235,317,328)}},
            {"UI_Map_LayeredMap_3260201.png",{cv::Rect2i(587,4594,272,340), cv::Rect2i(-2194,940,159,199)}},
            {"UI_Map_LayeredMap_3260301.png",{cv::Rect2i(1350,5014,272,212), cv::Rect2i(-2146,1003,159,124)}},
            {"UI_Map_LayeredMap_3270601.png",{cv::Rect2i(-3543,6683,300,96), cv::Rect2i(-3043,1353,175,56)}},
            {"UI_Map_LayeredMap_3270701.png",{cv::Rect2i(-273,5706,156,308), cv::Rect2i(-3298,1174,92,180)}},
            {"UI_Map_LayeredMap_3320601.png",{cv::Rect2i(2016,4762,312,288), cv::Rect2i(-2473,162,183,168)}},
            {"UI_Map_LayeredMap_3320602.png",{cv::Rect2i(-1339,4850,560,528), cv::Rect2i(-2608,-13,328,309)}},
            {"UI_Map_LayeredMap_3320603.png",{cv::Rect2i(-719,6423,248,412), cv::Rect2i(-2441,-13,145,241)}},
            {"UI_Map_LayeredMap_3320604.png",{cv::Rect2i(-273,6014,156,236), cv::Rect2i(-2426,-13,91,136)}},
            {"UI_Map_LayeredMap_3320101.png",{cv::Rect2i(-719,3838,446,479), cv::Rect2i(-3229,527,261,280)}},
            {"UI_Map_LayeredMap_3320103.png",{cv::Rect2i(-1339,6314,484,388), cv::Rect2i(-3232,536,283,227)}},
            {"UI_Map_LayeredMap_3320502.png",{cv::Rect2i(-3543,4701,858,826), cv::Rect2i(-3075,-222,502,484)}},
            {"UI_Map_LayeredMap_3320506.png",{cv::Rect2i(-3967,3838,1296,863), cv::Rect2i(-3355,-229,759,504)}},
            {"UI_Map_LayeredMap_3320401.png",{cv::Rect2i(-2107,3838,768,784), cv::Rect2i(-3013,-87,450,459)}},
            {"UI_Map_LayeredMap_3320302.png",{cv::Rect2i(971,4218,292,376), cv::Rect2i(-3165,220,171,220)}},
            {"UI_Map_LayeredMap_3320201.png",{cv::Rect2i(-273,4514,380,408), cv::Rect2i(-3178,-356,222,239)}},
            {"UI_Map_LayeredMap_3300201.png",{cv::Rect2i(-2107,4622,700,516), cv::Rect2i(-3852,-848,410,301)}},
            {"UI_Map_LayeredMap_3300101.png",{cv::Rect2i(-2107,5138,652,436), cv::Rect2i(-3762,-760,382,255)}},
            {"UI_Map_LayeredMap_3300102.png",{cv::Rect2i(-273,4254,468,260), cv::Rect2i(-3508,-695,274,152)}},
            {"UI_Map_LayeredMap_3310101.png",{cv::Rect2i(-2671,5375,416,540), cv::Rect2i(-3645,-1109,242,315)}},
            {"UI_Map_LayeredMap_3310102.png",{cv::Rect2i(971,5730,224,188), cv::Rect2i(-3650,-937,131,110)}},
            {"UI_Map_LayeredMap_3310201.png",{cv::Rect2i(-3543,6335,500,348), cv::Rect2i(-3307,-1236,292,202)}},
            {"UI_Map_LayeredMap_3310401.png",{cv::Rect2i(2016,6266,244,240), cv::Rect2i(-3181,-774,142,140)}},
            {"UI_Map_LayeredMap_3310301.png",{cv::Rect2i(587,6366,196,196), cv::Rect2i(-3265,-644,115,115)}},
            {"UI_Map_LayeredMap_3240101.png",{cv::Rect2i(-3043,6439,316,392), cv::Rect2i(-1213,-124,185,229)}},
            {"UI_Map_LayeredMap_3240201.png",{cv::Rect2i(-273,4922,268,404), cv::Rect2i(-1181,-265,157,236)}},
            {"UI_Map_LayeredMap_3200401.png",{cv::Rect2i(1646,4690,360,332), cv::Rect2i(-1591,-542,211,194)}},
            {"UI_Map_LayeredMap_3200301.png",{cv::Rect2i(1350,5720,256,240), cv::Rect2i(-1525,-391,150,140)}},
            {"UI_Map_LayeredMap_3220301.png",{cv::Rect2i(1646,6146,324,260), cv::Rect2i(-1809,-239,190,152)}},
            {"UI_Map_LayeredMap_3210401.png",{cv::Rect2i(587,3838,384,376), cv::Rect2i(-1533,288,223,219)}},
            {"UI_Map_LayeredMap_3210301.png",{cv::Rect2i(-719,4317,440,420), cv::Rect2i(-1194,495,258,246)}},
            {"UI_Map_LayeredMap_3210201.png",{cv::Rect2i(-2671,5915,352,464), cv::Rect2i(-1139,209,206,271)}},
            {"UI_Map_LayeredMap_3210101.png",{cv::Rect2i(2016,4582,328,180), cv::Rect2i(-1367,454,191,105)}},
            {"UI_Map_LayeredMap_3210102.png",{cv::Rect2i(2016,5302,300,284), cv::Rect2i(-1431,517,174,166)}},
            {"UI_Map_LayeredMap_3200101.png",{cv::Rect2i(587,4934,268,332), cv::Rect2i(-1277,-579,157,194)}},
            {"UI_Map_LayeredMap_3200501.png",{cv::Rect2i(195,3838,392,340), cv::Rect2i(-1326,-763,230,199)}},
            {"UI_Map_LayeredMap_3200201.png",{cv::Rect2i(1350,4210,284,324), cv::Rect2i(-1112,-524,166,190)}},
            {"UI_Map_LayeredMap_3220101.png",{cv::Rect2i(2016,3838,356,292), cv::Rect2i(-1759,56,207,171)}},
            {"UI_Map_LayeredMap_3220102.png",{cv::Rect2i(971,4926,276,252), cv::Rect2i(-1719,77,162,147)}},
            {"UI_Map_LayeredMap_3220103.png",{cv::Rect2i(195,4562,256,348), cv::Rect2i(-1829,-24,149,203)}},
            {"UI_Map_LayeredMap_3220104.png",{cv::Rect2i(1646,4154,364,232), cv::Rect2i(-1767,78,212,136)}},
            {"UI_Map_LayeredMap_3230301.png",{cv::Rect2i(-719,6087,420,336), cv::Rect2i(-2073,232,244,197)}},
            {"UI_Map_LayeredMap_3230101.png",{cv::Rect2i(-719,5847,424,240), cv::Rect2i(-1967,619,248,140)}},
            {"UI_Map_LayeredMap_3230201.png",{cv::Rect2i(1646,5614,324,332), cv::Rect2i(-1784,619,190,194)}},
            {"UI_Map_LayeredMap_3250101.png",{cv::Rect2i(2016,4130,348,200), cv::Rect2i(-2111,-69,204,117)}},
            {"UI_Map_LayeredMap_3250102.png",{cv::Rect2i(-273,3838,468,416), cv::Rect2i(-2206,-66,271,244)}},
            {"UI_Map_LayeredMap_3220201.png",{cv::Rect2i(587,4214,276,380), cv::Rect2i(-2125,-369,162,222)}},


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
        // 层级在地下，但是没有匹配到合适的点
        if (y > 3838 + 3962)
        {
            return { cv::Point(0,0),0 };
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
                                  {keypoint_scene[KNN_m[i][0].trainIdx].pt.x, keypoint_scene[KNN_m[i][0].trainIdx].pt.y}
                        });
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
