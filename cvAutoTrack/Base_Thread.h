#pragma once
#include <thread>
//#include <Windows.h>
using namespace std;

class Mat
{

};
template<class cOut, class cIn>
class Base_Thread
{
	thread *ptrThread = nullptr;

	Mat workInput;
	cOut(*ptr)(cIn in) = nullptr;
	bool isExitThread = false;
	bool isExistFunction = false;
	bool isRunWork = false;
	bool isEndWork = false;

	void run();

public:
	bool isInit = false;
	Base_Thread();
	~Base_Thread();
	Base_Thread(void*(*funPtr)(void* inMat));
	void setFunction(void*(*funPtr)(void* ));

	void start(Mat & inMat);
	bool isEnd();

};

