#pragma once

#include <windows.h>
#include <string>
#include <map>

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
	std::wstring compose_answer() const;

	//Adding dates, sizes etc
	//Return the success of processing
	bool records_processing();
private:
	//Process 
	virtual void process_record();
	std::map<const std::wstring, std::wstring> _records;
	CRITICAL_SECTION _cs;
	PTP_WORK _pool_pointer;
	decltype(_records)::iterator  _pending_record;
	bool _error_occured;
	
};