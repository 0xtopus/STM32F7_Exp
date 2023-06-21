#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
/************************************************
 ALIENTEK ������STM32F7������ ʵ��8
 PWM���ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 �������������ӿƼ����޹�˾
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
  u8 dir = 1;
  u16 led0pwmval = 0;
  Cache_Enable();                  // ��L1-Cache
  HAL_Init();                      // ��ʼ��HAL��
  Stm32_Clock_Init(432, 25, 2, 9); // ����ʱ��,216Mhz
  delay_init(216);                 // ��ʱ��ʼ��
  uart_init(115200);               // ���ڳ�ʼ��
  LED_Init();                      // ��ʼ��LED
  TIM3_PWM_Init(500 - 1, 108 - 1); // 108M/108=1M�ļ���Ƶ�ʣ��Զ���װ��Ϊ500����ôPWMƵ��Ϊ1M/500=2kHZ
  while (1)
  {
    delay_ms(10);
    if (dir)
      led0pwmval++; // dir==1 led0pwmval����
    else
      led0pwmval--; // dir==0 led0pwmval�ݼ�
    if (led0pwmval > 300)
      dir = 0; // led0pwmval����300�󣬷���Ϊ�ݼ�
    if (led0pwmval == 0)
      dir = 1;                       // led0pwmval�ݼ���0�󣬷����Ϊ����
    TIM_SetTIM3Compare4(led0pwmval); // �޸ıȽ�ֵ���޸�ռ�ձ�
  }
}