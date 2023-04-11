#ifndef UARTFUNCTION_H
#define UARTFUNCTION_H

#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>



extern int isPresent( uint8_t *line,  uint8_t *word);

//Modem AT COMMAND START

static uint8_t text[] = "AT\r\n";
static uint8_t text_size = sizeof(text);

static uint8_t Dect_sim[] = "AT+CPIN?\r\n";
static uint8_t Dect_sim_size = sizeof(Dect_sim);


static uint8_t Sigal_qul[] = "AT+CSQ\r\n";
static uint8_t Sigal_qul_size = sizeof(Sigal_qul);

static uint8_t LTE_ONLY[] = "AT+CNMP=38\r\n";
static uint8_t LTE_ONLY_size = sizeof(LTE_ONLY);

static uint8_t GSM_ONLY[] = "AT+CNMP=13\r\n";
static uint8_t GSM_ONLY_size = sizeof(LTE_ONLY);

static uint8_t SET_NBIOT[] = "AT+CMNB=2\r\n";
static uint8_t SET_NBIOT_size = sizeof(SET_NBIOT);

static uint8_t SET_RM[] = "AT+NBSC=1\r\n";
static uint8_t SET_RM_size = sizeof(SET_RM);

static uint8_t SET_APN[] = "AT+CSTT=\"iot.1nce.net\"\r\n";
static uint8_t SET_APN_size = sizeof(SET_APN);

static uint8_t APN_CHECK[] = "AT+CGNAPN\r\n";
static uint8_t APN_CHECK_size = sizeof(APN_CHECK);

static uint8_t NET_ADHERE[] = "AT+CGATT?\r\n";
static uint8_t NET_ADHERE_size = sizeof(NET_ADHERE);

static uint8_t NET_TYPE[] = "AT+CPSI?\r\n";
static uint8_t NET_TYPE_size = sizeof(NET_TYPE);

static uint8_t MOBILE_ACT[] = "AT+CIICR\r\n";
static uint8_t MOBILE_ACT_size = sizeof(MOBILE_ACT);

static uint8_t G_LOC_IP[] = "AT+CIFSR\r\n";
static uint8_t G_LOC_IP_size = sizeof(G_LOC_IP);

static uint8_t SHU_TCP[] = "AT+CIPSHUT\r\n";
static uint8_t SHU_TCP_size = sizeof(SHU_TCP);


static uint8_t  S_APN_MQTT[] = "AT+CNACT=1,\"cmnet\"\r\n"; //iot.1nce.net,CMNBIOT
static uint8_t  S_APN_MQTT_size = sizeof(S_APN_MQTT);

static uint8_t COM_MQTT_IP[] = "AT+CNACT?\r\n";
static uint8_t COM_MQTT_IP_size = sizeof(COM_MQTT_IP);

static uint8_t SET_MQTT_URL[] = "AT+SMCONF=\"URL\",iot-06z00a18ae93m7j.mqtt.iothub.aliyuncs.com,1883\r\n";
static uint8_t SET_MQTT_URL_size = sizeof(SET_MQTT_URL);

static uint8_t S_KEP_T[] = "AT+SMCONF=\"KEEPTIME\",60\r\n";
static uint8_t S_KEP_T_size = sizeof(S_KEP_T);

static uint8_t S_USR_N[] = "AT+SMCONF=\"USERNAME\",7000c&i0skjc3rzVY\r\n";
static uint8_t S_USR_N_size = sizeof(S_USR_N);

static uint8_t S_PASS_WD[] = "AT+SMCONF=\"PASSWORD\",408c0124beac2d5ee1271ae710314e7d8edc212d1d992fd48a0ff03b0337d412\r\n";
static uint8_t S_PASS_WD_size = sizeof(S_PASS_WD);

static uint8_t S_CLE_ID[] = "AT+SMCONF=\"CLIENTID\",\"i0skjc3rzVY.7000c|securemode=2,signmethod=hmacsha256,timestamp=1670342856035|\"\r\n";
static uint8_t S_CLE_ID_size = sizeof(S_CLE_ID);

static uint8_t C_T_MQTT[] = "AT+SMCONN\r\n";
static uint8_t C_T_MQTT_size = sizeof(C_T_MQTT);

static uint8_t DIS_MQTT[] = "AT+SMDISC\r\n";
static uint8_t DIS_MQTT_size = sizeof(DIS_MQTT);

static uint8_t DIS_WILESS[] = "AT+CNACT=0\r\n";
static uint8_t DIS_WILESS_size = sizeof(DIS_WILESS);


static uint8_t SUB_TOP[] = "AT+SMSUB=\"/i0skjc3rzVY/7000c/user/send\",1\r\n";
static uint8_t SUB_TOP_size = sizeof(SUB_TOP);

static uint8_t PUB_T_TOP[] = "AT+SMPUB=\"/i0skjc3rzVY/7000c/user/send\",7,1,0\r\n";
static uint8_t PUB_T_TOP_size = sizeof(PUB_T_TOP);

static uint8_t SEND_CHECK[] = "NET_OK\r\n";
static uint8_t SEND_CHECK_size = sizeof(SEND_CHECK);

static uint8_t SEND_MoSensor_SAT[] = "MSR_OK\r\n";
static uint8_t SEND_MoSensor_SAT_size = sizeof(SEND_MoSensor_SAT);

static uint8_t SEND_QSPI_SAT[] = "QSP_OK\r\n";
static uint8_t SEND_QSPI_SAT_size = sizeof(SEND_QSPI_SAT);

static uint8_t GNSS_POW_ON[] = "AT+CGNSPWR=1\r\n";
static uint8_t GNSS_POW_ON_size = sizeof(GNSS_POW_ON);

static uint8_t GNSS_SIG[] = "AT+CGNSINF\r\n";
static uint8_t GNSS_SIG_size = sizeof(GNSS_SIG);



//Modem AT COMMAND END