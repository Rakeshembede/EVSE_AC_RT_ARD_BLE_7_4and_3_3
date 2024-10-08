// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef METERINGSERVICE_H
#define METERINGSERVICE_H

#define METER_VALUE_SAMPLE_INTERVAL 60       //30 //in seconds For evre  //changed it to 60 as per semaconnect 

#define METER_VALUES_SAMPLED_DATA_MAX_LENGTH 1 //after 1 measurements, send the values to the CS

#include <LinkedList.h>
// #include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
//#include "../arduinoWebSockets-master/src/WebSocketsClient.h"
//#include "../WebSockets_Generic/src/WebSocketsClient_Generic.h"
#include "../websocket/WebSocketsClient.h"
#include "TimeHelper.h"
//#include "EEPROMLayout.h"
#include "../ATM90E36/ATM90E36.h"
#include "Variants.h"

//typedef float (*PowerSampler)();
typedef float (*EnergyASampler)(float volt, float current, time_t delta);
typedef float (*PowerASampler)(float volt, float current);
typedef float (*EnergyBSampler)();
typedef float (*EnergyCSampler)();
typedef float (*VoltageASampler)();
typedef float (*VoltageBSampler)();
typedef float (*VoltageCSampler)();
typedef float (*CurrentASampler)();
typedef float (*CurrentBSampler)();
typedef float (*CurrentCSampler)();
typedef float (*TemperatureSampler)();
extern ATM90E36 eic;
float roundingFloat(float var);
class MeteringService {
private:
  WebSocketsClient *webSocket;
  time_t lastSampleTimeA = 0; //0 means not charging right now
  time_t lastSampleTimeB = 0; //0 means not charging right now
  time_t lastSampleTimeC = 0; //0 means not charging right now
  float lastPower;
  float lastEnergyA;
  float lastEnergyB;
  float lastEnergyC;
  float lastVoltageA;
  float lastVoltageB;
  float lastVoltageC;
  float lastCurrentA;
  float lastCurrentB;
  float lastCurrentC;
  float lastTemperature;
  LinkedList<time_t> sampleTimeA;
  LinkedList<time_t> sampleTimeB;
  LinkedList<time_t> sampleTimeC;
  LinkedList<float> power;
  LinkedList<float> energyA;  
  LinkedList<float> energyB;  
  LinkedList<float> energyC;  
  LinkedList<float> voltageA;
  LinkedList<float> voltageB;
  LinkedList<float> voltageC;
  LinkedList<float> currentA;
  LinkedList<float> currentB;
  LinkedList<float> currentC;
  LinkedList<float> temperature;

  //float (*powerSampler)() = NULL;
  float (*energyASampler)(float volt, float current, time_t delta) = NULL;
  float (*powerASampler)(float volt, float current) = NULL;
  float (*energyBSampler)() = NULL;
  float (*energyCSampler)() = NULL;
  float (*voltageASampler)() = NULL;
  float (*voltageBSampler)() = NULL;
  float (*voltageCSampler)() = NULL;
  float (*currentASampler)() = NULL;
  float (*currentBSampler)() = NULL;
  float (*currentCSampler)() = NULL;
  float (*temperatureSampler)() = NULL; 
  void addDataPoint(time_t currentTime, float currentPower, float currentEnergy, float currentVoltage, float currentCurrent, float currentTemperature);
  bool timer_init = false;
  
public:
  MeteringService(WebSocketsClient *webSocket);

  void loop();
  void init(MeteringService *meteringService);
  float currentEnergy();

  //void setPowerSampler(PowerSampler powerSampler);
  void setEnergyASampler(EnergyASampler energySampler);
  void setPowerASampler(PowerASampler powerSampler);
  void setEnergyBSampler(EnergyBSampler energySampler);
  void setEnergyCSampler(EnergyCSampler energySampler);
  void setVoltageASampler(VoltageASampler voltageSampler);
  void setVoltageBSampler(VoltageBSampler voltageSampler);
  void setVoltageCSampler(VoltageCSampler voltageSampler);
  void setCurrentASampler(CurrentASampler currentSampler);
  void setCurrentBSampler(CurrentBSampler currentSampler);
  void setCurrentCSampler(CurrentCSampler currentSampler);
  void setTemperatureSampler(TemperatureSampler temperatureSampler);
  
  void flushLinkedListValues();

  float setPresentEnergyASampler(float volt, float current, time_t delta);

  float read_voltageASampler(void);
  float read_currentASampler(void);
  float read_powerASampler(void);
  float read_energyASampler(void);
  float read_temperatureSampler(void);
  void update_metervalues(void);
};

#endif
