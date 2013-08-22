#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include "static_methods.h"
#include "edr.h"

edr::edr(std::string the_imsi, std::string hpmn_number, std::string vpmn_number, std::string smsc_number, std::string hpmn_utc, std::string event, std::string the_date, std::string sbid, int rec_num, bool use_blank_utc)
: event_type(event), event_date(the_date), record_number(rec_num), serving_bid(sbid), imsi(the_imsi)
{
	memset(&record,' ',sizeof(record));
	populate_default_fields();
	populate_record_entity(smsc_number);
	//srand(time(0));
	if (!use_blank_utc)
	{
		populate(record.TIME_ZONE, hpmn_utc );
	}

	if (event_type == "01")			// Mobile originated
	{
		populate(record.TELE_SERVICE_CODE,"11");
		//todo: set bearer service code to 37 if emergency call
		// TSC = 11 for ivoice
		// TSC = 37 for ivideo
	}
	else if (event_type == "02")	// Mobile terminated
	{
		populate(record.TELE_SERVICE_CODE,"11");
	}
	else if (event_type == "03")	// Call Forward
	{
		populate(record.TELE_SERVICE_CODE,"11");
		populate(record.SUPPL_SERVICE,"20");
		populate(record.THIRD_PARTY_NUMBER, vpmn_number);
	}
	else if (event_type == "04")	// SMS originated
	{
		populate(record.TELE_SERVICE_CODE,"22");
		populate(record.CALL_DURATION, "000000");
	}
	else if (event_type == "05")	// SMS terminated
	{
		populate(record.TELE_SERVICE_CODE,"21");
		populate(record.CALL_DURATION, "000000");
	}
	else							// GPRS
	{
		populate_gprs_fields();
	}
	
	if ( is_incoming_call() )
	{
		populate(record.CALLING_NUMBER, vpmn_number);
		populate(record.CALLED_NUMBER, hpmn_number);
		populate(record.DIALLED_DIGITS, hpmn_number);
		populate(record.CELL_ID,"12345");
	}
	else
	{
		populate(record.CALLING_NUMBER, hpmn_number);
		if ( is_gprs_call() )
		{
			populate(record.CALLED_NUMBER,"GPRS");
			populate(record.DIALLED_DIGITS, "GPRS");
		}
		else
		{
			populate(record.CALLED_NUMBER, vpmn_number);
			populate(record.DIALLED_DIGITS, vpmn_number);
			populate(record.CELL_ID,"12345");
		}
	}
}

void edr::populate_default_fields(void)
{
	populate(record.IMSI, imsi);
	populate(record.EVENT_TYPE,event_type);
	populate(record.DATE, event_date);
	populate(record.CAUSE_FOR_TERMINATION, "00");
	populate(record.SERVING_BID, serving_bid);
	populate(record.CALL_DURATION, "000038");
	populate(record.VOLUME_UPLINK, "000000000000");
	populate(record.VOLUME_DOWNLINK, "000000000000");
}

std::string edr::make_random_num(int highest)
{
	std::string random_integer;
	std::stringstream out;
	int rnum = (rand () % highest) + 1;
	out << rnum;
	random_integer = out.str();
	return random_integer;
}

std::string edr::make_random_ip(void)
{
	std::string ip_address;
	std::string num1 = "123";
	std::string num2 = "123";
	std::string num3 = make_random_num(100);
	std::string num4 = make_random_num(253);
	ip_address =  num1 + "." + num2 + "." + num3 + "." + num4;
	return ip_address;
}

void edr::populate_gprs_fields(void)
{
	int id_base = 70000;
	std::string ggsn = make_random_ip();
	std::string sgsn = make_random_ip();
	std::string pdp = make_random_ip();
	populate(record.APN_NI, "www.flukeed.art");
	populate(record.APN_OI, "www.flukeed.art");
	populate(record.CHARGINGID, static_methods::pad_number( (id_base + record_number), 10) );
	//populate(record.GGSNADDRESS, "123.123.123.123");
	//populate(record.SGSNADDRESS, "123.123.123.124");
	//populate(record.PDPADDRESS,"123.123.123.125");
	populate(record.GGSNADDRESS, ggsn);
	populate(record.SGSNADDRESS, sgsn);
	populate(record.PDPADDRESS,pdp);
	populate(record.VOLUME_UPLINK, "000010242048");
	populate(record.VOLUME_DOWNLINK, "000020484096");
}

void edr::populate_record_entity(std::string smsc_number)
{
	if ( is_voice_call() )
	{
		populate(record.REC_ENTITY_TYPE,"1");
		populate(record.SWITCH,"MSC1");
	}
	if ( is_sms_call() )
	{
		populate(record.REC_ENTITY_TYPE,"2");
		populate(record.SWITCH,smsc_number);
	}
	if ( is_gprs_call() )
	{
		populate(record.REC_ENTITY_TYPE,"3");
	}
}

bool edr::is_incoming_call(void)
{
	bool is_incoming = false;
	if ( (event_type == "02") || (event_type == "05") )
	{
		is_incoming = true;
	}
	return is_incoming;
}

bool edr::is_gprs_call(void)
{
	bool is_gprs = false;
	if (event_type == "06")
	{
		is_gprs = true;
	}
	return is_gprs;
}

bool edr::is_sms_call(void)
{
	bool is_sms = false;
	if ( (event_type == "04") || (event_type == "05") )
	{
		is_sms = true;
	}
	return is_sms;
}

bool edr::is_voice_call(void)
{
	bool is_voice = false;
	if ( (event_type == "01") || (event_type == "02") || (event_type == "03") )
	{
		is_voice = true;
	}
	return is_voice;
}

void edr::to_file(std::string filename)
{
	FILE *fp;
	fp=fopen(filename.c_str(),"ab");
	fwrite(&record,1,sizeof(record),fp);
	fwrite("\n",1,1,fp);
	fclose(fp);
}

// copy the value of "from" to the struct member
// NB this does not check whether the struct member "to" has sufficient space allocated for the length of "from"
void edr::populate(char *to, std::string from)
{
	strncpy(to, from.c_str(), (int)from.length());
}

edr::~edr() {}
