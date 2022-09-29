#include "pch.h"
#include "ErrorCode.h"

ErrorCode::ErrorCode()
{
	fopen_s(&fptr, "./autoTrack.log", "w+");

	errCodeMsg.push_back("Ö´ĞĞ³É¹¦");
	errCodeMsg.push_back("Î´³õÊ¼»¯");
	errCodeMsg.push_back("Î´ÄÜÕÒµ½Ô­Éñ´°¿Ú¾ä±ú");
	errCodeMsg.push_back("´°¿Ú»­ÃæÎª¿Õ");
	errCodeMsg.push_back("Î´ÄÜÆ¥Åäµ½ÌØÕ÷µã");
	errCodeMsg.push_back("Ô­ÉñĞ¡µØÍ¼ÇøÓòÎª¿Õ»òÕßÇøÓò³¤¿íĞ¡ÓÚ60px");
	errCodeMsg.push_back("Î´ÄÜÆ¥Åäµ½ÅÉÃÉ");
	errCodeMsg.push_back("ÌØÕ÷µãÊı×é·ÃÎÊÔ½½ç");
	errCodeMsg.push_back("Î´ÄÜÔÚUIDÇøÓò¼ì²âµ½ÓĞĞ§UID");
	errCodeMsg.push_back("ÌáÈ¡Ğ¡¼ıÍ·ÌØÕ÷Îó²î¹ı´ó");
	errCodeMsg.push_back("ÎŞĞ§¾ä±ú»òÖ¸¶¨¾ä±úËùÖ¸Ïò´°¿Ú²»´æÔÚ");
	errCodeMsg.push_back("Î´ÄÜÈ¡µ½Ğ¡¼ıÍ·ÇøÓò");
	errCodeMsg.push_back("´°¿Ú¾ä±úÎª¿Õ");
	errCodeMsg.push_back("´°¿Ú¾ä±úÊ§Ğ§");
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
	string msg = "Î´¶¨Òå´íÎóĞÅÏ¢";
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
		fprintf_s(fptr, "Çå¿Õ´íÎó¶ÑÕ»\n");
		error_code_msg_list.clear();
	}
	else
	{
		// è·å–å½“å‰ç³»ç»Ÿæ—¶é—´ 
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		// è½¬æ¢ä¸ºç³»ç»Ÿæ—¶é—´
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		// è½¬æ¢ä¸ºæœ¬åœ°æ—¶é—´
		std::tm* now_tm = std::localtime(&now_c);
		// è½¬æ¢ä¸ºå­—ç¬¦ä¸²
		char time_str[100];
		std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);
		
		fprintf_s(fptr, u8"%s | é”™è¯¯ä»£ç ï¼š%dï¼Œé”™è¯¯ä¿¡æ¯ï¼š%s\n", time_str, code, msg.c_str());
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
