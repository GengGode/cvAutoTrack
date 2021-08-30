#include "pch.h"
#include "Base_SURF.h"

Base_SURF::Base_SURF()
{
}

Base_SURF::~Base_SURF()
{
}

void Base_SURF::setParmMinHessian(int minHessian)
{
	_MinHessian = minHessian;
}

void Base_SURF::setParmRatioThresh(double ratioThresh)
{
	_RatioThresh = ratioThresh;
}

bool Base_SURF::setCalcObject(Mat imgObject)
{
	return false;
}
