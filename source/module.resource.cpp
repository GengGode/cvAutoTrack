#include "pch.h"
#include "cvAutoTrack.h"
#include "global/global.include.h"

struct cvat_resource
{
    void *data;
    cvat_error begin_load();
    cvat_error end_load();
    cvat_error load_serialize_features(const char *path);
    cvat_error load_serialize_config(const char *path);

    static cvat_resource_t create()
    {
        return new cvat_resource();
    }
    static void destroy(cvat_resource_t resource)
    {
        delete resource;
    }
};

/// @brief 创建资源
/// @return 资源指针
cvat_resource_t CreateResource()
{
    return cvat_resource::create();
}
/// @brief 销毁资源
/// @param resource 资源指针
void DestroyResource(cvat_resource_t resource)
{
    cvat_resource::destroy(resource);
}

bool SetCacheConfig(const char* config_file, const char* blocks_dir, const char* config, int config_size)
{
    

    return false;
}
