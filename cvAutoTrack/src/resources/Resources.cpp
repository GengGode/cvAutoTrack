#include "pch.h"
#include "Resources.h"
#include "resource.h"
#include "resources/import/resources.import.h"
#include <wincodec.h>

#include <iostream>
#include <fstream>

#include "version/Version.h"

namespace TianLi::Resource::Utils
{
	void LoadBitmap_ID2Mat(int IDB, cv::Mat& mat)
	{
		auto H_Handle = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB));
		BITMAP bmp;
		GetObject(H_Handle, sizeof(BITMAP), &bmp);
		int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
		//int depth = bmp.bmBitsPixel == 1 ? 1 : 8;
		cv::Mat v_mat;
		v_mat.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
		GetBitmapBits(H_Handle, bmp.bmHeight * bmp.bmWidth * nChannels, v_mat.data);
		mat = v_mat;
	}
	bool HBitmap2MatAlpha(HBITMAP& _hBmp, cv::Mat& _mat)
	{
		//BITMAP操作
		BITMAP bmp;
		GetObject(_hBmp, sizeof(BITMAP), &bmp);
		int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
		//int depth = bmp.bmBitsPixel == 1 ? 1 : 8;
		//mat操作
		cv::Mat v_mat;
		v_mat.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
		GetBitmapBits(_hBmp, bmp.bmHeight * bmp.bmWidth * nChannels, v_mat.data);
		_mat = v_mat;
		return true;
	}
	//bool
	//	imdecode_(const cv::Mat& buf, int flags, cv::Mat& mat)
	//{
	//	CV_Assert(!buf.empty());
	//	CV_Assert(buf.isContinuous());
	//	CV_Assert(buf.checkVector(1, CV_8U) > 0);
	//	cv::Mat buf_row = buf.reshape(1, 1);  // decoders expects single row, avoid issues with vector columns

	//	cv::String filename;

	//	cv::ImageDecoder decoder = findDecoder(buf_row);
	//	if (!decoder)
	//		return 0;

	//	int scale_denom = 1;
	//	if (flags > IMREAD_LOAD_GDAL)
	//	{
	//		if (flags & IMREAD_REDUCED_GRAYSCALE_2)
	//			scale_denom = 2;
	//		else if (flags & IMREAD_REDUCED_GRAYSCALE_4)
	//			scale_denom = 4;
	//		else if (flags & IMREAD_REDUCED_GRAYSCALE_8)
	//			scale_denom = 8;
	//	}

	//	/// set the scale_denom in the driver
	//	decoder->setScale(scale_denom);

	//	if (!decoder->setSource(buf_row))
	//	{
	//		filename = tempfile();
	//		FILE* f = fopen(filename.c_str(), "wb");
	//		if (!f)
	//			return 0;
	//		size_t bufSize = buf_row.total() * buf.elemSize();
	//		if (fwrite(buf_row.ptr(), 1, bufSize, f) != bufSize)
	//		{
	//			fclose(f);
	//			CV_Error(Error::StsError, "failed to write image data to temporary file");
	//		}
	//		if (fclose(f) != 0)
	//		{
	//			CV_Error(Error::StsError, "failed to write image data to temporary file");
	//		}
	//		decoder->setSource(filename);
	//	}

	//	bool success = false;
	//	try
	//	{
	//		if (decoder->readHeader())
	//			success = true;
	//	}
	//	catch (const cv::Exception& e)
	//	{
	//		std::cerr << "imdecode_('" << filename << "'): can't read header: " << e.what() << std::endl << std::flush;
	//	}
	//	catch (...)
	//	{
	//		std::cerr << "imdecode_('" << filename << "'): can't read header: unknown exception" << std::endl << std::flush;
	//	}
	//	if (!success)
	//	{
	//		decoder.release();
	//		if (!filename.empty())
	//		{
	//			if (0 != remove(filename.c_str()))
	//			{
	//				std::cerr << "unable to remove temporary file:" << filename << std::endl << std::flush;
	//			}
	//		}
	//		return 0;
	//	}

	//	// established the required input image size
	//	Size size = validateInputImageSize(Size(decoder->width(), decoder->height()));

	//	int type = decoder->type();
	//	if ((flags & IMREAD_LOAD_GDAL) != IMREAD_LOAD_GDAL && flags != IMREAD_UNCHANGED)
	//	{
	//		if ((flags & IMREAD_ANYDEPTH) == 0)
	//			type = CV_MAKETYPE(CV_8U, CV_MAT_CN(type));

	//		if ((flags & IMREAD_COLOR) != 0 ||
	//			((flags & IMREAD_ANYCOLOR) != 0 && CV_MAT_CN(type) > 1))
	//			type = CV_MAKETYPE(CV_MAT_DEPTH(type), 3);
	//		else
	//			type = CV_MAKETYPE(CV_MAT_DEPTH(type), 1);
	//	}

	//	mat.create(size.height, size.width, type);

	//	success = false;
	//	try
	//	{
	//		if (decoder->readData(mat))
	//			success = true;
	//	}
	//	catch (const cv::Exception& e)
	//	{
	//		std::cerr << "imdecode_('" << filename << "'): can't read data: " << e.what() << std::endl << std::flush;
	//	}
	//	catch (...)
	//	{
	//		std::cerr << "imdecode_('" << filename << "'): can't read data: unknown exception" << std::endl << std::flush;
	//	}

	//	if (!filename.empty())
	//	{
	//		if (0 != remove(filename.c_str()))
	//		{
	//			std::cerr << "unable to remove temporary file:" << filename << std::endl << std::flush;
	//		}
	//	}

	//	if (!success)
	//	{
	//		mat.release();
	//		return false;
	//	}

	//	if (decoder->setScale(scale_denom) > 1) // if decoder is JpegDecoder then decoder->setScale always returns 1
	//	{
	//		resize(mat, mat, Size(size.width / scale_denom, size.height / scale_denom), 0, 0, INTER_LINEAR_EXACT);
	//	}

	//	/// optionally rotate the data if EXIF' orientation flag says so
	//	if (!mat.empty() && (flags & IMREAD_IGNORE_ORIENTATION) == 0 && flags != IMREAD_UNCHANGED)
	//	{
	//		ApplyExifOrientation(decoder->getExifTag(ORIENTATION), mat);
	//	}

	//	return true;
	//}
	void LoadPng_ID2Mat(int IDB, cv::Mat& mat)
	{
		IWICStream* pIWICStream = NULL;
		IWICBitmapDecoder* pIDecoder = NULL;
		IWICBitmapFrameDecode* pIDecoderFrame = NULL;
		IWICBitmapSource* bitmap_source = NULL;
		HRSRC imageResHandle = NULL;
		HGLOBAL imageResDataHandle = NULL;
		void* pImageFile = NULL;
		DWORD imageFileSize = 0;
		IWICImagingFactory* m_pIWICFactory = NULL;

		HMODULE hModu = GetModuleHandleW(L"CVAUTOTRACK.dll");

		if (hModu == NULL) throw "Get Dll Instance Fail!";

		imageResHandle = FindResource(hModu, MAKEINTRESOURCE(IDB), L"PNG");
		imageResDataHandle = LoadResource(hModu, imageResHandle);
		pImageFile = LockResource(imageResDataHandle);
		imageFileSize = SizeofResource(hModu, imageResHandle);
		/*
		{
			// 2nd
			char * ptr_data = (char*)malloc(imageFileSize);
			memcpy(ptr_data, pImageFile, imageFileSize);
			// to bin file
			cv::Mat mat_array = cv::Mat(1, (int)imageFileSize, CV_8UC1, (void*)ptr_data);
			// Debug下 imdecode 中 validateInputImageSize 中的 CV_IO_MAX_IMAGE_WIDTH 为0导致CV_Assert失败
			cv::Mat v_mat = cv::imdecode(mat_array, cv::IMREAD_UNCHANGED);
			mat = v_mat;
			free(ptr_data);
		}
		*/

		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pIWICFactory)
		);
		
		m_pIWICFactory->CreateStream(&pIWICStream);

		pIWICStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize);
		m_pIWICFactory->CreateDecoderFromStream(
			pIWICStream,                   // The stream to use to create the decoder
			NULL,                          // Do not prefer a particular vendor
			WICDecodeMetadataCacheOnLoad,  // Cache metadata when needed
			&pIDecoder);                   // Pointer to the decoder
		pIDecoder->GetFrame(0, &pIDecoderFrame);

		bitmap_source = pIDecoderFrame;

		UINT width = 0, height = 0, depht = 4;
		bitmap_source->GetSize(&width, &height);
		{
			std::vector<BYTE> buffer(width * height * depht);
			bitmap_source->CopyPixels(NULL, width * depht, static_cast<UINT>(buffer.size()), buffer.data());
			HBITMAP hPngMat = CreateBitmap(width, height, 1, 8 * depht, buffer.data());
			HBitmap2MatAlpha(hPngMat, mat);
			DeleteObject(hPngMat);
		}
		DeleteObject(bitmap_source);
		CoFreeUnusedLibraries();
		CoUninitialize();
	}
}
using namespace TianLi::Resource::Utils;
#ifdef USED_BINARY_IMAGE
#include "resources.load.h"
#endif // 

