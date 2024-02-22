#include "resources/binary/resources.binary.h"
#include "algorithms/features/features.serialize.hpp"

#include <fmt/format.h>

#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include <filesystem>
#include <iostream>

#include <opencv2/core/utils/logger.hpp>

using namespace tianli::algorithms::features_serialize;

std::string generate_gengeneric_config_filename(std::string version)
{
    return fmt::format("cvAutoTrack.Cache-{}.config", version);
}

std::string generate_gengeneric_block_filename(std::string version, std::string name)
{
    return fmt::format("cache-{}-{}.block", version, name);
}

std::string get_md5_hash(std::string file)
{
    std::string cmd = fmt::format("powershell $(Get-FileHash -Algorithm MD5 -Path {}).Hash", file);
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe)
    {
        throw std::runtime_error("popen failed");
    }
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != NULL)
        {
            result += buffer;
        }
    }
    _pclose(pipe);
    return result.substr(0, 32);
}

void init_config(std::string config_file, std::map<std::string, std::string> block_hashs)
{
    features_group_config config;
    config.version = "1.0.0";
    config.block_count = block_hashs.size();
    config.block_hashs = block_hashs;

    std::vector<std::string> names;
    for (auto& [name, hash] : block_hashs)
    {
        names.push_back(name);
    }
    config.names = names;

    // find config file
    std::ofstream file(config_file, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("features_group file not found");
    }
    try
    {
        cereal::BinaryOutputArchive ar(file);
        ar(config);
        file.close();
    }
    catch (const std::exception& e)
    {
        file.close();
        throw e;
    }
}

features get_features(cv::Mat mat)
{
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();
    surf->detectAndCompute(mat, cv::noArray(), keypoints, descriptors);
    return features(keypoints, descriptors);
}

features_block build_from_image(std::string image_name, cv::Mat image)
{
    fmt::println("build_from_image: {}", image_name);
    features_block block;
    block.config = features_config();
    block.feature = get_features(image);
    block.rect = cv::Rect2d(cv::Point2d(0, 0), image.size());
    block.name = image_name;
    return block;
}

std::string write_block_and_get_hash(features_block block, std::string file)
{
    fmt::println("write_block_and_get_hash: {}", file);
    block.write(file);
    return get_md5_hash(file);
}

void gen_cache()
{
    std::string images_dir = "./images";
    std::string cache_dir = "./cache";
    std::string version = "1.0.1";
    std::string config_file = generate_gengeneric_config_filename(version);

    std::vector<std::string> image_fils;
    for (const auto& entry : std::filesystem::directory_iterator(images_dir))
    {
        if (!entry.is_regular_file())
            continue;
        if (entry.path().extension() != ".png")
            continue;
        image_fils.push_back(entry.path().string());
    }

    fmt::println("image_fils: {}", image_fils.size());

    std::map<std::string, std::string> block_hashs;
    for (auto& image_file : image_fils)
    {
        cv::Mat image = cv::imread(image_file);
        if (image.empty())
        {
            throw std::runtime_error("image not found");
        }
        // 获取文件名，不包含扩展名
        auto image_name = std::filesystem::path(image_file).filename().string();
        image_name = image_name.substr(0, image_name.size() - 4);

        auto block_name = generate_gengeneric_block_filename(version, image_name);

        std::string block_file = (std::filesystem::path(cache_dir) / block_name).string();
        auto block = build_from_image(block_name, image);
        if (block.feature.empty())
        {
            continue;
        }
        block_hashs[block_name] = write_block_and_get_hash(block, block_file);
    }

    init_config(config_file, block_hashs);
}
void load_cache()
{
    std::string version = "1.0.1";
    std::string config_file = generate_gengeneric_config_filename(version);
    features_group group;
    try
    {
        group = features_group(config_file, "./cache");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::cout << group.config.version << std::endl;
    std::cout << group.config.block_count << std::endl;
    std::cout << group.config.names.size() << std::endl;

    for (auto& [name, block] : group.blocks)
    {
        std::cout << name << " " << block.feature.size() << std::endl;
    }
}
int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    gen_cache();

    load_cache();
    return 0;
}
