#pragma once
namespace TianLi::Resources::Import
{
	static int MAP_BLOCK_SIZE = 1024;
	
	struct MapBlock
	{
		int block_size = MAP_BLOCK_SIZE;
		cv::Mat origin;
		cv::Mat map;
		MapBlock() {}
		~MapBlock() {}
		MapBlock(cv::Mat origin) : origin(origin) {
			cv::resize(origin, map, cv::Size(block_size, block_size), cv::INTER_CUBIC);
		}
	};
	
	const int IMPORT_TYPE = CV_8UC4;
	
	class MapImporter
	{
		std::map<std::pair<int, int>, MapBlock> map;
	public:
		MapImporter() = default;
		~MapImporter() = default;
		MapImporter(const MapImporter&) = delete;
		MapImporter& operator=(const MapImporter&) = delete;
		//static MapImporter& getInstance();

		bool set_map_block(int id_x, int id_y, cv::Mat mat)
		{
			if (mat.empty())
			{
				return false;
			}
			map[std::make_pair(id_x, id_y)] = { mat };
			return true;
		}
		bool get_map_block(int id_x, int id_y, cv::Mat& mat)
		{
			if (map.find(std::make_pair(id_x, id_y)) == map.end())
			{
				return false;
			}
			mat = map[std::make_pair(id_x, id_y)].map;
			return true;
		}
		bool get_all_map(cv::Mat& mat)
		{
			if (map.empty())
			{
				return false;
			}
			int width = 0;
			int height = 0;
			for (auto& i : map)
			{
				if (i.second.map.empty())
				{
					return false;
				}
				width += i.second.map.cols;
				height = i.second.map.rows;
			}
			mat = cv::Mat(height, width, IMPORT_TYPE);
			int x = 0;
			for (auto& i : map)
			{
				i.second.map.copyTo(mat(cv::Rect(x, 0, i.second.map.cols, i.second.map.rows)));
				x += i.second.map.cols;
			}
			return true;
		}
	};
	
	bool set_map_block(int id_x, int id_y, cv::Mat mat);

	cv::Mat import_mat_from_data(int width, int height, const unsigned char* data, int data_size);
	
}