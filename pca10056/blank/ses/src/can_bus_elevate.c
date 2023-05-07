#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "mcp_can.h"
#include "mcp_can_dfs.h"

#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"

#include "can_bus_elevate.h"
//#include "nrf_drv_clock.h"


#define SPI_INSTANCE  0 /**< SPI instance index. */
 const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */

 volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

#define TEST_STRING "Nordic"
 uint8_t       m_tx_buf[] = TEST_STRING;           /**< TX buffer. */
 uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];    /**< RX buffer. */
 const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */

void mem_init(void)
{
    NRF_LOG_DEBUG("Memory Manager Initialization...");

    nrf_mem_init();

    NRF_LOG_DEBUG("Memory Manager Initialization COMPLETED.");
}


void can_init(void)
{
    NRF_LOG_DEBUG("CAN_500K Initialization...");

    mcp_spi_init();

    NRF_LOG_DEBUG(" CAN_500K Initialized");

    START_INIT:
	
    if(mcp_can_begin(CAN_500KBPS, MCP_16MHz) == CAN_OK)
    {
	NRF_LOG_DEBUG("CAN_500K BUS Initialization ok");
    }
    else
    {
	NRF_LOG_DEBUG("CAN_500K BUS Initialization failed");
        NRF_LOG_DEBUG("Init CAN BUS again");
        nrf_delay_ms(1000);
        goto START_INIT;
    }

    NRF_LOG_DEBUG("CAN_500K Initialization COMPLETED.");
}

void can_init2(void)
{
    NRF_LOG_DEBUG("CAN_250K Initialization...");

    mcp_spi_init2();

    NRF_LOG_DEBUG(" CAN_250K Initialized");

    START_INIT:
	
    if(mcp_can_begin(CAN_250KBPS, MCP_16MHz) == CAN_OK)
    {
	NRF_LOG_DEBUG("CAN_250K BUS Initialization ok");
    }
    else
    {
	NRF_LOG_DEBUG("CAN_250K BUS Initialization failed");
        NRF_LOG_DEBUG("Init CAN BUS again");
        nrf_delay_ms(1000);
        goto START_INIT;
    }

    NRF_LOG_DEBUG("CAN_250K Initialization COMPLETED.");
}


void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
}


void can_bus_start(void)
{

// bsp_board_init(BSP_INIT_LEDS);
    NRF_LOG_INFO("SPI example started.");
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,1));//CAN

    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,1), 1);
    nrf_delay_ms(1000);//Give period to change the status

    NRF_LOG_FLUSH();


    mem_init();
    nrf_delay_ms(1000);
     //   mcp_spi_init();
    can_init();
 //   nrf_drv_gpiote_in_init(MCP2515_PIN_INT, &mcp2515_int_config, mcp2515_int_pin_handler);
    nrf_drv_gpiote_in_event_enable(MCP2515_PIN_INT, false);
    nrf_drv_gpiote_in_uninit(MCP2515_PIN_INT);
    mcp_spi_uninit();
    can_init2();
    //nrf_drv_gpiote_in_event_enable(MCP2515_PIN_INT_2, false);
    //nrf_drv_gpiote_in_uninit(MCP2515_PIN_INT_2);
    //mcp_spi_uninit();
    //can_init();


}