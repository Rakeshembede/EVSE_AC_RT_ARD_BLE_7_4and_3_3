

#ifndef AUTHORIZE_H
#define AUTHORIZE_H

#include <EEPROM.h>

#include "OcppMessage.h"
//#include "EEPROMLayout.h"

class Authorize : public OcppMessage {
private:
	String idTag;
public:
	Authorize();

	Authorize(String &idTag);

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();

};

#endif
