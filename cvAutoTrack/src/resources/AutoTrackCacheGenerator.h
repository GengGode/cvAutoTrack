#pragma once
namespace Tianli::Resource
{
    class AutoTrackCacheGenerator {
    private:
        AutoTrackCacheGenerator() = default;
        static AutoTrackCacheGenerator* instance;

    public:
        static AutoTrackCacheGenerator* getInstance();

        // 从json文件中加载图像，并生成缓存
        // @param jsonPath json文件路径
        // @param resourcePath 资源路径
        // @return 缓存是否生成成功
        bool loadJson(std::string const& jsonPath = "./AutoTrack_config.json5");
    };
}