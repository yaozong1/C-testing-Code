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

#include <time.h>

//NBIOT
#define _AT_CHECK nrf_libuarte_async_tx(&libuarte, text, text_size);
#define _DET_SIM nrf_libuarte_async_tx(&libuarte, Dect_sim, Dect_sim_size);
#define _SIG_CHECK nrf_libuarte_async_tx(&libuarte, Sigal_qul, Sigal_qul_size);
#define _LTE_ONLY nrf_libuarte_async_tx(&libuarte, LTE_ONLY, LTE_ONLY_size);
#define _SET_NBIOT nrf_libuarte_async_tx(&libuarte, SET_NBIOT, SET_NBIOT_size);

#define _SET_CATM1 nrf_libuarte_async_tx(&libuarte, SET_CATM1, SET_CATM1_size);
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

#define _GSM_BD_CHECK nrf_libuarte_async_tx(&libuarte, GSM_BD_CHECK, GSM_BD_CHECK_size);
//#define _GSM_BD_CHECK nrf_libuarte_async_tx(&libuarte, GSM_BD_CHECK, GSM_BD_CHECK_size);
//NBIOT END

//CHECK IMEI NUMBER

#define _IMEI_CHECK nrf_libuarte_async_tx(&libuarte, IMEI_CHECK, IMEI_CHECK_size);


//CHECK IMEI NUMBER END
//UART2_FOR GNSS
//#define _GNSS_SNR nrf_libuarte_async_tx(&libuarte_vcu, GNSS_SNR, GNSS_SNR_size);

#define _SEND_ACK_STM nrf_libuarte_async_tx(&libuarte_vcu, SEND_ACK_STM, SEND_ACK_STM_size);

bool status_modem;
volatile int timerCount = 0; // 全局计数器，用于定时器中断

#define UART_RX_BUFFER_SIZE 1000
uint8_t Uart_AT[UART_RX_BUFFER_SIZE];
uint8_t Uart2_AT[UART_RX_BUFFER_SIZE];

//for extract the SN number
uint8_t Uart_Numbers[UART_RX_BUFFER_SIZE];
uint8_t Uart_Numbers_Size = 0;

//PERSIONAL FUNCTION DECLARATION END
int lte_gsm_gps = 0;

static   uint8_t MATCH[] = "\"CHECK";
static   uint8_t OK[] = "OK";
static   uint8_t SMSUB[] = "+SMSUB";
static   uint8_t ERROR[] = "ERROR";
static   uint8_t GNSS_ACK[] = "+CGNSINF";
static   uint8_t GNSS_CON[] = "+CGNSINF: 1";
static   uint8_t GSM_OL[] = "GSM";
static   uint8_t CAT_OL[] = "CAT";
static   uint8_t MD_ACTIVE[] = "PDP:";

NRF_LIBUARTE_ASYNC_DEFINE(libuarte, 0, 0, 0, NRF_LIBUARTE_PERIPHERAL_NOT_USED, 512, 100);
NRF_LIBUARTE_ASYNC_DEFINE(libuarte_vcu, 1, 2, 2, NRF_LIBUARTE_PERIPHERAL_NOT_USED, 512, 100);

typedef struct {
    uint8_t * p_data;
    uint32_t length;
} buffer_t;

NRF_QUEUE_DEF(buffer_t, m_buf_queue, 200, NRF_QUEUE_MODE_NO_OVERFLOW);
NRF_QUEUE_DEF(buffer_t, m_buf_queue_vcu, 200, NRF_QUEUE_MODE_NO_OVERFLOW);/////

static volatile bool m_loopback_phase;
static volatile bool m_loopback_phase_vcu;



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
               //每次进来之后清空
               int size = sizeof(Uart_AT);
               memset(Uart_AT, 0, size);

               NRF_LOG_INFO("Received: %s", p_evt->data.rxtx.p_data);
