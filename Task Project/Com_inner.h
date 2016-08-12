#pragma once

#include <string>
#include <map>

class Com_inner {
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
	virtual bool records_processing();
private:
	std::map<std::wstring, std::wstring> _records;
};