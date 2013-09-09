#include <algorithm>
#include <cstring>
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <ctime>
#include <typeinfo>
#include "csv_reader.h"
#include "edr.h"
#include "main.h"
#include "structures.h"
#include "static_methods.h"

//TODO: randomize data volumes and call durations
//TODO: ability to specify output dir from cmd line
//TODO: CFW records and third party numbers - make sure number makes sense
//TODO: randomize A and B numbers
//TODO: limit rec entities for SMS to a pool of randoms
//TODO: limit rec entitites for GPRS switch id to pool of numbers
//TODO: add flag to generate random errors, like validation errors, fields missing, fields longer than needed

int main(int argc, char **argv)
{
	string_vector record_types, op_choices;
	string_map franchises;
	op_map operators;
	sim_map test_sim_cards;
	init_types(record_types);

	std::string fra_choice, rec_choice, em_num, serving_bid, edr_req, date, operator_file, sim_file, output_path;

	char cdate[20];
	time_t date_seconds;
	time_t_to_char(time(0), cdate, sizeof(cdate));
	date = cdate;

	int num_edr = 6, test_interval = 0;
	output_path = "./";
	bool has_f = false, has_o = false, has_r = false, has_of = false, has_sf = false, use_blank_utc = false;
	bool verbose = false;

	if (argc > 1)
	{
		//franchise
		if(cmd_option_exists(argv, argv+argc, "-f"))
		{
			char **fra = get_cmd_option(argv, argv + argc, "-f");
			fra_choice = *fra;
			has_f = true;
		}
		//load operator export file
		if(cmd_option_exists(argv, argv+argc, "-of"))
		{
			char **opfile = get_cmd_option(argv, argv + argc, "-of");
			operator_file = *opfile;
			if (! file_exists(operator_file) )
			{
				std::cerr << "The specified file " <<operator_file << " does not exist..aborting!" << std::endl;
				return 1;
			}
			else
			{
				void (*process_function)(std::vector<std::string> &, op_map &) = map_operators;
				process_file(operator_file, process_function, operators);
				init_options(operators, franchises);
				has_of = true;
			}
		}
		//load test sim card export file
		if(cmd_option_exists(argv, argv+argc, "-sf"))
		{
			char **sfile = get_cmd_option(argv, argv + argc, "-sf");
			sim_file = *sfile;
			if (! file_exists(sim_file) )
			{
				std::cerr << "The specified file " <<sim_file << " does not exist..aborting!" << std::endl;
				return 1;
			}
			else
			{
				void (*process_function)(std::vector<std::string> &, sim_map &) = map_simcards;
				process_file(sim_file, process_function, test_sim_cards);
				has_sf = true;
			}
		}
		//operator
		char **begin = argv;
		while( cmd_option_exists(begin, argv+argc, "-o") )
		{
			begin = get_cmd_option(begin, argv + argc, "-o");  // match and move start search position (begin) along to last matched
			op_choices.push_back(*begin);
			has_o = true;
		}
		//records
		if(cmd_option_exists(argv, argv+argc, "-r"))
		{
			record_types.clear();	//clear default values
		}
		while( cmd_option_exists(begin, argv+argc, "-r") )
		{
			begin = get_cmd_option(begin, argv + argc, "-r");  // match and move start search position (begin) along to last matched
			record_types.push_back(*begin);
		}
		if (record_types.size() > 0 )
		{
			has_r = true;
		}
		//date
		if(cmd_option_exists(argv, argv+argc, "-d"))
		{
			char **sdate = get_cmd_option(argv, argv + argc, "-d");
			date = *sdate;
		}
		//serving bid
		if(cmd_option_exists(argv, argv+argc, "-s"))
		{
			char **sbid = get_cmd_option(argv, argv + argc, "-s");
			serving_bid = *sbid;
		}
		//number of records
		if(cmd_option_exists(argv, argv+argc, "-n"))
		{
			char **sedr = get_cmd_option(argv, argv + argc, "-n");
			num_edr = atoi (*sedr);
		}
		//use blank UTC
		if(cmd_option_exists(argv, argv+argc, "-b"))
		{
			char **blank_utc = get_cmd_option(argv, argv + argc, "-b");
			use_blank_utc = atoi (*blank_utc);
		}
		//test record interval
		if(cmd_option_exists(argv, argv+argc, "-i"))
		{
			char **test_iv = get_cmd_option(argv, argv + argc, "-i");
			test_interval = atoi (*test_iv);
		}
		//verbose
		if(cmd_option_exists(argv, argv+argc, "-v"))
		{
			verbose = true;
		}
		
		//output path
		if(cmd_option_exists(argv, argv+argc, "-p"))
		{
			char **cpath = get_cmd_option(argv, argv + argc, "-p");
			//std::cout << "Checking if " << *cpath << "exists" << std::endl;
			if (path_exists(*cpath) )
			{
				output_path = *cpath;
			}
			else
			{
				std::cerr << "The specified output path \""<< *cpath << "\" does not exist! " << std::endl;
				return 1;
			}
		}

	}
	if (has_f && has_o && has_r && has_of && has_sf)
	{
		int which_type = 0;
		int which_op = 0;
		int test_record_count = 0;
		std::string original_date = date;
		std::string op_choice;
		std::string filename = "ROM_" + fra_choice + original_date + ".DAT";

		std::cout << "Generating EDR file..please be patient." << std::endl;

		for (int i=0;i < num_edr; i++)
		{
			if ( which_type == record_types.size() )
			{
				which_type = 0;			//start with first record type again
			}
			if (which_op == op_choices.size())
			{
				which_op = 0;
			}
			op_choice = op_choices[which_op];

			std::string event_date;
			date_seconds = string_to_time_t(date) + i;
			time_t_to_char( date_seconds, cdate, sizeof(cdate));
			event_date = cdate;
			//std::cout << "event date [" << event_date << "]" << std::endl;

			std::string imsi = make_imsi(operators[op_choice].MCC, operators[op_choice].MNC, i);
			std::string hpmn_number = operators[fra_choice].COUNTRY_IDD + static_methods::pad_number(i, 8);
			std::string vpmn_number = operators[op_choice].COUNTRY_IDD + static_methods::pad_number(i, 8);
			std::string smsc_number = operators[op_choice].COUNTRY_IDD + static_methods::pad_number(i, 6);
			if ( (test_interval > 0) && (i % test_interval) == 0)
			{
				std::string test_imsi = test_sim_cards[fra_choice+op_choice].IMSI;
				std::string test_hpmn_number = test_sim_cards[fra_choice+op_choice].MSISDN;
				if (test_imsi.length() == 0)
				{
					std::cerr << "No Test SIM records found in " << sim_file << " for franchise " << fra_choice << " and operator " << op_choice << " !" << std::endl;
				}
				else
				{
					test_record_count++;
					imsi = test_imsi;
					hpmn_number = test_hpmn_number;
				}
			}
			if (verbose)
			{
				std::cout << "Generating record [" << i+1 << "] of type [" << record_types[which_type] << "] franchise [" << fra_choice << "]\toperator [" << op_choice << "] IMSI [" << imsi <<"]\tServing BID [" << serving_bid << "]" <<std::endl;
			}
			edr *EDR = new edr(imsi, hpmn_number, vpmn_number, smsc_number, operators[op_choice].UTC_TIME_OFFSET, record_types[which_type],event_date,serving_bid, i, use_blank_utc);

			EDR->to_file(filename);
			which_type++;
			which_op++;
			delete EDR;
		}
		std::cout << std::endl << "Saving to file [" << filename << "]" << std::endl;
	}
	else
	{
		print_help(operators, franchises, record_types, date, num_edr, use_blank_utc, em_num, serving_bid, test_interval, output_path);
		return 1;
	}
	return(0);
}

