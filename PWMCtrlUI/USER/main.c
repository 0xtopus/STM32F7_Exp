#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "sdram.h"
#include "tftlcd.h"
#include "ltdc.h"
#include "mpu.h"
#include "timer.h"
#include "usmart.h"
#include "malloc.h"
#include "touch.h"
#include "GUI.h"
#include "WM.h"
#include "GUIDEMO.h"
/************************************************
 ALIENTEK STM32开发板STemWin实验
 STemWin无操作系统移植实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
int main(void)
{  
    Write_Through();                //Cahce强制透写
    MPU_Memory_Protection();        //保护相关存储区域
    Cache_Enable();                 //打开L1-Cache
      
	Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
	HAL_Init();				        //初始化HAL库
    delay_init(216);                //延时初始化
	uart_init(115200);		        //串口初始化
    KEY_Init();                     //按键初始化
    LED_Init();                     //初始化LED
    SDRAM_Init();                   //SDRAM初始化
	TFTLCD_Init();                  //初始化LCD
    TP_Init();				        //触摸屏初始化
    TIM3_Init(999,107);             //1KHZ 定时器3设置为1ms
    TIM4_Init(999,1079);            //触摸屏扫描速度,100HZ.
    //TIM2_PWM_Init(500-1,108-1);
    
    my_mem_init(SRAMIN);            //初始化内部内存池
    my_mem_init(SRAMEX);            //初始化外部SDRAM内存池
    my_mem_init(SRAMDTCM);          //初始化内部DTCM内存池
	__HAL_RCC_CRC_CLK_ENABLE();		//使能CRC时钟
	WM_SetCreateFlags(WM_CF_MEMDEV);//开启STemWin存储设备
	GUI_Init();
    WM_MULTIBUF_Enable(1);			//开启STemWin多缓冲，RGB屏可能会用到
	MainTask();					//! STemWin 演示demo
    while(1)
    {

	}
}
