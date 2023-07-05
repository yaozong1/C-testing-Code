#include "uartfunction.h"
#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_libuarte_async.h"
#include "nrf_drv_clock.h"
#include <bsp.h>
#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_queue.h"


#include "boards.h"
#include "sdk_config.h"

#include "nordic_common.h"


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




bool status_modem;



//PERSIONAL FUNCTION DECLARATION END
uint8_t  Uart_AT[1000];
static   uint8_t MATCH[] = "\"CHECK";
static   uint8_t OK[] = "OK";
static   uint8_t SMSUB[] = "+SMSUB";
static   uint8_t ERROR[] = "ERROR";

NRF_LIBUARTE_ASYNC_DEFINE(libuarte, 0, 2, 2, NRF_LIBUARTE_PERIPHERAL_NOT_USED, 1024, 8);


typedef struct {
    uint8_t * p_data;
    uint32_t length;
} buffer_t;

NRF_QUEUE_DEF(buffer_t, m_buf_queue, 30, NRF_QUEUE_MODE_NO_OVERFLOW);

static volatile bool m_loopback_phase;



void uart_event_handler(void * context, nrf_libuarte_async_evt_t * p_evt)
{
    nrf_libuarte_async_t * p_libuarte = (nrf_libuarte_async_t *)context;
    ret_code_t ret;

    switch (p_evt->type)
    {
        case NRF_LIBUARTE_ASYNC_EVT_ERROR:
            bsp_board_led_invert(0);
            break;
        case NRF_LIBUARTE_ASYNC_EVT_RX_DATA:
        NRF_LOG_FLUSH();
            //ret = nrf_libuarte_async_tx(p_libuarte,p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
            //if (ret == NRF_ERROR_BUSY)
            //{
            //    buffer_t buf = {
            //        .p_data = p_evt->data.rxtx.p_data,
            //        .length = p_evt->data.rxtx.length,
            //    };

            //    ret = nrf_queue_push(&m_buf_queue, &buf);
            //    APP_ERROR_CHECK(ret);
            //}
            //else
            //{
            //    APP_ERROR_CHECK(ret);
            //}
           // NRF_LOG_INFO("Received: %s", p_evt->data.rxtx.p_data);
           NRF_LOG_INFO("Received: %s", p_evt->data.rxtx.p_data);
           Uart_AT[0]=0;
           strcpy(Uart_AT, p_evt->data.rxtx.p_data);//Copy the received Uart message for comparing        
           nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
            
            m_loopback_phase = false;//loop back function from e.g
            break;
        case NRF_LIBUARTE_ASYNC_EVT_TX_DONE:
        nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(1,14));
        
        NRF_LOG_FLUSH();
  
            
            if (m_loopback_phase)
            {
                nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
                if (!nrf_queue_is_empty(&m_buf_queue))
                {
                    buffer_t buf;
                    ret = nrf_queue_pop(&m_buf_queue, &buf);
                    APP_ERROR_CHECK(ret);
                    UNUSED_RETURN_VALUE(nrf_libuarte_async_tx(p_libuarte, buf.p_data, buf.length));
                }
            }

            break;
        default:
            break;
    }
}


void uart_init_elevate(void)
{
    ret_code_t err_code;
    nrf_libuarte_async_config_t nrf_libuarte_async_config = {
            .tx_pin     = TX_PIN_NUMBER,
            .rx_pin     = RX_PIN_NUMBER,
            .baudrate   = NRF_UARTE_BAUDRATE_115200,
            .pullup_rx  = false,
            .parity     = NRF_UARTE_PARITY_EXCLUDED,
            .hwfc       = NRF_UARTE_HWFC_DISABLED,
            .timeout_us = 100,
            .int_prio   = APP_IRQ_PRIORITY_LOW
            
    };

    err_code = nrf_libuarte_async_init(&libuarte, &nrf_libuarte_async_config, uart_event_handler, (void *)&libuarte);

    APP_ERROR_CHECK(err_code);

    nrf_libuarte_async_enable(&libuarte);
    nrf_delay_ms(100);
    err_code = nrf_libuarte_async_tx(&libuarte, text, text_size);
    APP_ERROR_CHECK(err_code);
}



bool AT_Match(void)//Define a AT respond function
{
 uint8_t MATCH[] = "OK";

 while(!isPresent(Uart_AT,  MATCH) == 1)//Matching AT:"OK" from module

{
   _AT_CHECK;//Sending AT ack
   
   NRF_LOG_INFO("Matching....");
   nrf_delay_ms(1000);
   NRF_LOG_FLUSH();
   
}
return 1;

}

bool SIM_DET(void)
{
  
  uint8_t ok[] = "OK";
  uint8_t ready[] = "READY";
  uint8_t error[] = "ERROR";
  _DET_SIM;
  nrf_delay_ms(1000);
  if(isPresent(Uart_AT,  ready)==1)
  
  
  return 1;
  
  
  return 0;
}


