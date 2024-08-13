/*******************************************************************************
 *
 * program : evse_dwin.cpp
 * author  : krishna & rakesh
 *
 *
 * Description :
 *
 *
 *******************************************************************************/

#include "evse_dwin.h"
#include "evse_wifi.h"

SoftwareSerial dwin(27, 26);

#include "../evse_lib/src/OCPP/ChargePointStatusService.h"

#include "evse_config.h"

bool dwin_checkResult = false;

extern bool wifi_connect;
extern bool gsm_connect;

extern uint8_t evse_wifiConnected;

extern uint8_t gu8_online_flag;

uint8_t dwin_fault = 0xFF;
uint8_t dwin_page = 0xFF;
uint8_t dwin_status = 0xFF;
uint8_t dwin_4g = 0xFF;
uint8_t dwin_wifi = 0xFF;
uint8_t dwin_cloud = 0xFF;
uint16_t dwin_current = 0xFFFF;
uint16_t dwin_voltage = 0xFFFF;
uint16_t dwin_energy = 0xFFFF;
uint16_t dwin_user_info = 0xFFFF;
uint16_t dwin_charger_info = 0xFFFF;
uint16_t dwin_logo = 0xFFFF;
uint8_t flag_log_clear = 0;

extern String cpSerial;
extern String csSerial;
extern String cscbSerial;
extern String cssnSerial;
extern String CP_Id_m;

extern int hr;   // Number of seconds in an hour
extern int mins; // Remove the number of hours and calculate the minutes.
extern int sec;

extern int gs32_st_time;

extern float current_energy;
extern float voltage_display;
extern float current_display;

extern enum EvseChargePointErrorCode_OtherError_t EvseChargePointErrorCode_OtherError;
extern enum EvseChargePointErrorCode EvseChargePoint_ErrorCode;
extern uint8_t dis_connection_flag;
// #define dwin Serial1

byte buf[8] = {0x5A, 0xA5, 0x05, 0x82, 0x10, 0x03, 0x00, 0x00};
const byte cmd[8] PROGMEM = {0x5A, 0xA5, 0x05, 0x82, 0x10, 0x03, 0x00, 0x00};

byte page_buf[10] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00};
const byte page_cmd[10] PROGMEM = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00};

void evse_dwin_setup()
{

  dwin.begin(115200);
  // Serial1.begin(115200, SERIAL_8N1, 27, 26);
}

/***************************************************************************************************************8*/
void evse_send_cmd(bool checkResult, uint8_t pkt_type)
{

  dwin.flush();

  switch (pkt_type)
  {
  case EVSE_SET_PACKET_ONE:
    /* code */
    dwin.write(page_buf, 10);
    break;

  case EVSE_SET_PACKET_TWO:
    /* code */
    dwin.write(buf, 8);
    break;

  case EVSE_SET_PACKET_THREE:
    /* code */
    break;
  default:
    break;
  }

  evse_dump_cmd(pkt_type);

  if (checkResult)
  {
    evse_check_return();
  }
}

void evse_dump_cmd(uint8_t pkt_type)
{
  Serial.println();
  Serial.print(millis());

  Serial.print(PSTR(" OUT>>"));
  uint8_t lu8_len = 0;
  switch (pkt_type)
  {
  case EVSE_SET_PACKET_ONE:
    /* code */
    lu8_len = 10;
    break;

  case EVSE_SET_PACKET_TWO:
    /* code */
    lu8_len = 8;
    break;

  case EVSE_SET_PACKET_THREE:
    /* code */
    break;

  default:
    break;
  }
  // for (int i = 0; i < 10; i++)
  for (int i = 0; i < lu8_len; i++)
  {
    // Serial.print(buf[i] < 0x10 ? PSTR(" 0") : PSTR(" "));
    // Serial.print(buf[i], HEX);

    switch (pkt_type)
    {
    case EVSE_SET_PACKET_ONE:
      Serial.print(page_buf[i] < 0x10 ? PSTR(" 0") : PSTR(" "));
      Serial.print(page_buf[i], HEX);
      break;

    case EVSE_SET_PACKET_TWO:
      Serial.print(buf[i] < 0x10 ? PSTR(" 0") : PSTR(" "));
      Serial.print(buf[i], HEX);

      break;

    case EVSE_SET_PACKET_THREE:
      /* code */
      break;

    default:
      break;
    }
  }
  Serial.println();
}

