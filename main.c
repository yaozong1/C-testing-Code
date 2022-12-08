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
#include "uartfunction.h"

#include <stdio.h>
#include <string.h>

//NBIOT
#define _AT_CHECK nrf_libuarte_async_tx(&libuarte, text, text_size);
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


//PERSIONAL FUNCTION DECLARATION
void AT_Match(void);
void Modem_Pwron(void);
int set_MQTT();


bool status_modem;
uint8_t Uart_AT[1000];
int Modem_test_result = 0;

//PERSIONAL FUNCTION DECLARATION END




//UART CONFIGURATION
static volatile bool m_loopback_phase;
NRF_LIBUARTE_ASYNC_DEFINE(libuarte, 0, 0, 0, NRF_LIBUARTE_PERIPHERAL_NOT_USED, 1024, 8);


typedef struct {
    uint8_t * p_data;
    uint32_t length;
} buffer_t;

NRF_QUEUE_DEF(buffer_t, m_buf_queue, 30, NRF_QUEUE_MODE_NO_OVERFLOW);






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
           strcpy(Uart_AT, p_evt->data.rxtx.p_data);
           nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
            


            m_loopback_phase = false;
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

//UART CONFIGURATION ENd






void AT_Match(void)//Define a AT respond function
{
 uint8_t MATCH[] = "OK";

 while(!isPresent(Uart_AT,  MATCH) == 1)

{
   _AT_CHECK;//Sending AT ack
   
   NRF_LOG_INFO("Matching....");
   nrf_delay_ms(1000);
   NRF_LOG_FLUSH();

   //NRF_LOG_INFO("Received: %s", Uart_AT);
   //NRF_LOG_INFO("COMPARED1: %s", MATCH);
   //NRF_LOG_INFO("COMPARED2: %s", Uart_AT);
    
}
}



void Modem_Pwron(void)
{
    
    NRF_LOG_INFO("Modem Power ON.....");       
    nrf_delay_ms(1000);
    NRF_LOG_FLUSH();
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,14));//Indicator
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,3));//PWRKEY
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,9));//WATCH DOD BUT DOESNOT WORK NOW
    nrf_gpio_cfg_input(NRF_GPIO_PIN_MAP(1,7),NRF_GPIO_PIN_NOPULL);
    nrf_delay_ms(500);
    status_modem = nrf_gpio_pin_read(NRF_GPIO_PIN_MAP(1,7));
    NRF_LOG_INFO("Modem Status: %d",status_modem);
    nrf_delay_ms(500);
    NRF_LOG_FLUSH();

    if (status_modem == 1)
    {
    NRF_LOG_INFO("Modem is on");
    printf("asdfghjk:%d",status_modem);
    }
    else
    {
    NRF_LOG_INFO("Modem is being powering on");
    
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,14), 0);
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(500);
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,14), 1);
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 1);
    nrf_delay_ms(3000);

    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,14), 0);
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,3), 0);
    nrf_delay_ms(3000);
    NRF_LOG_INFO("POWER ON DONE");
    
    NRF_LOG_FLUSH();
    }


}

int set_MQTT()
{

uint8_t MATCH[] = "\"CHECK";
uint8_t OK[] = "OK";
uint8_t SMSUB[] = "+SMSUB";
nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(0,9));
NRF_LOG_INFO("Watchdog fed");

_AT_CHECK;      
_SHU_TCP;       nrf_delay_ms(200);
_SIG_CHECK;     nrf_delay_ms(200);
_LTE_ONLY;      nrf_delay_ms(200);
_SET_NBIOT;     nrf_delay_ms(200);
_SET_RM;        nrf_delay_ms(200);
//_SET_APN;
//_APN_CHECK;
_NET_ADHERE;    nrf_delay_ms(200);
_NET_TYPE;      nrf_delay_ms(200);
while(!isPresent(Uart_AT,  OK)==1);

//mqtt
_DIS_MQTT;      nrf_delay_ms(500);
_DIS_WILESS;    nrf_delay_ms(500);
_S_APN_MQTT;    nrf_delay_ms(500);
_COM_MQTT_IP;   nrf_delay_ms(500);
_SET_MQTT_URL;  nrf_delay_ms(500);
_S_KEP_T;       nrf_delay_ms(500);
_S_USR_N;       nrf_delay_ms(500);
_S_PASS_WD;     nrf_delay_ms(500);
_S_CLE_ID;      nrf_delay_ms(500);
_C_T_MQTT;      nrf_delay_ms(500);
while(!isPresent(Uart_AT,  OK)==1);
// _DIS_MQTT;
//  nrf_delay_ms(200);
_SUB_TOP;       nrf_delay_ms(500);
_PUB_T_TOP;     nrf_delay_ms(500);
_SEND_CHECK;    while(!isPresent(Uart_AT,  SMSUB)==1);

if(isPresent(Uart_AT,  MATCH)==1)
{
NRF_LOG_INFO("MQTT test done");
return 1;

}
return 0;


}





int main(void)
{   
    

    // Initialize the Logger module and check if any error occured during initialization
  //  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	
	// Initialize the default backends for nrf logger
   // NRF_LOG_DEFAULT_BACKENDS_INIT();

	// print the log msg over uart port
  //  NRF_LOG_INFO("This is log data from nordic device..");

	// a variable to hold counter value
    uint32_t count = 0;



   
    
    ret_code_t ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
  
    nrf_drv_clock_lfclk_request(NULL);

    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_libuarte_async_config_t nrf_libuarte_async_config = {
            .tx_pin     = TX_PIN_NUMBER,
            .rx_pin     = RX_PIN_NUMBER,
            //.cts_pin    = CTS_PIN_NUMBER,
            //.rts_pin    = RTS_PIN_NUMBER,
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
    nrf_delay_ms(1000);
    err_code = nrf_libuarte_async_tx(&libuarte, text, text_size);
    APP_ERROR_CHECK(err_code);


//Modem Enable

Modem_Pwron();



//MODEM ENABLE DONE


AT_Match();
NRF_LOG_INFO("AT_Matching Done");
nrf_delay_ms(1000);
NRF_LOG_INFO("READY TO GO");
NRF_LOG_FLUSH();



while(!Modem_test_result==1)
{
  Modem_test_result = set_MQTT();
  nrf_delay_ms(1000);
  NRF_LOG_FLUSH();
}


NRF_LOG_FLUSH();

NRF_LOG_INFO("Modem Passed!!!!!!");

    while (true)
    {
        count = count+1;
         
        nrf_delay_ms(200); // delay for 500 ms 
        nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(0,9));
        NRF_LOG_FLUSH();


               
        } 


    }



/** @} */
