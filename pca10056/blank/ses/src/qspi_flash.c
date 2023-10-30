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


#define REMS_COMMAND 0x9F

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





// 获取制造商和设备       ID
uint32_t read_ids(uint8_t address, uint16_t *manufacturer_id, uint16_t *device_id) {
    uint32_t err_code;

    uint8_t command_sequence[4] = {
        REMS_COMMAND,       // REMS命令
        
        0x00,               // 虚拟字节1
        0x00,               // 虚拟字节1
        0x00,                // 虚拟字节2
        address            // 地址字节
    };

    uint8_t response[3] = {0x01, 0x02,0x3};

    // 使用   QSPI的自定义指令接口来发送         REMS命令及其后续字节
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = command_sequence[0],
        .length    = NRF_QSPI_CINSTR_LEN_5B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = false  // 不需要写使能
    };

    //nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,18));//try
    //nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,18), 0);

    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, command_sequence, response);
    NRF_LOG_INFO("IDs = %d", err_code);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    // 根据地址，确定是先读制造商           ID还是设备    ID
    NRF_LOG_INFO("Manufacturer ID = 0x%04x", response[0]);
    NRF_LOG_INFO("Manufacturer ID = 0x%04x", response[1]);
    NRF_LOG_INFO("Manufacturer ID = 0x%04x", response[2]);
    if (address == 0x00) {
        *manufacturer_id = response[0];
        *device_id = response[1];
    } else {
        *device_id = response[0];
        *manufacturer_id = response[1];
    }

    return NRF_SUCCESS;
}


int qspi_read_id(void)
{
uint32_t err_code;
    uint16_t manufacturer_id = 0xFE, device_id = 0xAA;

    // 初始化   QSPI（如果之前没有初始化）
    NRF_LOG_INFO("qspi init.....");
    nrf_drv_qspi_config_t config = NRF_DRV_QSPI_DEFAULT_CONFIG;
    //err_code = nrf_drv_qspi_init(&config, NULL, NULL);
    err_code = nrf_drv_qspi_init(&config, qspi_handler, NULL);
    APP_ERROR_CHECK(err_code);
    // Handle error...

    // 使用地址0   x00来读制造商     ID和设备   ID
    NRF_LOG_INFO("qspi reading IDs.....");
    err_code = read_ids(0x01, &manufacturer_id, &device_id);
    APP_ERROR_CHECK(err_code);

    if (err_code == false)
    {
    NRF_LOG_INFO("qspi reading IDs successfully.....");
    NRF_LOG_INFO("IDs = %d", manufacturer_id);
    NRF_LOG_INFO("Manufacturer ID = 0x%04X, Device ID = 0x%04X", manufacturer_id, device_id);

    
    }
    // Handle error...

    // 根据  ID进行其他操作...

    return 0;


}