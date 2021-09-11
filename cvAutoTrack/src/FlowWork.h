#pragma once
#include <vector>
#include "ErrorCode.h"
using namespace std;

class AutoTrack;

class FlowWork
{
	vector<bool(AutoTrack::*)(void)> funPtrList;
	vector<int> funErrCodeList;
	AutoTrack * AT;
	ErrorCode& err=ErrorCode::getInstance();
public:
	FlowWork();
	~FlowWork();

	void append(AutoTrack* at,bool(AutoTrack::* funPtr)(void),int errCode);

	bool run();
};

