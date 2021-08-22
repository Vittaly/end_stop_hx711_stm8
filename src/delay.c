
//http://www.count-zero.ru/2016/stm8_spl_tim4/
#include "delay.h"
//#include "stm8s_gpio.h"
//#include "stm8s_clk.h"
#include "stm8s_tim4.h"


volatile uint16_t count;

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
    if (count)
        count--;

    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

void delay_us(uint16_t us)
{
        TIM4_Cmd(DISABLE);       // stop
        TIM4_TimeBaseInit(TIM4_PRESCALER_4, TIM4_US_PERIOD);
        TIM4_ClearFlag(TIM4_FLAG_UPDATE);
        TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

        count = us>>1;

        TIM4_Cmd(ENABLE);       // let's go

        while(count);
}


void delay_ms(uint16_t ms)
{
        TIM4_Cmd(DISABLE);       // stop
        TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_MS_PERIOD);
        TIM4_ClearFlag(TIM4_FLAG_UPDATE);
        TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

        count = ms;

        TIM4_Cmd(ENABLE);       // let's go

    while(count);
}
