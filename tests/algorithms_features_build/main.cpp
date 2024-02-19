#include "resources/binary/resources.binary.h"
#include "algorithms/features/features.serialize.hpp"
#include <fmt/format.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <filesystem>

using namespace tianli::algorithms::features_serialize;

void init_config(std::string config_file,std::map<std::string,std::string> block_hashs)
{
    features_group_config config;
    config.version = "1.0.0";
    config.block_count = 2;
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

// 生成一张随机图片
cv::Mat gen_mat()
{
    cv::Mat mat(100, 100, CV_8UC3);
    cv::randu(mat, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    return mat;
}

features get_rand()
{
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();
    cv::Mat mat = gen_mat();
    surf->detectAndCompute(mat, cv::noArray(), keypoints, descriptors);
    return features(keypoints, descriptors);
}

void init_fts(std::string fts_file)
{
    features_config config;
    features fts = get_rand();
    features_block block;
    block.config = config;
    block.feature = fts;
    block.rect = cv::Rect2d(0, 0, 100, 100);
    block.name = "test";
    std::ofstream ofile(fts_file, std::ios::binary);
    if (!ofile.is_open())
    {
        throw std::runtime_error("features file not found");
    }
    try
    {
        cereal::BinaryOutputArchive ar(ofile);
        ar(block);
        ofile.close();
    }
    catch (const std::exception& e)
    {
        ofile.close();
        throw e;
    }
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


int main()
{
    std::string fts_file = "cvAutoTrack.Cache-1.0.0-fts-test1.block";
    init_fts(fts_file);

    features_block block = features_block::read(fts_file);

    auto hash = get_md5_hash(fts_file);
    std::cout << "hash: " << hash << std::endl;

    std::string config_file = "cvAutoTrack.Cache-1.0.0-config.cache";
    if(!std::filesystem::exists(config_file))
    {
        init_config(config_file,{{fts_file,hash}});
    }
    try 
    {
        features_group group(config_file,"./");
        if (!group.is_loaded)
        {
            throw std::runtime_error("features_group not loaded");
        }
        for (auto& [name, block] : group.blocks)
        {
            std::cout << "name: " << name << " block: " << block.name << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;

}