/* function definitions */
bool cmd_option_exists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

char** get_cmd_option(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if ( (itr != end) && (++itr != end) && (strcmp(*itr,"\0") != 0) && (*itr != NULL) )
    {
        return itr;
    }
    return '\0';
}

// initialize map of IMSI and franchises
void init_options(op_map &operators, string_map &franchise)
{
	for(op_map::iterator it = operators.begin() ; it != operators.end(); it++)
	{
		// it->first == PLMN, it->second == operator_struct
		if (it->second.TYPE == "FRA")
		{
			franchise[it->first] = it->first;
		}
	}
}

//initialize record types
void init_types(string_vector &record_types)
{
	record_types.push_back("01");
	record_types.push_back("02");
	record_types.push_back("03");
	record_types.push_back("04");
	record_types.push_back("05");
	record_types.push_back("06");
}

//check for the existence of a file
bool file_exists(std::string filename)
{
	bool result = false;
	std::ifstream ifile(filename.c_str());
	if (ifile)
	{
		result = true;
	}
  return result;
}

// retrieve complete list of operators
template <typename T> void process_file(std::string filename, void (*process_function)(std::vector<std::string> &, T &), T &container)
{
	csv_reader *reader = new csv_reader(filename, ',');
	while (reader->has_next_line())
	{
		std::vector<std::string> fields;
		reader->read_next_line(fields);
		(*process_function)(fields, container);
	}
}

// populate an operator_struct from the fields of a line read from the IR_OPERATOR_EXPORT
void map_operators(std::vector<std::string> &fields, op_map &operators)
{
	if (fields.size() == 16)
	{
		operator_struct op =
		{
			fields.at(0), fields.at(1), fields.at(2), fields.at(3),
			fields.at(4), fields.at(5), fields.at(6), fields.at(7),
			fields.at(8), fields.at(9), fields.at(10), fields.at(11),
			fields.at(12),fields.at(13), fields.at(14), fields.at(15)
		};
		operators[fields.at(0)] = op;
	}
}

