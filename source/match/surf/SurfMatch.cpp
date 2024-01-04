#include "pch.h"
#include "SurfMatch.h"
#include "global/global.genshin.h"
#include "resources/Resources.h"
#include "utils/Utils.h"
#include "algorithms/algorithms.solve.linear.h"
#include "algorithms/features/features.operate.h"

using namespace tianli::algorithms;

void SurfMatch::Init(std::shared_ptr<trackCache::CacheInfo> cache_info)
{
    if (isInit)return;
    this->cache_info = cache_info;
    all_map_features = features(cache_info->key_points, cache_info->descriptors);

    //如果图像非空，则设定调试图像
    if (Resources::getInstance().DbgMap.empty())
    {
        _mapMat = Resources::getInstance().DbgMap;
    }

    double hessian_threshold = cache_info->setting.hessian_threshold;
    //double hessian_threshold = 1.0;
    int octave = cache_info->setting.octaves;
    int octave_layers = cache_info->setting.octave_layers;
    //int octave_layers = 1;
    bool extended = cache_info->setting.extended;
    bool upright = cache_info->setting.up_right;
    matcher = Match(hessian_threshold, octave, octave_layers, extended, upright);
    // 
    isInit = true;
}

void SurfMatch::UnInit()
{
    if (!isInit)return;
    all_map_features.keypoints.clear();
    all_map_features.descriptors.release();
    isInit = false;
}

bool SurfMatch::GetNextPosition(cv::Mat mini_map, cv::Point2d& position)
{
    bool calc_is_faile = false;
    is_success_match = false;
    pos = match_all_map(matcher, mini_map,mini_map_features,all_map_features,calc_is_faile);
    //pos = match_ransac(calc_is_faile, cv::Mat(), 1000);
    is_success_match = !calc_is_faile;
    return calc_is_faile;
}

/*

cv::Point2d SurfMatch::match_ransac(bool &calc_is_faile, cv::Mat &affine_mat_out,
    int max_iter_num)
{
    // 同样的，首先获得一组包含外点的匹配

    // make the same name to copy the code 
    auto &mather = this->matcher;
    auto &map_mat = this->_mapMat;
    auto &mini_map_mat = this->_miniMapMat;
    auto &map = this->all_map_features;


    // return value
    cv::Point2d map_pos;

    // 提取小地图特征点
    cv::Mat img_object = TianLi::Utils::crop_border(mini_map_mat, 0.15);
    matcher.detect_and_compute(img_object, mini_map_features);
    if (mini_map_features.keypoints.size() == 0)
    {
        calc_is_faile = true;
        return map_pos;
    }

    // 通过最优比次优剔除部分较差的匹配点
    // MD 筛完了，把阈值调小
    std::vector<std::vector<cv::DMatch>> KNN_m = matcher.match(mini_map_features, map);
    std::vector<TianLi::Utils::MatchKeyPoint> keypoint_list;
    std::vector<cv::DMatch> keypoint_list_dmatch;
    calc_good_matches(map_mat, map.keypoints, img_object, mini_map_features.keypoints, KNN_m, 0.825, keypoint_list, keypoint_list_dmatch);
#ifdef _DEBUG
    std::cout << "keypoint_list.size(): " << keypoint_list.size() << std::endl;
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
    auto &best_H_21_ = affine_mat_out;
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
        const auto idxs = TianLi::Utils::create_random_array(min_set_size, 0U, num_matches - 1);
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
    std::cout << "solution_is_valid_: " << solution_is_valid_ << "inliers: " << num_inliers << std::endl;
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
    cv::Mat &H_21, std::vector<bool> &is_inlier_match,
    std::vector<cv::KeyPoint> &undist_keypts_1, std::vector<cv::KeyPoint> &undist_keypts_2,
    std::vector<TianLi::Utils::MatchKeyPoint> &matches_12) {
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
        const auto &keypt_1 = matches_12.at(i).query;
        const auto &keypt_2 = matches_12.at(i).train;

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
*/

