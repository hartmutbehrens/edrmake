#ifndef __EDR_H
#define __EDR_H
#include <string>
#include "structures.h"

class edr 
{
private:
	/* constructors not used */
	edr(const edr &) {}								//pre-emptive definition, avoid copy construction for now
	edr & operator=(const edr &) { return *this; }	//pre-emptive definition, avoid construction by assignment
	/* members */
	/* structures */
	typedef struct
	{
		char EVENT_TYPE				[2];
		char CALLING_NUMBER			[20];
		char CALLED_NUMBER			[20];
		char DATE				[14];
		char CALL_DURATION			[6];
		char VOLUME_UPLINK			[12];
		char VOLUME_DOWNLINK		        [12];
		char IMSI				[15];
		char ESN_IMEI				[15];
		char TIME_ZONE				[5];
		char SWITCH				[16];
		char CELL_ID				[5];
		char LOCATION_AREA			[5];
		char TELE_SERVICE_CODE	        	[2];
		char BEARER_SERVICE_CODE	        [2];
		char SUPPL_SERVICE			[2];
		char PDPADDRESS				[15];
		char APN_NI				[63];
		char APN_OI				[37];
		char CHARGINGID				[10];
		char GGSNADDRESS			[15];
		char SGSNADDRESS			[15];
		char REC_ENTITY_TYPE		        [1];
		char DIALLED_DIGITS			[20];
		char THIRD_PARTY_NUMBER		        [20];
		char CAUSE_FOR_TERMINATION	        [2];
		char CAMEL_SERV_KEY			[10];
		char CAMEL_SERV_LEVEL		        [1];
		char CAMEL_DEST_NUMBER		        [20];
		char CAMEL_CALL_REFERENCE	        [3];
		char DEFAULT_CALL_HAND_IND	        [1];
		char EPS_ERROR_CODE			[50];
		char PREVIOUS_DUR_VOL		        [12];
		char PREVIOUS_VALUE			[10];
		char PREVIOUS_DATE			[8];
		char ORIG_FILE_NAME_FROM_EPS            [54];
		char SERVING_BID			[5];
		char FILLER				[95];
	} edr_struct;
	std::string make_random_num(int highest);
	std::string make_random_ip(void);
public:
	/* members */
	edr_struct record;
	std::string event_date;
	std::string event_type;
	std::string imsi;
	std::string serving_bid;
	int record_number;
	/* constructors */
	edr(std::string imsi, std::string hpmn_number, std::string vpmn_number, std::string smsc_number, std::string hpmn_utc, std::string event, std::string date, std::string sbid, int rec_num, bool use_blank_utc);
	/* functions */
	bool is_incoming_call(void);
	bool is_gprs_call(void);
	bool is_sms_call(void);
	bool is_voice_call(void);
	void to_file(std::string filename);
	void populate(char *to, std::string from);
	void populate_default_fields(void);
	void populate_gprs_fields(void);
	void populate_record_entity(std::string smsc_number);
	/* destructor */
	~edr();
};
#endif
