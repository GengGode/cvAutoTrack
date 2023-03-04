#include "pch.h"
#include "utils.workflow.h"
#include "ErrorCode.h"

namespace TianLi::Utils
{
	Workflow::Workflow()
	{
	}

	Workflow::~Workflow()
	{
	}

	void Workflow::append(std::function<bool(void)> fun_ptr, int err_code, std::string err_msg)
	{
		fun_ptr_list.emplace_back(WorkflowItem{ fun_ptr,{ err_code, err_msg } });
	}

	bool Workflow::run()
	{
		for (auto& fun_ptr_item : fun_ptr_list)
		{
			if (!fun_ptr_item.fun_ptr())
			{
				ErrorCode::getInstance() = fun_ptr_item.err_info;
				return false;
			}
		}
		return true;
	}
}