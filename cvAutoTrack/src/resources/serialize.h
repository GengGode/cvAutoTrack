#pragma once
#include "pch.h"

namespace Tianli::Resources::Utils
{
	class serializeStream
	{
	private:
		std::ostream& fileOut;
	public:
		//将流对齐到size（2的幂次）的整数倍
		void align(int size = 4) {
			int p = 0;
			for (p = 0; size != 1; p++)
				size >>= 1;
			size <<= p;

			int fsLen = fileOut.tellp();
			int addLen = (fsLen >> p << p) + size - fsLen;
			if (addLen != size) {
				char* emptyBinarys = new char[addLen];
				memset(emptyBinarys, 0, addLen);
				fileOut.write(emptyBinarys, addLen);
				delete[] emptyBinarys;
			}
		}

		serializeStream(std::ostream& fileOut) :fileOut(fileOut) {}

		void operator <<(const std::string& s)
		{
			align(sizeof(DWORD));
			DWORD size = s.size() + 1;
			fileOut.write((char*)&size, sizeof(DWORD));
			fileOut.write(s.c_str(), size);
		}

		template<typename Digital>
			requires std::is_integral_v<Digital> || std::is_floating_point_v<Digital>
		void operator <<(const Digital & d)
		{
			align(sizeof(Digital));
			fileOut.write((char*)&d, sizeof(d));
		}

		void operator <<(const std::vector<cv::KeyPoint>& points)
		{
			align(sizeof(DWORD));
			DWORD size = points.size() * sizeof(cv::KeyPoint);
			fileOut.write((char*)&size, sizeof(DWORD));

			const cv::KeyPoint* keyPointPtr = points.data();
			fileOut.write((char*)keyPointPtr, size);
		}

		void operator <<(const cv::Mat& mat)
		{
			align(sizeof(DWORD));
			std::vector<uchar> buf;
			cv::imencode(".tiff", mat, buf, std::vector<int>(cv::IMWRITE_TIFF_COMPRESSION, 8));

			DWORD size = buf.size();
			fileOut.write((char*)&size, sizeof(DWORD));

			const uchar* bytes = buf.data();
			fileOut.write((char*)bytes, size);
		}
	};

	class deSerializeStream
	{
	private:
		std::istream& fileIn;
	public:
		deSerializeStream(std::istream& fileIn) :fileIn(fileIn) {}

		void align(int size = 4)		//将流对齐到size（2的幂次）的整数倍
		{
			int p = 0;
			for (p = 0; size != 1; p++)
				size >>= 1;
			size <<= p;

			int fsLen = fileIn.tellg();
			int addLen = (fsLen >> p << p) + size - fsLen;
			if (addLen != size) {
				fileIn.ignore(addLen);
			}
		}

		void operator >>(std::string& s)
		{
			align(sizeof(DWORD));
			DWORD size;
			if (fileIn.eof()) throw std::exception("尝试读取已经为空的流");
			fileIn.read((char*)&size, sizeof(DWORD));
			if (size > MAXSHORT) throw std::exception("解析到的字符串过长");

			char* bytes = new char[size];
			if (fileIn.eof()) throw std::exception("尝试读取已经为空的流");
			fileIn.read(bytes, size);

			s = std::string(bytes);

			delete[] bytes;
		}

		template<typename Digital>
			requires std::is_integral_v<Digital> || std::is_floating_point_v<Digital>
		void operator >>(Digital & d)
		{
			align(sizeof(Digital));
			if (fileIn.eof()) throw std::exception("尝试读取已经为空的流");
			fileIn.read((char*)&d, sizeof(d));
		}

		void operator >>(std::vector<cv::KeyPoint>& points)
		{
			align(sizeof(DWORD));
			DWORD size;
			if (fileIn.eof()) throw std::exception("尝试读取已经为空的流");
			fileIn.read((char*)&size, sizeof(DWORD));

			char* bytes = new char[size];
			if (fileIn.eof()) throw std::exception("尝试读取已经为空的流");
			fileIn.read(bytes, size);

			cv::KeyPoint* keyPointPtr = (cv::KeyPoint*)bytes;
			points = std::vector<cv::KeyPoint>(keyPointPtr, keyPointPtr + size / sizeof(cv::KeyPoint));

			delete[] bytes;
		}

		void operator >>(cv::Mat& mat)
		{
			align(sizeof(DWORD));
			DWORD size;
			if (fileIn.eof()) throw std::exception("尝试读取已经为空的流");
			fileIn.read((char*)&size, sizeof(DWORD));

			char* bytes = new char[size];
			if (fileIn.eof()) throw std::exception("尝试读取已经为空的流");
			fileIn.read(bytes, size);

			std::vector<uchar> inputArray(bytes, bytes + size);
			mat = cv::imdecode(inputArray, cv::IMREAD_ANYDEPTH | cv::IMREAD_UNCHANGED);

			delete[] bytes;
		}
	};
}