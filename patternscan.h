#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <utility>

char * PatternScan(const char* base, size_t size, const char* pattern, const char* mask);

void * PatternScanEx(HANDLE hProcess, uintptr_t begin, uintptr_t end, const char* pattern, const char* mask);

void * PatternScanExModule(HANDLE hProcess, const wchar_t * exeName, const wchar_t* module, const char* pattern, const char* mask);