#ifndef _TRANSU_H_
#define _TRANSU_H_
#include <locale.h>
#include <stdlib.h>
#include <string>
std::string wchar_to_char(const wchar_t* wstr);


#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

std::wstring char_to_wchar(const char* str, const char* locale = "en_US.UTF-8");
const char* get_system_timezone();
#endif