Resources::Resources()
{
#ifdef USED_BINARY_IMAGE
	PaimonTemplate = TianLi::Resources::Load::load_image("paimon");
	StarTemplate = TianLi::Resources::Load::load_image("star");
	MinimapCailbTemplate = TianLi::Resources::Load::load_image("cailb");
	UID = TianLi::Resources::Load::load_image("uid_");
	UIDnumber[0] = TianLi::Resources::Load::load_image("uid0");
	UIDnumber[1] = TianLi::Resources::Load::load_image("uid1");
	UIDnumber[2] = TianLi::Resources::Load::load_image("uid2");
	UIDnumber[3] = TianLi::Resources::Load::load_image("uid3");
	UIDnumber[4] = TianLi::Resources::Load::load_image("uid4");
	UIDnumber[5] = TianLi::Resources::Load::load_image("uid5");
	UIDnumber[6] = TianLi::Resources::Load::load_image("uid6");
	UIDnumber[7] = TianLi::Resources::Load::load_image("uid7");
	UIDnumber[8] = TianLi::Resources::Load::load_image("uid8");
	UIDnumber[9] = TianLi::Resources::Load::load_image("uid9");

	cv::cvtColor(StarTemplate, StarTemplate, cv::COLOR_RGB2GRAY);
	cv::cvtColor(UID, UID, cv::COLOR_RGB2GRAY);
	for (int i = 0; i < 10; i++)
	{
		cv::cvtColor(UIDnumber[i], UIDnumber[i], cv::COLOR_RGB2GRAY);
	}
#endif // 
	LoadBitmap_ID2Mat(IDB_BITMAP_PAIMON, PaimonTemplate);
	LoadBitmap_ID2Mat(IDB_BITMAP_STAR, StarTemplate);

	LoadPng_ID2Mat(IDB_PNG_MINIMAP_CAILB, MinimapCailbTemplate);

	LoadBitmap_ID2Mat(IDB_BITMAP_UID_, UID);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID0, UIDnumber[0]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID1, UIDnumber[1]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID2, UIDnumber[2]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID3, UIDnumber[3]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID4, UIDnumber[4]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID5, UIDnumber[5]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID6, UIDnumber[6]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID7, UIDnumber[7]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID8, UIDnumber[8]);
	LoadBitmap_ID2Mat(IDB_BITMAP_UID9, UIDnumber[9]);

	cv::cvtColor(StarTemplate, StarTemplate, cv::COLOR_RGBA2GRAY);
	cv::cvtColor(UID, UID, cv::COLOR_RGBA2GRAY);
	for (int i = 0; i < 10; i++)
	{
		cv::cvtColor(UIDnumber[i], UIDnumber[i], cv::COLOR_RGBA2GRAY);
	}
	
	// install();
}

