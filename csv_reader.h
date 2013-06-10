#ifndef __CSV_READER_H
#define __CSV_READER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class csv_reader
{
private:
	/* constructors not used */
	csv_reader(const csv_reader &) {}		                //pre-emptive definition, avoid copy construction for now
	csv_reader & operator=(const csv_reader &) { return *this; }	//pre-emptive definition, avoid construction by assignment
	/* members */
	std::fstream file;
	char record_separator;
public:
	/* constructors */
	csv_reader(const std::string &input_file, const char rsep);
	/* functions */
    bool has_next_line();
    void read_next_line(std::vector<std::string> &fields);
	/* destructors */
	~csv_reader();
};
#endif
