#include "pch.h"
#include "SurfMatch.h"
#include "match/type/MatchType.h"
#include "resources/Resources.h"
#include "utils/Utils.h"
#include "algorithms/algorithms.solve.linear.h"


bool judgesIsOnCity(std::vector<TianLi::Utils::MatchKeyPoint> goodMatches, double minimap_scale)
{
    auto get_average = [](const std::vector<double>& v) {
        return std::accumulate(v.begin(), v.end(), 0.0) / (double)v.size();
        };
    auto get_sigma = [](const std::vector<double>& v, const double average) {
        return sqrt(std::accumulate(v.begin(), v.end(), 0.0, [&](const double acc, const double x) {return acc + pow(x - average, 2); }) / (double)v.size());
        };

    std::vector<double> vec_distRatio;
    double distScene, distObject;
    size_t goodMatchesSize = goodMatches.size();

    if (goodMatchesSize < 5)
        return true;			//识别的点数少于5，可以认为在城里

    std::default_random_engine rand_engine;
    cv::Point2d minimap_scaled_pt1, minimap_scaled_pt2;		//缩放修正后的小地图点
    int rand_idx1, rand_idx2;
    for (int i = 0; i < 100; i++)
    {
        rand_idx1 = rand_engine() % goodMatchesSize;
        rand_idx2 = rand_engine() % goodMatchesSize;

        minimap_scaled_pt1 = goodMatches[rand_idx1].query / minimap_scale;
        minimap_scaled_pt2 = goodMatches[rand_idx2].query / minimap_scale;

        distObject = TianLi::Utils::dis(minimap_scaled_pt1 - minimap_scaled_pt2);
        distScene = TianLi::Utils::dis(goodMatches[rand_idx1].train - goodMatches[rand_idx2].train);
        if (isfinite(distScene / distObject) && distScene / distObject != 0)
            vec_distRatio.emplace_back(distScene / distObject);
    }
    //不断剔除异常数据，直到标准差足够小
    double e_distRatio, s_distRatio;
    do
    {
        e_distRatio = get_average(vec_distRatio);
        s_distRatio = get_sigma(vec_distRatio, e_distRatio);
        //剔除误差过大的距离，1倍sigma
        for (int i = 0; i < vec_distRatio.size(); i++)
        {
            if (abs(vec_distRatio[i] - e_distRatio) > 1 * s_distRatio)
            {
                vec_distRatio.erase(vec_distRatio.begin() + i);
                i--;
            }
        }
    } while (s_distRatio > 0.25);

    //重新计算距离比例均值，并判断是否接近城外缩放比例
    e_distRatio = get_average(vec_distRatio);

    if (e_distRatio < 0.6)
        return true;
    return false;
}

std::pair<std::vector<cv::Point2d>, double> judges_scale(std::vector<TianLi::Utils::MatchKeyPoint> match_points, double scale_a, double scale_b)
{
    std::vector<cv::Point2d> scale_a_list;
    std::vector<cv::Point2d> scale_b_list;

    for (auto& points : match_points)
    {
        scale_a_list.push_back(points.query * scale_a + points.train);
        scale_b_list.push_back(points.query * scale_b + points.train);
    }

    if (match_points.size() < 3)
    {
        return std::make_pair(scale_a_list, 1.0);
    }

    double stdev_a = 0;
    double stdev_b = 0;

    stdev_a = TianLi::Utils::stdev(scale_a_list);
    stdev_b = TianLi::Utils::stdev(scale_b_list);

    return stdev_a > stdev_b ? std::make_pair(scale_a_list, stdev_a) : std::make_pair(scale_a_list, stdev_a);
}


void SurfMatch::setMap_Dbg(cv::Mat gi_map)
{
    _mapMat = gi_map;
}

void SurfMatch::setMiniMap(cv::Mat miniMapMat)
{
    _miniMapMat = miniMapMat;
}

void SurfMatch::Init(std::shared_ptr<trackCache::CacheInfo> cache_info)
{
    if (isInit)return;
    map.keypoints = std::move(cache_info->key_points);
    map.descriptors = std::move(cache_info->descriptors);

    double hessian_threshold = cache_info->setting.hessian_threshold;
    int octave = cache_info->setting.octave;
    int octave_layers = cache_info->setting.octave_layers;
    bool extended = cache_info->setting.extended;
    bool upright = cache_info->setting.up_right;
    matcher = Match(hessian_threshold, octave, octave_layers, extended, upright);
    isInit = true;
}