Resources::~Resources()
{
	PaimonTemplate.release();
	MinimapCailbTemplate.release();
	StarTemplate.release();
	
	UID.release();
	UIDnumber[0].release();
	UIDnumber[1].release();
	UIDnumber[2].release();
	UIDnumber[3].release();
	UIDnumber[4].release();
	UIDnumber[5].release();
	UIDnumber[6].release();
	UIDnumber[7].release();
	UIDnumber[8].release();
	UIDnumber[9].release();
	release();
}

Resources& Resources::getInstance()
{
	static Resources instance;
	return instance;
}

void Resources::install()
{
	if (is_installed == false)
	{
		LoadPng_ID2Mat(IDB_PNG_GIMAP, MapTemplate);
		is_installed = true;
	}
}

void Resources::release()
{
	if (is_installed == true)
	{
		MapTemplate.release();
		MapTemplate = cv::Mat();
		is_installed = false;
	}
}

bool Resources::map_is_embedded()
{
	return true;
}

bool save_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
	cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(hessian_threshold, octaves, octave_layers, extended, upright);
	detector->detectAndCompute(Resources::getInstance().MapTemplate, cv::noArray(), keypoints, descriptors);

	cv::FileStorage fs("cvAutoTrack_Cache.xml", cv::FileStorage::WRITE);

	std::string build_time = __DATE__ " " __TIME__;
	fs << "build_time" << build_time;
	fs << "build_version" << TianLi::Version::build_version;
	fs << "hessian_threshold" << hessian_threshold;
	fs << "octaves" << octaves;
	fs << "octave_layers" << octave_layers;
	fs << "extended" << extended;
	fs << "upright" << upright;

	fs << "keypoints" << keypoints;
	fs << "descriptors" << descriptors;

	fs << "build_version_end" << TianLi::Version::build_version;		//防止缓存写入一半导致过期
	fs.release();
	return true;
}
bool load_map_keypoint_cache(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
	cv::FileStorage fs;
	if (std::filesystem::exists("cvAutoTrack_Cache.xml") == false)
	{
		return false;
	}
	try {
		fs = cv::FileStorage("cvAutoTrack_Cache.xml", cv::FileStorage::READ);
	}
	catch (std::exception)		//缓存损坏
	{
		return false;
	};
	//获取xml版本，如果不符，则重建
	std::string r_build_version = "";
	std::string r_build_version_end = "";
	fs["build_version"] >> r_build_version;
	fs["build_version_end"] >> r_build_version_end;

	//版本不匹配
	if (r_build_version != TianLi::Version::build_version || r_build_version_end != TianLi::Version::build_version)
	{
		return false;
	}
	

	double r_hessian_threshold = 1;
	int    r_octaves = 1;
	int    r_octave_layers = 1;
	bool   r_extended = false;
	bool   r_upright = false;

	fs["hessian_threshold"] >> r_hessian_threshold;
	fs["octaves"] >> r_octaves;
	fs["octave_layers"] >> r_octave_layers;
	fs["extended"] >> r_extended;
	fs["upright"] >> r_upright;

	if (r_hessian_threshold != hessian_threshold || r_octaves != octaves || r_octave_layers != octave_layers || r_extended != extended || r_upright != upright)
	{
		return false;
	}

	fs["keypoints"] >> keypoints;
	fs["descriptors"] >> descriptors;
	fs.release();
	return true;
}

bool get_map_keypoint(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, double hessian_threshold, int octaves, int octave_layers, bool extended, bool upright)
{
	if (std::filesystem::exists("cvAutoTrack_Cache.xml") == false)
	{
		return save_map_keypoint_cache(keypoints, descriptors, hessian_threshold, octaves, octave_layers, extended, upright);
	}
	else
	{
		if (load_map_keypoint_cache(keypoints, descriptors, hessian_threshold, octaves, octave_layers, extended, upright) == false)
		{
			return save_map_keypoint_cache(keypoints, descriptors, hessian_threshold, octaves, octave_layers, extended, upright);
		}
		else
		{
			return true;
		}
	}
}