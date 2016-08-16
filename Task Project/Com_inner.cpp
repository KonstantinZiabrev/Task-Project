#include "Com_inner.h"

#include <assert.h>

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

void CALLBACK worker_func(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work) {
	Com_inner* com_inner = static_cast<Com_inner*>(Context);
	com_inner->process_record();
}

Com_inner::Com_inner() {
	InitializeCriticalSectionAndSpinCount(&_cs, 0x00000400);
	_pool_pointer = CreateThreadpoolWork(&worker_func, static_cast<void*>(this), NULL);
}

Com_inner::~Com_inner() {
	CloseThreadpoolWork(_pool_pointer);
}

void Com_inner::add_filename(const wchar_t* filename) {
	_records[std::wstring(filename)] = std::wstring();
}

void Com_inner::reset() {
	_records.clear();
	_error_occured = false;
}

std::wstring Com_inner::compose_answer() const{
	std::wstring answer;
	for (const auto& iter : _records) {
		answer += iter.first + L" " + iter.second + L"\n";
	}
	return answer;
}

bool Com_inner::records_processing() {
	_pending_record = _records.begin();
	for (unsigned i = 0; i < _records.size(); ++i) {
		SubmitThreadpoolWork(_pool_pointer);
	}
	WaitForThreadpoolWorkCallbacks(_pool_pointer, false);
	return !(_error_occured);
}

void Com_inner::process_record() {
	EnterCriticalSection(&_cs);
	auto iter = _pending_record;
	++_pending_record;
	LeaveCriticalSection(&_cs);
	HANDLE handle = CreateFile(iter->first.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		EnterCriticalSection(&_cs);
		_error_occured = true;
		LeaveCriticalSection(&_cs);
	}
	unsigned char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	__int32 checksum = 0;
	while (true) {
		unsigned long bytes_read;
		ReadFile(handle, buffer, BUFFER_SIZE, &bytes_read, NULL);
		if (bytes_read == 0) {
			break;
		}
		for (unsigned i = 0; i < bytes_read; ++i) {
			checksum += buffer[i];
		}
	}
	LARGE_INTEGER file_size;
	GetFileSizeEx(handle, &file_size);
	FILETIME creation_time;
	GetFileTime(handle, &creation_time, NULL, NULL);
	CloseHandle(handle);
	iter->second += L" " + to_read_size(file_size.QuadPart) + L" " + to_read_date(creation_time) + L" checksum " + std::to_wstring(checksum);
}

