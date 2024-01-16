#include <pch.h>
#include "module.debug.h"
#include <resources/Resources.h>

namespace tianli::debug {
    bool LoadDebugMapImage(const char *path)
    {
#ifdef _DEBUG
        Resources::getInstance().debug_map_image = cv::imread(path);
        return true;
#else
        return false;
#endif
    }
}