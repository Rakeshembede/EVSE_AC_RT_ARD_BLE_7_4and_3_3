

#ifndef STOPTRANSACTION_H
#define STOPTRANSACTION_H

#include "OcppMessage.h"

class StopTransaction : public OcppMessage {
public:
  StopTransaction();

  const char* getOcppOperationType();

  DynamicJsonDocument* createReq();

  void processConf(JsonObject payload);

  void processReq(JsonObject payload);

  DynamicJsonDocument* createConf();
};

#endif
