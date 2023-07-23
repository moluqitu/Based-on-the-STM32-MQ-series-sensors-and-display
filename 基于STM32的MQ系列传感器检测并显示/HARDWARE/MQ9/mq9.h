#ifndef _MQ2_H
#define _MQ2_H
 
//C文件中需要的其他的头文件
#include <stm32f4xx.h>
#include "sys.h"
#include "delay.h"
#include "math.h"
 
#define SMOG_READ_TIMES	10				//定义烟雾传感器读取次数,读这么多次,然后取平均值
 
//C文件中定义的函数的声明
void MQ2_Init(void);
float Smog_Get_Vol(void);	//读取MQ7传感器的电压值
float MQ7_PPM_Calibration(void);
float Smog_GetPPM(void);
 
#endif

