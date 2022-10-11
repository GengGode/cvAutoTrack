#include "pch.h"
#include "FlowWork.h"
#include "AutoTrack.h"

FlowWork::FlowWork()
{
}

FlowWork::~FlowWork()
{
}

void FlowWork::append(AutoTrack* at_ptr, bool(AutoTrack::* fun_ptr)(void), int err_code, std::string err_msg)
{
	at = at_ptr;
	fun_ptr_err_list.push_back(std::make_pair(fun_ptr, std::make_pair(err_code, err_msg)));
}

bool FlowWork::run()
{
	for (auto& fun_ptr_err : fun_ptr_err_list)
	{
		if (!(at->*fun_ptr_err.first)())
		{
			err = fun_ptr_err.second;
			return false;
		}
	}
	return true;
}