//后期  加   UART123_AT三个缓存进行对比，每次对比前后进行数据清理
            // 复制数据到    Uart_AT 数组
            uint32_t length = p_evt->data.rxtx.length;
            if (length < UART_RX_BUFFER_SIZE)
             {
                memcpy(Uart_AT, p_evt->data.rxtx.p_data, length);
                
                Uart_AT[length] = 0; // 添加字符串结束符
             }
             else {
                    NRF_LOG_WARNING("TOO FULL TOO FULL");
                // 数据太大，处理异常或放弃
                  }

          if(isPresent(Uart_AT,  GNSS_CON)==1)
                  { 
         nrf_libuarte_async_tx(&libuarte_vcu, Uart_AT, sizeof(Uart_AT));
         nrf_delay_ms(10);
                  }

            //nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
            //处理完数据释放内存       CHATGPT提示  rxfree函数可能和    RTT存在复用冲突关系

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

void uart_event_handler_vcu(void * context, nrf_libuarte_async_evt_t * p_evt)
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

           NRF_LOG_INFO("UART2 Received: %s", p_evt->data.rxtx.p_data);
           //Uart2_AT[0]=0;
           //int size = sizeof(Uart2_AT);
           //memset(Uart2_AT, 0, size);
           //nrf_delay_ms(10);
           //int size2 = sizeof(p_evt->data.rxtx.p_data);
           //memset(p_evt->data.rxtx.p_data, 0, size);
          // strcpy(Uart_AT, p_evt->data.rxtx.p_data);//Copy the received Uart message for comparing        
         //  nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
            
            m_loopback_phase = false;//loop back function from e.g
            break;
        case NRF_LIBUARTE_ASYNC_EVT_TX_DONE:
       
        
        NRF_LOG_FLUSH();
  
            
            if (m_loopback_phase_vcu)
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
            .int_prio   = 1
            
    };

    err_code = nrf_libuarte_async_init(&libuarte, &nrf_libuarte_async_config, uart_event_handler, (void *)&libuarte);

    APP_ERROR_CHECK(err_code);

    nrf_libuarte_async_enable(&libuarte);
    nrf_delay_ms(10);
    err_code = nrf_libuarte_async_tx(&libuarte, text, text_size);
    APP_ERROR_CHECK(err_code);
}


void uart_init_elevate_vcu(void)
{
    ret_code_t err_code;
    nrf_libuarte_async_config_t nrf_libuarte_async_config = {
            .tx_pin     = 5,
            .rx_pin     = 6,
            .baudrate   = NRF_UARTE_BAUDRATE_115200,
            .pullup_rx  = false,
            .parity     = NRF_UARTE_PARITY_EXCLUDED,
            .hwfc       = NRF_UARTE_HWFC_DISABLED,
            .timeout_us = 100,
            .int_prio   = 2
            
    };

    err_code = nrf_libuarte_async_init(&libuarte_vcu, &nrf_libuarte_async_config, uart_event_handler_vcu, (void *)&libuarte_vcu);

    APP_ERROR_CHECK(err_code);

    nrf_libuarte_async_enable(&libuarte_vcu);
    nrf_delay_ms(10);
    err_code = nrf_libuarte_async_tx(&libuarte_vcu, text, text_size);
    APP_ERROR_CHECK(err_code);
}



//bool AT_Match(void)//Define a AT respond function
//{
// uint8_t MATCH[] = "OK";

// while(!isPresent(Uart_AT,  MATCH) == 1)//Matching AT:"OK" from module

//{
//   _AT_CHECK;//Sending AT ack
   
//   NRF_LOG_INFO("Matching....");
//   nrf_delay_ms(100);
   
   
//}
//  int size = sizeof(Uart_AT);//不请空的话在进     uart,会导致对比失效有可能对比的是上次的数据
//  memset(Uart_AT, 0, size);

//return 1;

//}

