#ifndef __PWMOUT_H
#define __PWMOUT_H
#include "sys.h"
/* 
 * 对控制pwm输出状态进行控制的头文件
 */

typedef struct
{
    uint8_t rotate_direction;// 转动的方向，0：正转，1：反转
    uint8_t left_motor;     // 左马达是否转动，0：不转，1：转；
    uint8_t right_motor;    // 右马达是否转动，0：不转，1：转；
    uint16_t speed;         // 电机转速的调节
    uint8_t pwm_enable;     // 使能pwm输出，0：不使能，1：使能
}pwmout_struct;


void PWM_Gen(pwmout_struct pwmout_config);
void PWM_Start(void);
void PWM_Stop(void);


#endif
/**********************************EOF*******************************/
