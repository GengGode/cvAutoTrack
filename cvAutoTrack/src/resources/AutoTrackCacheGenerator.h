#pragma once
#include <pch.h>
#include <json5.hpp>

namespace Tianli::Resource
{
    class AutoTrackCacheGenerator {
    private:
        AutoTrackCacheGenerator() = default;
        static AutoTrackCacheGenerator* instance;

        enum E_RectType {
            RECT, RANGE
        };
        //基本设置结构体
        struct S_SettingConf {
            //版本设置，与游戏版本保持一致
            std::string version;
            //以下为SURF::create的参数
            double hessian_threshold;
            int octave;
            int octave_layers;
            bool extended;
            bool upRight;
            //json中Rect的表示格式
            E_RectType position_mode;
            //资源的根目录的位置
            std::string resource_root_path;
            //使用自动拼图表的匹配表达式
            std::string auto_tile_path_regex;
        };
        //自动接图表结构体
        struct S_TiledConf {
            int area_id;
            std::string area_filename;
            std::string name;
            cv::Rect2i zero_tile_rect_in;
            cv::Rect2i zero_tile_rect_out;
            cv::Range x_range;
            cv::Range y_range;
            cv::Size2i tile_size;
            float zoom;
        };

        //手动拼贴结构体
        struct S_AreaConf {
            int area_id;
            std::string name;
            std::string path;
            cv::Rect2i area_rect;
            cv::Size2i tile_size;
            float zoom;
        };

        S_SettingConf m_setting_conf;
        //自动拼图表
        //成员归属于大区域
        std::vector<S_TiledConf> m_tiled_conf;
        //地区数据
        //键：归属的大区域
        //值：包含的小区域
        std::map<int, std::vector<S_AreaConf>> m_areas_conf;

    public:
        static AutoTrackCacheGenerator* getInstance();

        // 从json文件中加载图像，并生成缓存
        // @param jsonPath json文件路径
        // @param resourcePath 资源路径
        // @return 缓存是否生成成功
        bool loadJson(std::string const& jsonPath = "./AutoTrack_config.json5");

    private:
        bool loadSettingFromJson(json::object root);
        bool loadTiledConfFromJson(json::object root);
        bool loadAreaConfFromJson(json::object root);
    };
}