bool AT_Match(void) {
    uint8_t MATCH[] = "OK";
    timerCount = 0; // 重置计数器

    while (!isPresent(Uart_AT, MATCH)) {
        _AT_CHECK;
        NRF_LOG_INFO("Matching....");
        nrf_delay_ms(100);

        if (timerCount >= 3) { // 10秒超时（假设每2秒中断一次）
            int size = sizeof(Uart_AT);
            memset(Uart_AT, 0, size);
            NRF_LOG_INFO("Matching failed....");
            return 0;
        }
    }

    int size = sizeof(Uart_AT);
    memset(Uart_AT, 0, size);
    NRF_LOG_INFO("AT_Matching Done");
    return 1;
}




bool SIM_DET(void)
{
  int result = 0;
  uint8_t ok[] = "OK";
  uint8_t ready[] = "READY";
  uint8_t error[] = "ERROR";

  NRF_LOG_INFO("sim card detecting start");
  _DET_SIM;
   result = wait_uart_ack_x_second(ready, 5);  
   
  
  //if(isPresent(Uart_AT,  ready)==1)
 if (result == 1)
  return 1;
  
  return 0;
}


void Modem_Pwron(void)
{
    
    NRF_LOG_INFO("Modem Power ON.....");       
    nrf_delay_ms(5);
    NRF_LOG_FLUSH();
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,14));//Indicator
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,3));//PWRKEY
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,9));//WATCH DOD BUT DOESNOT WORK NOW
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,13));//GPS ANTENNA POWER
    nrf_gpio_cfg_input(NRF_GPIO_PIN_MAP(1,11),NRF_GPIO_PIN_PULLDOWN);
    nrf_delay_ms(5);
    status_modem = nrf_gpio_pin_read(NRF_GPIO_PIN_MAP(1,11));
    NRF_LOG_INFO("Modem Status: %d",status_modem);
    nrf_delay_ms(5);
    NRF_LOG_FLUSH();

    if (status_modem == 1)//Modem is on already when testing.
    {
    NRF_LOG_INFO("Modem is on");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(1000);
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("Reboot the modem....");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 1);
    nrf_delay_ms(2000);
    NRF_LOG_FLUSH();

    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    NRF_LOG_INFO("shut down the modem....");
    nrf_delay_ms(1000);
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("Modem is being powering on");
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(1000);
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 1);
    nrf_delay_ms(2000);

    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(1000);
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


   _AT_CHECK;      nrf_delay_ms(10);

   wait_uart_ack(OK);

   _SHU_TCP;       nrf_delay_ms(10);

   wait_uart_ack(OK);

   _GSM_ONLY;      nrf_delay_ms(10);

   wait_uart_ack(OK);
   //_LTE_ONLY;      nrf_delay_ms(500);
   //_SET_NBIOT;     nrf_delay_ms(500);
   _SET_RM;        nrf_delay_ms(10);

   wait_uart_ack(OK);
   //_SET_APN;
   //_APN_CHECK;


   NRF_LOG_INFO("Waiting for modem....");
   NRF_LOG_FLUSH();
   nrf_delay_ms(100);


   _SIG_CHECK;     nrf_delay_ms(10);

   wait_uart_ack(OK);

   _NET_ADHERE;    nrf_delay_ms(10);

   wait_uart_ack(OK);
 
   _NET_TYPE;      nrf_delay_ms(10);

   wait_uart_ack(OK);

    //while(!isPresent(Uart_AT,  OK)==1);
   
    //mqtt

       //_DIS_MQTT;      nrf_delay_ms(500);
       //_DIS_WILESS;    nrf_delay_ms(500);
   _S_APN_MQTT;    nrf_delay_ms(10);
   
   wait_uart_ack(OK);

   wait_uart_ack(MD_ACTIVE);
   
   _COM_MQTT_IP;   nrf_delay_ms(10);
   
   wait_uart_ack(OK);
  
   _SET_MQTT_URL;  nrf_delay_ms(10);
   
   wait_uart_ack(OK);
  
   _S_KEP_T;       nrf_delay_ms(10);
   
   wait_uart_ack(OK);

   _S_USR_N;       nrf_delay_ms(10);

   wait_uart_ack(OK);
  
   _S_PASS_WD;     nrf_delay_ms(10);
   
   wait_uart_ack(OK);
   
   _S_CLE_ID;      nrf_delay_ms(10);
   
    wait_uart_ack(OK);
   
   _C_T_MQTT;      nrf_delay_ms(10);
    
    wait_uart_ack(OK);


      

     // _DIS_MQTT;
     //  nrf_delay_ms(200);
    _SUB_TOP;       nrf_delay_ms(10);
    
    wait_uart_ack(OK);
  
    _PUB_T_TOP;     nrf_delay_ms(10);
    
    

    _SEND_CHECK;    nrf_delay_ms(10);
    
    wait_uart_ack(OK);

       

    NRF_LOG_INFO("Waiting for responding");
    nrf_delay_ms(100);


   for(i=1; i <=200;i++) //Added timeout for waiting
      {
        if(isPresent(Uart_AT,  SMSUB)==1)
       {
         break;
       }
        nrf_delay_ms(50);
      }


   if(isPresent(Uart_AT,  MATCH)==1)
     {
         nrf_delay_ms(50);
         NRF_LOG_INFO("MQTT test done");
         nrf_delay_ms(500);


         return 1;

     }
        return 0;

 }




