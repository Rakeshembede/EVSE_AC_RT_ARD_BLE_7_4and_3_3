

#include "Variants.h"

#include "SimpleOcppOperationFactory.h"

#include "Authorize.h"
#include "BootNotification.h"
#include "GetLocalListVersion.h"
#include "DataTransfer.h"
#include "Heartbeat.h"
#include "MeterValues.h"
#include "GetConfiguration.h"
#include "SetChargingProfile.h"
#include "StatusNotification.h"
#include "StartTransaction.h"
#include "StopTransaction.h"
#include "TriggerMessage.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"
#include "Reset.h"
#include "ChangeConfiguration.h"

/*
* @brief: Added by G. Raja Sumant 08/06/2022
*/
#include "FOTA_OCPP.h"
#include "FirmwareStatus_OCPP.h"
#include "ChangeAvailability.h"
#include "ClearCache.h"
#include "UnlockConnector.h"

/*
* @brief: Added by G. Raja Sumant and Krishna 14/07/2022
* for reservation
*/
#include "ReserveNow.h"
#include "CancelReservation.h"
// #include "SendLocalList.h"

#include "OcppEngine.h"

#include <string.h>

bool flag_trigger_metervalues = false;
#if 0
uint8_t flag_trigger_metervalues = false;
uint8_t flag_trigger_statusnotification = false;
uint8_t flag_trigger_bootnotification = false;
uint8_t flag_trigger_heartbeat = false;
uint8_t flag_trigger_firmwarestatusnotification = false;
uint8_t flag_trigger_diagnosticsstatusnotification = false;
#endif

OnReceiveReqListener onAuthorizeRequest;
void setOnAuthorizeRequestListener(OnReceiveReqListener listener){
  onAuthorizeRequest = listener;
}

OnReceiveReqListener onBootNotificationRequest;
void setOnBootNotificationRequestListener(OnReceiveReqListener listener){
  onBootNotificationRequest = listener;
}

OnReceiveReqListener onTargetValuesRequest;
void setOnTargetValuesRequestListener(OnReceiveReqListener listener) {
  onTargetValuesRequest = listener;
}

OnReceiveReqListener onSetChargingProfileRequest;
void setOnSetChargingProfileRequestListener(OnReceiveReqListener listener){
  onSetChargingProfileRequest = listener;
}

OnReceiveReqListener onStartTransactionRequest;
void setOnStartTransactionRequestListener(OnReceiveReqListener listener){
  onStartTransactionRequest = listener;
}

OnReceiveReqListener onTriggerMessageRequest;
void setOnTriggerMessageRequestListener(OnReceiveReqListener listener){
  onTriggerMessageRequest = listener;
}

OnReceiveReqListener onRemoteStartTransactionReceiveRequest;
void setOnRemoteStartTransactionReceiveRequestListener(OnReceiveReqListener listener) {
  onRemoteStartTransactionReceiveRequest = listener;
}

OnSendConfListener onRemoteStartTransactionSendConf;
void setOnRemoteStartTransactionSendConfListener(OnSendConfListener listener){
  onRemoteStartTransactionSendConf = listener;
}

OnReceiveReqListener onRemoteStopTransactionReceiveRequest;
void setOnRemoteStopTransactionReceiveRequestListener(OnReceiveReqListener listener) {
  onRemoteStopTransactionReceiveRequest = listener;
}

OnSendConfListener onRemoteStopTransactionSendConf;
void setOnRemoteStopTransactionSendConfListener(OnSendConfListener listener){
  onRemoteStopTransactionSendConf = listener;
}

OnSendConfListener onResetSendConf;
void setOnResetSendConfListener(OnSendConfListener listener){
  onResetSendConf = listener;
}

/*
* @brief : Reservation profile. Added on 14/07/2022
*/ 
OnSendConfListener onReserveNowSendConf;
void setOnReserveNowSendConfListener(OnSendConfListener listener){
  onReserveNowSendConf = listener;
}


