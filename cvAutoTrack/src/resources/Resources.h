#pragma once

//图片资源 加载类
class Resources
{
private:
    Resources();
public:
    ~Resources();

    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;
    static Resources& getInstance();

public:
    std::map<std::pair<int, int>, cv::Mat> MapBlockCache;

public:
    cv::Mat PaimonTemplate;
    cv::Mat MinimapCailbTemplate;
    cv::Mat StarTemplate;
    cv::Mat MapTemplate;
    cv::Mat UID;
    cv::Mat UIDnumber[10];

    // 天理坐标映射关系参数 地图中心
    // 地图中天理坐标中心的像素坐标
    const cv::Point2d map_relative_center = { 3967, 3962 }; // 天理坐标中点
    // 地图中图片像素与天理坐标系的比例
    const double map_relative_scale = 3.413333; // 天理坐标缩放
public:
    void install();
    void release();
public:
    //void get_map_keypoint_cache();
    bool map_is_embedded();
private:
    bool is_installed = false;
};

class MapKeypointCache {

public:
    std::string bulid_time;
    std::string bulid_version;
    float hessian_threshold;
    WORD octave;
    WORD octave_layers;
    WORD extended;
    WORD upRight;
    std::vector<cv::KeyPoint> keyPoints;
    cv::Mat descriptors;
    std::string bulid_version_end;

    MapKeypointCache() {}

    MapKeypointCache(std::string bulid_time,
        std::string bulid_version,
        float hessian_threshold,
        WORD octave,
        WORD octave_layer,
        WORD extended,
        WORD upRight,
        std::vector<cv::KeyPoint> keyPoints,
        cv::Mat descriptors) :
        bulid_time(bulid_time), bulid_version(bulid_version), hessian_threshold(hessian_threshold),
        octave(octave), octave_layers(octave_layer), extended(extended), upRight(upRight),
        keyPoints(keyPoints), descriptors(descriptors), bulid_version_end(bulid_version) {}

    void serialize(std::string outfileName);
    void deSerialize(std::string infileName);
};

bool save_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);
bool load_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors);

bool get_map_keypoint(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors);