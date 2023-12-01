#pragma once
using namespace std;

class ErrorCode
{
	std::fstream log_file;
	bool is_use_file = false;
private:
	ErrorCode();

public:
	~ErrorCode();
	ErrorCode(const ErrorCode&) = delete;
	ErrorCode& operator=(const ErrorCode&) = delete;
	static ErrorCode& getInstance();
	ErrorCode& operator=(const std::pair<int,std::string>& err_code_msg);
	operator int();
	friend std::ostream & operator<<(std::ostream & os, const ErrorCode & err);

	bool disableWirteFile();
	bool enableWirteFile();
	int writeFile(char const* const format, ...);
public:
	int getLastError();
	std::string getLastErrorMsg();
	std::string toJson();

private:
	int error_code=0;
	int error_code_last = 0;
	std::vector<std::pair<int, std::string>> error_code_msg_list;
private:
	void push(int code, string msg);
};

inline bool clear_error_logs()
{
	ErrorCode::getInstance() = { 0,"调用成功" };
	return true;
}