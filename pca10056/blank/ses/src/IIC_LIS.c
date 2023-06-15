#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "IIC_LIS.h"
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

uint8_t lis_address = 0x19;
uint8_t lis_sample_data = 0x7F;

void twi_init(void)
{
  ret_code_t err_code;

  const nrf_drv_twi_config_t twi_config = {
    .scl   = 27,
    .sda   = 26,
    .frequency = NRF_DRV_TWI_FREQ_100K,
    .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
    .clear_bus_init  = false,
    .hold_bus_uninit = false

  
  };

  //nrf_drv_twi_enable(&m_twi);
  err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
  APP_ERROR_CHECK(err_code);
  
  nrf_drv_twi_enable(&m_twi);


}

void enable_3v3(void)
{
//LED ENABLE
nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,16));//CAN
nrf_delay_ms(2);

nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,17));//CAN

nrf_delay_ms(2);
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,16), 0);
nrf_delay_ms(2);
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,17), 0);


nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,14));

//ENABLE CE_CHARGER
nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,8));
nrf_delay_ms(2);
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,8),0);

//3v3 ENABLE
nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,6));
nrf_delay_ms(2);     //doesn't work on Roam_PCBA
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,6),1);
nrf_delay_ms(2);

nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,0));//CAN
nrf_delay_ms(2);
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,0), 1);

nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,12));//GPS
nrf_delay_ms(2);
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,12), 1);
nrf_delay_ms(2);

}


bool Lis_test(void)
{
//IIC ACK TEST

ret_code_t err_code;
err_code = nrf_drv_twi_rx(&m_twi, lis_address, &lis_sample_data, sizeof(lis_sample_data));

if(err_code == NRF_SUCCESS)
  {
  
  NRF_LOG_INFO("Successfully detected a device at address: 0x%x", lis_address);
   return 1;
  

  }
else NRF_LOG_WARNING("Cannot detected a device at address");

//IIC ACK TEST DONE
}