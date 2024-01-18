#include "pch.h"
#include <resources/Resources.h>

bool __stdcall DebugLoadMapImagePath(const char* path)
{
#ifdef _DEBUG
    INSTALL_DUMP_();
    Resources::getInstance().debug_map_image = cv::imread(path);
    return true;
#else
    return false;
#endif
}