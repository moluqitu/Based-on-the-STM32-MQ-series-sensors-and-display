#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "mq9.h"

//ALIENTEK 探索者STM32F407开发板 实验13
//LCD显示实验-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK
u8 Smog_yu = 30;//CO的阈值
int main(void)
{ 
	float Smog_ppm = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	MQ2_Init();
	LED_Init();					  //初始化LED
 	LCD_Init();           //初始化LCD FSMC接口				 	
  	while(1) 
	{		 
		
//		int a;
//		a = MQ7_PPM_Calibration();
//		LCD_ShowNum(30+64,170,a,2,16);
		
    Smog_ppm = Smog_GetPPM();
//		printf("烟雾浓度：%.3f  烟雾阈值：%d\n",Smog_ppm, Smog_yu);		
		LCD_ShowString(30,150,200,12,16,"Smog_ppm:   .   ");
		LCD_ShowString(30,170,200,12,16,"Smog_yu:  ");
		LCD_ShowNum(30+72,150,Smog_ppm,3,16);		//显示烟雾浓度	  
    LCD_ShowNum(30+104,150,(int)(Smog_ppm*1000)%1000,3,16);		//显示烟雾浓度		
		LCD_ShowNum(30+64,170,Smog_yu,2,16);		//显示烟雾阈值	 
			
		LED0=!LED0;	 
		delay_ms(100);	
	} 
}

