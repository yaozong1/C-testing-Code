#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "yz_timer.h"

#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define LED_INTERVAL1 APP_TIMER_TICKS(500)
//#define LED_INTERVAL2 APP_TIMER_TICKS(500)

#define TIMER1_PRIORITY 3
//#define TIMER2_PRIORITY 2

APP_TIMER_DEF(m_app_timer_id_1);
//APP_TIMER_DEF(m_app_timer_id_2);


void lfclk_config(void)
{
  ret_code_t err_code = nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);

  nrf_drv_clock_lfclk_request(NULL);

}


static void app_timer_handler(void * p_context)
{

  nrf_gpio_pin_toggle(LED_Pin1);
  NRF_LOG_FLUSH();

}

/*
static void app_timer_handler2(void * p_context)
{

  nrf_gpio_pin_toggle(LED_Pin2);

}
*/


void timers_init(void)
{
  ret_code_t err_code;

  err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);

  err_code = app_timer_create(&m_app_timer_id_1, APP_TIMER_MODE_REPEATED, app_timer_handler);
  APP_ERROR_CHECK(err_code);
 // NVIC_SetPriority(TIMER1_IRQn, TIMER1_PRIORITY);//from chatGpt
  //err_code = app_timer_priority_set(&m_app_timer_id_1, TIMER1_PRIORITY);
  //APP_ERROR_CHECK(err_code);
/*

  err_code = app_timer_create(&m_app_timer_id_2, APP_TIMER_MODE_REPEATED, app_timer_handler2);
  APP_ERROR_CHECK(err_code);
  NVIC_SetPriority(TIMER2_IRQn, TIMER2_PRIORITY);


*/


}


void start_timer(void)
{


    //LED ENABLE
nrf_gpio_cfg_output(LED_Pin1);
nrf_gpio_cfg_output(LED_Pin2);


 //   lfclk_config();

    timers_init();

    uint32_t err_code = app_timer_start(m_app_timer_id_1, LED_INTERVAL1, NULL);
 //   uint32_t err_code_2 = app_timer_start(m_app_timer_id_2, LED_INTERVAL2, NULL);

}




/*

int main(void)
{


    //LED ENABLE
nrf_gpio_cfg_output(LED_Pin1);
nrf_gpio_cfg_output(LED_Pin2);


    lfclk_config();

    timers_init();

    uint32_t err_code = app_timer_start(m_app_timer_id_1, LED_INTERVAL1, NULL);
 //   uint32_t err_code_2 = app_timer_start(m_app_timer_id_2, LED_INTERVAL2, NULL);

    while (true)
    {
       
    }
}

*/

