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

#include <stdio.h>
#include <string.h>


//Modem AT COMMAND START

static uint8_t text[] = "AT\r\n";
static uint8_t text_size = sizeof(text);

static uint8_t Sigal_qul[] = "AT+CSQ\r\n";
static uint8_t Sigal_qul_size = sizeof(Sigal_qul);

static uint8_t LTE_ONLY[] = "AT+CNMP=38\r\n";
static uint8_t LTE_ONLY_size = sizeof(LTE_ONLY);

static uint8_t SET_NBIOT[] = "AT+CMNB=2\r\n";
static uint8_t SET_NBIOT_size = sizeof(SET_NBIOT);

static uint8_t SET_RM[] = "AT+NBSC=1\r\n";
static uint8_t SET_RM_size = sizeof(SET_RM);

static uint8_t SET_APN[] = "AT+CSTT=\"CMNBIOT\"\r\n";
static uint8_t SET_APN_size = sizeof(SET_APN);

static uint8_t APN_CHECK[] = "AT+CGNAPN\r\n";
static uint8_t APN_CHECK_size = sizeof(APN_CHECK);

static uint8_t NET_ADHERE[] = "AT+CGATT?\r\n";
static uint8_t NET_ADHERE_size = sizeof(NET_ADHERE);

static uint8_t NET_TYPE[] = "AT+CPSI?\r\n";
static uint8_t NET_TYPE_size = sizeof(NET_TYPE);

#define _AT_CHECK nrf_libuarte_async_tx(&libuarte, text, text_size);
#define _SIG_CHECK nrf_libuarte_async_tx(&libuarte, Sigal_qul, Sigal_qul_size);
#define _LTE_ONLY nrf_libuarte_async_tx(&libuarte, LTE_ONLY, LTE_ONLY_size);
#define _SET_NBIOT nrf_libuarte_async_tx(&libuarte, SET_NBIOT, SET_NBIOT_size);
#define _SET_RM nrf_libuarte_async_tx(&libuarte, SET_RM, SET_RM_size)
#define _SET_APN nrf_libuarte_async_tx(&libuarte, SET_APN, SET_APN_size)
#define _APN_CHECK nrf_libuarte_async_tx(&libuarte, APN_CHECK, APN_CHECK_size)
#define _NET_ADHERE nrf_libuarte_async_tx(&libuarte, NET_ADHERE, NET_ADHERE_size)
#define _NET_TYPE nrf_libuarte_async_tx(&libuarte, NET_TYPE, NET_TYPE_size)

//Modem AT COMMAND END


//PERSIONAL FUNCTION DECLARATION
void AT_Match(void);
void Modem_Pwron(void);
int isPresent( uint8_t *line,  uint8_t *word);

bool status_modem;
uint8_t Uart_AT[64];

//PERSIONAL FUNCTION DECLARATION END




//UART CONFIGURATION
static volatile bool m_loopback_phase;
NRF_LIBUARTE_ASYNC_DEFINE(libuarte, 0, 0, 0, NRF_LIBUARTE_PERIPHERAL_NOT_USED, 255, 8);


typedef struct {
    uint8_t * p_data;
    uint32_t length;
} buffer_t;

NRF_QUEUE_DEF(buffer_t, m_buf_queue, 10, NRF_QUEUE_MODE_NO_OVERFLOW);






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
           strcpy(Uart_AT, p_evt->data.rxtx.p_data);
           
            


            m_loopback_phase = true;
            break;
        case NRF_LIBUARTE_ASYNC_EVT_TX_DONE:
        nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(1,14));
        NRF_LOG_FLUSH();
  
            
            //if (m_loopback_phase)
            //{
            //    nrf_libuarte_async_rx_free(p_libuarte, p_evt->data.rxtx.p_data, p_evt->data.rxtx.length);
            //    if (!nrf_queue_is_empty(&m_buf_queue))
            //    {
            //        buffer_t buf;
            //        ret = nrf_queue_pop(&m_buf_queue, &buf);
            //        APP_ERROR_CHECK(ret);
            //        UNUSED_RETURN_VALUE(nrf_libuarte_async_tx(p_libuarte, buf.p_data, buf.length));
            //    }
            //}

            break;
        default:
            break;
    }
}

//UART CONFIGURATION END







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



    while (true)
    {
        count = count+1;
         
        nrf_delay_ms(200); // delay for 500 ms 
        nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(0,9));
        NRF_LOG_FLUSH();

        if ((count % 50) == 0)
        {
        _AT_CHECK;
        NRF_LOG_INFO("Counter Value: %d", count) 
        nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(0,9));
        NRF_LOG_INFO("WatchDog Fed");
        NRF_LOG_FLUSH();
        
        
        
        } 


    }
}


/** @} */
