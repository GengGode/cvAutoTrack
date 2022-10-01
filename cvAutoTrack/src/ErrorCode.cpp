#include "pch.h"
#include "ErrorCode.h"

ErrorCode::ErrorCode()
{
	fopen_s(&fptr, "./autoTrack.log", "w+");

	errCodeMsg.push_back("执行成功");
	errCodeMsg.push_back("未初始化");
	errCodeMsg.push_back("未能找到原神窗口句柄");
	errCodeMsg.push_back("窗口画面为空");
	errCodeMsg.push_back("未能匹配到特征点");
	errCodeMsg.push_back("原神小地图区域为空或者区域长宽小于60px");
	errCodeMsg.push_back("未能匹配到派蒙");
	errCodeMsg.push_back("特征点数组访问越界");
	errCodeMsg.push_back("未能在UID区域检测到有效UID");
	errCodeMsg.push_back("提取小箭头特征误差过大");
	errCodeMsg.push_back("无效句柄或指定句柄所指向窗口不存在");
	errCodeMsg.push_back("未能取到小箭头区域");
	errCodeMsg.push_back("窗口句柄为空");
	errCodeMsg.push_back("窗口句柄失效");
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

ErrorCode & ErrorCode::operator=(const int & code)
{
	string msg = "未定义错误信息";
	if (code < errCodeMsg.size())
	{
		msg = errCodeMsg[code];
	}
	(* this) = {code,msg};
	return *this;
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
