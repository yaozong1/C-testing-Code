#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "IIC_LIS.h"




void twi_init(void)
{
  ret_code_t err_code;

  const nrf_drv_twi_config_t twi_config = {
    .scl   = 27,
    .sda   = 26,
    .frequency = NRF_DRV_TWI_FREQ_100K,
    .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
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
nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,14));

//ENABLE CE_CHARGER
nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,8));
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,8),0);

//3v3 ENABLE
nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1,6));
nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(1,6),1);

}