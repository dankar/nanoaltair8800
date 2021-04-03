#include "main.h"
#include "sys/usb.h"
#include "altair8800/irq_controller.h"

extern TIM_HandleTypeDef htim1;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    irq_rtc_fire();
}

void hw_timer_start_tim1()
{
    HAL_TIM_Base_Start_IT(&htim1);
}
void hw_timer_stop_tim1()
{
    HAL_TIM_Base_Stop_IT(&htim1);
}

