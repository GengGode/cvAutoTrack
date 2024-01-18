#pragma once
//std
#include <string>
#include <fstream>
#include <vector>
#include <map>
//cv
#include <opencv2/opencv.hpp>
//cereal序列化类
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>

//#define _FP16

namespace trackCache
{
    //基础设置结构体
    struct Setting
    {
        std::string version = "1.0.0";
        int32_t octaves = 1;
        int32_t octave_layers = 1;
        float_t hessian_threshold = 1.0;
        bool extended = false;
        bool up_right = false;
        cv::Rect2i roi = cv::Rect(0, 0, 0, 0);        //坐标的总范围
        //序列化模板
        template <class Archive>
        void serialize(Archive& ar)
        {
            ar(version, extended, up_right, octaves, octave_layers, hessian_threshold, roi.x, roi.y, roi.width, roi.height);
        }
    };

    //标签信息结构体
    struct TagInfo
    {
        int id = 0;
        int parent = -1;
        float zoom = 1.0;
        std::string name;
        std::string value;
        cv::Point2i fixed_offset;
        //序列化模板
        template <class Archive>
        void save(Archive& ar) const
        {
            int x, y;
            x = fixed_offset.x;
            y = fixed_offset.y;
            ar(id, parent, zoom, name, value, x, y);
        }
        template <class Archive>
        void load(Archive& ar)
        {
            int x, y;
            ar(id, parent, zoom, name, value, x, y);
            fixed_offset.x = x;
            fixed_offset.y = y;
        }
    };


    //缓存文件结构
    struct CacheFile
    {
        Setting setting;
        std::map<int, TagInfo> tag_info_map;
        std::vector<cv::KeyPoint> key_points;
        cv::Mat descriptors;
    private:
        //特征点容器定义，压缩特征点数据
        struct _Sample_KeyPoint
        {
            float_t x;
            float_t y;
            float_t response;
            int32_t class_id;
            //序列化模板
            template <class Archive>
            void serialize(Archive& ar)
            {
                ar(x, y, response, class_id);
            }
        };

    public:
        //序列化模板
        template <class Archive>
        void save(Archive& ar) const
        {
            //序列化OpenCV的数据
            //优化特征点数据
            std::vector<_Sample_KeyPoint> sample_key_points;
            for (const auto& kp : key_points)
            {
                _Sample_KeyPoint sample_kp;
                sample_kp.x = kp.pt.x;
                sample_kp.y = kp.pt.y;
                sample_kp.response = kp.response;
                sample_kp.class_id = kp.class_id;
                sample_key_points.emplace_back(sample_kp);
            }

#ifdef _FP16
            //优化特征矩阵数据，转换为半精度，并使用char数组保存
            cv::Mat descriptors_half;
            descriptors.convertTo(descriptors_half, CV_16F);
            std::vector<char> descriptors_vector(descriptors_half.data,
                descriptors_half.data + descriptors_half.total() * 2);
#else
            std::vector<char> descriptors_vector(descriptors.data, descriptors.data + descriptors.total() * 4);
#endif // _FP16
            //矩阵shape
            int descriptor_shape[2] = { descriptors.rows, descriptors.cols };
            //序列化数据
            ar(setting, tag_info_map, sample_key_points, descriptor_shape, descriptors_vector);
        }

        //反序列化模板
        template <class Archive>
        void load(Archive& ar)
        {
            //准备反序列化的中间容器
            std::vector<_Sample_KeyPoint> sample_key_points; //压缩后的特征点数据
            std::vector<char> descriptors_vector; //压缩后的特征矩阵数据
            int descriptor_shape[2]; //矩阵尺寸数据
            //反序列化数据
            ar(setting, tag_info_map, sample_key_points, descriptor_shape, descriptors_vector);
            //解压特征点
            key_points.clear();
            for (const auto& sample_kp : sample_key_points)
            {
                cv::KeyPoint kp;
                kp.pt.x = sample_kp.x;
                kp.pt.y = sample_kp.y;
                kp.size = 16.0;
                kp.response = sample_kp.response;
                kp.angle = 270.0;
                kp.octave = 0;
                kp.class_id = sample_kp.class_id;
                key_points.emplace_back(kp);
            }
#ifdef _FP16
            //解压特征矩阵
            auto descriptors_half = cv::Mat(descriptor_shape[0], descriptor_shape[1], CV_16F);
            memcpy(descriptors_half.data, descriptors_vector.data(), descriptors_vector.size());
            descriptors_half.convertTo(descriptors, CV_32F);
#else
            descriptors = cv::Mat(descriptor_shape[0], descriptor_shape[1], CV_32F);
            memcpy(descriptors.data, descriptors_vector.data(), descriptors_vector.size());
#endif // _FP16
        }
    };

    using CacheInfo = CacheFile;

    inline std::shared_ptr<CacheInfo> Deserialize(const std::string& fileName)
    {
        auto cache_info = std::make_shared<CacheInfo>();
        std::ifstream ifs(fileName, std::ios::binary);
        cereal::BinaryInputArchive archive(ifs);
        archive(*cache_info);
        ifs.close();
        return cache_info;
    }

    inline void Serialize(const std::string& fileName, std::shared_ptr<CacheInfo> cacheInfo)
    {
        std::ofstream ofs(fileName, std::ios::binary);
        cereal::BinaryOutputArchive archive(ofs);
        archive(*cacheInfo);
        ofs.close();
    }
}