void evse_check_return()
{
  unsigned long startMs = millis();
  while (((millis() - startMs) < 100) && (!dwin.available()))
    ;
  if (dwin.available())
  {
    Serial.print(millis());
    Serial.print(PSTR(" IN>>>"));
    while (dwin.available())
    {
      byte ch = (byte)dwin.read();
      Serial.print((ch < 0x10 ? PSTR(" 0") : PSTR(" ")));
      Serial.print(ch, HEX);
    }
    Serial.println();
  }
}

void evse_page_change(uint8_t page_num)
{
  if (dwin_page != page_num)
  {
    memcpy_P(page_buf, page_cmd, 10);

    page_buf[9] = page_num;
    evse_send_cmd(true, EVSE_SET_PACKET_ONE);
    dwin_page = page_num;
  }
}

void evse_page_change_zero(void)
{
  evse_page_change(EVSE_SET_PAGE_ZERO);
}

void evse_page_change_one(void)
{
  evse_page_change(EVSE_SET_PAGE_ONE);
}

void evse_page_change_two(void)
{
  evse_page_change(EVSE_SET_PAGE_TWO);
}

void evse_page_change_three(void)
{
  evse_page_change(EVSE_SET_PAGE_THREE);
}

void evse_page_change_four(void)
{
  evse_page_change(EVSE_SET_PAGE_FOUR);
}
/*******************************************8*/
void evse_update_4G_logo(uint8_t logo)
{
  if (dwin_4g != logo)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_4G_CLEAR_LOGO_ADDR_MSB;
    buf[5] = EVSE_4G_AVAILABLE_LOGO_ADDR_LSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = logo;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_4g = logo;
  }
}

