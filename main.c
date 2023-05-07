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

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"

//mcp

#include "nrf_sdh.h"
#include "app_error.h"
#include "app_timer.h"
#include "fds.h"
#include "nrf_pwr_mgmt.h"

/************************************************
/
/ CAN MCP2515 Include Files
/
*************************************************/

#include "inttypes.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"
#include "mcp_can.h"
#include "SEGGER_RTT.h"
#include "nrf_drv_spi.h"
#include "nrfx_spim.h"
#include "yz_timer.h"





//PERSIONAL FUNCTION DECLARATION



uint8_t Uart_AT[1000];
int Modem_test_result = 0;

uint8_t lis_address = 0x19;
uint8_t lis_sample_data = 0x7F;

bool sim_status = 0 ;
bool result_modem = 0 ;


bool result_qspi_flash = 0 ;
bool result_motion_sensor = 0 ;


bool sim_testing_flag = 1;//To define if SIM CARD TESTING OR NOT.
bool aliyun_testing_FLAG = 1;//To define if gsm/4g TESTING OR NOT.
bool result_aliyun = 0 ;



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
      
        result_qspi_flash = 1 ;//QSPI PASS FLAG
    }
    else
    {
        NRF_LOG_INFO("Data inconsistent");//QSPI FAIL FLAG
     
    }

    nrf_drv_qspi_uninit();

}
//QSPI END





int main(void)
{   

    
    uint32_t count = 0;
 
    ret_code_t ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
  
    nrf_drv_clock_lfclk_request(NULL);

    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();



uart_init_elevate();

//Modem&iic Enable

start_timer();
enable_3v3();
Modem_Pwron();
result_modem = AT_Match();
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


if (sim_testing_flag ==1)//if SIM CARD TESTING OR NOT.
{ 
sim_status = SIM_DET();
if (sim_status == 1)
 {
  NRF_LOG_INFO("Found the SIM card");

 }
else NRF_LOG_WARNING("SIM CARD ERROR");
}
else
NRF_LOG_WARNING("Proceed without sim card");



if (aliyun_testing_FLAG== 1 )
{
//while(!Modem_test_result==1)
//{
  result_aliyun = set_MQTT();
  nrf_delay_ms(1000);
  NRF_LOG_FLUSH();
//}

NRF_LOG_FLUSH();
}


qspi_test();

  //LED ENABLE




//Testing Result show
NRF_LOG_INFO("Testing Result:---------------------------------------------- \r\n");

NRF_LOG_INFO("     MCU(nRF52840):            Passed \r\n");//Of course if passed

if (result_modem == 1)
{
NRF_LOG_INFO("     Modem(SIM7000G):          Passed \r\n");
}
else 
NRF_LOG_INFO("     Modem(SIM7000G):          Failed\r\n");

if (sim_testing_flag ==1)
 {
if (sim_status == 1)
{
NRF_LOG_INFO("     SIMCARD(SIM7000G):        Passed \r\n");
}
else 
NRF_LOG_INFO("     SIMCARD(SIM7000G):        Failed\r\n");
 }

if (aliyun_testing_FLAG ==1)
{
if (result_aliyun == 1)
{
NRF_LOG_INFO("     GSM/LTE(SIM7000G):        Passed \r\n");
}
else 
NRF_LOG_INFO("     GSM/LTE(SIM7000G):        Failed\r\n");
 }
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
