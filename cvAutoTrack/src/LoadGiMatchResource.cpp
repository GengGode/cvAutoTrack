#include "pch.h"
#include "LoadGiMatchResource.h"


BOOL HBitmap2Mat(HBITMAP& _hBmp, cv::Mat& _mat)
{
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	cv::Mat v_mat;
	v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
	GetBitmapBits(_hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, v_mat.data);
	_mat = v_mat;
	return FALSE;
}

BOOL LoadPNG2Mat(cv::Mat& _mat)
{
	//HRESULT hr = S_OK;
	HMODULE hModu = NULL;
	IWICStream *pIWICStream = NULL;
	IWICBitmapDecoder *pIDecoder = NULL;
	IWICBitmapFrameDecode *pIDecoderFrame = NULL;
	IWICImagingFactory *m_pIWICFactory = NULL;
	IWICBitmapSource *bitmap_source = NULL; //
	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;

	hModu = GetModuleHandleW(L"CVAUTOTRACK.dll");

	if (hModu == NULL) throw "Get Dll Instance Fail!";

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pIWICFactory)
	);

	imageResHandle = FindResource(hModu, MAKEINTRESOURCE(IDB_PNG_GIMAP), L"PNG");
	imageResDataHandle = LoadResource(hModu, imageResHandle);
	pImageFile = LockResource(imageResDataHandle);
	imageFileSize = SizeofResource(hModu, imageResHandle);
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
		bitmap_source->CopyPixels(NULL, width * depht, buffer.size(), buffer.data());

		HBITMAP bitmap = CreateBitmap(width, height, 1, 8 * depht, buffer.data());

		HBitmap2Mat(bitmap, _mat);

		buffer.resize(0);
		buffer.reserve(0);

		DeleteObject(bitmap);
	}
	DeleteObject(bitmap_source);
	CoUninitialize();


	if (_mat.empty())
	{
		return false;
	}
	cv::cvtColor(_mat, _mat, cv::COLOR_RGBA2RGB);
	if (_mat.empty())
	{
		return false;
	}
	return true;
}

LoadGiMatchResource::LoadGiMatchResource()
{
	install();
}

LoadGiMatchResource::~LoadGiMatchResource()
{
	release();
}

void LoadGiMatchResource::install()
{
	HBITMAP gHmp;
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_PAIMON));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, PaimonTemplate);

	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_STAR));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, StarTemplate);

	//gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_GIMAP));
	//if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	//HBitmap2Mat(gHmp, MapTemplate);

	if (LoadPNG2Mat(MapTemplate) == false)throw"LoadSource Get Resource From Dll HBitmap faile";

	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID_));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UID);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID0));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[0]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID1));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[1]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID2));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[2]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID3));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[3]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID4));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[4]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID5));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[5]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID6));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[6]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID7));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[7]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID8));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[8]);
	gHmp = LoadBitmap(GetModuleHandleW(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID9));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[9]);
	
	StarRGBA2A();
	UIDnumberRGBA2A();
}

void LoadGiMatchResource::release()
{
	PaimonTemplate.release();
	StarTemplate.release();
	MapTemplate.release();
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
}

void LoadGiMatchResource::StarRGBA2A()
{
	cv::cvtColor(StarTemplate, StarTemplate, cv::COLOR_RGBA2GRAY);
}

void LoadGiMatchResource::UIDnumberRGBA2A()
{
	cv::cvtColor(UID, UID, cv::COLOR_RGBA2GRAY);
	for (int i = 0; i < 10; i++)
	{
		cv::cvtColor(UIDnumber[i], UIDnumber[i], cv::COLOR_RGBA2GRAY);
	}
}

double dis(std::vector<cv::Point> &TransfornHistory)
{
	return 0;
}
double dis(cv::Point2d p)
{
	return sqrt(p.x*p.x + p.y*p.y);
}
cv::Point2d SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy)
{
	cv::Point2d pos;
	double meanx = sumx / lisx.size(); //均值
	double meany = sumy / lisy.size(); //均值
	double x = meanx;
	double y = meany;
	if (lisx.size() > 3 && lisy.size() > 3)
	{
		double accumx = 0.0;
		double accumy = 0.0;
		for (int i = 0; i < (lisx.size()>lisy.size()? lisy.size(): lisx.size()); i++)
		{
			accumx += (lisx[i] - meanx)*(lisx[i] - meanx);
			accumy += (lisy[i] - meany)*(lisy[i] - meany);
		}

		double stdevx = sqrt(accumx / (lisx.size() - 1)); //标准差
		double stdevy = sqrt(accumy / (lisy.size() - 1)); //标准差

		sumx = 0;
		sumy = 0;
		double numx = 0;
		double numy = 0;
		for (int i = 0; i < (lisx.size() > lisy.size() ? lisy.size() : lisx.size()); i++)
		{
			if (abs(lisx[i] - meanx) < 1 * stdevx)
			{
				sumx += lisx[i];
				numx++;
			}

			if (abs(lisy[i] - meany) < 1 * stdevy)
			{
				sumy += lisy[i];
				numy++;
			}
		}
		double x = sumx / numx;
		double y = sumy / numy;
		pos = cv::Point2d(x, y);
	}
	else
	{
		pos = cv::Point2d(x, y);
	}
	return pos;
}

int getMaxID(double lis[], int len)
{
	int maxId = 0;
	for (int i = 1; i < len; i++)
	{
		if (lis[i] > lis[maxId])
		{
			maxId = i;
		}
	}
	return maxId;
}

int getMinID(double lis[], int len)
{
	int maxId = 0;
	for (int i = 1; i < len; i++)
	{
		if (lis[i] < lis[maxId])
		{
			maxId = i;
		}
	}
	return maxId;
}

std::vector<cv::Point2f> Vector2UnitVector(std::vector<cv::Point2f> pLis)
{
	double length = 1;
	std::vector<cv::Point2f> res;
	for (int i = 0; i < pLis.size(); i++)
	{
		length = sqrt(pLis[i].x*pLis[i].x + pLis[i].y*pLis[i].y);
		res.push_back(cv::Point2f((float)(pLis[i].x / length), (float)(pLis[i].y / length)));
	}
	return res;
}

double Line2Angle(cv::Point2f p)
{
	const double rad2degScale = 180 / CV_PI;
	double res = atan2(-p.y, p.x)*rad2degScale;
	res = res - 90; //从屏幕空间左侧水平线为0度转到竖直向上为0度
	if (res > 270.0)res = res - 360;
	return res;
}

cv::Point2d TransferTianLiAxes(cv::Point2d pos, cv::Point2d origin, double scale)
{
	return cv::Point2d((pos + origin)*scale);
}

cv::Point2d TransferUserAxes(cv::Point2d pos, double x, double y, double scale)
{
	return cv::Point2d((pos.x + x)*scale, (pos.y + y)*scale);
}