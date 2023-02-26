#pragma once

#ifdef _WIN64

#ifdef EXPORT

#define EUGENE_API __declspec(dllexport)

#else

#define  EUGENE_API __declspec(dllimport)

#endif

#ifdef _CPPRTTI
#define ACTIVE_RTTI
#endif
#endif


