#include "pwmout.h"
#include "timer.h"



//TODO : PWM_Gen()
void PWM_Gen(pwmout_struct pwmout_config) {
    TIM2_PWM_Init(500-1,108-1);

}


//TODO£ºPWM_Start()
void PWM_Start(void) {
    

}


void PWM_Stop(void) {
    HAL_TIM_PWM_Stop(&TIM2_Handler, TIM_CHANNEL_1);
}


/**********************************EOF*******************************/
