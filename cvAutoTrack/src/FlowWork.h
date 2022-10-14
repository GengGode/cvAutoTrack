#pragma once
#include <vector>
#include "ErrorCode.h"
using namespace std;

class AutoTrack;

class FlowWork
{
	std::vector<std::pair<bool(AutoTrack::*)(void), std::pair<int, std::string>>> fun_ptr_err_list;
	AutoTrack* at = nullptr;
	ErrorCode& err = ErrorCode::getInstance();
public:
	FlowWork();
	~FlowWork();
	
	void append(AutoTrack* at_ptr,bool(AutoTrack::* fun_ptr)(void),int err_code,std::string err_msg);

	bool run();
};

