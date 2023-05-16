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
#include "yz_timer.h"

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

#include "can_bus_elevate.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"


int Modem_test_result = 0;

bool sim_status = 0 ;
bool result_modem = 0 ;

bool result_qspi_flash = 0 ;
bool result_motion_sensor = 0 ;

bool sim_testing_flag = 0;//To define if SIM CARD TESTING OR NOT.
bool aliyun_testing_FLAG = 0;//To define if gsm/4g TESTING OR NOT.
bool result_aliyun = 0 ;


uint8_t start[] = "START";


//-------CAN DEFINE--------//

uint32_t can_idd = 0x055;
uint8_t ext_send;
uint8_t buff[8] = {'a',2,3,4,5,6,7,8};                
uint8_t lens = 0x08;
bool can_confirm = 0;

//-------CAN DEFINE--------//


int main(void)
{   

    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();



    
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    

    
    ret_code_t ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
  
    nrf_drv_clock_lfclk_request(NULL);

    

    enable_3v3();

    can_bus_start();

    twi_init();

    uart_init_elevate();  

    start_timer();
    
    Modem_Pwron();
   
    result_modem = AT_Match();
    nrf_delay_ms(500);
    
    nrf_delay_ms(500);
    NRF_LOG_INFO("IIC testing start....");
    nrf_delay_ms(50);
    

    result_motion_sensor = Lis_test();

    nrf_delay_ms(500);
    
    
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

         result_aliyun = set_MQTT();
          nrf_delay_ms(2000);

      }

    result_qspi_flash = qspi_test();





//Testing Result show
NRF_LOG_INFO("Testing Result:---------------------------------------------- \r\n");

NRF_LOG_INFO("     MCU(nRF52840):            Passed \r\n");//Of course if passed

if (result_modem == 1)
{
  buff[1] = 11;
  NRF_LOG_INFO("     Modem(SIM7000G):          Passed \r\n");
}

else 
{
  NRF_LOG_INFO("     Modem(SIM7000G):          Failed\r\n");
  buff[1] = 00;
}


if (sim_testing_flag ==1)
 {
if (sim_status == 1)
{
  buff[2] = 11;
  NRF_LOG_INFO("     SIMCARD(SIM7000G):        Passed \r\n");
}

else 
{ buff[2] = 00;
  NRF_LOG_INFO("     SIMCARD(SIM7000G):        Failed\r\n");
}
 }

if (aliyun_testing_FLAG ==1)
{
if (result_aliyun == 1)
  {
 buff[3] = 11;
 NRF_LOG_INFO("     GSM/LTE(SIM7000G):        Passed \r\n");
  }
else 
  { 
  buff[3] = 00;
  NRF_LOG_INFO("     GSM/LTE(SIM7000G):        Failed\r\n");
  }
 }


if (result_motion_sensor == 1)
{
buff[4] = 11;
NRF_LOG_INFO("     Motion Sensor(LIS2DH12):  Passed \r\n");
}
else 
{
buff[4] = 00;
NRF_LOG_INFO("     Motion Sensor(LIS2DH12):  Failed\r\n");
}
if (result_qspi_flash == 1)
{
buff[5] = 11;
NRF_LOG_INFO("     QSPI Flash(MX25R64):      Passed \r\n");
}
else
{
buff[5] = 00;
NRF_LOG_INFO("     QSPI Flash(MX25R64):      Failed\r\n");
}

NRF_LOG_INFO("Testing Result:---------------------------------------------- \r\n");

    while (true)
    {
         //if(isPresent(Uart_AT,  start)==1)
         //{
         //NRF_LOG_INFO("Testing start\r\n");
         //nrf_delay_ms(2000);

         //uint32_t can_idd = 0x0001;
         //uint8_t ext_send;
         //uint8_t buff[8] = {'a',2,3,4,5,6,7,8};                
         //uint8_t lens = 0x08;


         if(!can_confirm)
     {
         NRF_LOG_INFO("Send the result to ESP32 again\r\n");
         mcp_can_send_msg(can_idd, ext_send, lens, buff);//CAN with testing reslut
         nrf_delay_ms(5000);  
          }
                
        if (!nrf_gpio_pin_read(MCP2515_PIN_INT))
      {         
           // nrf_gpio_pin_clear(BSP_LED_3);
            
            if(CAN_MSGAVAIL == mcp_can_check_receive())
            {
                uint32_t can_id;
                uint8_t buf[8];                
                uint8_t len;
                mcp_can_read_msg(&can_id, &len, buf);
                NRF_LOG_INFO("CAN ID: %x\t Data length: %u\t Data:", can_id, len);
                NRF_LOG_HEXDUMP_DEBUG(NRF_LOG_PUSH(buf), len);
                NRF_LOG_FLUSH();
                if ( buf[0] == 'b')
                {
                can_confirm = 1;
                NRF_LOG_INFO("   ESP32 Reviced confirmation \r\n");

                }
            }

            nrf_delay_ms(1);

            }        
                
                


  //       }
  //     __WFE();
    
        } 


    }



/** @} */
