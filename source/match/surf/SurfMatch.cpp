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
    int octave = cache_info->setting.octaves;
    int octave_layers = cache_info->setting.octave_layers;
    bool extended = cache_info->setting.extended;
    bool upright = cache_info->setting.up_right;
    matcher = Match(hessian_threshold, octave, octave_layers, extended, upright);
    // 
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
    // _mapMat = cv::Mat::zeros(2304, 1740, CV_8UC3);
    pos = match_no_continuity(calc_is_faile);

    // pos = match_ransac(calc_is_faile, cv::Mat(), 1000);
    is_success_match = !calc_is_faile;
}



cv::Point2d SurfMatch::match_ransac(bool& calc_is_faile, cv::Mat& affine_mat_out, 
                                    int max_iter_num) 
{
    // 同样的，首先获得一组包含外点的匹配

    // make the same name to copy the code 
    auto& mather = this->matcher;
    auto& map_mat = this->_mapMat;
    auto& mini_map_mat = this->_miniMapMat;
    auto& map = this->map;

    
    // return value
    cv::Point2d map_pos;

    // 提取小地图特征点
    cv::Mat img_object = TianLi::Utils::crop_border(mini_map_mat, 0.15);
    matcher.detect_and_compute(img_object, mini_map);
    if (mini_map.keypoints.size() == 0)
    {
        calc_is_faile = true;
        return map_pos;
    }

    // 通过最优比次优剔除部分较差的匹配点
    // MD 筛完了，把阈值调小
    std::vector<std::vector<cv::DMatch>> KNN_m = matcher.match(mini_map, map);
    std::vector<TianLi::Utils::MatchKeyPoint> keypoint_list;
    std::vector<cv::DMatch> keypoint_list_dmatch;
    TianLi::Utils::calc_good_matches(map_mat, map.keypoints, img_object, mini_map.keypoints, KNN_m, 0.825, keypoint_list, keypoint_list_dmatch);
#ifdef _DEBUG
    std::cout<<"keypoint_list.size(): "<<keypoint_list.size()<<std::endl;
#endif
    if (keypoint_list.size() == 0 || keypoint_list.size() < 4) // add: < 3 constraint
    {
        calc_is_faile = true;
        return map_pos;
    }

    // 然后请出RANSAC剔除外点
    // 这里需要自行实现仅有缩放和平移的仿射变换的ransac

    // 稍微处理一下名字以适应 openvslam 超来的代码
    
    auto best_score_ = 0.0;
    auto& best_H_21_ = affine_mat_out;
    const auto num_matches = static_cast<unsigned int>(keypoint_list.size());
    std::vector<cv::KeyPoint> undist_keypts_1, undist_keypts_2;
    undist_keypts_1.reserve(num_matches);
    undist_keypts_2.reserve(num_matches);
    for (unsigned int i = 0; i < num_matches; ++i) {
        cv::KeyPoint t_p1 = cv::KeyPoint(keypoint_list.at(i).query, 1.0);
        cv::KeyPoint t_p2 = cv::KeyPoint(keypoint_list.at(i).train, 1.0);
        undist_keypts_1.push_back(t_p1);
        undist_keypts_2.push_back(t_p2);
    }


    // 0. Normalize keypoint coordinates
    std::vector<cv::Point2d> normalized_keypts_1, normalized_keypts_2;
    cv::Mat transform_1, transform_2;
    TianLi::Utils::normalize(undist_keypts_1, normalized_keypts_1, transform_1);
    TianLi::Utils::normalize(undist_keypts_2, normalized_keypts_2, transform_2);
    cv::Mat transform_2_inv = transform_2.inv();


    // 1. Prepare for RANSAC
    auto matches_12 = keypoint_list;

    constexpr unsigned int min_set_size = 2; // homography 只需要4个点，openvslam用了8个点；这里也+1
    // RANSAC variables
    best_score_ = 0.0;
    std::vector<bool> is_inlier_match_;


    // 用于求解的最小集合
    std::vector<cv::Point2d> min_set_keypts_1(min_set_size);
    std::vector<cv::Point2d> min_set_keypts_2(min_set_size);
    
    // shared var in ransac loop
    // homography matrix from shot 1 to shot 2, and
    // homography matrix from shot 2 to shot 1
    cv::Mat H_21_in_sac, H_12_in_sac;
    // inlier/outlier flags
    std::vector<bool> is_inlier_match_in_sac(num_matches, true);
    // score of homography matrix
    double score_in_sac;

    // 2. RANSAC loop

    for (unsigned int i = 0; i < max_iter_num; ++i) { // 好好好，用++i
        // 2-1. 获取最小集合，这里是两个点
        const auto idxs = TianLi::Utils::create_random_array(min_set_size, 0U, num_matches-1);
        for (unsigned int j = 0; j < min_set_size; ++j) {
            min_set_keypts_1.at(j) = normalized_keypts_1.at(idxs.at(j));
            min_set_keypts_2.at(j) = normalized_keypts_2.at(idxs.at(j));
        }

        // 2-2. 求解仿射变换
        cv::Mat normalized_H_21 = cv::Mat::eye(3, 3, CV_64F);
        double s_, dx_, dy_;
        solve_linear_s_dx_dy(min_set_keypts_1, min_set_keypts_2, s_, dx_, dy_);
        // std::cout<<"s_: "<<s_<<" dx_: "<<dx_<<" dy_: "<<dy_<<std::endl;
        normalized_H_21.at<double>(0, 0) = s_;
        normalized_H_21.at<double>(0, 2) = dx_;
        normalized_H_21.at<double>(1, 1) = s_;
        normalized_H_21.at<double>(1, 2) = dy_;
        // as types

        H_21_in_sac = transform_2_inv * normalized_H_21 * transform_1;

        // 2-3. 计算内点以及score
        score_in_sac = check_inliers(
            H_21_in_sac, is_inlier_match_in_sac,
            undist_keypts_1, undist_keypts_2,
            matches_12
        );
        // std::cout<<"score_in_sac: "<<score_in_sac<<std::endl;
        if (best_score_ < score_in_sac) {
            best_score_ = score_in_sac;
            is_inlier_match_ = is_inlier_match_in_sac;
        }
    }
    // std::cout<<"best_score_: "<<best_score_<<" is_inlier_match_.size(): "<<is_inlier_match_.size()<<std::endl;
    auto num_inliers = std::count(is_inlier_match_.begin(), is_inlier_match_.end(), true);
    auto solution_is_valid_ = (best_score_ > 0.0) && (num_inliers >= min_set_size); // the negative side of calc_is_faile
#ifdef _DEBUG
    std::cout<<"solution_is_valid_: "<<solution_is_valid_<<"inliers: "<<num_inliers<<std::endl;
#endif

    if (!solution_is_valid_) {
        calc_is_faile = true;
        return map_pos;
    }

    // 3. Recompute a homography matrix only with the inlier matches
    std::vector<cv::Point2d> inlier_normalized_keypts_1;
    std::vector<cv::Point2d> inlier_normalized_keypts_2;
    inlier_normalized_keypts_1.reserve(matches_12.size());
    inlier_normalized_keypts_2.reserve(matches_12.size());
    for (unsigned int i = 0; i < num_matches; ++i) {
        if (!is_inlier_match_.at(i)) {
            continue;
        }
        inlier_normalized_keypts_1.push_back(normalized_keypts_1.at(i));
        inlier_normalized_keypts_2.push_back(normalized_keypts_2.at(i));
    }

    cv::Mat normalized_H_21 = cv::Mat::eye(3, 3, CV_64F);
    double s_, dx_, dy_;
    solve_linear_s_dx_dy(inlier_normalized_keypts_1, inlier_normalized_keypts_2, s_, dx_, dy_);
    normalized_H_21.at<double>(0, 0) = s_;
    normalized_H_21.at<double>(0, 2) = dx_;
    normalized_H_21.at<double>(1, 1) = s_;
    normalized_H_21.at<double>(1, 2) = dy_;
    best_H_21_ = transform_2_inv * normalized_H_21 * transform_1;
    // best_score_ = check_inliers(best_H_21_, is_inlier_match_);

    // 抄完了，该计算offset了
    // H_21 实际上是 ^W_A T, 即 对于特征点P, 有 ^W P = T dot ^A P
    // 为了得到坐标系A原点在世界坐标的位置，
    // 实际就是变换矩阵的最后一列，即 T = [dx, dy, 1]^T

    map_pos.x = best_H_21_.at<double>(0, 2);
    map_pos.y = best_H_21_.at<double>(1, 2);
    calc_is_faile = false;

    return map_pos;

}