#if 0
void evse_4g_avaliable(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_4G_CLEAR_LOGO_ADDR_MSB;
  buf[5] = EVSE_4G_AVAILABLE_LOGO_ADDR_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_4G_AVAILABLE_LOGO;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_4g_unavaliable(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_4G_CLEAR_LOGO_ADDR_MSB;
  buf[5] = EVSE_4G_AVAILABLE_LOGO_ADDR_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_4G_UNAVAILABLE_LOGO;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
void evse_4g_clear_logo(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_4G_CLEAR_LOGO_ADDR_MSB;
  buf[5] = EVSE_4G_AVAILABLE_LOGO_ADDR_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_4G_CLEAR_LOGO;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
#endif

void evse_update_wifi_logo(uint8_t logo)
{
  if (dwin_wifi != logo)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_WIFI_CLEAR_LOGO_ADDR_MSB;
    buf[5] = EVSE_WIFI_AVAILABLE_LOGO_ADDR_LSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = logo;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_wifi = logo;
  }
}

#if 0
void evse_wifi_available(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_WIFI_CLEAR_LOGO_ADDR_MSB;
  buf[5] = EVSE_WIFI_AVAILABLE_LOGO_ADDR_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_WIFI_AVAILABLE_LOGO;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
void evse_wifi_unavailable(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_WIFI_CLEAR_LOGO_ADDR_MSB;
  buf[5] = EVSE_WIFI_AVAILABLE_LOGO_ADDR_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_WIFI_UNAVAILABLE_LOGO;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_wifi_clear_logo(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_WIFI_CLEAR_LOGO_ADDR_MSB;
  buf[5] = EVSE_WIFI_AVAILABLE_LOGO_ADDR_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_WIFI_CLEAR_LOGO;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
#endif
/*******************************************************/
/**
 * Updates the status if it has changed, sends the updated status command, and sets the internal status.
 *
 * @param status The new status to be updated.
 *
 * @return None
 *
 * @throws None
 */
void evse_update_status(uint8_t status)
{
  if (dwin_status != status)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_OCPP_STATUS_MSB;
    buf[5] = EVSE_OCPP_STATUS_LSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = status;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_status = status;
  }
}

#if 0
void evse_available_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_AVAILABLE_STATUS;

  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_preparing_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_PREPARING_STATUS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_charging_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_CHARGING_STATUS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_suspendedev_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_SUSPENDEDEV_STATUS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_suspendedevse_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_SUSPENDEDEVSE_STATUS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_finishing_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_FINISHING_STATUS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_unavailable_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_UNAVAILABLE_STATUS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
#endif
/**
 * Updates the fault status if it has changed, sends the updated fault command, and sets the internal fault status.
 *
 * @param fault The new fault status to be updated.
 *
 * @throws None
 */
void evse_update_fault(uint8_t fault)
{
  if (dwin_fault != fault)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_FAULT_MSB;
    buf[5] = EVSE_FAULT_LSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = fault;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_fault = fault;
  }
}
/**
 * Updates the user information if it has changed.
 *
 * @param user_info The new user information to update.
 *
 * @throws None
 */
void evse_update_user_info(uint8_t user_info)
{
  if (dwin_user_info != user_info)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_USER_INFO_MSB;
    buf[5] = EVSE_USER_INFO_LSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = user_info;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_user_info = user_info;
  }
}

void evse_update_charger_info(uint8_t charger_info)
{
  if (dwin_charger_info != charger_info)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_USER_INFO_MSB;
    buf[5] = EVSE_USER_INFO_MSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = charger_info;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_charger_info = charger_info;
  }
}

#if 0
void evse_faulted_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_OCPP_STATUS_MSB;
  buf[5] = EVSE_OCPP_STATUS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_FAULTED_STATUS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_earth_disconnect_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_EARTH_DISCONNECT;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_emergency_stop_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_EMERGANCY_STOP;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_power_loss_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_POWER_LOSS;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_other_error_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_OTHER_ERROR;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_over_volatge_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_OVER_VOLTAGE;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_under_voltage_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_UNDER_VOLTAGE;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_over_temp_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_OVER_TEMP;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_over_current_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_OVER_CURRENT;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_clear_error_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_FAULT_MSB;
  buf[5] = EVSE_FAULT_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_ERROR_CLEAR;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
#endif
void evse_cloud_null_status(void)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_CLOUD_MSB;
  buf[5] = EVSE_CLOUD_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = EVSE_CLOUD_NULL;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_cloud_online_status(void)
{
  if (dwin_cloud != EVSE_CLOUD_ONLINE)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_CLOUD_MSB;
    buf[5] = EVSE_CLOUD_LSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = EVSE_CLOUD_ONLINE;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_cloud = EVSE_CLOUD_ONLINE;
  }
}

void evse_cloud_offline_status(void)
{
  if (dwin_cloud != EVSE_CLOUD_OFFLINE)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_CLOUD_MSB;
    buf[5] = EVSE_CLOUD_LSB;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = EVSE_CLOUD_OFFLINE;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_cloud = EVSE_CLOUD_OFFLINE;
  }
}

