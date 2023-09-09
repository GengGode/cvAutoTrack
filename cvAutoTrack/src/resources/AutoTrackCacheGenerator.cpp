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
        std::string_view jsonString;

        std::fstream jsonFile;
        jsonFile.open(jsonPath, std::ios::in);

        std::stringstream jsonStream;
        jsonStream << jsonFile.rdbuf();

        jsonString = jsonStream.str();
        jsonFile.close();

        //将json字符串转换为json对象
        auto jsonDoc = json::parse5(jsonString);
        return true;
    }
}