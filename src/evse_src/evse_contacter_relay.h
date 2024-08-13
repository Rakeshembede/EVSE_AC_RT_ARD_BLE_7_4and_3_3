#ifndef _EVSE_CONTACTER_RELAY_H
#define _EVSE_CONTACTER_RELAY_H
#include <Arduino.h>

#define RELAY_PIN (18)

class RELAY
{
public:
    void evse_relay_init(void);
    void evse_relay_on(void);
    void evse_relay_off(void);
};

#endif