void evse_voltage_value(uint16_t voltage)
{
  uint16_t dwin_voltage = voltage * 10;
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_VOLTAGE_MSB;
  buf[5] = EVSE_VOLTAGE_LSB;
  buf[6] = (dwin_voltage >> 8);
  buf[7] = (dwin_voltage & 0xff);
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
void evse_current_value(uint16_t current)
{
  uint16_t dwin_current = current * 100;
  //  uint16_t dwin_current = current ;
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_CURRENT_MSB;
  buf[5] = EVSE_CURRENT_LSB;
  buf[6] = (dwin_current >> 8);
  buf[7] = (dwin_current & 0xff);
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_energy_value(uint16_t energy)
{
  // uint16_t dwin_energy = energy * 100;
  uint16_t dwin_energy = energy / 10; // for kwh caluclation
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_ENERGY_MSB;
  buf[5] = EVSE_ENERGY_LSB;
  buf[6] = (dwin_energy >> 8);

  buf[7] = (dwin_energy & 0xff);

  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
/**
 * Updates the EVSE time based on the input time value.
 *
 * @param time the total time in seconds
 *
 * @throws None
 */
void evse_update_time(uint32_t time)
{

  uint8_t hours = time / 3600;
  uint8_t minutes = (time % 3600) / 60;
  uint8_t seconds = time % 60;

  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_TIME_MSB;
  buf[5] = EVSE_TIME_HOURS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = hours;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);

  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_TIME_MSB;
  buf[5] = EVSE_TIME_MINUTES_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = minutes;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);

  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_TIME_MSB;
  buf[5] = EVSE_TIME_SECONDS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = seconds;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_connecter_icon(uint8_t logo)
{
  if (dwin_logo != logo)
  {
    memcpy_P(buf, cmd, 8);
    buf[4] = EVSE_SET_SIXTEEN;
    buf[5] = EVSE_SET_THIRTYSIX;
    buf[6] = EVSE_SET_ZERO;
    buf[7] = logo;
    evse_send_cmd(true, EVSE_SET_PACKET_TWO);
    dwin_logo = logo;
  }
}

#if 0
void evse_hours_value(uint8_t hours)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_TIME_MSB;
  buf[5] = EVSE_TIME_HOURS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = hours;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_minutes_value(uint8_t minutes)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_TIME_MSB;
  buf[5] = EVSE_TIME_MINUTES_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = minutes;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}

void evse_seconds_value(uint8_t seconds)
{
  memcpy_P(buf, cmd, 8);
  buf[4] = EVSE_TIME_MSB;
  buf[5] = EVSE_TIME_SECONDS_LSB;
  buf[6] = EVSE_SET_ZERO;
  buf[7] = seconds;
  evse_send_cmd(true, EVSE_SET_PACKET_TWO);
}
#endif
void connection_status_update()
{
  // evse_update_charger_info(EVSE_SET_ONE);

  if (flag_log_clear == 0)
  {
    evse_update_wifi_logo(EVSE_WIFI_CLEAR_LOGO);
    evse_update_4G_logo(EVSE_4G_CLEAR_LOGO);
    flag_log_clear = 1;
  }

  if (gu8_online_flag == 1)
  {
    evse_cloud_online_status();
  }
  else
  {
    evse_cloud_offline_status();
  }

  if (evse_wifiConnected == 1)
  {
    evse_update_wifi_logo(EVSE_WIFI_AVAILABLE_LOGO);
  }
  else
  {
    evse_update_wifi_logo(EVSE_WIFI_UNAVAILABLE_LOGO);
  }
}

#if 1
void evse_dwin_state_update()
{
  static ChargePointStatus lenum_evse_ChargePointStatus = NOT_SET;
  static enum EvseChargePointErrorCode_OtherError_t dip_EvseChargePointErrorCode_OtherError = EVSE_NO_Other_Error;

  static uint8_t lenum_dis_connection_flag = -1;
  static enum evse_display_chargeing EVSE_display_charging;

  connection_status_update();

  switch (evse_ChargePointStatus)
  {

  case Available:
  {
    evse_page_change(EVSE_SET_ZERO);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    evse_update_status(EVSE_SET_TWO);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    // evse_connecter_icon(EVSE_SET_TWO);
    evse_update_user_info(EVSE_SET_THREE);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  break;
  case Preparing:
  {
    evse_page_change(EVSE_SET_ZERO);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    evse_update_status(EVSE_SET_THREE);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    evse_update_user_info(EVSE_SET_FOUR);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  break;
  case Charging:
  {
  }
  break;

  case SuspendedEVSE:
  {
  }
  break;

  case SuspendedEV:
  {
  }
  break;
  case Finishing: // not supported by this client
  {
  }
  break;
  case Reserved: // Implemented reserve now
  {
    // Serial.print("Reserved");
  }
  break;
  case Unavailable: // Implemented Change Availability
  {
  }
  break;
  case Faulted: // Implemented Faulted.
  {
    evse_page_change(EVSE_SET_ONE);
  }
  break;
  default:

    break;
  }

  switch (EvseChargePoint_ErrorCode)
  {
  case EVSE_ConnectorLockFailure:
    break;

  case EVSE_EVCommunicationError:
    break;

  case EVSE_GroundFailure:
    //  evse_update_fault(EVSE_EARTH_DISCONNECT);
    break;

  case EVSE_HighTemperature:
    //  evse_update_fault(EVSE_EARTH_DISCONNECT);
    break;

  case EVSE_InternalError:
    break;

  case EVSE_LocalListConflict:
    break;

  case EVSE_NoError:
  {
  }
  break;

  case EVSE_OtherError:
  {

    switch (EvseChargePointErrorCode_OtherError)
    {
    case EVSE_Earth_Disconnect:
    {
      evse_page_change(EVSE_SET_ONE);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      evse_update_status(EVSE_SET_SIX);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      evse_update_fault(EVSE_SET_ONE);
    }
    break;

    case EVSE_EmergencyStop:
    {
      evse_page_change(EVSE_SET_ONE);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      evse_update_status(EVSE_SET_SIX);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      evse_update_fault(EVSE_SET_TWO);
    }
    break;

    case EVSE_PowerLoss:
    {
      evse_page_change(EVSE_SET_ONE);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      evse_update_status(EVSE_SET_SIX);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      evse_update_fault(EVSE_SET_THREE);
    }
    break;
    }
  }

  break;

  case EVSE_OverCurrentFailure:

    break;

  case EVSE_OverVoltage:

    break;

  case EVSE_PowerMeterFailure:

    break;

  case EVSE_ReaderFailure:

    break;

  case EVSE_ResetFailure:

    break;

  case EVSE_UnderVoltage:

    break;

  case EVSE_WeakSignal:

    break;

  default:
    break;
  }

  switch (evse_ChargePointStatus)
  {
  case Charging:

    switch (EVSE_display_charging)
    {
    case VOLTAGE_DISPLAY:
    {
      // evse_voltage_value(voltage_display);
      // Serial.println(" voltage in display>>>>>>>>: " + String(voltage_display));
      // evse_voltage_value(250);
      EVSE_display_charging = CURRENT_DISPLAY;
    }
    break;

    case CURRENT_DISPLAY:
    {
      // evse_current_value(current_display);
      // Serial.println(" current in display>>>>>>>>: " + String(current_display));
      // evse_current_value(16);
      EVSE_display_charging = ENERGY_DISPLAY;
    }
    break;

    case ENERGY_DISPLAY:
    {

      //  evse_energy_value((current_energy)/1000);
      // evse_energy_value(current_energy);
      // Serial.println(" currEnergy display>>>>>>>>: " + String(current_energy));
      // evse_energy_value(112);
      EVSE_display_charging = TIME_HOURS_DISPLAY;
    }
    break;
    case TIME_HOURS_DISPLAY:
    {

      // evse_hours_value(hr);
      // Serial.println(" currEnergy houres>>>>>>>>: " + String(hr));
      // evse_energy_value(112);
      EVSE_display_charging = TIME_MINUTES_DISPLAY;
    }
    break;

    case TIME_MINUTES_DISPLAY:
    {
      // evse_minutes_value(mins);
      // Serial.println(" currEnergy minutes>>>>>>>>: " + String(mins));
      EVSE_display_charging = TIME_SECONDS_DISPLAY;
    }
    break;

    case TIME_SECONDS_DISPLAY:
    {
      // evse_seconds_value(sec);
      // Serial.println(" currEnergy seconds>>>>>>>>: " + String(sec));
      EVSE_display_charging = VOLTAGE_DISPLAY;
    }
    break;

    default:
      break;
    }
    //   dip_EvseChargePointErrorCode_OtherError = EvseChargePointErrorCode_OtherError;
    // }
  }
}

#endif

void convertStrToHex(const String &str, String &strH)
{
  strH = ""; // Clear the output string

  for (size_t i = 0; i < str.length(); ++i)
  {
    char hex[3]; // Buffer to hold two hex digits and a null terminator
    sprintf(hex, "%02X", (unsigned char)str.charAt(i));
    strH += hex;
  }
}