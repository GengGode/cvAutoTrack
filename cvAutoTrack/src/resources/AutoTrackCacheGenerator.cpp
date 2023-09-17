#include "pch.h"
#include "AutoTrackCacheGenerator.h"
#include <ErrorCode.h>
#include <json5.hpp>
#include <regex>
#include <utils/Utils.h>

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

    bool AutoTrackCacheGenerator::GenCache(std::string const& jsonPath, std::string const& resourcePath)
    {
        m_json_path = jsonPath;
        m_resource_path = resourcePath;
        //读取json的配置信息
        loadJson();

        //初始化SURF
        cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(
            m_setting_conf.hessian_threshold,
            m_setting_conf.octaves,
            m_setting_conf.octave_layers,
            m_setting_conf.extended,
            m_setting_conf.upRight);

        GenTiledKeyPoints(detector);
        GenAreaKeyPoints(detector);
        return true;
    }

    void AutoTrackCacheGenerator::GenTiledKeyPoints(cv::Ptr<cv::xfeatures2d::SURF>& detector)
    {
        //拼接自动拼图表，并生成特征点
        for (auto& kw_item : m_tiled_conf)
        {
            auto& tile_conf = kw_item.second;
            //拼接图像，并计算边界
            auto marged_img = margeTileImage(tile_conf, tile_conf.ex_rect_in, tile_conf.ex_rect_out);
            tile_conf.ex_rect_abs = cv::Rect2i(cv::Point(), marged_img.size());
            //生成特征点和描述信息
            vector<cv::KeyPoint> key_points;
            cv::Mat descriptors;
            detector->detectAndCompute(marged_img, cv::noArray(), key_points, descriptors);

            //将坐标换算到新的坐标系下
            for (auto& kp : key_points)
            {
                //换算坐标
                kp.pt = TianLi::Utils::TransferAxes(kp.pt, tile_conf.ex_rect_abs, tile_conf.ex_rect_out);
            }
            //合并特征点
            appendKeyPoint(key_points, descriptors);
        }
    }

    void AutoTrackCacheGenerator::GenAreaKeyPoints(cv::Ptr<cv::xfeatures2d::SURF>& detector)
    {
        //遍历手动拼图对象
        for (auto& kw_item : m_areas_conf)
        {
            auto& areas_conf = kw_item.second;
            //读取图像，并生成特征点
            auto img = cv::imread((fs::path(m_setting_conf.resource_root_path) / fs::path(areas_conf.path)).string());
            if (img.empty())
            {
                err = { 471,std::format("配置文件指向的图像\"{}\"不存在，请检查路径是否正确",areas_conf.path) };
            }
            areas_conf.ex_rect_abs = cv::Rect2i(cv::Point(), img.size());
            //生成特征点和描述信息
            vector<cv::KeyPoint> key_points;
            cv::Mat descriptors;
            detector->detectAndCompute(img, cv::noArray(), key_points, descriptors);

            //将坐标换算到新的坐标系下
            for (auto& kp : key_points)
            {
                //换算坐标
                kp.pt = TianLi::Utils::TransferAxes(kp.pt, areas_conf.ex_rect_abs, areas_conf.area_rect);
                //读取父地区的坐标，再次换算
                auto parent_id = areas_conf.parent_id;
                auto inRect = m_tiled_conf[parent_id].ex_rect_in;
                auto outRect = m_tiled_conf[parent_id].ex_rect_out;
                kp.pt = TianLi::Utils::TransferAxes(kp.pt, inRect, outRect);
            }

            //合并特征点
            appendKeyPoint(key_points, descriptors);
        }
    }

    void AutoTrackCacheGenerator::appendKeyPoint(std::vector<cv::KeyPoint>& key_points, cv::Mat& descriptors)
    {
        //将特征点合并到总特征点表中
        m_key_points.insert(m_key_points.end(), key_points.begin(), key_points.end());
        if (m_descriptors.empty())
            m_descriptors = descriptors;
        else {
            cv::vconcat(m_descriptors, descriptors);
        }
    }

    bool AutoTrackCacheGenerator::loadJson()
    {
        //检查json文件，如果不存在，报错并返回false
        if (!fs::exists(m_json_path)) {
            err = { 470,std::format("配置文件\"{}\"不存在，请检查路径是否正确",m_json_path.string()) };
            return false;
        }

        //打开json文件，并存入到字符串中
        std::string jsonString;

        std::fstream jsonFile;
        jsonFile.open(m_json_path, std::ios::in);

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
        //解析子地区串
        loadAreaConfFromJson(rootObj);
        return true;
    }

    bool AutoTrackCacheGenerator::loadSettingFromJson(json::object root)
    {
        //json示例
        /*
        setting:
        {
          version: "4.0.0",
          hessian_threshold: 1,
          octave:1,
          octave_layers:1,
          extended:false,
          upRight:false,
          position_mode: "range",
          resource_root_path: "./Maps/",
          auto_tile_path_regex: "UI_MapBack_<$name>_?<$y>_<$x>.png",
        },
        */
        json::object settingObj = root["setting"].as_object();
        m_setting_conf.version = settingObj["version"].as_string();
        //读取SURF参数
        m_setting_conf.hessian_threshold = settingObj["hessian_threshold"].as_double();
        m_setting_conf.octaves = settingObj["octaves"].as_integer();
        m_setting_conf.octave_layers = settingObj["octave_layers"].as_integer();
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
        //json示例
        /*
        tiled:[
          {
            area_id: 0,
            area_filename: "",
            name:"提瓦特大陆",
            zero_tile_rect_in:[-854,-2191,513,-824],
            zero_tile_rect_out:[0,0,1024,1024],
            x_range: [-6,7],
            y_range: [-7,5],
            tile_size:[600,600],
            zoom: 1.0
          },
        ]
        */
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

            //获取tile尺寸
            auto tile_size = tiledObj["tile_size"].as_array();
            tiledConf.tile_size.width = tile_size[0].as_integer();
            tiledConf.tile_size.height = tile_size[1].as_integer();

            //获取缩放
            tiledConf.zoom = tiledObj["zoom"].as_double();

            //将结果压入数组
            m_tiled_conf[tiledConf.area_id] = (tiledConf);
        }
        return true;
    }

    bool AutoTrackCacheGenerator::loadAreaConfFromJson(json::object root)
    {
        //json示例
        /*
        other_map:
        [
          {
            parent: 0,
            areas:
            [
              {
                area_id: 1100101,
                name: "蒙德城",
                path: "Ui_Map_City_Mondstadt.png",
                area_rect: [1267,-4102,1892,-3625],
                tile_size: [830,628],
                zoom: 3.0,
              },
            ],
          }
        ]
        */

        //解析other_map数组
        json::array otherMapArray = root["other_map"].as_array();
        //逐个解析对象
        for (auto& it : otherMapArray)
        {
            json::object otherMapObj = it.as_object();
            //获取父区域id
            int parentId = otherMapObj["parent"].as_integer();
            //解析areas数组
            json::array areasArray = otherMapObj["areas"].as_array();
            //逐个解析子地区
            for (auto& subIt : areasArray)
            {
                json::object areaObj = subIt.as_object();
                S_AreaConf areaConf;
                //获取区域id
                areaConf.area_id = areaObj["area_id"].as_integer();
                //获取区域名称
                areaConf.name = areaObj["name"].as_string();
                //获取区域路径
                areaConf.path = areaObj["path"].as_string();
                //获取区域矩形
                auto area_rect = areaObj["area_rect"].as_array();
                areaConf.area_rect.x = area_rect[0].as_integer();
                areaConf.area_rect.y = area_rect[1].as_integer();
                if (m_setting_conf.position_mode == RECT)
                {
                    areaConf.area_rect.width = area_rect[2].as_integer();
                    areaConf.area_rect.height = area_rect[3].as_integer();
                }
                else if (m_setting_conf.position_mode == E_RectType::RANGE)
                {
                    areaConf.area_rect.width = area_rect[2].as_integer() - area_rect[0].as_integer();
                    areaConf.area_rect.height = area_rect[3].as_integer() - area_rect[1].as_integer();
                }
                //获取tile尺寸
                auto tile_size = areaObj["tile_size"].as_array();
                areaConf.tile_size.width = tile_size[0].as_integer();
                areaConf.tile_size.height = tile_size[1].as_integer();
                //获取缩放
                areaConf.zoom = areaObj["zoom"].as_double();
                //设定父节点
                areaConf.parent_id = parentId;
                //将结果放入到手动拼图配置中
                m_areas_conf[areaConf.area_id] = areaConf;
            }
        }
        return true;
    }

    cv::Mat AutoTrackCacheGenerator::margeTileImage(const S_TiledConf& tiledConf, cv::Rect2i& retRectIn, cv::Rect2i& retRectOut)
    {
        //获取匹配表达式
        std::string match_regex = m_setting_conf.auto_tile_path_regex;
        //获取x,y字段在表达式的先后顺序
        bool isXFirst = false;
        if (match_regex.find(R"(<$x>)") < match_regex.find(R"(<$y>)"))
            isXFirst = true;
        else
            isXFirst = false;
        //替换表达式，用于初步筛选
        match_regex = std::regex_replace(match_regex, std::regex(R"(<\$name>)"), tiledConf.area_filename);
        match_regex = std::regex_replace(match_regex, std::regex(R"(<\$y>)"), R"((-?[0-9]+))");
        match_regex = std::regex_replace(match_regex, std::regex(R"(<\$x>)"), R"((-?[0-9]+))");
        std::regex auto_path_regex(match_regex);

        //对路径递归遍历，并记录下所有符合名称的文件名
        std::vector<fs::path> file_list;
        fs::path root_path = m_resource_path / fs::path(m_setting_conf.resource_root_path);

        for (const auto& entry : fs::directory_iterator(root_path))
        {
            if (!fs::is_regular_file(entry.status())) {
                continue; // 如果当前项不是普通文件，则跳过该项
            }

            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, auto_path_regex)) {
                file_list.emplace_back(entry.path());
            }
        }

        //创建最初的图像
        int width = tiledConf.tile_size.width;
        int height = tiledConf.tile_size.height;
        cv::Mat buf = cv::Mat(width, height, CV_8UC3, cv::Scalar(0x48, 0x35, 0x14));
        //定义原点的位置
        cv::Point2i origin;

        //定义x，y的范围
        cv::Range range_x(0, 0);
        cv::Range range_y(0, 0);
        //依次打开文件列表的文件，并拼接
        for (auto& filepath : file_list)
        {
            //查找并捕获两个数字捕获组，作为坐标
            int x = 0, y = 0;
            std::smatch match;
            std::string path_str = filepath.string();
            if (std::regex_search(path_str, match, auto_path_regex))
            {
                if (isXFirst)
                {
                    x = std::stoi(match[1].str());
                    y = std::stoi(match[2].str());
                }
                else
                {
                    x = std::stoi(match[2].str());
                    y = std::stoi(match[1].str());
                }
                //更新x,y的范围
                range_x.start = std::min(range_x.start, x);
                range_x.end = std::max(range_x.end, x);
                range_y.start = std::min(range_y.start, y);
                range_y.end = std::max(range_y.end, y);
            }
            //读取瓦片图
            cv::Mat tile = cv::imread(filepath.string(), cv::IMREAD_COLOR);
            //拼接瓦片图
            AddTileInBuf(buf, tile, tiledConf.tile_size, cv::Point2i(x, y), origin);
        }

        //计算全图的输入，输出坐标矩形
        retRectIn = cv::Rect2i(
            -range_x.end * tiledConf.zero_tile_rect_in.width + tiledConf.zero_tile_rect_in.x,
            -range_y.end * tiledConf.zero_tile_rect_in.height + tiledConf.zero_tile_rect_in.y,
            tiledConf.zero_tile_rect_in.width * (range_x.end - range_x.start + 1),
            tiledConf.zero_tile_rect_in.height * (range_y.end - range_y.start + 1)
        );
        retRectOut = cv::Rect2i(
            -range_x.end * tiledConf.zero_tile_rect_out.width + tiledConf.zero_tile_rect_out.x,
            -range_y.end * tiledConf.zero_tile_rect_out.height + tiledConf.zero_tile_rect_out.y,
            tiledConf.zero_tile_rect_out.width * (range_x.end - range_x.start + 1),
            tiledConf.zero_tile_rect_out.height * (range_y.end - range_y.start + 1)
        );
        return buf;
    }

    void AutoTrackCacheGenerator::AddTileInBuf(cv::Mat& imgBuf, cv::Mat imgTile, cv::Size2i tileSize, cv::Point2i pos, cv::Point2i& originPos) {
        //空白填充颜色
        auto emptyColor = cv::Scalar(0x48, 0x35, 0x14);
        //缩放到tile大小
        cv::Mat imgResized(tileSize, CV_8UC3, emptyColor);
        cv::resize(imgTile, imgResized, tileSize, cv::INTER_AREA);
        imgTile = imgResized;

        cv::Point pixelPos(-pos.x * tileSize.width, -pos.y * tileSize.height);

        if (pixelPos.x < originPos.x) {
            int border_size = originPos.x - pixelPos.x;
            cv::copyMakeBorder(imgBuf, imgBuf, 0, 0, border_size, 0, cv::BORDER_CONSTANT, emptyColor);
            originPos.x -= border_size;
        }

        if (pixelPos.y < originPos.y) {
            int border_size = originPos.y - pixelPos.y;
            cv::copyMakeBorder(imgBuf, imgBuf, border_size, 0, 0, 0, cv::BORDER_CONSTANT, emptyColor);
            originPos.y -= border_size;
        }

        if (pixelPos.x + tileSize.width - originPos.x > imgBuf.cols) {
            int border_size = pixelPos.x + tileSize.width - originPos.x - imgBuf.cols;
            cv::copyMakeBorder(imgBuf, imgBuf, 0, 0, 0, border_size, cv::BORDER_CONSTANT, emptyColor);
        }

        if (pixelPos.y + tileSize.height - originPos.y > imgBuf.rows) {
            int border_size = pixelPos.y + tileSize.height - originPos.y - imgBuf.rows;
            cv::copyMakeBorder(imgBuf, imgBuf, 0, border_size, 0, 0, cv::BORDER_CONSTANT, emptyColor);
        }

        cv::Rect roi(pixelPos.x - originPos.x, pixelPos.y - originPos.y, tileSize.width, tileSize.height);
        imgTile.copyTo(imgBuf(roi));
    }
}