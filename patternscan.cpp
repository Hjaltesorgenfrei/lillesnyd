#include "patternscan.h"
#include "processtools.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

char * VectorToMask(std::vector<std::pair<char, bool>> pattern) {
	char * buffer = new char[128];
	std::size_t i = 0;
	for (; i != pattern.size(); ++i) {
		if (std::get<bool>(pattern[i]))
			buffer[i] = 'x';
		else
			buffer[i] = '?';
	}
	buffer[i] = '\0';
	return buffer;
}

char * VectorToPattern(std::vector<std::pair<char, bool>> pattern) {
	char * buffer = new char[128];
	std::size_t i = 0;
	for (; i != pattern.size(); ++i) {
		buffer[i] = std::get<char>(pattern[i]);
	}
	buffer[i] = '\0';
	return buffer;
}

char * PatternScan(char* base, size_t size, const char* pattern, const char* mask)
{
	size_t patternLength = strlen(mask);

	for (unsigned int i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (unsigned int j = 0; j < patternLength; j++)
		{
			std::cout << "Nr. " << j << " Mask: " << mask[j] << " Pattern Value: " << (int)(unsigned char)pattern[j] << " == " << (int)(unsigned char)(*(base + i + j)) << "\n";
			if (mask[j] != '?' && pattern[j] != *(base + i + j))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return (char*)(base + i);
		}
	}
	return nullptr;
}

void * PatternScan(char* base, size_t size, std::vector<std::pair<char, bool>> &pattern)
{
	size_t patternLength = pattern.size();
	bool found = true;

	for (unsigned int i = 0; i < size - patternLength; i++)
	{
		for (std::size_t j = 0, e = pattern.size(); j != e; ++j) {
			if (j > 5 && std::get<bool>(pattern[j])) std::cout << "Nr. " << j << " Mask: " << std::get<bool>(pattern[j]) << " Pattern Value: " << (int)(unsigned char)std::get<char>(pattern[j]) << " == " << (int)(unsigned char)*(base + i + j) << "\n";
			if (std::get<bool>(pattern[j]) && std::get<char>(pattern[j]) != *(base + i + j)) {
				found = false;
				break;
			}
		}
		if (found)
		{
			return (void*)(base + i);
		}
	}
	return nullptr;
}

//External Wrapper
void * PatternScanEx(HANDLE hProcess, uintptr_t begin, uintptr_t end, char* pattern, char*  mask)
{
	uintptr_t currentChunk = begin;
	SIZE_T bytesRead;



	while (currentChunk < end)
	{
		char buffer[4096];

		DWORD oldprotect;
		VirtualProtectEx(hProcess, (void*)currentChunk, sizeof(buffer), PAGE_EXECUTE_READWRITE, &oldprotect);
		ReadProcessMemory(hProcess, (void*)currentChunk, &buffer, sizeof(buffer), &bytesRead);
		VirtualProtectEx(hProcess, (void*)currentChunk, sizeof(buffer), oldprotect, &oldprotect);

		if (bytesRead == 0)
		{
			return nullptr;
		}



		void * internalAddress = PatternScanOld((char*)&buffer, bytesRead, pattern, mask);


		if (internalAddress != nullptr)
		{
			//calculate from internal to external
			uintptr_t offsetFromBuffer = (uintptr_t)internalAddress - (uintptr_t)&buffer;
			return (void*)(currentChunk + offsetFromBuffer);
		}
		else
		{
			//advance to next chunk
			currentChunk = currentChunk + bytesRead;
		}
	}
	return nullptr;
}

//Module wrapper for external pattern scan
void * PatternScanExModule(HANDLE hProcess, wchar_t * exeName, wchar_t* module, char* pattern, char* mask)
{
	DWORD processID = GetProcID(exeName);
	MODULEENTRY32 modEntry = GetModule(processID, module);

	if (!modEntry.th32ModuleID)
	{
		return nullptr;
	}

	uintptr_t begin = (uintptr_t)modEntry.modBaseAddr;
	uintptr_t end = begin + modEntry.modBaseSize;
	return PatternScanEx(hProcess, begin, end, pattern);
}