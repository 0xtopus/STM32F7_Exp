#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "touch.h"
#include "timer.h"
/************************************************
 �����̻��ڣ�
 ALIENTEK ������STM32F7������ ʵ��31
 ������ʵ��-HAL�⺯����
 �޸Ķ���
************************************************/

u8 lcd_id[12];

// �����Ļ����ʾLCD��ID��PWM_GEN��ť
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);								  // ����
	POINT_COLOR = BLUE;								  // ��������Ϊ��ɫ
	LCD_ShowString(160, 400, 200, 32, 32, "PWM_GEN"); // ��ʾ��������
	LCD_ShowString(10, 10, 240, 16, 16, lcd_id); 		// ��ʾLCD ID
	POINT_COLOR = RED;								  // ���û�����ɫ

	//LED0_Toggle;
}
////////////////////////////////////////////////////////////////////////////////
// ���ݴ�����ר�в���
// ��ˮƽ��
// x0,y0:����
// len:�߳���
// color:��ɫ
void gui_draw_hline(u16 x0, u16 y0, u16 len, u16 color)
{
	if (len == 0)
		return;
	LCD_Fill(x0, y0, x0 + len - 1, y0, color);
}
// ��ʵ��Բ
// x0,y0:����
// r:�뾶
// color:��ɫ
void gui_fill_circle(u16 x0, u16 y0, u16 r, u16 color)
{
	u32 i;
	u32 imax = ((u32)r * 707) / 1000 + 1;
	u32 sqmax = (u32)r * (u32)r + (u32)r / 2;
	u32 x = r;
	gui_draw_hline(x0 - r, y0, 2 * r, color);
	for (i = 1; i <= imax; i++)
	{
		if ((i * i + x * x) > sqmax) // draw lines from outside
		{
			if (x > imax)
			{
				gui_draw_hline(x0 - i + 1, y0 + x, 2 * (i - 1), color);
				gui_draw_hline(x0 - i + 1, y0 - x, 2 * (i - 1), color);
			}
			x--;
		}
		// draw lines from inside (center)
		gui_draw_hline(x0 - x, y0 + i, 2 * x, color);
		gui_draw_hline(x0 - x, y0 - i, 2 * x, color);
	}
}
// ������֮��ľ���ֵ
// x1,x2����ȡ��ֵ��������
// ����ֵ��|x1-x2|
u16 my_abs(u16 x1, u16 x2)
{
	if (x1 > x2)
		return x1 - x2;
	else
		return x2 - x1;
}
// ��һ������
//(x1,y1),(x2,y2):��������ʼ����
// size�������Ĵ�ϸ�̶�
// color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2, u8 size, u16 color)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	if (x1 < size || x2 < size || y1 < size || y2 < size)
		return;
	delta_x = x2 - x1; // ������������
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // ���õ�������
	else if (delta_x == 0)
		incx = 0; // ��ֱ��
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // ˮƽ��
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // ѡȡ��������������
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // �������
	{
		gui_fill_circle(uRow, uCol, size, color); // ����
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
// 10�����ص����ɫ(���ݴ�������)
const u16 POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, GRED, BRED, GBLUE, LIGHTBLUE, BRRED, GRAY};
// ���败�������Ժ���
void rtp_test(void)
{
	u8 key;
	u8 i = 0;
	while (1)
	{
		key = KEY_Scan(0);
		tp_dev.scan(0);
		if (tp_dev.sta & TP_PRES_DOWN) // ������������
		{
			if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
			{
				if (tp_dev.x[0] > (lcddev.width - 24) && tp_dev.y[0] < 16)
					Load_Drow_Dialog(); // ���
				else
					TP_Draw_Big_Point(tp_dev.x[0], tp_dev.y[0], RED); // ��ͼ
			}
		}
		else
			delay_ms(10);	  // û�а������µ�ʱ��
		if (key == KEY0_PRES) // KEY0����,��ִ��У׼����
		{
			LCD_Clear(WHITE); // ����
			TP_Adjust();	  // ��ĻУ׼
			TP_Save_Adjdata();
			Load_Drow_Dialog();
		}
		i++;
		if (i % 20 == 0)
			LED0_Toggle;
	}
}
// ���ݴ��������Ժ���
void ctp_test(void)
{
	char PWM_Flag = 0;
	u8 t = 0;
	u8 i = 0;
	u16 lastpos[10][2]; // ���һ�ε�����
	u8 maxp = 5;
	if (lcddev.id == 0X1018)
		maxp = 10;
	while (1)
	{
		tp_dev.scan(0);		//! ��������scan������gt9147�u8 GT9147_Scan(u8 mode)��
		for (t = 0; t < maxp; t++)
		{
			if ((tp_dev.sta) & (1 << t))	// ����t�����Ƿ���
			{
				if (tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)	// �жϵ�������Ƿ����
				{
					if (lastpos[t][0] == 0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]); // ����
					lastpos[t][0] = tp_dev.x[t];
					lastpos[t][1] = tp_dev.y[t];
					if (tp_dev.x[t] < (300) && tp_dev.x[t] > (150) && tp_dev.y[t] > 400 && tp_dev.y[t] < 500)
					{
						Load_Drow_Dialog(); // ���
						if (PWM_Flag)
						{
							PWM_Flag = 0;
							HAL_TIM_PWM_Stop(&TIM3_Handler,TIM_CHANNEL_1);	//! �ر�PWMͨ��1
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
						}
						else
						{
							PWM_Flag =1;
							HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_1); //! ����PWMͨ��1
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
						}
						
					}
				}
			}
			else
				lastpos[t][0] = 0XFFFF;
		}
		delay_ms(5);
		i++;
		// if (i % 20 == 0)
		//	LED0_Toggle;
	}
}
int main(void)
{
	Cache_Enable();					 // ��L1-Cache
	HAL_Init();						 // ��ʼ��HAL��
	Stm32_Clock_Init(432, 25, 2, 9); // ����ʱ��,216Mhz
	delay_init(216);				 // ��ʱ��ʼ��
	uart_init(115200);				 // ���ڳ�ʼ��
	LED_Init();						 // ��ʼ��LED

	TIM3_PWM_Init(500 - 1, 108 - 1);	//! ��ʼ��TIM3��PWM
										// 108M/108=1M�ļ���Ƶ�ʣ��Զ���װ��Ϊ500����ôPWMƵ��Ϊ1M/500=2kHZ
	KEY_Init();						 // ��ʼ������
	SDRAM_Init();					 // ��ʼ��SDRAM
	LCD_Init();						 // LCD��ʼ��
	tp_dev.init();					 // ��������ʼ��
	POINT_COLOR = RED;

	sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id); // ��LCD ID��ӡ��lcd_id���顣

	LCD_ShowString(30, 50, 200, 16, 16, "Apollo STM32F4/F7");
	LCD_ShowString(30, 70, 200, 16, 16, "TOUCH TEST");
	LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
	LCD_ShowString(30, 110, 200, 16, 16, "2016/7/12");
	if (tp_dev.touchtype != 0XFF)
		LCD_ShowString(30, 130, 200, 16, 16, "Press KEY0 to Adjust"); // ����������ʾ
	delay_ms(1500);
	Load_Drow_Dialog();

	if (tp_dev.touchtype & 0X80)
		ctp_test(); // ����������
	else
		rtp_test(); // ����������
}