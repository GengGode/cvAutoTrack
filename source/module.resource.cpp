#include "pch.h"
#include "cvAutoTrack.h"
#include "global/global.include.h"

struct tianli::global::cvat_resource
{
    void *data;
    cvat_error begin_load();
    cvat_error end_load();
    cvat_error load_serialize_features(const char *path);
    cvat_error load_serialize_config(const char *path);

    static cvat_resource_t create();
    static void destroy(cvat_resource_t resource);
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