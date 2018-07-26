#include "processtools.h"

#include <algorithm>

//Get Process ID From an executable name using toolhelp32Snapshot
DWORD GetProcID(const wchar_t * exeName)
{
	PROCESSENTRY32 procEntry = { 0 };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!hSnapshot)
	{
		return 0;
	}

	procEntry.dwSize = sizeof(procEntry);

	if (!Process32First(hSnapshot, &procEntry))
	{
		return 0;
	}

	do
	{
		if (!wcscmp(procEntry.szExeFile, exeName))
		{
			CloseHandle(hSnapshot);
			return procEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &procEntry));

	CloseHandle(hSnapshot);
	return 0;
}

//Get ModuleEntry from module name, using toolhelp32snapshot
MODULEENTRY32 GetModule(DWORD dwProcID, const wchar_t * moduleName)
{
	MODULEENTRY32 modEntry = { 0 };

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);

	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 curr = { 0 };

		curr.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &curr))
		{
			do
			{
				if (!wcscmp(curr.szModule, moduleName))
				{
					modEntry = curr;
					break;
				}
			} while (Module32Next(hSnapshot, &curr));
		}
		CloseHandle(hSnapshot);
	}
	return modEntry;
}



std::vector<std::string> StringToVector(std::string const& str, char const delimiter) {

	std::vector<std::string> vec;
	std::string element;


	//we are going to loop through each character of the string slowly building an element string.
	//whenever we hit a delimiter, we will push the element into the vector, and clear it to get ready for the next element
	for_each(begin(str), end(str), [&](char const ch) {
		if (ch != delimiter) {
			element += ch;
		}
		else {
			if (element.length() > 0) {
				vec.push_back(element);
				element.clear();
			}
		}
	});


	//push in the last element if the string does not end with the delimiter
	if (element.length() > 0) {
		vec.push_back(element);
	}


	return vec;
}

std::vector<std::pair<char, bool>> convertToHexVector(const char* str) {
	std::vector<std::pair<char, bool>> buffer;
	for (; *str; ++str) {
		if (str[0] != ' ' && str[0] != '?') {
			buffer.push_back(std::make_pair((char)getByte(str), true));
			++str;
		}
		else if (str[0] == '?') {
			buffer.push_back(std::make_pair('\x00', false));
		}

	}
	return buffer;
}