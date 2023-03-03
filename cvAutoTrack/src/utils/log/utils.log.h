#pragma once
namespace TianLi::Utils
{
	class Log
	{
		struct LogItem
		{
			int err_code;
			std::string err_msg;
		};
	private:
		Log();
	public:
		~Log();
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;
		static Log& getInstance();
		Log& operator=(const std::pair<int, std::string>& err_code_msg);
		operator int();
		friend std::ostream& operator<<(std::ostream& os, const Log& err);
		
		bool disableWirteFile();
		bool enableWirteFile();
	public:
		int getLastError();
		std::string getLastErrorMsg();
		std::string toJson();
		
	private:
		std::fstream log_file;
		bool is_use_file = false;
		
		int error_code = 0;
		std::vector<LogItem> error_code_msg_list;
	private:
		void push(int code, std::string msg);
		
	public:
		static bool clear_error_Logs();
	};
}