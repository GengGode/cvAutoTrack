// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 TEST2PROJECT1_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// TEST2PROJECT1_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef TEST2PROJECT1_EXPORTS
#define TEST2PROJECT1_API __declspec(dllexport)
#else
#define TEST2PROJECT1_API __declspec(dllimport)
#endif

//坐标点类数据（x，y）
struct TEST2PROJECT1_API GPoint
{
	int x;
	int y;
	GPoint();
	GPoint(int _x, int _y);
	void toArray(int _array[2]);
};

//尺寸类数据（width，height）
struct TEST2PROJECT1_API GSize
{
	int width;
	int height;
	GSize();
	GSize(int _width, int _height);
	void toArray(int _array[2]);
	//int* toArray();
};

// 此类是从 dll 导出的
// 原神专用图像匹配类
class TEST2PROJECT1_API GGenshinImpactMatch {
public:
	GGenshinImpactMatch(void);
	~GGenshinImpactMatch(void);
	// TODO: 在此处添加方法。

	//设置匹配源图像，输入为cv::Mat类型，但需要强制转换为void*
	bool setSource(void *_source);
	bool setTarget(void *_target);

	void* getSource(int _id);
	int getMaxCount();

	GPoint getPosition();

	//BOOL isMatchEnd();

private:
	//返回相对于匹配模板的左上角的目标中心对应坐标点
	GPoint gPosition;
	//Mat列表
	void **gMat = nullptr;
	//HBITMAP列表
	void **gHmp = nullptr;
	//导入dll需要加载的bitmap的数量
	int gLoadDllResMaxCount = 14;
	//BOOL isMatchEndFlag = false;

	//加载MAT资源
	bool gLoadSource();
	//获取资源的HBITMAP
	bool gLoadSourceGetRes();
	//通过HBITMAP获取Mat
	bool gLoadSourceBitMap(void * _hBmp, void *_mat);
};

extern "C" TEST2PROJECT1_API int nTest2Project1;

TEST2PROJECT1_API int fnTest2Project1(void);
