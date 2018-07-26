#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <utility>

void * PatternScan(char* base, size_t size, std::vector<std::pair<char, bool>> pattern);

void * PatternScanEx(HANDLE hProcess, uintptr_t begin, uintptr_t end, char* pattern, char* mask);

void * PatternScanExModule(HANDLE hProcess, wchar_t * exeName, wchar_t* module, char* pattern, char* mask);