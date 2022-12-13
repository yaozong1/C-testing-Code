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


#include "IIC_LIS.h"

#include "nrf_drv_qspi.h"
#include "boards.h"
#include "sdk_config.h"



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

//NBIOT END

//PERSIONAL FUNCTION DECLARATION
void AT_Match(void);
void Modem_Pwron(void);
int set_MQTT();


bool status_modem;
uint8_t Uart_AT[1000];
int Modem_test_result = 0;

uint8_t lis_address = 0x19;
uint8_t lis_sample_data = 0x7F;



bool result_modem = 0 ;
bool result_qspi_flash = 0 ;
bool result_motion_sensor = 0 ;


//PERSIONAL FUNCTION DECLARATION END




//QSPI

#define QSPI_STD_CMD_WRSR   0x01
#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99
#define QSPI_TEST_DATA_SIZE 256

#define WAIT_FOR_PERIPH() do { \
        while (!m_finished) {} \
        m_finished = false;    \
    } while (0)

static volatile bool m_finished = false;
static uint8_t m_buffer_tx[QSPI_TEST_DATA_SIZE];
static uint8_t m_buffer_rx[QSPI_TEST_DATA_SIZE];

static void qspi_handler(nrf_drv_qspi_evt_t event, void * p_context)
{
    UNUSED_PARAMETER(event);
    UNUSED_PARAMETER(p_context);
    m_finished = true;
}

static void configure_memory()
{
    uint8_t temporary = 0x40;
    uint32_t err_code;
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = QSPI_STD_CMD_RSTEN,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Send reset enable
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Switch to qspi mode
    cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, &temporary, NULL);
    APP_ERROR_CHECK(err_code);
}

void qspi_test(void)
{
    uint32_t i;
    uint32_t err_code;
    srand(0);
    for (i = 0; i < QSPI_TEST_DATA_SIZE; ++i)
    {
        m_buffer_tx[i] = (uint8_t)rand();
    }
        nrf_drv_qspi_config_t config = NRF_DRV_QSPI_DEFAULT_CONFIG;

    err_code = nrf_drv_qspi_init(&config, qspi_handler, NULL);
   // NRF_LOG_INFO("::::%d",err_code);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("QSPI example started.");

    configure_memory();

    m_finished = false;
    err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, 0);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_PERIPH();
    NRF_LOG_INFO("Process of erasing first block start");

    err_code = nrf_drv_qspi_write(m_buffer_tx, QSPI_TEST_DATA_SIZE, 0);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_PERIPH();
    NRF_LOG_INFO("Process of writing data start");

    err_code = nrf_drv_qspi_read(m_buffer_rx, QSPI_TEST_DATA_SIZE, 0);
    WAIT_FOR_PERIPH();
    NRF_LOG_INFO("Data read");

    NRF_LOG_INFO("Compare...");
    if (memcmp(m_buffer_tx, m_buffer_rx, QSPI_TEST_DATA_SIZE) == 0)
    {
        NRF_LOG_INFO("Data consistent");
      //  NRF_LOG_INFO("QSPI Flash Passed!!!!!!");
        result_qspi_flash = 1 ;
    }
    else
    {
        NRF_LOG_INFO("Data inconsistent");
     //   NRF_LOG_INFO("QSPI Flash Failed!!!!!!");
    }

    nrf_drv_qspi_uninit();

}
//QSPI END



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
           //Uart_AT[0]=0;
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

//UART CONFIGURATION END





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
result_modem = 1;
}



void Modem_Pwron(void)
{
    
    NRF_LOG_INFO("Modem Power ON.....");       
    nrf_delay_ms(1000);
    NRF_LOG_FLUSH();
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,14));//Indicator
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,3));//PWRKEY
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,9));//WATCH DOD BUT DOESNOT WORK NOW
    nrf_gpio_cfg_input(NRF_GPIO_PIN_MAP(1,7),NRF_GPIO_PIN_PULLDOWN);
    nrf_delay_ms(500);
    status_modem = nrf_gpio_pin_read(NRF_GPIO_PIN_MAP(1,7));
    NRF_LOG_INFO("Modem Status: %d",status_modem);
    nrf_delay_ms(500);
    NRF_LOG_FLUSH();

    if (status_modem == 1)
    {
    NRF_LOG_INFO("Modem is on");
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


//Modem&iic Enable
enable_3v3();
Modem_Pwron();
nrf_delay_ms(500);
twi_init();
nrf_delay_ms(500);
NRF_LOG_INFO("IIC testing start....");
nrf_delay_ms(500);
NRF_LOG_FLUSH();

//MODEM&iic ENABLE DONE

//IIC ACK TEST
err_code = nrf_drv_twi_rx(&m_twi, lis_address, &lis_sample_data, sizeof(lis_sample_data));

if(err_code == NRF_SUCCESS)
  {
  
  NRF_LOG_INFO("Successfully detected a device at address: 0x%x", lis_address);
  result_motion_sensor = 1;
  
  
  }
else NRF_LOG_WARNING("Cannot detected a device at address");

//IIC ACK TEST DONE


nrf_delay_ms(500);
NRF_LOG_FLUSH();
nrf_delay_ms(50);
AT_Match();
NRF_LOG_INFO("AT_Matching Done");
nrf_delay_ms(1000);
NRF_LOG_INFO("READY TO GO");


//while(!Modem_test_result==1)
//{
//  Modem_test_result = set_MQTT();
//  nrf_delay_ms(1000);
//  NRF_LOG_FLUSH();
//}

NRF_LOG_FLUSH();
//NRF_LOG_WARNING("Modem Passed!!!!!!");
qspi_test();



//Testing Result show
NRF_LOG_INFO("Testing Result:---------------------------------------------- \r\n");

NRF_LOG_INFO("     MCU(nRF52840):            Passed \r\n");//Of course if passed

if (result_modem == 1)
{
NRF_LOG_INFO("     Modem(SIM7000G):          Passed \r\n");
}
else 
NRF_LOG_INFO("     Modem(SIM7000G):          Failed\r\n");

if (result_motion_sensor == 1)
{
NRF_LOG_INFO("     Motion Sensor(LIS2DH12):  Passed \r\n");
}
else 
NRF_LOG_INFO("     Motion Sensor(LIS2DH12):  Failed\r\n");

if (result_qspi_flash == 1)
{
NRF_LOG_INFO("     QSPI Flash(MX25R64):      Passed \r\n");
}
else
NRF_LOG_INFO("     QSPI Flash(MX25R64):      Failed\r\n");

NRF_LOG_INFO("Testing Result:---------------------------------------------- \r\n");

    while (true)
    {
        count = count+1;
         
        nrf_delay_ms(200); // delay for 500 ms 
        nrf_gpio_pin_toggle(NRF_GPIO_PIN_MAP(0,9));
        NRF_LOG_FLUSH();


               
        } 


    }



/** @} */
