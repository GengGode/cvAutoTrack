#include "pch.h"
#include "ErrorCode.h"

ErrorCode::ErrorCode()
{
	errCodeMsg.push_back("0: ERR_SUCCESS");
	errCodeMsg.push_back("1: ERR_UNINITIALIZED");
	errCodeMsg.push_back("2: ERR_HWND_NO_FOUND");
	errCodeMsg.push_back("3: ERR_SCREEN_EMPTY");
	errCodeMsg.push_back("4: ERR_SURF_KEYPOINT_NO");
	errCodeMsg.push_back("5: ERR_MINIMAP_TOO_SMALL");
	errCodeMsg.push_back("6: ERR_PAIMON_NO_FOUND");
	errCodeMsg.push_back("7: ERR_KEYPOINT_OUT_OF_INDEX");
	errCodeMsg.push_back("8: ERR_UIDRECT_INVALID");
	errCodeMsg.push_back("9: ERR_AVATARKEYPOINT_ERROR");
	errCodeMsg.push_back("10: ERR_HWND_INVALID");
	errCodeMsg.push_back("11: ERR_AVATARRECT_UNFIND");
	errCodeMsg.push_back("12: ERR_GIHANDLE_NULL");
	errCodeMsg.push_back("13: ERR_GIHANDLE_INVALID");
}

ErrorCode::~ErrorCode()
{
}

ErrorCode & ErrorCode::getInstance()
{
	static ErrorCode instance;
	return instance;
}

ErrorCode & ErrorCode::operator=(const int & code)
{
	this->errorCode = code;
	if (code == 0)
	{
		errCodeList.clear();
	}
	else
	{
		push(code);
	}

	return *this;
}


ErrorCode::operator int()
{
	return this->errorCode;
}

void ErrorCode::setError(int code)
{
	this->errorCode = code;
	push(code);
}

int ErrorCode::getLastError()
{
	return this->errorCode;
}

string ErrorCode::getLastErrorMsg()
{
	if (errorCode <= errCodeMsg.size())
	{
		return errCodeMsg[errorCode];
	}
	else
	{
		return to_string(errorCode) + string(": UNDEFINE_ERROR");
	}
}

void ErrorCode::push(int code)
{
	errCodeList.push_back(code);
	if (errCodeList.size() > 9)
	{
		vector<int>::iterator index = errCodeList.begin();
		errCodeList.erase(index);
	}
}

ostream & operator<<(ostream & os, const ErrorCode & err)
{
	for (int i = err.errCodeList.size()-1; i >= 0; i--)
	{
		for (int j = 1; j < err.errCodeList.size() - i; j++)
		{
			os << "    ";
		}
		os << "\u2514 \u2500 \u252C \u2500>";

		if (err.errCodeList[i] < err.errCodeMsg.size())
		{
			os << err.errCodeMsg[err.errCodeList[i]] << endl;
		}
		else
		{
			os << to_string(err.errCodeList[i]) + string(": UNDEFINE_ERROR") << endl;
		}
	}
	return os;
}
