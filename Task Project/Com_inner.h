#pragma once

#include <string>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/asio/io_service.hpp>
#include "Defines.h"


static const unsigned BUFFER_SIZE = 1024;

class Com_inner {
	//Should use only process_record()
	friend static void CALLBACK worker_func(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);
public:
	Com_inner	();
	Com_inner	(const Com_inner&) = delete;
	Com_inner&	operator=(const Com_inner&) = delete;
	~Com_inner	();

	void add_filename(const wchar_t* filename);

	//clears data for new data gathering
	void reset();

	//Composing final answer for message Box
	bool do_answer(const std::wstring& filename);

	//Adding dates, sizes etc
	//Return the success of processing
	void records_processing();
private:
	using Value_pair = std::pair<boost::filesystem::path, std::wstring>;
	//map<short_filename,<full_path, file_info>>
	std::map<const std::wstring, Value_pair> _records;
	//For thread pool
	boost::asio::io_service _service;	

	virtual void process_record(Value_pair& pair);
};