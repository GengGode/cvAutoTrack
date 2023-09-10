#include "pch.h"
#include "AutoTrackCacheGenerator.h"
#include <ErrorCode.h>
#include <json5.hpp>

namespace fs = std::filesystem;

namespace Tianli::Resource {
    ErrorCode& err = ErrorCode::getInstance();
    AutoTrackCacheGenerator* AutoTrackCacheGenerator::instance = nullptr;

    AutoTrackCacheGenerator* AutoTrackCacheGenerator::getInstance() {
        if (instance == nullptr) {
            instance = new AutoTrackCacheGenerator();
        }
        return instance;
    }

    bool AutoTrackCacheGenerator::loadJson(std::string const& jsonPath)
    {
        //检查json文件，如果不存在，报错并返回false
        if (!fs::exists(jsonPath)) {
            err = { 470,std::format("配置文件\"{}\"不存在，请检查路径是否正确",jsonPath) };
            return false;
        }

        //打开json文件，并存入到字符串中
        std::string jsonString;

        std::fstream jsonFile;
        jsonFile.open(jsonPath, std::ios::in);

        std::stringstream jsonStream;
        jsonStream << jsonFile.rdbuf();

        jsonString = jsonStream.str();
        jsonFile.close();

        //将json字符串转换为json对象
        auto jsonDoc = json::parse5(jsonString);
        json::object rootObj = jsonDoc.value().as_object();

        //解析设置串
        loadSettingFromJson(rootObj);
        //解析自动拼图表串
        loadTiledConfFromJson(rootObj);
        //解析手动拼贴串
        return true;
    }

    bool AutoTrackCacheGenerator::loadSettingFromJson(json::object root)
    {
        json::object settingObj = root["setting"].as_object();
        m_setting_conf.version = settingObj["version"].as_string();
        //读取SURF参数
        m_setting_conf.hessian_threshold = settingObj["hessian_threshold"].as_double();
        m_setting_conf.octave = settingObj["octave"].as_integer();
        m_setting_conf.extended = settingObj["extended"].as_boolean();
        m_setting_conf.upRight = settingObj["upRight"].as_boolean();
        //读取其他设置参数
        std::string position_mode = settingObj["position_mode"].as_string();
        if (position_mode == "rect")
            m_setting_conf.position_mode = E_RectType::RECT;
        else if (position_mode == "range")
            m_setting_conf.position_mode = E_RectType::RANGE;

        m_setting_conf.resource_root_path = settingObj["resource_root_path"].as_string();
        m_setting_conf.auto_tile_path_regex = settingObj["auto_tile_path_regex"].as_string();

        return true;
    }

    bool AutoTrackCacheGenerator::loadTiledConfFromJson(json::object root)
    {
        json::array tiledArray = root["tiled"].as_array();

        for (auto& it : tiledArray)
        {
            json::object tiledObj = it.as_object();
            S_TiledConf tiledConf;
            //获取区域id
            tiledConf.area_id = tiledObj["area_id"].as_integer();
            //获取区域文件名标识
            tiledConf.area_filename = tiledObj["area_filename"].as_string();
            //获取区域名称
            tiledConf.name = tiledObj["name"].as_string();

            //获取映射输入坐标系
            auto zero_tile_rect_in = tiledObj["zero_tile_rect_in"].as_array();
            tiledConf.zero_tile_rect_in.x = zero_tile_rect_in[0].as_integer();
            tiledConf.zero_tile_rect_in.y = zero_tile_rect_in[1].as_integer();

            if (m_setting_conf.position_mode == E_RectType::RECT)
            {
                tiledConf.zero_tile_rect_in.width = zero_tile_rect_in[2].as_integer();
                tiledConf.zero_tile_rect_in.height = zero_tile_rect_in[3].as_integer();
            }
            else if (m_setting_conf.position_mode == E_RectType::RANGE)
            {
                tiledConf.zero_tile_rect_in.width = zero_tile_rect_in[2].as_integer() - zero_tile_rect_in[0].as_integer();
                tiledConf.zero_tile_rect_in.height = zero_tile_rect_in[3].as_integer() - zero_tile_rect_in[1].as_integer();
            }
            //获取映射输出坐标系
            auto zero_tile_rect_out = tiledObj["zero_tile_rect_out"].as_array();
            tiledConf.zero_tile_rect_out.x = zero_tile_rect_out[0].as_integer();
            tiledConf.zero_tile_rect_out.y = zero_tile_rect_out[1].as_integer();
            if (m_setting_conf.position_mode == E_RectType::RECT)
            {
                tiledConf.zero_tile_rect_out.width = zero_tile_rect_out[2].as_integer();
                tiledConf.zero_tile_rect_out.height = zero_tile_rect_out[3].as_integer();
            }
            else if (m_setting_conf.position_mode == E_RectType::RANGE)
            {
                tiledConf.zero_tile_rect_out.width = zero_tile_rect_out[2].as_integer() - zero_tile_rect_out[0].as_integer();
                tiledConf.zero_tile_rect_out.height = zero_tile_rect_out[3].as_integer() - zero_tile_rect_out[1].as_integer();
            }

            //获取x，y范围
            auto x_range = tiledObj["x_range"].as_array();
            tiledConf.x_range.start = x_range[0].as_integer();
            tiledConf.x_range.end = x_range[1].as_integer();

            auto y_range = tiledObj["y_range"].as_array();
            tiledConf.y_range.start = y_range[0].as_integer();
            tiledConf.y_range.end = y_range[1].as_integer();

            //获取tile尺寸
            auto tile_size = tiledObj["tile_size"].as_array();
            tiledConf.tile_size.width = tile_size[0].as_integer();
            tiledConf.tile_size.height = tile_size[1].as_integer();

            //获取缩放
            tiledConf.zoom = tiledObj["zoom"].as_double();

            //将结果压入数组
            m_tiled_conf.emplace_back(tiledConf);
        }
        return true;
    }
}