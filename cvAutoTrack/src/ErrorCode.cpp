#include "pch.h"
#include "ErrorCode.h"

ErrorCode::ErrorCode()
{
	fopen_s(&fptr, "./autoTrack.log", "w+");

	errCodeMsg.push_back(u8"执行成功");
	errCodeMsg.push_back(u8"未初始化");
	errCodeMsg.push_back(u8"未能找到原神窗口句柄");
	errCodeMsg.push_back(u8"窗口画面为空");
	errCodeMsg.push_back(u8"未能匹配到特征点");
	errCodeMsg.push_back(u8"原神小地图区域为空或者区域长宽小于60px");
	errCodeMsg.push_back(u8"未能匹配到派蒙");
	errCodeMsg.push_back(u8"特征点数组访问越界");
	errCodeMsg.push_back(u8"未能在UID区域检测到有效UID");
	errCodeMsg.push_back(u8"提取小箭头特征误差过大");
	errCodeMsg.push_back(u8"无效句柄或指定句柄所指向窗口不存在");
	errCodeMsg.push_back(u8"未能取到小箭头区域");
	errCodeMsg.push_back(u8"窗口句柄为空");
	errCodeMsg.push_back(u8"窗口句柄失效");
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
	string msg = u8"未定义错误信息";
	if (code < errCodeMsg.size())
	{
		msg = errCodeMsg[code];
	}
	(* this) = {code,msg};
	return *this;
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
		// 获取当前系统时间 
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		// 转换为系统时间
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		// 转换为本地时间
		std::tm* now_tm = std::localtime(&now_c);
		// 转换为字符串
		char time_str[100];
		std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);
		
		fprintf_s(fptr, u8"%s | 错误代码：%d，错误信息：%s\n", time_str, code, msg.c_str());
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
			os << u8"\u2514\u2500\u2500\u2500\u2192";
		}
		else
		{
			os << u8"\u2514\u2500\u252C\u2500\u2192";
		}
		//os << "\u2514 \u2500 \u252C \u2500 \u2192";

		os << to_string(err.error_code_msg_list[i].first) + ": " + err.error_code_msg_list[i].second + '\n';
	}
	return os;
}
