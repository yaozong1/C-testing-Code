#ifndef _UART_C_H_
#define _UART_C_H_
//NBIOT
#define _AT_CHECK nrf_libuarte_async_tx(&libuarte, text, text_size);
#define _DET_SIM nrf_libuarte_async_tx(&libuarte, Dect_sim, Dect_sim_size);
#define _SIG_CHECK nrf_libuarte_async_tx(&libuarte, Sigal_qul, Sigal_qul_size);
#define _LTE_ONLY nrf_libuarte_async_tx(&libuarte, LTE_ONLY, LTE_ONLY_size);
#define _SET_NBIOT nrf_libuarte_async_tx(&libuarte, SET_NBIOT, SET_NBIOT_size);
#define _SET_RM nrf_libuarte_async_tx(&libuarte, SET_RM, SET_RM_size)
#define _SET_APN nrf_libuarte_async_tx(&libuarte, SET_APN, SET_APN_size)
#define _APN_CHECK nrf_libuarte_async_tx(&libuarte, APN_CHECK, APN_CHECK_size)
#define _NET_ADHERE nrf_libuarte_async_tx(&libuarte, NET_ADHERE, NET_ADHERE_size)
#define _NET_TYPE nrf_libuarte_async_tx(&libuarte, NET_TYPE, NET_TYPE_size)
#define _SHU_TCP nrf_libuarte_async_tx(&libuarte, SHU_TCP, SHU_TCP_size)
//mqtt
#define _S_APN_MQTT nrf_libuarte_async_tx(&libuarte, S_APN_MQTT, S_APN_MQTT_size)
#define _COM_MQTT_IP nrf_libuarte_async_tx(&libuarte, COM_MQTT_IP, COM_MQTT_IP_size)
#define _SET_MQTT_URL nrf_libuarte_async_tx(&libuarte, SET_MQTT_URL, SET_MQTT_URL_size)
#define _S_KEP_T nrf_libuarte_async_tx(&libuarte, S_KEP_T, S_KEP_T_size)
#define _S_USR_N nrf_libuarte_async_tx(&libuarte, S_USR_N, S_USR_N_size)
#define _S_PASS_WD nrf_libuarte_async_tx(&libuarte, S_PASS_WD, S_PASS_WD_size)
#define _S_CLE_ID nrf_libuarte_async_tx(&libuarte, S_CLE_ID, S_CLE_ID_size)
#define _C_T_MQTT nrf_libuarte_async_tx(&libuarte, C_T_MQTT, C_T_MQTT_size)
#define _DIS_MQTT nrf_libuarte_async_tx(&libuarte, DIS_MQTT, DIS_MQTT_size)
#define _DIS_WILESS nrf_libuarte_async_tx(&libuarte, DIS_WILESS, DIS_WILESS_size)
#define _SUB_TOP nrf_libuarte_async_tx(&libuarte, SUB_TOP, SUB_TOP_size)
#define _PUB_T_TOP nrf_libuarte_async_tx(&libuarte, PUB_T_TOP, PUB_T_TOP_size)
#define _SEND_CHECK nrf_libuarte_async_tx(&libuarte, SEND_CHECK, SEND_CHECK_size)

#define _GNSS_POW_ON nrf_libuarte_async_tx(&libuarte, GNSS_POW_ON, GNSS_POW_ON_size)
#define _GNSS_SIG nrf_libuarte_async_tx(&libuarte, GNSS_SIG, GNSS_SIG_size)

//GSM ONLY
#define _GSM_ONLY nrf_libuarte_async_tx(&libuarte, GSM_ONLY, GSM_ONLY_size);
//NBIOT END

uint8_t Uart_AT[1000];


extern void uart_init(void);

#endif