#ifndef YZ_TIMER_H
#define YZ_TIMER_H


#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"




#define WDT_WAVE NRF_GPIO_PIN_MAP(0,9)
#define LED_Pin2 NRF_GPIO_PIN_MAP(1,7)

extern  void timers_init(void);
extern  void lfclk_config(void);
extern  void start_timer(void);

#ifdef __cplusplus
extern "C" {
#endif

#endif 