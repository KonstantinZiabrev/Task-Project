#include "Com_inner.h"

#include <ctime>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/ref.hpp>
#include <cstdint>

static std::wstring to_read_size(int64_t size) {
	int i = 0;
	const wchar_t* units[] = { L"B", L"kB", L"MB", L"GB", L"TB" };
	while (size > 1024 && i != (ARRAYSIZE(units) - 1)) {
		size /= 1024;
		i++;
	}
	return std::wstring(std::to_wstring(size) + units[i]);
}

static std::wstring to_read_date(const boost::filesystem::path& file) {
	std::time_t int_time = boost::filesystem::last_write_time(file);
	std::tm date;
	localtime_s(&date, &int_time);
	wchar_t buffer[64];
	std::wcsftime(buffer, 32, L"%a, %d.%m.%Y %H:%M:%S", &date);
	return std::wstring(buffer);
}

Com_inner::Com_inner() {

}

Com_inner::~Com_inner() {
	
}

void Com_inner::add_filename(const wchar_t* filename) {
	boost::filesystem::path helper(filename);
	boost::filesystem::path short_name = helper.filename();
	_records[short_name.wstring()] = std::make_pair(helper, std::wstring());
}

void Com_inner::reset() {
	_records.clear();
}

bool Com_inner::do_answer(const std::wstring& filename){
	records_processing();
	std::wstring answer;
	for (const auto& iter : _records) {
		answer += iter.first + L" " + iter.second.second + L"\n";
	}
	boost::filesystem::fstream output_file(filename, boost::filesystem::fstream::out| boost::filesystem::fstream::app);
	if (!output_file.is_open()) {
		return false;
	}
	output_file << answer;
	output_file.close();
	return true;
}

void Com_inner::records_processing() {
	boost::thread_group thread_pool;
	//Adds work to thread pool
	for (auto& iter : _records) {
		_service.post(boost::bind(&Com_inner::process_record, this, boost::ref(iter.second)));
	}
	//Creates thread for thread pool
	for (unsigned i = 1; i < boost::thread::hardware_concurrency(); ++i) {
		thread_pool.create_thread(boost::bind(&boost::asio::io_service::run, &_service));
	}
	//Run thread pool works
	_service.run();
	//Waits until thread pool finished its works
	thread_pool.join_all();
	return;
}

void Com_inner::process_record(Value_pair& pair) {
	boost::filesystem::fstream file (pair.first, boost::filesystem::fstream::in | boost::filesystem::fstream::binary);
	uint32_t sum = 0;
	while (file) {
		uint8_t helper;
		file.read(reinterpret_cast<char*>(&helper), 1);
		sum += helper;
	}
	file.close();
	pair.second = to_read_size(boost::filesystem::file_size(pair.first)) + L" " + to_read_date(pair.first) + L" bytesum " + std::to_wstring(sum);
}

