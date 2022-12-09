//For iic include
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern void twi_init(void);
extern void enable_3v3(void);


//iic define

#define TWI_INSTANCE_ID     0
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
//iic define done
