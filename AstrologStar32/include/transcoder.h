#ifndef TRANSCODER_H
#define TRANSCODER_H

#include <string>

std::string wchar_to_char(const wchar_t* wstr);
std::wstring char_to_wchar(const char* str, const char* locale = "");
const char* get_system_timezone();

#endif // TRANSCODER_H