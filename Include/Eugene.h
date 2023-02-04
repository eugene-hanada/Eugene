#pragma once
#include <Windows.h>

#define EngineEntry(sceneName)	\
int main()									\
{											\
	auto h = LoadLibrary(L"./Eugene.dll");	\
	auto run = reinterpret_cast<int(*)(const char*)>(GetProcAddress(h,"Run")); 
	int rtn = run(sceneName); \
	FreeLibrary(h);
	return rtn;			\
}						\


