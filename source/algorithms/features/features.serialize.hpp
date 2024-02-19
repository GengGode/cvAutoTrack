#pragma once
#include "algorithms/algorithms.include.h"
#include "algorithms/algorithms.serialize.hpp"

#include <fstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace cereal
{
    // features
    template <class Archive> void serialize(Archive& ar, features& fts)
    {
        ar(fts.indexs, fts.keypoints, fts.descriptors);
    }
} // namespace cereal

namespace tianli::algorithms::features_serialize
{
    struct features_config
    {
        int32_t octaves = 1;
        int32_t octave_layers = 1;
        float_t hessian_threshold = 1.0;
        bool extended = false;
        bool up_right = false;
        template <class Archive> void serialize(Archive& ar) { ar(octaves, octave_layers, hessian_threshold, extended, up_right); }
    };

    struct features_block
    {
        features_config config;
        features feature;
        cv::Rect2d rect;
        std::string name;
        template <class Archive> void serialize(Archive& ar) { ar(config, feature, rect, name); }
        static features_block read(const std::string& file)
        {
            std::ifstream ifs(file, std::ios::binary);
            if (!ifs.is_open())
            {
                throw std::runtime_error("features_block file not found");
            }
            try
            {
                features_block block;
                cereal::BinaryInputArchive ar(ifs);
                ar(block);
                ifs.close();
                return block;
            }
            catch (const std::exception& e)
            {
                ifs.close();
                throw e;
            }
        }
    };

    struct features_group_config
    {
        std::string version;
        std::vector<std::string> names;
        std::map<std::string, std::string> block_hashs;
        int block_count;
        template <class Archive> void serialize(Archive& ar) { ar(version, names, block_hashs, block_count); }

        static features_group_config read(const std::string& file)
        {
            std::ifstream ifs(file, std::ios::binary);
            if (!ifs.is_open())
            {
                throw std::runtime_error("features_group file not found");
            }
            try
            {
                features_group_config config;
                cereal::BinaryInputArchive ar(ifs);
                ar(config);
                ifs.close();
                return config;
            }
            catch (const std::exception& e)
            {
                ifs.close();
                throw e;
            }
        }
    };

    struct features_group
    {
        std::map<std::string, features_block> blocks;
        features_group_config config;
        bool is_loaded = false;
        features_group() = default;
        features_group(std::string config_file, std::string blocks_dir)
        {
            std::ifstream file(config_file, std::ios::binary);
            if (!file.is_open())
            {
                throw std::runtime_error("features_group file not found");
            }
            try
            {
                cereal::BinaryInputArchive ar(file);
                ar(config);
                file.close();
            }
            catch (const std::exception& e)
            {
                file.close();
                throw e;
            }

            for (auto& name : config.names)
            {
                std::string block_file = blocks_dir + "/" + name;
                std::ifstream file(block_file, std::ios::binary);
                if (!file.is_open())
                {
                    throw std::runtime_error("features_block file not found");
                }
                try
                {
                    cereal::BinaryInputArchive ar(file);
                    features_block block;
                    ar(block);
                    blocks[name] = block;
                    file.close();
                }
                catch (const std::exception& e)
                {
                    file.close();
                    throw e;
                }
            }
            is_loaded = true;
        }
    };
} // namespace tianli::algorithms::features_serialize
