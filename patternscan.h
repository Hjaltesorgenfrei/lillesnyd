#pragma once
#include <Windows.h>
#include <TlHelp32.h>

void * PatternScanEx(HANDLE hProcess, uintptr_t begin, uintptr_t end, char* pattern, char* mask);

void * FindPattern(char* pBaseAddress, char* pbMask, const char* pszMask, size_t nLength);

void * PatternScanExModule(HANDLE hProcess, wchar_t * exeName, wchar_t* module, char* pattern, char* mask);