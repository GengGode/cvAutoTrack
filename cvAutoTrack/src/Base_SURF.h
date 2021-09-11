#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
using namespace cv;
enum SurfMatchFlag
{
	defalut,
	rasan
};

// SURF 基类
/*
	不保存图片
	只保留特征点
*/
class Base_SURF
{
	//Mat _Object;
	//Mat _Target;
	Ptr<xfeatures2d::SURF> _Detector;
	std::vector<KeyPoint> _KeyPointObject;
	std::vector<KeyPoint> _KeyPointTarget;
	Mat _DataPointObject;
	Mat _DataPointTarget;

	int _MinHessian = 400;
	double _RatioThresh = 0.667;

	//std::vector<DMatch> _ResMatch;
public:
	Base_SURF();
	~Base_SURF();
	
	//设置SURF参数
	void setParmMinHessian(int minHessian);
	void setParmRatioThresh(double ratioThresh);

	//设置特征点匹配函数
	void setMatchMode(std::vector<DMatch>(*matchFunptr)(std::vector<KeyPoint>, std::vector<KeyPoint>));

	//设置SURF匹配源
	bool setCalcObject(Mat imgObject);
	bool setCalcTarget(Mat imgTarget);

	//获取特征点列表
	std::vector<KeyPoint> getKeyPointObject();
	std::vector<KeyPoint> getKeyPointTarget();
	

};

class SURF_Map
{
	Base_SURF surf;

	std::vector<DMatch> matchFunptr(std::vector<KeyPoint> keyPointObject, std::vector<KeyPoint> keyPointTarget);


};