void lte_gsm_switch_for_toby(void)
{

int net_types = 0;

//////////////////////
NRF_LOG_INFO("Enter loop of lte gsm detecting");
NRF_LOG_WARNING("Need to quit by rebooting");

while(1)

   {
  if (net_types == 0)
  {
  lte_gsm_gps = 0;
  net_types =  try_login_4G();
  lte_gsm_gps = 1;
  }

  if (net_types == 1)
  while(1)
     {
       NRF_LOG_WARNING("4G..............");
       //gps_receive();
       nrf_delay_ms(2000);
     }

  if (net_types == 0)
    {
  lte_gsm_gps = 0;
  net_types = try_login_2G();
  lte_gsm_gps = 1;
    }
  if (net_types == 1)
  while(1)
     {
       NRF_LOG_WARNING("2G..............");
      // gps_receive();
       nrf_delay_ms(2000);
     }
  
   }

}


bool try_login_2G(void)
{

  bool gsm_connected = 0;

  _AT_CHECK;      nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)//如果拿不到读取不到       OK就退出，函数就作为等待使用
   return 0;

   _SHU_TCP;       nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;

   _GSM_ONLY;      nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;

   _SET_RM;        nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;



   NRF_LOG_INFO("Waiting for modem....");
   NRF_LOG_FLUSH();
   nrf_delay_ms(100);


   _SIG_CHECK;     nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;

   _NET_ADHERE;    nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;
 
   _NET_TYPE;      nrf_delay_ms(10);
      
   if(wait_uart_ack_x_second(GSM_OL, 5) ==1)
   gsm_connected = 1;

   return gsm_connected;

}



bool try_login_4G(void)
{

    bool lte_connected = 0;

     _AT_CHECK;      nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)//如果拿不到读取不到       OK就退出，函数就作为等待使用
   return 0;

   _SHU_TCP;       nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;

   _GSM_ONLY;      nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;

   _SET_RM;        nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;



   NRF_LOG_INFO("Waiting for modem....");
   NRF_LOG_FLUSH();
   nrf_delay_ms(100);


   _SIG_CHECK;     nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;

   _NET_ADHERE;    nrf_delay_ms(10);

   if(wait_uart_ack_x_second(OK, 5) ==0)
   return 0;
 
   _NET_TYPE;      nrf_delay_ms(10);
     
   if(wait_uart_ack_x_second(CAT_OL, 5) ==1)
   lte_connected = 1;

   
   return lte_connected;
}


