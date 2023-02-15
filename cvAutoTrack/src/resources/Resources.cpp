#include "pch.h"
#include "Resources.h"
#include "resource.h"
#include <wincodec.h>
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

		auto hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (SUCCEEDED(hr))
		{
			hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&m_pIWICFactory)
			);
		}
		if (SUCCEEDED(hr))
		{
			imageResHandle = FindResource(hModu, MAKEINTRESOURCE(IDB), L"PNG");
			
		}
		if (imageResHandle != NULL)
		{
			imageResDataHandle = LoadResource(hModu, imageResHandle);
		}
		if (imageResDataHandle != NULL)
		{
			pImageFile = LockResource(imageResDataHandle);
		}
		if (pImageFile != NULL)
		{
			imageFileSize = SizeofResource(hModu, imageResHandle);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pIWICFactory->CreateStream(&pIWICStream);
		}
		if (SUCCEEDED(hr))
		{
			hr = pIWICStream->InitializeFromMemory((BYTE*)pImageFile, imageFileSize);
		}
		if (SUCCEEDED(hr))
		{
			m_pIWICFactory->CreateDecoderFromStream(
				pIWICStream,                   // The stream to use to create the decoder
				NULL,                          // Do not prefer a particular vendor
				WICDecodeMetadataCacheOnLoad,  // Cache metadata when needed
				&pIDecoder);                   // Pointer to the decoder
		}
		if (SUCCEEDED(hr))
		{
			hr = pIDecoder->GetFrame(0, &pIDecoderFrame);
		}
		
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
//#include "resources.load.h"
Resources::Resources()
{
	//auto mat = TianLi::Resources::Load::load_image("paimon");
	//mat = TianLi::Resources::Load::load_image("cailb");
	//mat = TianLi::Resources::Load::load_image("star");
	//mat = TianLi::Resources::Load::load_image("uid_");
	//mat = TianLi::Resources::Load::load_image("uid0");
	//mat = TianLi::Resources::Load::load_image("uid1");
	//mat = TianLi::Resources::Load::load_image("uid2");
	//mat = TianLi::Resources::Load::load_image("uid3");
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
		//LoadXml(xmlPtr);
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
		//ReleaseXml(xmlPtr);
		is_installed = false;
	}
}

void Resources::get_map_keypoint_cache()
{

}