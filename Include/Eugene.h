#pragma once
#include <Windows.h>
#include "Engine.h"

namespace Eugene
{
	using RunFunc = int(*)(const char*);
}

#define EngineEntry(sceneName)	\
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)\
{											\
	auto h = LoadLibrary(L"./Engine/Eugene.dll");	\
	if (h == 0)\
	{\
		return -1;\
	}\
	auto run = reinterpret_cast<Eugene::RunFunc>(static_cast<void*>(GetProcAddress(h,"Run"))); \
	int rtn = run(sceneName); \
	FreeLibrary(h);\
	return rtn;			\
}						\


