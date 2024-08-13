
#ifndef REMOTESTARTTRANSACTION_H
#define REMOTESTARTTRANSACTION_H

#include "Variants.h"

#include "OcppMessage.h"
#include "ChargePointStatusService.h"

class RemoteStartTransaction : public OcppMessage {
private:
	String idTag;
	int connectorId = 1; 
	bool accepted;
public:
	RemoteStartTransaction();

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
};

#endif
