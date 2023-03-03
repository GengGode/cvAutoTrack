#include "pch.h"
#include "utils.log.h"

namespace TianLi::Utils
{
	TianLi::Utils::Log::Log()
	{
	}

	TianLi::Utils::Log::~Log()
	{
	}

	Log& TianLi::Utils::Log::getInstance()
	{
		static Log instance;
		return instance;
	}

	Log& TianLi::Utils::Log::operator=(const std::pair<int, std::string>& err_code_msg)
	{
		const int& code = err_code_msg.first;
		const std::string& msg = err_code_msg.second;

		this->error_code = code;
		if (code == 0)
		{
			//fprintf_s(fptr, "清空错误堆栈\n");
			error_code_msg_list.clear();
		}
		else
		{
			//writeFile("%s | 错误代码：%-6d，错误信息：%s\n", time_stamp().c_str(), code, msg.c_str());
			if (is_use_file == false) return *this;
			if (log_file.is_open() == false)
			{
				log_file.open("./autoTrack.Log", std::ios::in | std::ios::out | std::ios::app);
				if (log_file.is_open() == false) return *this;
				//log_file << "系统版本号：" << get_sys_version() << "\n";
				//log_file << "显卡：" << get_gpu_name() << "\n";
			}
			//write_Log(log_file, time_stamp(), code, msg);
			log_file.flush();
			push(code, msg);
		}

		return *this;
	}

	Log::operator int()
	{
		return this->error_code;
	}
	std::ostream& TianLi::Utils::operator<<(std::ostream& os, const Log& err)
	{
		for (int i = static_cast<int>(err.error_code_msg_list.size()) - 1; i >= 0; i--)
		{
			for (int j = 1; j < err.error_code_msg_list.size() - i; j++)
			{
				os << "  ";
			}
			if (i == 0)
			{
				os << "\u2514\u2500\u2500\u2500\u2192";
			}
			else
			{
				os << "\u2514\u2500\u252C\u2500\u2192";
			}
			os << std::to_string(err.error_code_msg_list[i].err_code) + ": " + err.error_code_msg_list[i].err_msg + '\n';
		}
		return os;
	}

	bool Log::disableWirteFile()
	{
		is_use_file = false;
		if (log_file.is_open())
		{
			log_file.close();
		}
		return true;
	}

	bool Log::enableWirteFile()
	{
		is_use_file = true;
		return true;
	}

	int Log::getLastError()
	{
		return error_code;
	}

	std::string Log::getLastErrorMsg()
	{
		if (this->error_code != 0)
		{
			return std::to_string(error_code_msg_list.back().err_code) + ": " + error_code_msg_list.back().err_msg;
		}
		else
		{
			return "0: Successfully called";
		}
	}

	std::string Log::toJson()
	{
		std::string json;
		json += "{";
		json += "\"errorCode\":" + std::to_string(this->error_code) + ",";
		json += "\"errorList\":[";
		for (auto& item : error_code_msg_list)
		{
			json += "{\"code\":" + std::to_string(item.err_code) + ",\"msg\":\"" + item.err_msg + "\"},";
		}
		if (json.back() == ',')
		{
			json.pop_back();
		}
		json += "]";
		json += "}";
		return json;
	}

	void Log::push(int code, std::string msg)
	{
		error_code_msg_list.push_back({ code,msg });

		if (error_code_msg_list.size() > 9)
		{
			std::vector<Log::LogItem>::iterator index = error_code_msg_list.begin();
			error_code_msg_list.erase(index);
		}
	}

	bool Log::clear_error_Logs()
	{
		Log::getInstance() = { 0,"Successfully called" };
		return true;
	}
}