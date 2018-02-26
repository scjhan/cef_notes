#ifndef CEF_TESTS_CEF_SIMPLE_UTILS_H_
#define CEF_TESTS_CEF_SIMPLE_UTILS_H_
#pragma once

#include <string>
#include <Windows.h>
#include "include/internal/cef_string.h"

static std::string GetApplicationDir()
{
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR   wpath[MAX_PATH];

	GetModuleFileNameW(hModule, wpath, MAX_PATH);
	std::wstring wide(wpath);

	std::string path = CefString(wide);
	path = path.substr(0, path.find_last_of("\\/"));
	return path;
}

#endif // !CEF_TESTS_CEF_SIMPLE_UTILS_H_