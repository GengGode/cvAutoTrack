#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

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
	cv::Mat PaimonTemplate;
	cv::Mat MinimapCailbTemplate;
	cv::Mat StarTemplate;
	cv::Mat MapTemplate;
	cv::Mat UID;
	cv::Mat UIDnumber[10];
public:
	void install();
	void release();
public:
	void get_map_keypoint_cache();

private:
	bool is_installed = false;
};

bool save_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);
bool load_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);
bool get_map_keypoint(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold = 1, int octaves = 1, int octave_layers = 1, bool extended = false, bool upright = false);