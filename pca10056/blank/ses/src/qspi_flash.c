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

#include "qspi_flash.h"



//QSPI

#define QSPI_STD_CMD_WRSR   0x01
#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99
#define QSPI_TEST_DATA_SIZE 256


#define REMS_COMMAND 0x9F//RDID METHOD REFER TO DATASHEET

#define WAIT_FOR_PERIPH() do { \
        while (!m_finished) {} \
        m_finished = false;    \
    } while (0)

static volatile bool m_finished = false;
static uint8_t m_buffer_tx[QSPI_TEST_DATA_SIZE];
static uint8_t m_buffer_rx[QSPI_TEST_DATA_SIZE];



static void qspi_handler(nrf_drv_qspi_evt_t event, void * p_context)
{
    NRF_LOG_INFO("interupt.....");
     NRF_LOG_FLUSH();
    UNUSED_PARAMETER(event);
    UNUSED_PARAMETER(p_context);
    m_finished = true;
}

static bool configure_memory()
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
    if (err_code != NRF_SUCCESS)
    return 0 ;
    // APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    if (err_code != NRF_SUCCESS)
    return 0 ;
    //APP_ERROR_CHECK(err_code);

    // Switch to qspi mode
    cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, &temporary, NULL);
    if (err_code != NRF_SUCCESS)
    return 0; 
    //APP_ERROR_CHECK(err_code);
    return 1;
}



bool qspi_test(void)
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
   
    if (err_code != NRF_SUCCESS)
    return false ;

   // APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("QSPI example started.");

     if (configure_memory() == false)

     return false ; 

    m_finished = false;
    err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, 0);
   // APP_ERROR_CHECK(err_code);
    WAIT_FOR_PERIPH();
    NRF_LOG_INFO("Process of erasing first block start");

    err_code = nrf_drv_qspi_write(m_buffer_tx, QSPI_TEST_DATA_SIZE, 0);
   // APP_ERROR_CHECK(err_code);
    WAIT_FOR_PERIPH();
    NRF_LOG_INFO("Process of writing data start");

    err_code = nrf_drv_qspi_read(m_buffer_rx, QSPI_TEST_DATA_SIZE, 0);
    WAIT_FOR_PERIPH();
    NRF_LOG_INFO("Data read");

    NRF_LOG_INFO("Compare...");
    if (memcmp(m_buffer_tx, m_buffer_rx, QSPI_TEST_DATA_SIZE) == 0)
    {
        NRF_LOG_INFO("Data consistent");
        nrf_drv_qspi_uninit();
        return 1 ;//QSPI PASS FLAG
    }
    else
    {
        NRF_LOG_INFO("Data inconsistent");//QSPI FAIL FLAG
        nrf_drv_qspi_uninit();
        return 0 ;
     
    }

    //nrf_drv_qspi_uninit();

}
//QSPI END





// ��ȡ�����̺��豸       ID
uint32_t read_ids(uint8_t address, uint16_t *manufacturer_id, uint16_t *device_id) 

{
    uint32_t err_code;

    uint8_t command_sequence[4] = {
        REMS_COMMAND,       // REMS����    
        0x00,               // �����ֽ�1
        0x00,               // �����ֽ�2
        address             // ��ַ�ֽ�
    };
//9F��������Ҫ�����ֽ�1&2���е�ַ�ֽ�
    
    uint8_t response[3] = {0x01, 0x02, 0x03};

    // ʹ��   QSPI���Զ���ָ��ӿ�������         REMS���������ֽ�
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = command_sequence[0],
        .length    = NRF_QSPI_CINSTR_LEN_5B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = false  // ����Ҫдʹ��
    };


    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, command_sequence, response);
    //if ( err_code != false ) 
    //{
      
    //}
     if (err_code != NRF_SUCCESS)
         return NRF_ERROR_TIMEOUT ;
     else
     {
        NRF_LOG_INFO("Get response from flash successfully");
        *manufacturer_id = response[0];
         *device_id      = response[1];
      }

    // ���ݵ�ַ��ȷ�����ȶ�������           ID�����豸    ID     //for COMMAND 0x90
    //    if (address == 0x00) {
    //    *manufacturer_id = response[0];
    //    *device_id = response[1];
    //} else {
    //    *device_id = response[0];
    //    *manufacturer_id = response[1];
    //}


    NRF_LOG_INFO("Manufacturer ID = 0x%x", response[0]);
    NRF_LOG_INFO("Memory Type     = 0x%x", response[1]);
    NRF_LOG_INFO("Memory density  = 0x%x", response[2]);


    return NRF_SUCCESS;
}


bool qspi_read_id(uint16_t *manufacturer_id_readback)
{
    uint32_t err_code;
    
    uint16_t manufacturer_id = 0xFF, device_id = 0xFF;

    // ��ʼ��   QSPI�����֮ǰû�г�ʼ����
    NRF_LOG_INFO("qspi init.....");
 
    nrf_drv_qspi_config_t config = NRF_DRV_QSPI_DEFAULT_CONFIG;

    err_code = nrf_drv_qspi_init(&config, qspi_handler, NULL);
     if (err_code != NRF_SUCCESS)
    {   
     NRF_LOG_INFO("Init..... failed..");
     return false ;
    }
     else
        NRF_LOG_INFO("Init..... successfully \r\n");

//    APP_ERROR_CHECK(err_code);
    // Handle error...

    // ʹ�õ�ַ0    x00����������     ID���豸   ID
    
    err_code = read_ids(0x01, &manufacturer_id, &device_id);//device_id������   memory type����Ϊ�˴��õ�������  9     9  F
//  APP_ERROR_CHECK(err_code);

    if (err_code == NRF_SUCCESS)
    {
    NRF_LOG_INFO("qspi reading IDs successfully.....\r\n");

    NRF_LOG_INFO("Manufacturer ID = 0x%x, Menory Type = 0x%X", manufacturer_id, device_id);

    *manufacturer_id_readback = manufacturer_id;
    
    }
    // Handle error...

    // ����  ID������������...

    return NRF_SUCCESS ;


}