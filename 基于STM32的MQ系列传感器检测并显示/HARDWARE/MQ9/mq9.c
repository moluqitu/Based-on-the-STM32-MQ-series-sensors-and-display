#include "mq9.h"
 
#define CAL_PPM  10  // 校准环境中PPM值
#define RL	     10  // RL阻值
#define R0	     97  // R0阻值
 
 
void MQ2_Init(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	/* 引脚和ADC的时钟使能　*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 
	/*　配置引脚为模拟功能模式　*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//模拟功能模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*　ADC的常规配置　*/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;		//独立模式
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;		//84ＭＨｚ/2　＝　42ＭＨｚ
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;	//禁止MDA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//ADC通道采用间隔
	ADC_CommonInit(&ADC_CommonInitStructure);
 
	/* ADC1 初始化 ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;		//分辨率
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;				//禁止扫描			
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//不需要外部触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		//数据右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;					//一次转换
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//ADC1的采样通道4放入规则通道１中
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_3Cycles);
 
	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}
 
uint32_t MQ2_ADC_Read(void)
{
	/* 启动软件触发检测 */ 
	ADC_SoftwareStartConv(ADC1);
	
	//等待转换结束
	while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	return ADC_GetConversionValue(ADC1);
}
 
//计算平均值
u16 ADC1_Average_Data(u8 ADC_Channel)
{
	u16 temp_val=0;
	u8 t;
	for(t=0;t<SMOG_READ_TIMES;t++)	//#define SMOG_READ_TIMES	10	定义烟雾传感器读取次数,读这么多次,然后取平均值
 
	{
		temp_val+=MQ2_ADC_Read();	//读取ADC值
		delay_ms(5);
	}
	temp_val/=SMOG_READ_TIMES;//得到平均值
    return (u16)temp_val;//返回算出的ADC平均值
}
 
//读取MQ7传感器的电压值
float Smog_Get_Vol(void)
{
	u16 adc_value = 0;//这是从MQ-7传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;//MQ-7传感器模块的电压输出，与一氧化碳的浓度成正比
	
	adc_value = ADC1_Average_Data(ADC_Channel_4);//#define SMOG_ADC_CHX	ADC_Channel_4	定义烟雾传感器所在的ADC通道编号
	delay_ms(5);
	
    voltage  = (3.3/4096.0)*(adc_value);
	
	return voltage;
}
///*********************
// 传感器校准函数，根据当前环境PPM值与测得的RS电压值，反推出R0值。
// 在空气中运行过后测出R0为26
//float MQ7_PPM_Calibration()
//{
//	float RS = 0;
//	float R0 = 0;
//	RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;//RL	10  // RL阻值
//	R0 = RS / pow(CAL_PPM / 98.322, 1 / -1.458f);//CAL_PPM  10  // 校准环境中PPM值
//	return R0;
//}
//**********************/
 
// 计算Smog_ppm
float Smog_GetPPM()
{
	float RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;
	float ppm = 98.322f * pow(RS/R0, -1.458f);
	return  ppm;
}