void gps_receive(void)
{


   _AT_CHECK;      nrf_delay_ms(500);

   _GNSS_POW_ON;   nrf_delay_ms(500);

   _GNSS_SIG;

   wait_uart_ack(GNSS_ACK);
      
   nrf_delay_ms(200);


}



void clean_uart_buffer()
{

  int size = sizeof(Uart_AT);//不请空的话在进     uart,会导致对比失效有可能对比的是上次的数据
  memset(Uart_AT, 0, size);

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


int wait_uart_ack(uint8_t *word)
{
int result = 0;
for(int i=1; i <=500;i++) //Added timeout for waiting
       {
        if( isPresent(Uart_AT,  word)==1 )
        {
            NRF_LOG_INFO("Found the ack: %s", word);
            result = 1;
          //  NRF_LOG_FLUSH();
            break; // Exit the loop when the ack is found
         
        }
        nrf_delay_ms(50);
       }
  
  int size = sizeof(Uart_AT);//不请空的话在进     uart,会导致对比失效有可能对比的是上次的数据
  memset(Uart_AT, 0, size);
  return result;
}



int wait_uart_ack_x_second(uint8_t *word, int second)
{

int result = 0;

for(int i=1; i <=second*100;i++) //Added timeout for waiting
       {
        if( isPresent(Uart_AT,  word)==1 )
        {
            NRF_LOG_INFO("Found the ack: %s", word);
            result = 1;
          //  NRF_LOG_FLUSH();
            break; // Exit the loop when the ack is found
         
        }
        nrf_delay_ms(10);
       }
  
  int size = sizeof(Uart_AT);//不请空的话在进     uart,会导致对比失效有可能对比的是上次的数据
  memset(Uart_AT, 0, size);
  return result;
}


void send_ack_to_stm( uint8_t *send_array, size_t size )
{
//uint8_t SEND_ACK_STM[] = {'c', 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};


nrf_libuarte_async_tx(&libuarte_vcu, send_array, size);

// NRF_LOG_INFO("SENT ACK TO STM");

}

void send_ack_to_stm_timer( void )
{

_SEND_ACK_STM;
 NRF_LOG_INFO("SENT ACK TO STM");

}


bool IMEI_NM_CD(uint8_t * serial_number)
{

    NRF_LOG_INFO("IMEI NUMBER CHECKING.....");
    nrf_delay_ms(10);

    _IMEI_CHECK;

//进行寻找，其实就是       wait_uart_ack_x_second(OK, 10);但因为要在清空      Uart_AT之前把值传出来，所以重新写这部分
int result = 0;

for(int i=1; i <=500;i++) //等5秒
       {
        if( isPresent(Uart_AT,  OK)==1 )
        {
            result = 1;

            NRF_LOG_INFO("Get SN number......\r\n");

            break; // Exit the loop when the ack is found
        }

        nrf_delay_ms(10);
       }

//uint8_t *serial_number_8 = (uint8_t *) serial_number;//创建一个指针指向传递过来的指针变量，然后用数组的方式给它赋值，最后值就到达指定数组中


for (int i = 0; i < UART_RX_BUFFER_SIZE; i++) //Extract SN number
    {
        if (is_digit(Uart_AT[i])) {

            serial_number[Uart_Numbers_Size++] = Uart_AT[i]; //serial_number是个指针   把找到的数字一个个存到数组内存上
                   
        }
    }

  //  NRF_LOG_INFO("Uart_Numbers Received: %s", serial_number);

  int size = sizeof(Uart_AT);//不请空的话在进     uart,会导致对比失效有可能对比的是上次的数据
  memset(Uart_AT, 0, size);
  return result;
}



bool is_digit(uint8_t ch) {
    return ch >= '0' && ch <= '9';
}

void extract_numbers() {
    for (uint8_t i = 0; i < UART_RX_BUFFER_SIZE; i++) {
        if (is_digit(Uart_AT[i])) {
            Uart_Numbers[Uart_Numbers_Size++] = Uart_AT[i];
        }
    }
}