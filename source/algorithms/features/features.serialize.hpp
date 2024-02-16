#pragma once
#include "algorithms/algorithms.include.h"

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace tianli::algorithms
{
    // features
    template <class Archive> void serialize(Archive& ar, features& fts)
    {
        ar(fts.indexs, fts.keypoints, fts.descriptors);
    }

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
        std::string hash;
        std::string name;
        template <class Archive> void serialize(Archive& ar) { ar(config, feature, rect, hash, name); }
    };

    struct features_group
    {
        std::map<std::string, features_block> blocks;
    };
}
