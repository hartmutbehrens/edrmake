#ifndef __STRUCTURES_H
#define __STRUCTURES_H
#include <map>
#include <string>
#include <vector>

typedef struct
{
	std::string PLMN;
	std::string NAME;
	std::string TYPE;
	std::string MCC;
	std::string MNC;
	std::string MARKET_TYPE;
	std::string NETWORK_TYPE;
	std::string THRESHOLD;
	std::string TAX_APPLIED;
	std::string TAX_IN_TAP;
	std::string TAX_RETRIEVED;
	std::string COUNTRY_CODE;
	std::string COUNTRY_IDD;
	std::string COUNTRY_NAME;
	std::string UTC_TIME_OFFSET;
	std::string CURRENCY;
} operator_struct;									// structure to hold data from ir_operator table

typedef struct
{
	std::string FRANCHISE;
	std::string OPERATOR;
	std::string IMSI;
	std::string MSISDN;
	std::string OWNER;
	std::string THRESHOLD;
	std::string RATIO;
	std::string CARD_STATUS;
} simcard_struct;									// structure to hold data from ir_tst_sim_card table

typedef std::map<std::string, std::string> string_map;	
typedef std::vector<std::string> string_vector;
typedef std::map<std::string, operator_struct> op_map;
typedef std::map<std::string, std::string> sim_map;
#endif
