#include "csv_reader.h"

csv_reader::csv_reader(const std::string &input_file, const char rsep)
{
	record_separator = rsep;
	file.open(input_file.c_str(), std::ios::in);
}

bool csv_reader::has_next_line()
{
	bool result = true;
	if ( file.eof() || file.bad() || file.fail() )
	{
		result = false;
	}
	return result;
}

 void csv_reader::read_next_line(std::vector<std::string> &fields)
 {
	std::string line;
	std::getline(file,line);
	std::istringstream line_stream(line);
	std::string cell;
	while(std::getline(line_stream,cell, record_separator))
	{
		fields.push_back(cell);
	}
 }

 csv_reader::~csv_reader()
 {
 }
