#pragma once
#include <pch.h>
#include <json5.hpp>

namespace fs = std::filesystem;

namespace Tianli::Resource
{
    class AutoTrackCacheGenerator {
    private:
        AutoTrackCacheGenerator() = default;
        static AutoTrackCacheGenerator* instance;
        //矩形区域表示的枚举
        enum E_RectType {
            RECT, RANGE
        };
        //基本设置结构体
        struct S_SettingConf {
            //版本设置，与游戏版本保持一致
            std::string version;
            //以下为SURF::create的参数
            double hessian_threshold;
            int octaves;
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
            //地区的id编号
            int area_id;
            //地区的文件名
            std::string area_filename;
            //地区的显示名
            std::string name;
            //地区的0号tile的输入坐标映射
            cv::Rect2i zero_tile_rect_in;
            //地区的0号tile的输出坐标映射
            cv::Rect2i zero_tile_rect_out;
            //地区的tile的大小
            cv::Size2i tile_size;
            //接口输出的缩放倍数
            float zoom;
            //【扩展】地区的绝对坐标映射
            cv::Rect2i ex_rect_abs;
            //【扩展】地区的输入坐标映射
            cv::Rect2i ex_rect_in;
            //【扩展】地区的输出坐标映射
            cv::Rect2i ex_rect_out;
        };

        //手动拼贴结构体
        struct S_AreaConf {
            //归属的自动拼图父节点
            int parent_id;
            //地区的id编号
            int area_id;
            //地区的显示名称
            std::string name;
            //地区的文件路径
            std::string path;
            //地区的映射范围
            cv::Rect2i area_rect;
            //地区的tile的大小
            cv::Size2i tile_size;
            //接口输出的缩放倍数
            float zoom;
            //【扩展】地区的绝对坐标映射
            cv::Rect2i ex_rect_abs;
        };
        //json文件的路径
        fs::path m_json_path;
        //资源根目录
        fs::path m_resource_path;

        //基本设置
        S_SettingConf m_setting_conf;
        //自动拼图表
        //成员归属于大区域
        std::map<int, S_TiledConf> m_tiled_conf;
        //地区数据
        //键：归属的大区域
        //值：包含的小区域
        std::map<int, S_AreaConf> m_areas_conf;

        //标签
        std::vector<int> m_tags;

        //定位点
        std::vector<cv::KeyPoint> m_key_points;
        //描述矩阵
        cv::Mat m_descriptors;
    public:
        // 返回缓存生成器的单例
        static AutoTrackCacheGenerator* getInstance();

        // 生成追踪缓存
        // @param jsonPath json文件路径
        // @param resourcePath 资源根目录
        // @return 是否生成成功
        bool GenCache(std::string const& jsonPath = "./AutoTrack_config.json5", std::string const& resourcePath = ".");

        //获取自动接图的KeyPoints
        //@param detector SURF SURF对象
        void GenTiledKeyPoints(cv::Ptr<cv::xfeatures2d::SURF>& detector);

        //获取手动图的KeyPoints
        //@param detector SURF SURF对象
        void GenAreaKeyPoints(cv::Ptr<cv::xfeatures2d::SURF>& detector);

        //将特征点合并到总特征点表中
        //@param key_points 特征点数据
        //@param descriptors 特征点描述
        void appendKeyPoint(std::vector<cv::KeyPoint>& key_points, cv::Mat& descriptors);

    private:
        // 反序列化json
        // @param jsonPath json文件路径
        // @return 是否读取成功
        bool loadJson();
        // 反序列化json的配置信息
        // @param root json根节点
        // @return 是否读取成功
        bool loadSettingFromJson(json::object root);
        // 反序列化json的自动拼图表信息
        // @param root json根节点
        // @return 是否读取成功
        bool loadTiledConfFromJson(json::object root);
        // 反序列化json的手动拼图信息
        // @param root json根节点
        // @return 是否读取成功
        bool loadAreaConfFromJson(json::object root);

        // 获取自动拼图表合并后的图像
        // @param tiledConf 自动拼图表
        // @param retRectIn 自动拼图表的图像输入坐标
        // @param retRectOut 自动拼图表的图像输出坐标
        // @return 自动拼图表的图像
        cv::Mat margeTileImage(const S_TiledConf& tiledConf, cv::Rect2i& retRectIn, cv::Rect2i& retRectOut);

        // 拼接一张tile图到图片缓存中
        // @param imgBuf 图片缓存
        // @param imgTile 单张图片
        // @param tileSize 单张图片的大小
        // @param pos 图片的tiled位置
        // @param originPos 拼接时的原点位置，第一次使用传入(0,0)，之后自动更新
        void AddTileInBuf(cv::Mat& imgBuf, cv::Mat imgTile, cv::Size2i tileSize, cv::Point2i pos, cv::Point2i& originPos);
    };
}