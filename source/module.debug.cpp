#include "pch.h"
#include <cvAutoTrack.h>
#include "global/global.windump.h"
#include <resources/Resources.h>

bool DebugLoadMapImagePath(const char* path)
{
#ifdef _DEBUG
    INSTALL_DUMP_();
    Resources::getInstance().debug_map_image = cv::imread(path);
    return true;
#else
    return false;
#endif
}
