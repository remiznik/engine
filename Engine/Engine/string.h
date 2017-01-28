#ifndef _STRING_H
#define _STRING_H

#include <tchar.h>
#include <string>

#ifdef UNICODE
	#define tstring wstring
	#define TOSTRING(value) std::to_wstring(value)
#else
	#define tstring string
	#define TOSTRING(value) std::to_string(value)
#endif 

#endif
