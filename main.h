#ifndef __MAIN_H
#define __MAIN_H

//function declarations
std::string make_smsc(std::string country_idd, int rec_num);
std::string make_xpmn_suffix(std::string country_idd);
bool cmd_option_exists(char** begin, char** end, const std::string& option);
char** get_cmd_option(char ** begin, char ** end, const std::string & option);
void init_options(op_map &operators, string_map &);
void init_events(string_map &events);
void init_types(string_vector &);
bool file_exists(std::string);
template <typename T> void process_file(std::string filename, void (*process_function)(std::vector<std::string> &, T &), T &container);
void map_operators(std::vector<std::string> &fields, op_map &operators);
void map_simcards(std::vector<std::string> &fields, sim_map &sim_cards);
void print_help(op_map &operators, string_map &franchises, string_vector &record_types, std::string &date, int edr_num, bool use_blank, std::string em_num, std::string sbid, int test_interval, std::string output_path);
template<typename T> void print_choices(T &choices);
template<typename T> std::string get_iterator_element(T &it);
std::string get_iterator_element(string_vector::iterator &it);
void time_t_to_char(time_t now, char* buf, size_t max_size);
time_t string_to_time_t(std::string time_in);
std::string make_imsi(std::string &mcc, std::string &mnc, int record);
bool path_exists(const char *path);

#endif
