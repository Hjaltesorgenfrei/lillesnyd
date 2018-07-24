#include "patternscan.h"
#include "processtools.h"

#include <Windows.h>
#include <TlHelp32.h>


void * FindPattern(char* pBaseAddress, char* pbMask, const char* pszMask, size_t nLength)
{
	auto DataCompare = [](const char* pData, const char* mask, const char* cmask, char chLast, size_t iEnd) -> bool {
		if (pData[iEnd] != chLast) return false;
		for (; *cmask; ++cmask, ++pData, ++mask) {
			if (*cmask == 'x' && *pData != *mask) {
				return false;
			}
		}

		return true;
	};

	auto iEnd = strlen(pszMask) - 1;
	auto chLast = pbMask[iEnd];

	auto* pEnd = pBaseAddress + nLength - strlen(pszMask);
	for (; pBaseAddress < pEnd; ++pBaseAddress) {
		if (DataCompare(pBaseAddress, pbMask, pszMask, chLast, iEnd)) {
			return pBaseAddress;
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



		void * internalAddress = FindPattern((char*)&buffer, pattern, mask, bytesRead);


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
	PatternScanEx(hProcess, begin, end, pattern, mask);
	return PatternScanEx(hProcess, begin, end, pattern, mask);
}