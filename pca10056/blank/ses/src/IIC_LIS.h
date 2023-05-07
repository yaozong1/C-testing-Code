//For iic include

#ifndef IIC_LIS_H
#define IIC_LIS_H

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
extern bool Lis_test(void);


//iic define

#define TWI_INSTANCE_ID     0
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
//iic define done

#endif