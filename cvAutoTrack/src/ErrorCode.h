#pragma once
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class ErrorCode
{
	FILE * fptr ;
private:
	ErrorCode();

public:
	~ErrorCode();
	ErrorCode(const ErrorCode&) = delete;
	ErrorCode& operator=(const ErrorCode&) = delete;
	static ErrorCode& getInstance();
	ErrorCode& operator=(const int& code);
	ErrorCode& operator=(const std::pair<int,string>& err_code_msg);
	operator int();

	friend ostream & operator<<(ostream & os, const ErrorCode & err);

public:
	int getLastError();
	string getLastErrorMsg();

private:
	int errorCode=0;
	vector<int> errCodeList;
	vector<string> errCodeMsg;

	std::vector<std::pair<int, std::string>> error_code_msg_list;
	

private:
	void push(int code, string msg);
};
