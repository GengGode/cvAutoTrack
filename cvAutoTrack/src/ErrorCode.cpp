#include "pch.h"
#include "ErrorCode.h"

std::string get_sys_version()
{
	// Get the Windows version.
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	//if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi)))
	//{
	//	// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

	//	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	//	if (!GetVersionEx((OSVERSIONINFO*)&osvi))
	//		return "Unknown";
	//}

	// get the major and minor version numbers
	int major = osvi.dwMajorVersion;
	int minor = osvi.dwMinorVersion;

	// get the build number
	int build = osvi.dwBuildNumber & 0xFFFF;

	// get the service pack (if any) and build number
	std::string service_pack = "";
	//if (osvi.szCSDVersion[0] != '\0')
	//{
	//	service_pack = std::string(osvi.szCSDVersion);
	//}

	// get the product type
	std::string product_type = "";
	switch (osvi.wProductType)
	{
	case VER_NT_WORKSTATION:
		if (major == 4)
			product_type = "Workstation 4.0";
		else if (major == 5 && minor == 0)
			product_type = "Workstation 2000";
		else if (major == 5 && minor == 1)
			product_type = "Workstation XP";
		else if (major == 6 && minor == 0)
			product_type = "Workstation Vista";
		else if (major == 6 && minor == 1)
			product_type = "Workstation 7";
		else if (major == 6 && minor == 2)
			product_type = "Workstation 8";
		else if (major == 6 && minor == 3)
			product_type = "Workstation 8.1";
		else if (major == 10 && minor == 0)
			product_type = "Workstation 10";
		else
			product_type = "Workstation";
		break;
	case VER_NT_DOMAIN_CONTROLLER:
		if (major == 4)
			product_type = "Domain Controller 4.0";
		else if (major == 5 && minor == 0)
			product_type = "Domain Controller 2000";
		else if (major == 5 && minor == 1)
			product_type = "Domain Controller XP";
		else if (major == 5 && minor == 2)
			product_type = "Domain Controller 2003";
		else if (major == 6 && minor == 0)
			product_type = "Domain Controller Vista";
		else if (major == 6 && minor == 1)
			product_type = "Domain Controller 7";
		else if (major == 6 && minor == 2)
			product_type = "Domain Controller 8";
		else if (major == 6 && minor == 3)
			product_type = "Domain Controller 8.1";
		else if (major == 10 && minor == 0)
			product_type = "Domain Controller 10";
		else
			product_type = "Domain Controller";
		break;
	case VER_NT_SERVER:
		if (major == 4)
			product_type = "Server 4.0";
		else if (major == 5 && minor == 0)
			product_type = "Server 2000";
		else if (major == 5 && minor == 1)
			product_type = "Server XP";
		else if (major == 5 && minor == 2)
			product_type = "Server 2003";
		else if (major == 6 && minor == 0)
			product_type = "Server Vista";
		else if (major == 6 && minor == 1)
			product_type = "Server 7";
		else if (major == 6 && minor == 2)
			product_type = "Server 8";
		else if (major == 6 && minor == 3)
			product_type = "Server 8.1";
		else if (major == 10 && minor == 0)
			product_type = "Server 10";
		else
			product_type = "Server";
		break;
	default:
		product_type = "Unknown";
		break;
	}

	// get the architecture
	std::string architecture = "";
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);	
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		architecture = "x64";
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		architecture = "x86";
	else
		architecture = "Unknown";
	
	// return the version string
	return product_type + " " + architecture + " " + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build) + " " + service_pack;
	
}
std::string get_gpu_name()
{
	// Get the name of the GPU
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;
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
	std::string str(ws.begin(), ws.end());

	return str;
}
std::string get_gpu_memory()
{
	// Get the amount of memory on the GPU
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;
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

	return std::to_string(desc.DedicatedVideoMemory / 1024 / 1024) + " MB";
}
std::string get_directx_version()
{
	// 获取系统 DirectX 版本
	DWORD dwVersion = 0;
	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	DWORD dwBuild = 0;

	//// Get the DirectX version.
	//dwVersion = DirectXVersion();

	//// Get the DirectX major version.
	//dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	//// Get the DirectX minor version.

	//dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	//// Get the build number.

	//dwBuild = (DWORD)(HIWORD(dwVersion));

	// return the version string
	return std::to_string(dwMajorVersion) + "." + std::to_string(dwMinorVersion) + "." + std::to_string(dwBuild);
}
std::string get_sys_name()
{
	// 获取系统名称
	std::string name = "";
	char computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerNameA(computerName, &size);
	name = computerName;
	return name;
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
		//writeFile("%s | 错误代码：%-6d，错误信息：%s\n", time_stamp().c_str(), code, msg.c_str());
		if (isUseFile == false) return *this;
		if (fptr == nullptr)
		{
			fopen_s(&fptr, "./autoTrack.log", "w+");
			if (fptr == nullptr) return *this;
		}
		
		fprintf_s(fptr, "%s | 错误代码：%-6d，错误信息：%s\n", time_stamp().c_str(), code, msg.c_str());
		push(code,msg);
	}
	
	return *this;
}

bool ErrorCode::disableWirteFile()
{
	isUseFile = false;
	if (fptr != nullptr)
	{
		fclose(fptr);
		fptr = nullptr;
	}
	return true;
}

bool ErrorCode::enableWirteFile()
{
	isUseFile = true;
	return true;
}

int ErrorCode::writeFile(char const* const format ,...)
{
	if (isUseFile == false) return -1;
	if (fptr == nullptr)
	{
		fopen_s(&fptr, "./autoTrack.log", "w+");
		// 获取系统信息写入日志 系统名称 系统版本号 显卡 显存 DirectX版本
		//writeFile("系统名称:");
		//writeFile(get_sys_name().c_str());
		//writeFile("系统版本号:");
		//writeFile(get_sys_version().c_str());
		//writeFile("显卡:");
		//writeFile(get_gpu_name().c_str());
		//writeFile("显存:");
		//writeFile(get_gpu_memory().c_str());
		//writeFile("DirectX版本:");
		//writeFile(get_directx_version().c_str());
		//writeFile("\n");
		if (fptr == nullptr) return -2;
	}

	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, format);
	_Result = fprintf_s(fptr, format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
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