void SurfMatch::UnInit()
{
    if (!isInit)return;
    _mapMat.release();
    _mapMat = cv::Mat();
    map.keypoints.clear();
    map.descriptors.release();
    isInit = false;
}

void SurfMatch::match()
{
    bool calc_is_faile = false;
    is_success_match = false;
    pos = match_no_continuity(calc_is_faile);
    is_success_match = !calc_is_faile;
}



/// <summary>
/// 匹配 不连续 全局匹配
/// </summary>
/// <param name="calc_is_faile"></param>
/// <returns></returns>
cv::Point2d SurfMatch::match_no_continuity(bool& calc_is_faile)
{
    return match_no_continuity_1st(calc_is_faile);
}

cv::Point2d match_all_map(Match& matcher, const cv::Mat& map, const cv::Mat& mini_map, Match::KeyMatPoint& query, Match::KeyMatPoint& train, bool& calc_is_faile, double& stdev, double minimap_scale_param = 1.0);

/// <summary>
/// 非连续匹配，从大地图中确定角色位置
/// 直接通过SURF特征点匹配精确定位角色位置
/// </summary>
/// <param name="calc_is_faile">匹配结果是否有效</param>
/// <returns></returns>
cv::Point2d SurfMatch::match_no_continuity_1st(bool& calc_is_faile)
{
    double match_stdev = 0.0;
    return match_all_map(matcher, _mapMat, _miniMapMat, mini_map, map, calc_is_faile, match_stdev, 1.0);
}

cv::Point2d match_all_map(Match& matcher, const cv::Mat& map_mat, const cv::Mat& mini_map_mat, Match::KeyMatPoint& mini_map, Match::KeyMatPoint& map, bool& calc_is_faile, double& stdev, double minimap_scale_param)
{
    cv::Point2d map_pos;

    cv::Mat img_object = TianLi::Utils::crop_border(mini_map_mat, 0.15);
    cv::resize(img_object, img_object, cv::Size(0, 0), minimap_scale_param, minimap_scale_param, cv::INTER_CUBIC);

    // 小地图区域计算特征点
#ifdef _DEBUG
    auto beg_time = std::chrono::steady_clock::now();
#endif
    matcher.detect_and_compute(img_object, mini_map);
#ifdef _DEBUG
    auto end_time = std::chrono::steady_clock::now();
    auto cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count();
    // std::cout<<"detect_and_compute time cost: " << cost_time << " ms" << std::endl;
#endif
    // 没有提取到特征点直接返回，结果无效
    if (mini_map.keypoints.size() == 0)
    {
        calc_is_faile = true;
        return map_pos;
    }
    // 匹配特征点
#ifdef _DEBUG
    beg_time = std::chrono::steady_clock::now();
#endif
    std::vector<std::vector<cv::DMatch>> KNN_m = matcher.match(mini_map, map);
#ifdef _DEBUG
    end_time = std::chrono::steady_clock::now();
    cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count();
    // std::cout << "match time cost: " << cost_time << " ms" << std::endl;
#endif

    std::vector<TianLi::Utils::MatchKeyPoint> keypoint_list;
    TianLi::Utils::calc_good_matches(map_mat, map.keypoints, img_object, mini_map.keypoints, KNN_m, SURF_MATCH_RATIO_THRESH, keypoint_list);

    if (keypoint_list.size() == 0)
    {
        calc_is_faile = true;
        return map_pos;
    }

    auto scale_mini2map = 1.3 / minimap_scale_param;
#ifdef USE_LINEAR_SOLVE_SCALE

    if (keypoint_list.size() < 2) {
        calc_is_faile = true;
        return map_pos;
    }
    // keypoint_list: 角色中心小地图坐标下的特征点 -> 大地图坐标下的特征点
    // 使用基于线性方程的方法，求解s、dx、dy自由度的仿射变换 ^W_A T
    double s, dx, dy;
    std::vector<cv::Point2f> src, dst;
    for (auto& keypoint : keypoint_list)
    {
        src.push_back(keypoint.query);
        dst.push_back(keypoint.train);
    }
    auto liear_valid = solve_linear_s_dx_dy(src, dst, s, dx, dy);
    if (!liear_valid)
    {
        goto old_style;
        // calc_is_faile = true;
        // return map_pos;
    }
    scale_mini2map = s;

    #ifdef _FULL_LINEAR_SOLVE
    {
        // 组装 ^W_A T
        cv::Mat affine_mat = cv::Mat::eye(3, 3, CV_64F);
        affine_mat.at<double>(0, 0) = s;
        affine_mat.at<double>(0, 2) = dx;
        affine_mat.at<double>(1, 1) = s;
        affine_mat.at<double>(1, 2) = dy;

        cv::Mat A_origin_in_coor_A = cv::Mat::zeros(3, 1, CV_64F);
        A_origin_in_coor_A.at<double>(0, 2) = 1.0;
        cv::Mat A_origin_in_coor_M = affine_mat * A_origin_in_coor_A;
        return cv::Point2d(A_origin_in_coor_M.at<double>(0, 0), A_origin_in_coor_M.at<double>(1, 0));
    }
    #endif

#endif
old_style:
    std::vector<double> lisx;
    std::vector<double> lisy;
    TianLi::Utils::RemoveKeypointOffset(keypoint_list, 1.3 / minimap_scale_param, lisx, lisy);

    std::vector<cv::Point2d> list_point;
    for (int i = 0; i < keypoint_list.size(); i++)
    {
        list_point.push_back(cv::Point2d(lisx[i], lisy[i]));
    }
    list_point = TianLi::Utils::extract_valid(list_point);

    lisx.clear();
    lisy.clear();
    for (int i = 0; i < list_point.size(); i++)
    {
        lisx.push_back(list_point[i].x);
        lisy.push_back(list_point[i].y);
    }

    double x_stdev = TianLi::Utils::stdev_abs(lisx);
    double y_stdev = TianLi::Utils::stdev_abs(lisy);

    stdev = sqrt(x_stdev + y_stdev);

    // 没有最佳匹配结果直接返回，结果无效
    if (std::min(lisx.size(), lisy.size()) == 0)
    {
        calc_is_faile = true;
        return map_pos;
    }
    // 从最佳匹配结果中剔除异常点计算角色位置返回
    if (!TianLi::Utils::SPC(lisx, lisy, map_pos))
    {
        calc_is_faile = true;
        return map_pos;
    }
    return map_pos;
}