// populate an operator_struct from the fields of a line read from the IR_OPERATOR_EXPORT
void map_simcards(std::vector<std::string> &fields, sim_map &sim_cards)
{
	if (fields.size() == 8)
	{
		simcard_struct op =
		{
			fields.at(0), fields.at(1), fields.at(2), fields.at(3),
			fields.at(4), fields.at(5), fields.at(6), fields.at(7)
		};
		sim_cards[fields.at(0)+fields.at(1)] = op;
	}
}

void print_help
	(
	op_map &operators, 
	string_map &franchises,
	string_vector &record_types,
	std::string &date, 
	int edr_num, 
	bool use_blank, 
	std::string em_num, 
	std::string sbid,
	int test_interval,
	std::string output_path
	)
{
	std::cerr << "Usage: edrmake [OPTIONS]" << std::endl;
	std::cerr << "Generate EDR records, given a suitable IR_OPERATOR_EXPORT table" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Mandatory arguments:" << std::endl;
	std::cerr << "\t-of\tSpecify IR_OPERATOR_EXPORT file";
	std::cerr << std::endl;
	std::cerr << "\t-sf\tSpecify IR_TST_SIM_CARD_EXPORT file";
	std::cerr << std::endl;
	if (operators.empty())
	{
		std::cerr << "\t-f\tFRANCHISE\t\t\t (Specify -of and rerun to see choices)";
		std::cerr << std::endl;
		std::cerr << "\t-o\tOPERATORS\t\t\t (Specify -of and rerun to see choices)";
		std::cerr << std::endl;
	}
	else
	{
		std::cerr << "\t-f\tFRANCHISE\t\t\t Choices are:";
		print_choices(franchises);
		std::cerr << std::endl;
		std::cerr << "\t-o\tOPERATORS\t\t\t Choices are:";
		print_choices(operators);
		std::cerr << std::endl;
	}

	std::cerr << std::endl;
	std::cerr << "Optional arguments:" << std::endl;
	std::cerr << "\t-r RECORDTYPE\t\t\t Choices are (default): ";
	print_choices(record_types);
	std::cerr << std::endl;
	std::cerr << "\t-d STARTDATE (YYYYMMDDhhmmss)\t Default choice: " << date;
	std::cerr << std::endl;
	std::cerr << "\t-b Use blank UTC Time Zone\t Default choice: " << use_blank;
	std::cerr << std::endl;
	std::cerr << "\t-s Serving Bid\t\t\t Default choice: " << sbid;
	std::cerr << std::endl;
	std::cerr << "\t-n Number of EDR's to generate\t Default choice: " << edr_num;
	std::cerr << std::endl;
	std::cerr << "\t-i Test SIM interval (0=none)\t Default choice: " << test_interval;
	std::cerr << std::endl;
	std::cerr << "\t-v Enable verbose output " << std::endl;
	std::cerr << std::endl;
	//std::cerr << "\t-p Output path\t\t\t Default choice: " << output_path;
	//std::cerr << std::endl;
}

template<typename T> void print_choices(T &choices)
{
	for(typename T::iterator it = choices.begin() ; it != choices.end(); it++)
	{
		std::cerr << " " << get_iterator_element(it);
		//get_iterator_element(it);
	}
}

template<typename T> std::string get_iterator_element(T &it)
{
	return it->first;
}

std::string get_iterator_element(string_vector::iterator &it)
{
	return *it;
}



// Get current date/time, format is YYYYMMDDhhmmss
void time_t_to_char(time_t now, char *buf, size_t max_size)
{
    struct tm  tstruct;
    tstruct = *localtime(&now);
    strftime(buf, max_size, "%Y%m%d%H%M%S", &tstruct);
}

time_t string_to_time_t(std::string time_in)
{
	struct tm *tstruct;
	time_t rawtime;
	time(&rawtime);
	tstruct = localtime(&rawtime);
	tstruct->tm_year = atoi( time_in.substr(0,4).c_str()) - 1900;
	tstruct->tm_mon = atoi( time_in.substr(4,2).c_str()) - 1;
	tstruct->tm_mday = atoi( time_in.substr(6,2).c_str() );
	tstruct->tm_hour = atoi( time_in.substr(8,2).c_str() );
	tstruct->tm_min = atoi( time_in.substr(10,2).c_str() );
	tstruct->tm_sec = atoi( time_in.substr(12,2).c_str() );

	time_t seconds = mktime(tstruct);
	return seconds;
}

std::string make_imsi(std::string &mcc, std::string &mnc, int record)
{
	std::string imsi;
	imsi = (mcc.length() < 3) ? ('0' + mcc) : mcc;
	imsi = (mnc.length() < 2) ? (imsi + '0' + mnc) : (imsi + mnc);
	imsi = imsi + static_methods::pad_number(record, 10);
	return imsi;
}

bool path_exists(const char *path)
{
	if (path == NULL)
	{
		return false;
	}

	bool exists = false;
	DIR *the_dir;
	the_dir = opendir(path);

	if (the_dir != NULL)
	{
		exists = true;
		closedir(the_dir);
	}
	return exists;
}
