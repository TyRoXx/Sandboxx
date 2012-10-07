#include "dynamic_library.h"

#ifdef _WIN32
#include <Windows.h>

dyn_lib_t dyn_lib_open(char const *file_name)
{
	return LoadLibraryA(file_name);
}

void dyn_lib_close(dyn_lib_t lib)
{
	FreeLibrary(lib);
}

void *dyn_lib_find(dyn_lib_t lib, char const *symbol)
{
	return GetProcAddress(lib, symbol);
}

#else

dyn_lib_t dyn_lib_open(char const *file_name)
{
	return 0;
}

void dyn_lib_close(dyn_lib_t lib)
{
}

void *dyn_lib_find(dyn_lib_t lib, char const *symbol)
{
	return 0;
}

#endif