#ifdef _DELETE
/// <summary>
/// 匹配 在城镇内
/// </summary>
/// <param name="calc_continuity_is_faile"></param>
/// <returns></returns>
cv::Point2d SurfMatch::match_continuity_on_city(bool& calc_continuity_is_faile)
{
    static cv::Mat img_scene(_mapMat);
    //const auto minimap_scale_param = 2.0;

    cv::Point2d pos_on_city;

    cv::Mat img_object = TianLi::Utils::crop_border(_miniMapMat, 0.15);

    cv::Mat someMap = TianLi::Utils::get_some_map(img_scene, pos, DEFAULT_SOME_MAP_SIZE_R);
    cv::Mat miniMap(img_object);

    cv::resize(someMap, someMap, cv::Size(), 2.0, 2.0, cv::INTER_CUBIC);
    //resize(miniMap, miniMap, cv::Size(0, 0), minimap_scale_param, minimap_scale_param, cv::INTER_CUBIC);

    matcher.detect_and_compute(someMap, some_map);
    matcher.detect_and_compute(miniMap, mini_map);

    // 如果搜索范围内可识别特征点数量少于2，则认为计算失败
    if (some_map.size() <= 2 || mini_map.size() <= 2)
    {
        calc_continuity_is_faile = true;
        return pos_on_city;
    }

    std::vector<std::vector<cv::DMatch>> KNN_mTmp = matcher.match(mini_map, some_map);

    std::vector<TianLi::Utils::MatchKeyPoint> keypoint_on_city_list;
    TianLi::Utils::calc_good_matches(someMap, some_map.keypoints, img_object, mini_map.keypoints, KNN_mTmp, SURF_MATCH_RATIO_THRESH, keypoint_on_city_list);

    std::vector<double> lisx;
    std::vector<double> lisy;
    TianLi::Utils::RemoveKeypointOffset(keypoint_on_city_list, 0.8667, lisx, lisy);


    std::vector<cv::Point2d> list_on_city;
    for (int i = 0; i < keypoint_on_city_list.size(); i++)
    {
        list_on_city.push_back(cv::Point2d(lisx[i], lisy[i]));
    }
    list_on_city = TianLi::Utils::extract_valid(list_on_city);

    lisx.clear();
    lisy.clear();
    for (int i = 0; i < list_on_city.size(); i++)
    {
        lisx.push_back(list_on_city[i].x);
        lisy.push_back(list_on_city[i].y);
    }

    if (std::max(lisx.size(), lisy.size()) <= NOT_ON_CITY__ON_CITY_MIN_GOODMATCHS)
    {
        calc_continuity_is_faile = true;
        return pos_on_city;
    }

    //// 根据匹配点的方差判断点集是否合理，偏差过大即为无效数据
    //if (TianLi::Utils::is_valid_keypoints(lisx, sumx, lisy, sumy, DEFAULT_SOME_MAP_SIZE_R/4.0) == false)
    //{
    //	calc_continuity_is_faile = true;
    //	return pos_on_city;
    //}

    isOnCity = judgesIsOnCity(keypoint_on_city_list, 0.5);		//大地图放大了2倍，所以小地图坐标也要这样处理

    cv::Point2d pos_continuity_on_city;

    if (!TianLi::Utils::SPC(lisx, lisy, pos_continuity_on_city))
    {
        calc_continuity_is_faile = true;
        return pos_continuity_on_city;
    }

    pos_continuity_on_city.x = (pos_continuity_on_city.x - someMap.cols / 2.0) / 2.0;
    pos_continuity_on_city.y = (pos_continuity_on_city.y - someMap.rows / 2.0) / 2.0;

    pos_on_city = cv::Point2d(pos_continuity_on_city.x + pos.x, pos_continuity_on_city.y + pos.y);

    return pos_on_city;
}
/// <summary>
/// 匹配 在野外
/// </summary>
/// <param name="calc_continuity_is_faile"></param>
/// <returns></returns>
cv::Point2d SurfMatch::match_continuity_not_on_city(bool& calc_continuity_is_faile)
{
    static cv::Mat img_scene(_mapMat);
    const auto minimap_scale_param = 2.0;
    int real_some_map_size_r = DEFAULT_SOME_MAP_SIZE_R;

    cv::Point2d pos_not_on_city;

    cv::Mat img_object = TianLi::Utils::crop_border(_miniMapMat, 0.15);
    //不在城镇中时
    cv::Mat someMap = TianLi::Utils::get_some_map(img_scene, pos, DEFAULT_SOME_MAP_SIZE_R);
    cv::Mat miniMap(img_object);
    cv::Mat miniMap_scale = img_object.clone();

    cv::resize(miniMap_scale, miniMap_scale, cv::Size(0, 0), minimap_scale_param, minimap_scale_param, cv::INTER_CUBIC);

    matcher.detect_and_compute(someMap, some_map);
    matcher.detect_and_compute(miniMap_scale, mini_map);

    // 如果搜索范围内可识别特征点数量少于2，则认为计算失败
    if (some_map.size() <= 2 || mini_map.size() <= 2)
    {
        calc_continuity_is_faile = true;
        return pos_not_on_city;
    }

    std::vector<std::vector<cv::DMatch>> KNN_not_no_city = matcher.match(mini_map, some_map);


    std::vector<TianLi::Utils::MatchKeyPoint> keypoint_not_on_city_list;
    TianLi::Utils::calc_good_matches(someMap, some_map.keypoints, miniMap_scale, mini_map.keypoints, KNN_not_no_city, SURF_MATCH_RATIO_THRESH, keypoint_not_on_city_list);

    // auto t = judges_scale(keypoint_not_on_city_list, MAP_BOTH_SCALE_RATE / minimap_scale_param, 0.8667);

    std::vector<double> lisx;
    std::vector<double> lisy;
    TianLi::Utils::RemoveKeypointOffset(keypoint_not_on_city_list, MAP_BOTH_SCALE_RATE / minimap_scale_param, lisx, lisy);

    std::vector<cv::Point2d> list_not_on_city;
    for (int i = 0; i < keypoint_not_on_city_list.size(); i++)
    {
        list_not_on_city.push_back(cv::Point2d(lisx[i], lisy[i]));
    }
    list_not_on_city = TianLi::Utils::extract_valid(list_not_on_city);

    lisx.clear();
    lisy.clear();
    for (int i = 0; i < list_not_on_city.size(); i++)
    {
        lisx.push_back(list_not_on_city[i].x);
        lisy.push_back(list_not_on_city[i].y);
    }


    if (!judgesIsOnCity(keypoint_not_on_city_list, MAP_BOTH_SCALE_RATE))
    {
        isOnCity = false;
        cv::Point2d p;
        if (!TianLi::Utils::SPC(lisx, lisy, p))
        {
            calc_continuity_is_faile = true;
            return pos_not_on_city;
        }
        pos_not_on_city = cv::Point2d(p.x + pos.x - real_some_map_size_r, p.y + pos.y - real_some_map_size_r);
        return pos_not_on_city;
    }

    cv::Point2d pos_on_city;
    someMap = TianLi::Utils::get_some_map(img_scene, pos, DEFAULT_SOME_MAP_SIZE_R);
    cv::resize(someMap, someMap, cv::Size(), 2.0, 2.0, cv::INTER_CUBIC);

    matcher.detect_and_compute(someMap, some_map);
    matcher.detect_and_compute(miniMap, mini_map);

    // 如果搜索范围内可识别特征点数量少于2，则认为计算失败
    if (some_map.size() <= 2 || mini_map.size() <= 2)
    {
        calc_continuity_is_faile = true;
        return pos_not_on_city;
    }

    std::vector<std::vector<cv::DMatch>> KNN_mabye_on_city = matcher.match(mini_map, some_map);

    std::vector<TianLi::Utils::MatchKeyPoint> keypoint_on_city_list;
    TianLi::Utils::calc_good_matches(someMap, some_map.keypoints, miniMap, mini_map.keypoints, KNN_mabye_on_city, SURF_MATCH_RATIO_THRESH, keypoint_on_city_list);

    std::vector<double> list_x_on_city;
    std::vector<double> list_y_on_city;
    TianLi::Utils::RemoveKeypointOffset(keypoint_on_city_list, 0.8667, list_x_on_city, list_y_on_city);

    std::vector<cv::Point2d> list_on_city;
    for (int i = 0; i < keypoint_on_city_list.size(); i++)
    {
        list_on_city.push_back(cv::Point2d(list_x_on_city[i], list_y_on_city[i]));
    }
    list_on_city = TianLi::Utils::extract_valid(list_on_city);

    list_x_on_city.clear();
    list_y_on_city.clear();
    for (int i = 0; i < list_on_city.size(); i++)
    {
        list_x_on_city.push_back(list_on_city[i].x);
        list_y_on_city.push_back(list_on_city[i].y);
    }

    if (std::min(list_x_on_city.size(), list_y_on_city.size()) <= NOT_ON_CITY__ON_CITY_MIN_GOODMATCHS)
    {
        calc_continuity_is_faile = true;
        return pos_not_on_city;
    }

    isOnCity = judgesIsOnCity(keypoint_on_city_list, 0.5);

    cv::Point2d p;
    if (!TianLi::Utils::SPC(list_x_on_city, list_y_on_city, p))
    {
        calc_continuity_is_faile = true;
        return pos_on_city;
    }

    double x = (p.x - someMap.cols / 2.0) / 2.0;
    double y = (p.y - someMap.rows / 2.0) / 2.0;

    pos_on_city = cv::Point2d(x + pos.x, y + pos.y);
    return pos_on_city;
    }

#endif // _DELETE

cv::Point2d SurfMatch::getLocalPos()
{
    return pos;
}

bool SurfMatch::getIsContinuity()
{
    return isContinuity;
}

Match::Match(double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
    detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
    //matcher  = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
}

std::vector<std::vector<cv::DMatch>> Match::match(const cv::Mat& query_descriptors, const cv::Mat& train_descriptors)
{
    std::vector<std::vector<cv::DMatch>> match_group;
    matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
    matcher->knnMatch(query_descriptors, train_descriptors, match_group, 2);
    return match_group;
}

std::vector<std::vector<cv::DMatch>> Match::match(KeyMatPoint& query_key_mat_point, KeyMatPoint& train_key_mat_point)
{
    return match(query_key_mat_point.descriptors, train_key_mat_point.descriptors);
}

bool Match::detect_and_compute(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors)
{
    if (img.empty()) return  false;
    detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
    if (keypoints.size() == 0) return false;
    return true;
}

bool Match::detect_and_compute(const cv::Mat& img, Match::KeyMatPoint& key_mat_point)
{
    return detect_and_compute(img, key_mat_point.keypoints, key_mat_point.descriptors);
}

