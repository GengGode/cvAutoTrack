#include "pch.h"
#include "ErrorCode.h"
#include "utils/Utils.h"

inline std::string wstring2string(std::wstring wstr)
{
	std::string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}



std::string get_sys_version()
{
	std::string result = "";
	std::string ProductName;
	std::string DisplayVersion;
	std::string CurrentBuildNumber;
	int UBR;
	if (!TianLi::Utils::getRegValue_REG_SZ(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(ProductName)", ProductName, 64))
		ProductName = "null";
	if(!TianLi::Utils::getRegValue_REG_SZ(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(DisplayVersion)", DisplayVersion, 64))
		DisplayVersion = "null";
	if (!TianLi::Utils::getRegValue_REG_SZ(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(CurrentBuildNumber)", CurrentBuildNumber, 64))
		CurrentBuildNumber = "null";
	if (!TianLi::Utils::getRegValue_DWORD(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", LR"(UBR)", UBR))
		UBR = 0;
	result = std::format("{0}-{1}-{2}.{3}", ProductName,DisplayVersion,CurrentBuildNumber,UBR);
	return result;
}
std::string get_gpu_name()
{
	// Get the name of the GPU
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;
	try
	{
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
		if (FAILED(hr))
		{
			return "Unknown";
		}

		hr = pFactory->EnumAdapters(0, &pAdapter);
		if (FAILED(hr))
		{
			return "Unknown";
		}

		DXGI_ADAPTER_DESC desc;
		hr = pAdapter->GetDesc(&desc);
		if (FAILED(hr))
		{
			return "Unknown";
		}

		std::wstring ws(desc.Description);
		return wstring2string(ws);
	}
	catch (...) {
		return "获取GPU信息失败";
	}
}

ErrorCode::ErrorCode()
{
}

ErrorCode::~ErrorCode()
{
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

inline void write_log(std::fstream& log_file,const std::string& time_stamp, const int error_code, const std::string& error_message)
{
	auto file_size = std::filesystem::file_size("./autoTrack.log");
	// 如果文件大于2MB，则清空文件，只保留末尾的1MB
	if (file_size > 1024 * 1024 * 2)
	{
		std::stringstream buffer;
		buffer << log_file.rdbuf();
		std::string content = buffer.str();
		
		// 清空文件
		log_file.close();
		log_file.open("./autoTrack.log", std::ios::in | std::ios::out | std::ios::trunc);
		log_file << "\xef\xbd\xbf";
		log_file << "系统版本号：" << get_sys_version() << "\n";
		log_file << "显卡：" << get_gpu_name() << "\n";
		
		// 保留末尾的512kb
		auto keep_size = 1024 * 1024;
		auto start = content.size() - keep_size;
		auto line_start = content.find_last_of("\n", start) + 1;
		auto end = content.size();
		std::string keep_content = content.substr(line_start, end);
		log_file << keep_content;
	}
	log_file << std::format("{} | 错误代码：{:6d}，错误信息：{}\n", time_stamp, error_code, error_message);
}

ErrorCode& ErrorCode::operator=(const std::pair<int, string>& err_code_msg)
{
	const int& code = err_code_msg.first;
	const string& msg = err_code_msg.second;
	
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
			log_file.open("./autoTrack.log", std::ios::in | std::ios::out | std::ios::app);
			if (log_file.is_open() == false) return *this;
			log_file << "\xef\xbd\xbf";
			log_file << "系统版本号：" << get_sys_version() << "\n";
			log_file << "显卡："<<get_gpu_name() << "\n";
		}

		if(this->error_code_last != this->error_code)		//不往日志中输出重复的错误
			write_log(log_file, time_stamp(), code, msg);
		this->error_code_last = this->error_code;

		log_file.flush();
		push(code,msg);
	}
	
	return *this;
}

bool ErrorCode::disableWirteFile()
{
	is_use_file = false;
	if (log_file.is_open())
	{
		log_file.close();
	}
	return true;
}

bool ErrorCode::enableWirteFile()
{
	is_use_file = true;
	return true;
}
/*
int ErrorCode::writeFile(char const* const format ,...)
{
	if (is_use_file == false) return -1;
	if (fptr == nullptr)
	{
		fopen_s(&fptr, "./autoTrack.log", "w+");
		// 获取系统信息写入日志 系统名称 系统版本号 显卡 显存 DirectX版本
		//writeFile("系统版本号:");
		//writeFile(get_sys_version().c_str());
		//writeFile("显卡:");
		//writeFile(get_gpu_name().c_str());
		if (fptr == nullptr) return -2;
	}

	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, format);
	_Result = fprintf_s(fptr, format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}
*/

ErrorCode::operator int()
{
	return this->error_code;
}

int ErrorCode::getLastError()
{
	return this->error_code;
}

string ErrorCode::getLastErrorMsg()
{
	if (this->error_code != 0)
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
	json += "\"errorCode\":" + to_string(this->error_code) + ",";
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
		os << to_string(err.error_code_msg_list[i].first) + ": " + err.error_code_msg_list[i].second + '\n';
	}
	return os;
}
