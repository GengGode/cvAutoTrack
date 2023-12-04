#pragma once
#include <opencv2/opencv.hpp>
#include "utils/Utils.h"

typedef std::pair<int, std::string> error_info;

struct direction_calculation_config
{
    bool error = false;
    error_info err = {0, ""};
};

struct rotation_calculation_config
{
    bool error = false;
    error_info err = {0, ""};
};

struct position_calculation_config
{
    bool error = false;
    error_info err = {0, ""};
};

struct odometer_config
{
    bool error = false;
    error_info err = {0, ""};
    double scale = 1.0;
};