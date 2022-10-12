#include "pch.h"
#include "ErrorCode.h"

ErrorCode::ErrorCode()
{
	fopen_s(&fptr, "./autoTrack.log", "w+");
}

ErrorCode::~ErrorCode()
{
	fclose(fptr);
}

ErrorCode & ErrorCode::getInstance()
{
	static ErrorCode instance;
	return instance;
}

inline std::tm localtime_xp(std::time_t timer)
{
	std::tm bt{};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *std::localtime(&timer);
#endif
	return bt;
}

// default = "YYYY-MM-DD HH:MM:SS"
inline std::string time_stamp(const std::string& fmt = "%F %T")
{
	auto bt = localtime_xp(std::time(0));
	char buf[64];
	return { buf, std::strftime(buf, sizeof(buf), fmt.c_str(), &bt) };
}
ErrorCode& ErrorCode::operator=(const std::pair<int, string>& err_code_msg)
{
	const int& code = err_code_msg.first;
	const string& msg = err_code_msg.second;
	
	this->errorCode = code;
	if (code == 0)
	{
		//fprintf_s(fptr, "清空错误堆栈\n");
		error_code_msg_list.clear();
	}
	else
	{
		fprintf_s(fptr, "%s | 错误代码：%-6d，错误信息：%s\n", time_stamp().c_str(), code, msg.c_str());
		push(code,msg);
	}
	
	fflush(fptr);

	return *this;
}


ErrorCode::operator int()
{
	return this->errorCode;
}

int ErrorCode::getLastError()
{
	return this->errorCode;
}

string ErrorCode::getLastErrorMsg()
{
	if (this->errorCode != 0)
	{
		return to_string(error_code_msg_list.back().first) +": "+ error_code_msg_list.back().second;
	}
	else
	{
		return "0: SUCCESS";
	}
}

string ErrorCode::toJson()
{
	std::string json;
	json += "{";
	json += "\"errorCode\":" + to_string(this->errorCode) + ",";
	json += "\"errorList\":[";
	for (auto& item : error_code_msg_list)
	{
		json += "{\"code\":" + to_string(item.first) + ",\"msg\":\"" + item.second + "\"},";
	}
	if (json.back() == ',')
	{
		json.pop_back();
	}
	json += "]";
	json += "}";
	return json;
}

void ErrorCode::push(int code, string msg)
{
	error_code_msg_list.push_back({ code,msg });
	
	if (error_code_msg_list.size() > 9)
	{
		vector<pair<int, string>>::iterator index = error_code_msg_list.begin();
		error_code_msg_list.erase(index);
	}
}

ostream & operator<<(ostream & os, const ErrorCode & err)
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
		//os << "\u2514 \u2500 \u252C \u2500 \u2192";

		os << to_string(err.error_code_msg_list[i].first) + ": " + err.error_code_msg_list[i].second + '\n';
	}
	return os;
}
