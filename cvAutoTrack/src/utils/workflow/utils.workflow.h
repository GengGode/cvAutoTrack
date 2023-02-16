#pragma once
namespace TianLi::Utils
{
	class Workflow
	{
		struct WorkflowItem
		{
			std::function<bool(void)> fun_ptr;
			std::pair<int, std::string> err_info;
			//int err_code;
			//std::string err_msg;
		};
		std::vector<WorkflowItem> fun_ptr_list;
	public:
		Workflow();
		~Workflow();
	
		void append(std::function<bool(void)> fun_ptr, int err_code, std::string err_msg);
		bool run();
	};
}