OnSendConfListener onCancelReservationSendConf;
void setOnCancelReservationSendConfListener(OnSendConfListener listener){
  onCancelReservationSendConf = listener;
}

/*
OnReceiveReqListener onGetLocalListVersionReceiveRequest;
void setOnGetLocalListVersionReceiveRequestListener(OnReceiveReqListener listener) {
  onGetLocalListVersionReceiveRequest = listener;
}

OnSendConfListener onGetLocalListVersionSendConf;
void setOnGetLocalListVersionSendConfListener(OnSendConfListener listener){
  onGetLocalListVersionSendConf = listener;
}

OnReceiveReqListener onSendLocalListReceiveRequest;
void setOnSendLocalListReceiveRequestListener(OnReceiveReqListener listener) {
  onSendLocalListReceiveRequest = listener;
}

OnSendConfListener onSendLocalListSendConf;
void setOnSendLocalListSendConfListener(OnSendConfListener listener){
  onSendLocalListSendConf = listener;
}
*/

OnReceiveReqListener onGetConfigurationReceiveRequest;
void setOnGetConfigurationReceiveRequestListener(OnReceiveReqListener listener) {
  onGetConfigurationReceiveRequest = listener;
}

OnSendConfListener onGetConfigurationSendConf;
void setOnGetConfigurationSendConfListener(OnSendConfListener listener){
  onGetConfigurationSendConf = listener;
}

/*
* @brief Added by G. Raja Sumant for Clear Cache
* 09/07/2022
*/

OnReceiveReqListener onClearCacheReceiveRequest;
void setOnClearCacheReceiveRequestListener(OnReceiveReqListener listener) {
  onClearCacheReceiveRequest = listener;
}

OnSendConfListener onClearCacheSendConf;
void setOnClearCacheSendConfListener(OnSendConfListener listener){
  onClearCacheSendConf = listener;
}

/*
* @brief Added by G. Raja Sumant for FOTA
*/

OnReceiveReqListener onFotaNotificationReceiveRequest;
void setOnFotaNotificationRequestListener(OnReceiveReqListener listener){
  onFotaNotificationReceiveRequest = listener;
}

/*
* @brief Added by G. Raja Sumant for Unlock Connector
*/

OnReceiveReqListener onUnlockConnectorReceiveRequest;
void setOnUnlockConnectorRequestListener(OnReceiveReqListener listener){
  onUnlockConnectorReceiveRequest = listener;
}


/*
* @brief Added by G. Raja Sumant for Change Availability 09/07/2022
*/
OnReceiveReqListener onChangeAvailabilityReceiveRequest;
void setOnChangeAvailabilityRequestListener(OnReceiveReqListener listener){
  onChangeAvailabilityReceiveRequest = listener;
}

/*
OnSendConfListener onGetConfigurationSendConf;
void setOnGetConfigurationSendConfListener(OnSendConfListener listener){
  onGetConfigurationSendConf = listener;
}
*/


OcppOperation* makeFromTriggerMessage(WebSocketsClient *ws, JsonObject payload) {

  //int connectorID = payload["connectorId"]; <-- not used in this implementation
  const char *messageType = payload["requestedMessage"];

  /*
  * @brief : Check if meter values were requested from trigger message. If yes, then set a flag
  */
  if (!strcmp(messageType, "MeterValues"))
  {
    flag_trigger_metervalues = true;
  }
#if 0
    /*
  * @brief : Check if StatusNotification were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "StatusNotification"))
  {
    flag_trigger_statusnotification = true;
  }
  
 /*
  * @brief : Check if BootNotification were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "BootNotification"))
  {
    flag_trigger_bootnotification = true;
  }
  
 /*
  * @brief : Check if Heartbeat were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "Heartbeat"))
  {
    flag_trigger_heartbeat = true;
  }
 /*
  * @brief : Check if FirmwareStatusNotification were requested from trigger message. If yes, then set a flag
  */
  else if (!strcmp(messageType, "FirmwareStatusNotification"))
  {
    flag_trigger_firmwarestatusnotification = true;
  }

 /*
  * @brief : Check if FirmwareStatusNotification were requested from trigger message. If yes, then set a flag
  */ 
  else if (!strcmp(messageType, "DiagnosticsStatusNotification"))
  {
    flag_trigger_diagnosticsstatusnotification = true;
  }
  if (DEBUG_OUT) {
    Serial.print("[SimpleOcppOperationFactory] makeFromTriggerMessage for message type ");
    Serial.print(messageType);
    Serial.print("\n");
  }
  #endif  
  return makeOcppOperation(ws, messageType);
}

