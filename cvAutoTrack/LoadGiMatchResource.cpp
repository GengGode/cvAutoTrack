#include "pch.h"
#include "LoadGiMatchResource.h"

#include "resource.h"

BOOL HBitmap2Mat(HBITMAP& _hBmp, cv::Mat& _mat)
{
	//BITMAP操作
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	//mat操作
	cv::Mat v_mat;
	v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
	//v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8UC3, nChannels));
	GetBitmapBits(_hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, v_mat.data);
	_mat = v_mat;
	//if (nChannels == 4)
	//{
	//	cv::cvtColor(v_mat, _mat, CV_RGBA2RGB);
	//	return TRUE;
	//}
	return FALSE;
}

LoadGiMatchResource::LoadGiMatchResource()
{
	HBITMAP gHmp;
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP1));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile"; 
	HBitmap2Mat(gHmp, PaimonTemplate);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP2));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, MapTemplate);

	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID_));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UID);

	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID0));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[0]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID1));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[1]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID2));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[2]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID3));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[3]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID4));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[4]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID5));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[5]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID6));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[6]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID7));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[7]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID8));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[8]);
	gHmp = LoadBitmap(GetModuleHandle(L"CVAUTOTRACK.dll"), MAKEINTRESOURCE(IDB_BITMAP_UID9));
	if (gHmp == NULL)throw"LoadSource Get Resource From Dll HBitmap faile";
	HBitmap2Mat(gHmp, UIDnumber[9]);

	UIDnumberRGBA2A();

}

LoadGiMatchResource::~LoadGiMatchResource()
{

}

void LoadGiMatchResource::UIDnumberRGBA2A()
{
	std::vector<cv::Mat> channels;

	cv::cvtColor(UID, UID, cv::COLOR_RGBA2GRAY);
	//split(UID, channels);
	//UID = channels[3];

	for (int i = 0; i < 10; i++)
	{
		//split(UIDnumber[i], channels);
		//UIDnumber[i] = channels[3];
		cv::cvtColor(UIDnumber[i], UIDnumber[i], cv::COLOR_RGBA2GRAY);
	}
}

double dis(std::vector<cv::Point> &TransfornHistory)
{
	return 0;
}
double dis(cv::Point p)
{
	return sqrt(p.x*p.x + p.y*p.y);
}
cv::Point SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy)
{
	cv::Point pos;
	double meanx = sumx / lisx.size(); //均值
	double meany = sumy / lisy.size(); //均值
	int x = (int)meanx;
	int y = (int)meany;
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
		int numx = 0;
		int numy = 0;
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
		int x = (int)(sumx / numx);
		int y = (int)(sumy / numy);
		pos = cv::Point(x, y);
	}
	else
	{
		pos = cv::Point(x, y);
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