void Modem_Pwron(void)
{
    
    NRF_LOG_INFO("Modem Power ON.....");       
    nrf_delay_ms(100);
    NRF_LOG_FLUSH();
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,14));//Indicator
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,3));//PWRKEY
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,9));//WATCH DOD BUT DOESNOT WORK NOW
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,13));//GPS ANTENNA POWER
    nrf_gpio_cfg_input(NRF_GPIO_PIN_MAP(1,11),NRF_GPIO_PIN_PULLDOWN);
    nrf_delay_ms(100);
    status_modem = nrf_gpio_pin_read(NRF_GPIO_PIN_MAP(1,11));
    NRF_LOG_INFO("Modem Status: %d",status_modem);
    nrf_delay_ms(100);
    NRF_LOG_FLUSH();

    if (status_modem == 1)//Modem is on already when testing.
    {
    NRF_LOG_INFO("Modem is on");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(500);
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("Reboot the modem....");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 1);
    nrf_delay_ms(2000);
    NRF_LOG_FLUSH();

    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    NRF_LOG_INFO("shut down the modem....");
    nrf_delay_ms(2000);
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("Modem is being powering on");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(500);
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 1);
    nrf_delay_ms(2000);

    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(500);
    NRF_LOG_INFO("POWER ON DONE");


    NRF_LOG_INFO("ENABLE GNSS ");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,13), 1);
    nrf_delay_ms(100);
    NRF_LOG_FLUSH();
    }
    else
    {
    NRF_LOG_INFO("Modem is being powering on");
    
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(500);
    
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 1);
    nrf_delay_ms(2000);

    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(500);
    NRF_LOG_INFO("POWER ON DONE");
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("ENABLE GNSS ");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,13), 1);
    nrf_delay_ms(100);
    NRF_LOG_FLUSH();
    }


}

 bool set_MQTT()
 {
   int i = 1;
   nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(0,9));
   NRF_LOG_INFO("Watchdog fed");

   _AT_CHECK;      
   _SHU_TCP;       nrf_delay_ms(500);
   _GSM_ONLY;      nrf_delay_ms(500);
   //_LTE_ONLY;      nrf_delay_ms(500);
   //_SET_NBIOT;     nrf_delay_ms(500);
   _SET_RM;        nrf_delay_ms(500);
   //_SET_APN;
   //_APN_CHECK;


   NRF_LOG_INFO("Waiting for modem....");
   NRF_LOG_FLUSH();
   nrf_delay_ms(5000);
   NRF_LOG_FLUSH();
   nrf_delay_ms(3000);
   NRF_LOG_FLUSH();
   nrf_delay_ms(3000);
   NRF_LOG_FLUSH();
   nrf_delay_ms(2000);
   NRF_LOG_FLUSH();

   _SIG_CHECK;     nrf_delay_ms(1000);
   _NET_ADHERE;    nrf_delay_ms(1000);
   error_uart_detect();
   _NET_TYPE;      nrf_delay_ms(500);
    //while(!isPresent(Uart_AT,  OK)==1);
   error_uart_detect();



    //mqtt

       //_DIS_MQTT;      nrf_delay_ms(500);
       //_DIS_WILESS;    nrf_delay_ms(500);
      _S_APN_MQTT;    nrf_delay_ms(500);
      error_uart_detect();
      _COM_MQTT_IP;   nrf_delay_ms(500);
      error_uart_detect();
      _SET_MQTT_URL;  nrf_delay_ms(500);
      error_uart_detect();
      _S_KEP_T;       nrf_delay_ms(500);
      error_uart_detect();
      _S_USR_N;       nrf_delay_ms(500);
      error_uart_detect();
      _S_PASS_WD;     nrf_delay_ms(500);
      error_uart_detect();
      _S_CLE_ID;      nrf_delay_ms(500);
      error_uart_detect();
      _C_T_MQTT;      nrf_delay_ms(500);
    //while(!isPresent(Uart_AT,  OK)==1);


      error_uart_detect();

     // _DIS_MQTT;
     //  nrf_delay_ms(200);
    _SUB_TOP;       nrf_delay_ms(500);
      error_uart_detect();
    _PUB_T_TOP;     nrf_delay_ms(500);
    _SEND_CHECK;    

    NRF_LOG_INFO("Waiting for responding");
    nrf_delay_ms(3000);


   for(i=1; i <=20;i++) //Added timeout for waiting
      {
        if(isPresent(Uart_AT,  SMSUB)==1)
       {
         i=20;
       }
        nrf_delay_ms(500);
      }


   if(isPresent(Uart_AT,  MATCH)==1)
     {
         nrf_delay_ms(50);
         NRF_LOG_INFO("MQTT test done");
         nrf_delay_ms(500);


//For Antenna Testing

      while(1)
      {

     _PUB_T_TOP;     nrf_delay_ms(500);
     _SEND_CHECK;   
                     nrf_delay_ms(10000);
     nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(1,7)); 

      }

//For Antenna Testing


         return 1;

     }
        return 0;


 }

void error_uart_detect()
{

   for( int  i = 1; i <= 20; i++) //Added timeout for waiting
  {
     if(isPresent(Uart_AT,  OK)==1)
    {
       i=20;

    }

     if(isPresent(Uart_AT, ERROR)==1)
       {
         NRF_LOG_INFO("Found the error, rebooting");
         nrf_delay_ms(500);
         NVIC_SystemReset();
       }   

     nrf_delay_ms(500);
   }

}

int isPresent( uint8_t *line,  uint8_t *word)//Define a String searching function
{
	int i, j, status, n, m;
	n = strlen(line);
	m = strlen(word);
	for (i = 0; i <= n - m; i++)
        {       
		if (strncmp(line+i, word, m)==0) return 1;
                }
	return 0;

}