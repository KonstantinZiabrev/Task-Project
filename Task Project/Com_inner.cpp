#include "Com_inner.h"

#include <windows.h>

static std::wstring to_read_size(int64_t size) {
	int i = 0;
	const wchar_t* units[] = { L"B", L"kB", L"MB", L"GB", L"TB" };
	while (size > 1024 && i != (ARRAYSIZE(units) - 1)) {
		size /= 1024;
		i++;
	}
	return std::wstring(std::to_wstring(size) + units[i]);
}

static std::wstring to_read_date(const FILETIME& filetime) {
	SYSTEMTIME helper;
	FileTimeToSystemTime(&filetime, &helper);
	std::wstring answer;
	answer += std::to_wstring(helper.wYear) + L"-" + std::to_wstring(helper.wMonth) + L"-" + std::to_wstring(helper.wDay) + L"T"
		+ std::to_wstring(helper.wHour) + L"-" + std::to_wstring(helper.wMinute) + L"-" + std::to_wstring(helper.wSecond);
	return answer;
}

Com_inner::Com_inner() {
}

Com_inner::~Com_inner() {

}

void Com_inner::add_filename(const wchar_t* filename) {
	_records[std::wstring(filename)] = std::wstring();
}

void Com_inner::reset() {
	_records.clear();
}

std::wstring Com_inner::compose_answer() const{
	std::wstring answer;
	for (const auto& iter : _records) {
		answer += iter.first + L" " + iter.second + L"\n";
	}
	return answer;
}

bool Com_inner::records_processing() {
	bool answer = true;
	for (auto& iter : _records) {
		HANDLE handle = CreateFile(iter.first.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (handle == INVALID_HANDLE_VALUE) {
			return false;
		}
		LARGE_INTEGER file_size;
		GetFileSizeEx(handle, &file_size);
		FILETIME creation_time;
		GetFileTime(handle, &creation_time, NULL, NULL);
		CloseHandle(handle);
		iter.second += L" " + to_read_size(file_size.QuadPart) + L" "+ to_read_date(creation_time);
	}
	return true;
}