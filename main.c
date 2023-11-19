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

#include "qspi_flash.h"

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

uint8_t result[9]; //for sending to esp32 by stm32___can____

uint8_t manufacturer_id_readback_send= 0xFF;

uint8_t imei[15];  //���߸���ȡ���������Ҫ

uint8_t SN_C[30];

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

 // can_bus_start(); There's no canbus in Roam p5 anymore.

    twi_init();

    uart_init_elevate(); 
    
    uart_init_elevate_vcu(); 
      
    start_timer();
 
    Modem_Pwron();
   
    result_modem = AT_Match();

    nrf_delay_ms(10);
    
    NRF_LOG_INFO("AT_Matching Done");

    nrf_delay_ms(10);

    IMEI_NM_CD(imei);

    NRF_LOG_INFO("IMEI_Numbers Received: %s", imei);

    nrf_delay_ms(10);
    
    nrf_delay_ms(10);

    NRF_LOG_INFO("IIC testing start....");

    nrf_delay_ms(10);
    
    result_motion_sensor = Lis_test();

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



//lte_gsm_switch_for_toby();

//    ce_fcc_testing();


 if (aliyun_testing_FLAG== 1 )
      {

         result_aliyun = set_MQTT();
          nrf_delay_ms(10);

      }



    result_qspi_flash = qspi_read_id(&manufacturer_id_readback_send);//Return result and id

    NRF_LOG_INFO("Manufacturer ID_readback_send = 0x%x", manufacturer_id_readback_send);

 //   result_qspi_flash = qspi_test();//�Ϸ���





//Testing Result show
NRF_LOG_INFO("Testing Result:---------------------------------------------- \r\n");

nrf_delay_ms(10);

NRF_LOG_INFO("     MCU(nRF52840):            Passed \r\n");//Of course if passed

if (result_modem == 1)
{
result[0] = 0x11;
NRF_LOG_INFO("     Modem(SIM7000G):          Passed \r\n");
}
else 
NRF_LOG_INFO("     Modem(SIM7000G):          Failed\r\n");

nrf_delay_ms(10);

if (sim_testing_flag ==1)
 {
if (sim_status == 1)
{
result[1] = 0x11;
NRF_LOG_INFO("     SIMCARD(SIM7000G):        Passed \r\n");
}
else 
NRF_LOG_INFO("     SIMCARD(SIM7000G):        Failed\r\n");
 }

else
{
NRF_LOG_INFO("     SIMCARD(SIM7000G):        SKIPED\r\n");
result[1] = 0x04;
}

nrf_delay_ms(10);

if (aliyun_testing_FLAG ==1)
{
if (result_aliyun == 1)
{
result[2] = 0x11;
NRF_LOG_INFO("     GSM/LTE(SIM7000G):        Passed \r\n");
}
else 
NRF_LOG_INFO("     GSM/LTE(SIM7000G):        Failed\r\n");
 }
 else
{
NRF_LOG_INFO("     GSM/LTE(SIM7000G):        SKIPED\r\n");
result[2] = 0x04;
}

nrf_delay_ms(10);

if (result_motion_sensor == 1)
{
result[3] = 0x11;
NRF_LOG_INFO("     Motion Sensor(LIS2DH12):  Passed \r\n");
}
else 
NRF_LOG_INFO("     Motion Sensor(LIS2DH12):  Failed\r\n");

nrf_delay_ms(10);

if (manufacturer_id_readback_send == 0xC2)
{
result[4] = 0x11;
NRF_LOG_INFO("     QSPI Flash(MX25R64):      Passed \r\n");
}
else
NRF_LOG_INFO("     QSPI Flash(MX25R64):      Failed\r\n");

nrf_delay_ms(10);

result[5] = 0x11;//��Ĭ��c  CAN BUS û������

NRF_LOG_INFO("Testing Result:---------------------------------------------- \r\n");

nrf_delay_ms(10);

//NRF_LOG_FLUSH();//���ﲻҪȥ������Ȼ�����ױ�ˢ��


//������Խ����      IMEI,DEVICE ID�� �ϳ�

//First 10pcs number is for result[]
    SN_C[0]  =  'c';
    SN_C[29] =  's';

    memcpy(&SN_C[1], result, sizeof(result)); 
   // memcpy(&SN_C[2], imei,   sizeof(imei)   - 1); 

// 11~25 pcs number is for imei

    // ��  imei���Ƶ�   SN_C�ĺ���λ��
    //SN_C[10]������Ϊ     qt�ı�ʶ
    SN_C[10] =  'q';
    memcpy(&SN_C[11], imei, sizeof(imei)); 

//First 11~25 pcs number is for imei

    SN_C[26] = manufacturer_id_readback_send;
    SN_C[27] =  'n';//ûɶ���ã�ֻ����       qt��ʾһ��
    SN_C[28] =  'n';
    

    //so the qt recognization number is 'q''s' 
    // ȷ��   SN_C��  null��β
    // SN_C[sizeof(SN_C)-1] = 'n';
    // SN_C[sizeof(SN_C)-1] = '\0';

    NRF_LOG_INFO("SNC = %s", SN_C);



    while (true)
    {

     nrf_delay_ms(3000);
     //result[6] = 0x11;

     int size = sizeof(SN_C);

     send_ack_to_stm(SN_C, size);

      NRF_LOG_INFO("SENT TO STM");

     //size = sizeof(SN_C);
     //send_ack_to_stm(SN_C, size);
     //NRF_LOG_INFO("SENT ARRAY TO STM");


     

     /*     
       do{
         if(isPresent(Uart_AT,  start)==1)
         {
         NRF_LOG_INFO("Testing start\r\n");
         nrf_delay_ms(2000);
                uint32_t can_idd = 0x0005;
                uint8_t ext_send;
                uint8_t buff[8] = {1,2,3,4,5,6,7,8};                
                uint8_t lens = 0x08;
                mcp_can_send_msg(can_idd, ext_send, lens, buff);
                nrf_delay_ms(2000);        
         }
         
   
         }while(0);

      do{
      
      
      
      
      ;
      
      
         }while(1);
*/




        } 


    }



/** @} */
