#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
/************************************************
 ALIENTEK 阿波罗STM32F7开发板 实验8
 PWM输出实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
  u8 dir = 1;
  u16 led0pwmval = 0;
  Cache_Enable();                  // 打开L1-Cache
  HAL_Init();                      // 初始化HAL库
  Stm32_Clock_Init(432, 25, 2, 9); // 设置时钟,216Mhz
  delay_init(216);                 // 延时初始化
  uart_init(115200);               // 串口初始化
  LED_Init();                      // 初始化LED
  TIM3_PWM_Init(500 - 1, 108 - 1); // 108M/108=1M的计数频率，自动重装载为500，那么PWM频率为1M/500=2kHZ
  while (1)
  {
    delay_ms(10);
    if (dir)
      led0pwmval++; // dir==1 led0pwmval递增
    else
      led0pwmval--; // dir==0 led0pwmval递减
    if (led0pwmval > 300)
      dir = 0; // led0pwmval到达300后，方向为递减
    if (led0pwmval == 0)
      dir = 1;                       // led0pwmval递减到0后，方向改为递增
    TIM_SetTIM3Compare4(led0pwmval); // 修改比较值，修改占空比
  }
}