OcppOperation *makeFromJson(WebSocketsClient *ws, JsonDocument *json) {
  const char* messageType = (*json)[2];
  return makeOcppOperation(ws, messageType);
}

OcppOperation *makeOcppOperation(WebSocketsClient *ws, const char *messageType) {
  OcppOperation *operation = makeOcppOperation(ws);
  OcppMessage *msg = NULL;

  if (!strcmp(messageType, "Authorize")) {
    msg = new Authorize();
    operation->setOnReceiveReqListener(onAuthorizeRequest);
  } else if (!strcmp(messageType, "BootNotification")) {
    msg = new BootNotification();
    operation->setOnReceiveReqListener(onBootNotificationRequest);
  } else if (!strcmp(messageType, "Heartbeat")) {
    msg = new Heartbeat();
  } else if (!strcmp(messageType, "MeterValues")) {
    msg = new MeterValues();
  } else if (!strcmp(messageType, "SetChargingProfile")) {
    msg = new SetChargingProfile(getSmartChargingService());
    operation->setOnReceiveReqListener(onSetChargingProfileRequest);
  } else if (!strcmp(messageType, "StatusNotification")) {
    msg = new StatusNotification();
  } else if (!strcmp(messageType, "StartTransaction")) {
    msg = new StartTransaction();
    operation->setOnReceiveReqListener(onStartTransactionRequest);
  } else if (!strcmp(messageType, "StopTransaction")) {
    msg = new StopTransaction();
  } else if (!strcmp(messageType, "TriggerMessage")) {
    msg = new TriggerMessage(ws);
    operation->setOnReceiveReqListener(onTriggerMessageRequest);
  } else if (!strcmp(messageType, "RemoteStartTransaction")) {
    msg = new RemoteStartTransaction();
    operation->setOnReceiveReqListener(onRemoteStartTransactionReceiveRequest);
    if (onRemoteStartTransactionSendConf == NULL) 
      Serial.print("[SimpleOcppOperationFactory] Warning: RemoteStartTransaction is without effect when the sendConf listener is not set. Set a listener which initiates the StartTransaction operation.\n");
    operation->setOnSendConfListener(onRemoteStartTransactionSendConf);
  } else if (!strcmp(messageType, "RemoteStopTransaction")) {
    msg = new RemoteStopTransaction();
    operation->setOnReceiveReqListener(onRemoteStopTransactionReceiveRequest);
    if (onRemoteStopTransactionSendConf == NULL) 
      Serial.print("[SimpleOcppOperationFactory] Warning: RemoteStopTransaction is without effect when the sendConf listener is not set. Set a listener which initiates the StopTransaction operation.\n");
    operation->setOnSendConfListener(onRemoteStopTransactionSendConf);
  } /*else if (!strcmp(messageType, "GetLocalListVersion")) {
    msg = new GetLocalListVersion();
    operation->setOnReceiveReqListener(onGetLocalListVersionReceiveRequest);
    if (onGetLocalListVersionSendConf == NULL) 
      Serial.print(F("[SimpleOcppOperationFactory] Warning: GetLocalListVersion is without effect when the sendConf listener is not set. Set a listener which initiates the GetLocalListVersion operation.\n"));
    operation->setOnSendConfListener(onGetLocalListVersionSendConf);
  } else if (!strcmp(messageType, "SendLocalList")) {
    msg = new SendLocalList();
    operation->setOnReceiveReqListener(onSendLocalListReceiveRequest);
    if (onSendLocalListSendConf == NULL) 
      Serial.print(F("[SimpleOcppOperationFactory] Warning: SendLocalList is without effect when the sendConf listener is not set. Set a listener which initiates the SendLocalList operation.\n"));
    operation->setOnSendConfListener(onSendLocalListSendConf);
  } */
  else if (!strcmp(messageType, "ReserveNow")) {
    msg = new ReserveNow();
    if (onReserveNowSendConf == NULL)
      Serial.print("[SimpleOcppOperationFactory] Warning: ReserveNow is without effect when the sendConf listener is not set. Set a listener which ReserveNow your device.\n");
    operation->setOnSendConfListener(onReserveNowSendConf);
  }else if (!strcmp(messageType, "CancelReservation")) {
    msg = new CancelReservation();
    if (onCancelReservationSendConf == NULL)
      Serial.print("[SimpleOcppOperationFactory] Warning: CancelReservation is without effect when the sendConf listener is not set. Set a listener which CancelReservation your device.\n");
    operation->setOnSendConfListener(onCancelReservationSendConf);
  }
  else if (!strcmp(messageType, "Reset")) {
    msg = new Reset();
    if (onResetSendConf == NULL)
      Serial.print("[SimpleOcppOperationFactory] Warning: Reset is without effect when the sendConf listener is not set. Set a listener which resets your device.\n");
    operation->setOnSendConfListener(onResetSendConf);
  } else if (!strcmp(messageType, "GetConfiguration")) {
  /**/
    msg = new GetConfiguration();
    if (onGetConfigurationSendConf == NULL)
      Serial.print("[SimpleOcppOperationFactory] Warning: GetConfiguration is without effect when the sendConf listener is not set. Set a listener which sends getConfiguration.\n");
    operation->setOnSendConfListener(onGetConfigurationSendConf);
    /**/
  }
  /*
  * @brief : Feature added by G. Raja Sumant 09/07/2022
  */
  else if (!strcmp(messageType, "ClearCache")) {
  /**/
    msg = new ClearCache();
    if (onClearCacheSendConf == NULL)
      Serial.print("[SimpleOcppOperationFactory] Warning: ClearCache is without effect when the sendConf listener is not set. Set a listener which sends ClearCache.\n");
    operation->setOnSendConfListener(onClearCacheSendConf);
    /**/
  }
  else if(!strcmp(messageType, "ChangeConfiguration")){
    msg = new ChangeConfiguration();
  }else if(!strcmp(messageType, "DataTransfer")){
    msg = new DataTransfer();

  }
  //Added by G. Raja Sumant for FOTA
  else if(!strcmp(messageType, "UpdateFirmware")){
    msg = new FotaNotification();
  }
   //Added by G. Raja Sumant for Change Availability
  else if(!strcmp(messageType, "ChangeAvailability")){
    msg = new ChangeAvailability();
  }
  //Added by G. Raja Sumant for Unlock Connector
  else if(!strcmp(messageType, "UnlockConnector")){
    msg = new UnlockConnector();
  }
  else {
    Serial.print("[SimpleOcppOperationFactory] Operation not supported");
      //TODO reply error code
  }

  if (msg == NULL) {
    delete operation;
    return NULL;
  } else {
    operation->setOcppMessage(msg);
    operation->print_debug();
    return operation;
  }
}

OcppOperation* makeOcppOperation(WebSocketsClient *ws, OcppMessage *msg){
  if (msg == NULL) {
    Serial.print("[SimpleOcppOperationFactory] in makeOcppOperation(webSocket, ocppMessage): ocppMessage is null!\n");
    return NULL;
  }
  OcppOperation *operation = makeOcppOperation(ws);
  operation->setOcppMessage(msg);
  return operation;
}

OcppOperation* makeOcppOperation(WebSocketsClient *ws){
  OcppOperation *result = new OcppOperation(ws);
  return result;
}