double SurfMatch::check_inliers(
    cv::Mat& H_21, std::vector<bool>& is_inlier_match,
    std::vector<cv::KeyPoint>& undist_keypts_1, std::vector<cv::KeyPoint>& undist_keypts_2,
    std::vector<TianLi::Utils::MatchKeyPoint>& matches_12) {
    const auto num_matches = matches_12.size();
    double sigma_ = 1.0f; // just hard code it, same as openvslam

    // chi-squared value (p=0.05, n=2)
    constexpr double chi_sq_thr = 5.9915;

    is_inlier_match.resize(num_matches);

    const cv::Mat H_12 = H_21.inv();

    double score = 0.0f;

    const double inv_sigma_sq = 1.0 / (sigma_ * sigma_);

    for (unsigned int i = 0; i < num_matches; ++i) {
        // const auto& keypt_1 = undist_keypts_1.at(matches_12.at(i).first);
        // const auto& keypt_2 = undist_keypts_2.at(matches_12.at(i).second);
        const auto& keypt_1 = matches_12.at(i).query;
        const auto& keypt_2 = matches_12.at(i).train;

        // 1. Transform to homogeneous coordinates

        auto pt_1 = TianLi::Utils::to_homogeneous(keypt_1);
        auto pt_2 = TianLi::Utils::to_homogeneous(keypt_2);

        // 2. Compute symmetric transfer error

        // 2-1. Transform a point in shot 1 to the epipolar line in shot 2,
        //      then compute a transfer error (= dot product)

        cv::Mat transformed_pt_1 = H_21 * pt_1;
        transformed_pt_1 = transformed_pt_1 / transformed_pt_1.at<double>(2);

        cv::Mat delta_ = transformed_pt_1 - pt_2;
        auto squared_norm = delta_.dot(delta_);
        const double dist_sq_1 = squared_norm;

        // standardization
        const double chi_sq_1 = dist_sq_1 * inv_sigma_sq;

        // if a match is inlier, accumulate the score
        if (chi_sq_thr < chi_sq_1) {
            is_inlier_match.at(i) = false;
            continue;
        }
        else {
            is_inlier_match.at(i) = true;
            score += chi_sq_thr - chi_sq_1;
        }

        // 2. Transform a point in shot 2 to the epipolar line in shot 1,
        //    then compute a transfer error (= dot product)

        cv::Mat transformed_pt_2 = H_12 * pt_2;
        transformed_pt_2 = transformed_pt_2 / transformed_pt_2.at<double>(2);

        delta_ = transformed_pt_2 - pt_1;
        squared_norm = delta_.dot(delta_);
        const double dist_sq_2 = squared_norm;

        // standardization
        const double chi_sq_2 = dist_sq_2 * inv_sigma_sq;

        // if a match is inlier, accumulate the score
        if (chi_sq_thr < chi_sq_2) {
            is_inlier_match.at(i) = false;
            continue;
        }
        else {
            is_inlier_match.at(i) = true;
            score += chi_sq_thr - chi_sq_2;
        }
    }

    return score;
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
    auto beg_time = std::chrono::steady_clock::now();
    matcher.detect_and_compute(img_object, mini_map);
    auto end_time = std::chrono::steady_clock::now();
    auto cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count();
    // std::cout<<"detect_and_compute time cost: " << cost_time << " ms" << std::endl;
    // 没有提取到特征点直接返回，结果无效
    if (mini_map.keypoints.size() == 0)
    {
        calc_is_faile = true;
        return map_pos;
    }
    // 匹配特征点
    beg_time = std::chrono::steady_clock::now();
    std::vector<std::vector<cv::DMatch>> KNN_m = matcher.match(mini_map, map);
    end_time = std::chrono::steady_clock::now();
    cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count();
    // std::cout << "match time cost: " << cost_time << " ms" << std::endl;

    std::vector<TianLi::Utils::MatchKeyPoint> keypoint_list;
    std::vector<cv::DMatch> keypoint_list_dmatch;
    TianLi::Utils::calc_good_matches(map_mat, map.keypoints, img_object, mini_map.keypoints, KNN_m, SURF_MATCH_RATIO_THRESH, keypoint_list, keypoint_list_dmatch);

    if (keypoint_list.size() < 2)
    {
        calc_is_faile = true;
        return map_pos;
    }

    // 添加通过求解线性方程组的方法求解缩放+平移
    // 通过缩放判断是否在城镇内
    double scale_mini2map, dx_mini2map, dy_mini2map;
    // make two point set
    std::vector<cv::Point2d> mini_map_points;
    std::vector<cv::Point2d> map_points;
    for (auto& point : keypoint_list)
    {
        mini_map_points.push_back(point.query);
        map_points.push_back(point.train);
    }
    solve_linear_s_dx_dy(mini_map_points, map_points, scale_mini2map, dx_mini2map, dy_mini2map);
    
    // 难绷，小地图坐标系是反的
    scale_mini2map = - scale_mini2map;
    dx_mini2map = - dx_mini2map;
    dy_mini2map = - dy_mini2map;

    // TODO: 按照scale 比例判断是否在城镇内

    // std::cout<<"keypoint_list.size(): "<<keypoint_list.size()<<std::endl;
    // std::cout<<"s: " << std::to_string(scale_mini2map) << " dx: " << std::to_string(dx_mini2map) << " dy: " << std::to_string(dy_mini2map) << std::endl;
    
    // 绘制匹配结果
    // cv::Mat img_matches;
    // cv::drawMatches(img_object, mini_map.keypoints, map_mat, map.keypoints, keypoint_list_dmatch, img_matches);
    // cv::imwrite("match.jpg", img_matches);
    // exit(0);
    // double scale_mini2map = 1.3 / minimap_scale_param;
    // std::cout<<"scale_mini2map: "<<scale_mini2map<<std::endl;



    std::vector<double> lisx;
    std::vector<double> lisy;
    TianLi::Utils::RemoveKeypointOffset(keypoint_list, 1.3 / minimap_scale_param, lisx, lisy);

    std::vector<cv::Point2d> list_point;
    for (int i = 0; i < keypoint_list.size(); i++)
    {
        list_point.push_back(cv::Point2d(lisx[i], lisy[i]));
    }
    list_point = TianLi::Utils::extract_valid(list_point);

    lisx.reserve(list_point.size());
    lisy.reserve(list_point.size());
    for (int i = 0; i < list_point.size(); i++)
    {
        lisx[i] = list_point[i].x;
        lisy[i] = list_point[i].y;
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