/// <summary>
/// 非连续匹配，从大地图中确定角色位置
/// 直接通过SURF特征点匹配精确定位角色位置
/// </summary>
/// <param name="calc_is_faile">匹配结果是否有效</param>
/// <returns></returns>
cv::Point2d SurfMatch::match_all_map(Match &matcher, const cv::Mat &mini_map_mat, features &mini_map, features &map, bool &calc_is_faile)
{
    cv::Point2d map_pos;

    // 小地图区域计算特征点
    auto beg_time = std::chrono::steady_clock::now();
    matcher.detect_and_compute(mini_map_mat, mini_map);
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

    features translated_map_feature = features_operate::TransferAxes(map, cache_info->setting.roi, cv::Rect(cv::Point(), Resources::getInstance().DbgMap.size()));
    draw_matched_keypoints(Resources::getInstance().DbgMap, translated_map_feature.keypoints, mini_map_mat, mini_map.keypoints, KNN_m);

    // 获取最佳匹配点，尽量一次就筛选出正确的匹配点对
    std::vector<MatchKeyPoint> keypoint_list;
    std::vector<cv::DMatch> keypoint_list_dmatch;
    calc_good_matches(map.keypoints, mini_map.keypoints, KNN_m, keypoint_list, keypoint_list_dmatch);

    // 绘制匹配结果
    draw_matched_keypoints(Resources::getInstance().DbgMap, translated_map_feature.keypoints, mini_map_mat, mini_map.keypoints, keypoint_list_dmatch);

    if (keypoint_list.size() < 2)
    {
        calc_is_faile = true;
        return map_pos;
    }


    std::vector<double> lisx;
    std::vector<double> lisy;
    //TianLi::Utils::RemoveKeypointOffset(keypoint_list, 1.3 / 1.0, lisx, lisy);

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

    float stdev = sqrt(x_stdev + y_stdev);

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

cv::Point2d SurfMatch::getCurrentPosition()
{
    return pos;
}

bool SurfMatch::getIsContinuity()
{
    return isContinuity;
}

void SurfMatch::draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint>& keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint>& keypoint_object, const std::vector<std::vector<cv::DMatch>>&d_matches)
{
    cv::Mat img_matches, imgmap, imgminmap;
    drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1));
    drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1));
    drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, d_matches,img_matches,
        cv::Scalar::all(-1),cv::Scalar::all(-1),std::vector<std::vector<char>>(),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

void SurfMatch::draw_matched_keypoints(const cv::Mat &img_scene, const std::vector<cv::KeyPoint> &keypoint_scene, const cv::Mat &img_object, const std::vector<cv::KeyPoint> &keypoint_object, const std::vector<cv::DMatch> &d_matches)
{
    cv::Mat img_matches, imgmap, imgminmap;
    drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1));
    drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1));
    drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, d_matches, img_matches, cv::Scalar::all(-1),cv::Scalar::all(-1),std::vector<char>(),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

void SurfMatch::calc_good_matches(const std::vector<cv::KeyPoint> &keypoint_scene, const std::vector<cv::KeyPoint> &keypoint_object,
    const std::vector<std::vector<cv::DMatch>> &KNN_m, std::vector<MatchKeyPoint> &out_good_keypoints,
    std::vector<cv::DMatch> &out_good_matches)
{
    float ratio_thresh = 0.75;
    for (size_t i = 0; i < KNN_m.size(); i++)
    {
        if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
        {

            if (KNN_m[i][0].queryIdx >= keypoint_object.size())
            {
                continue;
            }
            out_good_keypoints.push_back(MatchKeyPoint{
                    keypoint_object[KNN_m[i][0].queryIdx],
                     keypoint_scene[KNN_m[i][0].trainIdx]
            });
        }
    }
}

void SurfMatch::RemoveKeypointOffset(std::vector<MatchKeyPoint> keypoints, double scale, std::vector<double> &x_list, std::vector<double> &y_list)
{
    for (int i = 0; i < keypoints.size(); i++)
    {
        auto mini_keypoint = keypoints[i].query;
        auto map_keypoint = keypoints[i].train;

        auto diff_pos = mini_keypoint.pt * scale + map_keypoint.pt;

        x_list.push_back(diff_pos.x);
        y_list.push_back(diff_pos.y);
    }
}


Match::Match(double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
    detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
    matcher  = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
}

std::vector<std::vector<cv::DMatch>> Match::match(const cv::Mat &query_descriptors, const cv::Mat &train_descriptors)
{
    std::vector<std::vector<cv::DMatch>> match_group;
    //matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
    matcher->knnMatch(query_descriptors, train_descriptors, match_group, 2);
    return match_group;
}

std::vector<std::vector<cv::DMatch>> Match::match(features &query_key_mat_point, features &train_key_mat_point)
{
    return match(query_key_mat_point.descriptors, train_key_mat_point.descriptors);
}

bool Match::detect_and_compute(const cv::Mat &img, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
    if (img.empty()) return  false;
    detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
    if (keypoints.size() == 0) return false;
    return true;
}

bool Match::detect_and_compute(const cv::Mat &img, features &key_mat_point)
{
    return detect_and_compute(img, key_mat_point.keypoints, key_mat_point.descriptors);
}

