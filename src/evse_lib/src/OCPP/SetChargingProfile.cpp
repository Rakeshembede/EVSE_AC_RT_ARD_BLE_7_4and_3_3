

#include "Variants.h"

#include "SetChargingProfile.h"

// SetChargingProfile Set_ChargingProfile;

SetChargingProfile::SetChargingProfile(SmartChargingService *smartChargingService) 
  : smartChargingService(smartChargingService) {

}

const char* SetChargingProfile::getOcppOperationType(){
    return "SetChargingProfile";
}

void SetChargingProfile::processReq(JsonObject payload) {

  int connectorID = payload["connectorId"]; //<-- not used in this implementation

  JsonObject csChargingProfiles = payload["csChargingProfiles"];

  smartChargingService->updateChargingProfile(&csChargingProfiles);
}

DynamicJsonDocument* SetChargingProfile::createConf(){ //TODO review
  DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
  JsonObject payload = doc->to<JsonObject>();
  payload["status"] = "Accepted";
  return doc;
}
