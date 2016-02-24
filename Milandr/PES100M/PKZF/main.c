/*
*Программа ПКЗФ миландр ПЭС100М
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_eeprom.h"
#include <stdbool.h>
#include <string.h>
//#include <stdlib.h>


//#define  TEST_P	 	MDR_PORTA->RXTX ^= PORT_Pin_4

#define K1_ON		PORT_ResetBits(MDR_PORTA, (PORT_Pin_0))
#define K1_OFF		PORT_SetBits(MDR_PORTA, (PORT_Pin_0))
#define K2_ON		PORT_ResetBits(MDR_PORTA, (PORT_Pin_1))
#define K2_OFF		PORT_SetBits(MDR_PORTA, (PORT_Pin_1))

#define OVER_K1_ON		PORT_ResetBits(MDR_PORTA, (PORT_Pin_2))
#define OVER_K1_OFF		PORT_SetBits(MDR_PORTA, (PORT_Pin_2))
#define OVER_K2_ON		PORT_ResetBits(MDR_PORTA, (PORT_Pin_3))
#define OVER_K2_OFF		PORT_SetBits(MDR_PORTA, (PORT_Pin_3))

#define ALARM_ON		PORT_ResetBits(MDR_PORTA, (PORT_Pin_5))
#define ALARM_OFF		PORT_SetBits(MDR_PORTA, (PORT_Pin_5))

#define  TEST_MODE	 	!PORT_ReadInputDataBit(MDR_PORTA, (PORT_Pin_4))

#define NUMBER_PDG	(uint8_t)0xA0
#define NUMBER_PZF	(uint8_t)0xA1

#define EEPROM_ADD	(uint32_t)0x08000000	//page0

#define MAX_MESSAGE	6

typedef struct
{
	uint8_t		Error;				//1
	uint8_t		TBapPz;				//2	
	uint8_t		KN_ZF;				//3
	uint16_t	IAF1;				//4
	uint16_t	IBF1;				//5
	uint16_t	ICF1;				//6
	uint16_t	IAF2;				//7
	uint16_t	IBF2;				//8
	uint16_t	ICF2;				//9
	uint8_t		Calb;				//10
	bool		F1_Off;				//11
	bool		F2_Off;				//12
}RegS;


volatile RegS Status = {0,0,0,0,0,0,0,0,0,0,0,0};

volatile uint16_t	delay_count = 0;

const char data_str[]=__DATE__;//mmm dd yyyy

#define ARR_CAL 9
volatile uint8_t 	calib_data[ARR_CAL];
const uint8_t		calib_data_max[ARR_CAL] = {255,	255,	255,255,255,255,	5,	5,	30},
					calib_data_min[ARR_CAL] = {10,	10,		10,	 10,10,	10,		1,	1,	1},
					calib_data_nom[ARR_CAL] = {230,	160,	70,100,100,100,		1,	1,	8};


volatile bool		ADC_compl = false, 
					RX_Ok = false,
					rst_alarm = false,
					F1_ON = false, 
					F2_ON = false,
					calibr_on = false,
					TM= false;
uint32_t 			sum_Nd = 0, sum_Niz = 0, RegimTime = 0,
					vref_50 = 2049;
//#define DEBUG_ON					
	
#define RX_FRAME_SIZE	4
volatile uint8_t ArrayRX_PI23[RX_FRAME_SIZE];
	
#define ADC_COUNT_CH 9
volatile uint32_t	ADC_Arr[ADC_COUNT_CH],
					time[16],
					ErrTime[16],
					RegimError;
const uint16_t 		time0[16] =		{5,	4,	10,	4,	10,	10,	30,	600,	6,	0,	0,	0,	0,	0,	0,	1};
const uint16_t 		ErrTime0[16] =	{5,	5,	6,	2,	10,	20,	10,	1,		5,	15,	1,	1,	1,	1,	1,	1};




void GPIO_init(void);
void TIM1_init(void);
void TIM2_init(void);
void TIM3_init(void);
void ADC_init(void);
void UART_init(void);
void PLL_init(void);
void WDT_init(void);
void EEPROM_init(void);
void sys_init(void);
void save_calib_data(void);
void restore_calib_data(void);
void calk_Ifid(void);
void calk_Tbap(void);
void send_PI23(void);
void Kalibrovka(void);
void Read_KN(void);
void UART2_send_byte(uint8_t byte);
bool Read_Data(uint8_t add_dev);
void SetError(void);
void ChangeCtError(void);
void ChangeCtTime(void);
void ChangeCallB(uint8_t ch_byte);

//==================================================================================
void delay_ms(uint16_t delay)
{
	delay_count = delay;
	while(delay_count);
}
//==================================================================================
void UART2_IRQHandler(void)
{
	uint8_t tmp=0;
	static bool start=false;
	static uint8_t cnt_rx_byte=0;
	
	tmp = UART_ReceiveData(MDR_UART2);
	
	if((tmp == 0xAA)&&(!start))
		{
			start = true;
		}
		else
		{
			if(start)
			{
				if(!RX_Ok)
					ArrayRX_PI23[cnt_rx_byte] = tmp; 
				
				++cnt_rx_byte;
				if(cnt_rx_byte == RX_FRAME_SIZE)
				{	
					RX_Ok = true;
					start = false;
					cnt_rx_byte = 0;
				}
			}
		}
}
//==================================================================================

//#define CT_AD_SUM	32
//#define	AD_SAMPL	300

//void ADC_IRQHandler(void)//0.144 ms //36 тактов на один канал
//{
//	uint8_t cnt_ch = 0;
//	static uint32_t countAD = AD_SAMPL+2,
//					CtAdSum = CT_AD_SUM;
//	static uint32_t AdTempResult = 0,
//					AdTempMax = 0,
//					AdTempMin = 0;
//	uint32_t tmp = 0;

//	tmp = ADC1_GetResult();


//		cnt_ch = (tmp>>16)&0x1F;

//		tmp &= 0xFFF; //12 bit
//		

//		if(cnt_ch == ADC_CH_TEMP_SENSOR)
//		{
//			if(countAD)
//			{
//				--countAD;
//				AdTempResult += tmp;
//			}
//			else
//			{
//				countAD = AD_SAMPL;
//				AdTempResult /= AD_SAMPL; 
//				ADC_Arr[(uint8_t)(ADC_COUNT_CH-1)] = AdTempResult;
//				ADC_compl = true;
//				AdTempResult = 0;
//				
//				ADC1_SetChannel(ADC_CH_ADC0);
//			}
//		}
//		else
//		{
//			if(countAD)
//			{
//				--countAD;
//				if(countAD < AD_SAMPL-2)
//				{		
//					if(tmp > AdTempMax)
//						AdTempMax = tmp;
//						
//					if(tmp < AdTempMin)
//						AdTempMin = tmp;
//				}
//			}
//			else 
//			{
//				if(CtAdSum)
//				{
//					--CtAdSum;
//					tmp = AdTempMax - AdTempMin;
//					if(tmp >= 70)
//						tmp -= 70;
//					else
//						tmp = 0;
//						
//					AdTempResult += tmp;
//				}
//				else
//				{
//					ADC_Arr[cnt_ch] = AdTempResult >> 5;
//					CtAdSum = CT_AD_SUM;
//					AdTempResult = 0;

//					if(cnt_ch == ADC_CH_ADC6)
//						cnt_ch = ADC_CH_TEMP_SENSOR;
//					else
//						++cnt_ch;
//					
//					ADC1_SetChannel(cnt_ch);
//				}
//				
//				countAD = AD_SAMPL;
//				AdTempMax = 0;
//				AdTempMin = 0xffff;
//			}
//		}
//}
#define CT_AD_SUM	64
//#define	AD_SAMPL	124
#define	AD_SAMPL	70
void ADC_IRQHandler(void)//0.029ms 29 ?????? ?? ???? ?????
{
	uint32_t cnt_ch = 0;
	static uint8_t 	countAD = AD_SAMPL;
	static uint32_t tmp1 =0;
	uint32_t tmp = 0;//,tmp12 = 0;

	tmp = ADC1_GetResult();
	cnt_ch = (tmp>>16);

	tmp &= 0xFFF;
	
	switch(cnt_ch)
	{
		case ADC_CH_ADC1:
				ADC_Arr[cnt_ch] = tmp;	
//				--countAD;
		
//				if(!countAD)
//				{
//				countAD = AD_SAMPL;
				++cnt_ch;
				ADC1_SetChannel(cnt_ch);		
//				}
				break;	
		
		case ADC_CH_ADC0:	
		case ADC_CH_ADC2:
		case ADC_CH_ADC3:
		case ADC_CH_ADC4:
		case ADC_CH_ADC5:
		case ADC_CH_ADC6:
			
#ifndef DEBUG_ON			
				--countAD;
		
				if(countAD)
				{
					if(countAD < AD_SAMPL-2)
					{
						if(tmp > vref_50)
							tmp1 += (tmp-vref_50);
						else
						if(tmp < vref_50)
							tmp1 += (vref_50-tmp);
					}
				}
				else 
				{
					//TEST_P;
					tmp1 /= AD_SAMPL;
					
					if(tmp1<5)
						tmp1=0;
					
					ADC_Arr[cnt_ch] = tmp1;	
#else					
				{
					ADC_Arr[cnt_ch] = tmp;	
#endif
					tmp1=0;
					
					
					
					if(cnt_ch < ADC_CH_ADC6)
					{
						++cnt_ch;
						ADC1_SetChannel(cnt_ch);						
					}
					else
					{
						cnt_ch = 0;
						ADC_compl = true;
						ADC1_SetChannel(ADC_CH_ADC0);
						
					}
					countAD = AD_SAMPL;
				}
				break;
		default:
			ADC1_SetChannel(ADC_CH_ADC0);
			break;
	}
}
//==================================================================================
void SysTick_Handler(void)//1 ms
{
	if(delay_count)
	{
		delay_count--;
	}
	
//	TEST_P;
}


//==================================================================================
void Timer1_IRQHandler(void)//10ms
{
	static uint8_t cnt_read=0,cnt_sec=0;
	uint8_t cnt=0;
	
	MDR_TIMER1->CNT = 0x0000;
	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
	 
	if(cnt_read > 8)
	{
		cnt_read = 0;
		sum_Nd = MDR_TIMER3 -> CNT;
		MDR_TIMER3 -> CNT = 0;
		sum_Niz = MDR_TIMER2 -> CNT;
		MDR_TIMER2 -> CNT = 0;
	}
	else
		++cnt_read;
	
	if(cnt_sec > 99)
	{
		ChangeCtTime();
		ChangeCtError();
		SetError();
		cnt_sec = 0;
	}
	else
		++cnt_sec;
	
	Read_KN();
	
	if(RX_Ok && (!TM))
	{
		if(Read_Data(NUMBER_PZF))
			send_PI23();

		RX_Ok = false;
	}
	
	if(rst_alarm)
	{
		RegimError = 0;
		Status.Error = 0;
		ALARM_OFF;
		OVER_K2_OFF;
		OVER_K1_OFF;
		
		for(cnt=0;cnt<16;++cnt)
		{
			ErrTime[cnt]=ErrTime0[cnt];
		}
	}
}


//==================================================================================
void Timer2_IRQHandler(void)//3.5ms
{
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
}

//==================================================================================
//==================================================================================
//==================================================================================
int main(void)
{
	uint32_t cnt = 0xFFFF;
	uint8_t pr_b = 0;
	
	cnt = 0xFFFF;
	while(--cnt)
		__nop();
	
	
	sys_init();

	IWDG_ReloadCounter();//WDT reset //409.5ms
	delay_ms(1);
	
 	restore_calib_data();
	
	if( TEST_MODE )
		TM=true;
	else
		TM=false;
	
	if( TM )
	{
		while(1)
		{
			if( ADC_compl )//100 ms //36 тактов на один канал
			{
				ADC_compl = false;
			}
			
			if( RX_Ok )
			{
				for(cnt=0; cnt<ADC_COUNT_CH; cnt++)
					UART2_send_byte((uint8_t)(ADC_Arr[cnt]>>4));
				UART2_send_byte((uint8_t)Status.KN_ZF);
				UART2_send_byte((uint8_t)(0));
				UART2_send_byte((uint8_t)0);
				UART2_send_byte((uint8_t)0);
				UART2_send_byte((uint8_t)0);
				UART2_send_byte((uint8_t)0);
				
				K1_OFF;
				K2_OFF;
				OVER_K1_OFF;
				OVER_K2_OFF;
				ALARM_OFF;
				for(cnt=0; cnt<8; cnt++)
				{
					if((ArrayRX_PI23[0]>>cnt)&1)
						switch(cnt)
						{
							case 0:
								K1_ON;
								break;
							case 1:
								K2_ON;
								break;
							case 2:
								OVER_K1_ON;
								break;
							case 3:
								OVER_K2_ON;
								break;
							case 4:
								ALARM_ON;
								break;
							case 5:
								break;
							case 6:
								break;
							case 7:
								break;
						}
				}
				RX_Ok = false;
			}
		}
	}
	
	while(1)
	{
		if(ADC_compl)//100 ms //36 тактов на один канал
		{
			//calk_Tbap();
			calk_Ifid();
			
			ADC_compl = false;
		}
		
		if(Status.KN_ZF&(1<<0)) // вкл К2
		{
			K2_ON;
			F2_ON = true;
		}
		if(Status.KN_ZF&(1<<2)) // вкл К1
		{
			K1_ON;
			F1_ON = true;
		}
		if((Status.KN_ZF&(1<<1)) || Status.F2_Off) // откл К2
		{
			K2_OFF;
			F2_ON = false;
		}
		if((Status.KN_ZF&(1<<3)) || Status.F1_Off) // откл К1
		{
			K1_OFF;
			F1_ON = false;
		}
		
		if(F1_ON && (!((Status.KN_ZF>>5)&1)))
		{
			RegimError |= 8;
		}
 		else
		if(F2_ON && (!((Status.KN_ZF>>4)&1)))
		{
			RegimError |= 8;
		}
 		else
 			RegimError &= ~8;
		
		if((Status.IAF1>660) || (Status.IBF1>660) || (Status.ICF1>660)) //>200%
		{
			Status.Error |= 4;
			K1_OFF;
			K2_OFF;
			OVER_K1_ON;
			OVER_K2_ON;
		}
		
		if((Status.IAF1>399) || (Status.IBF1>399) || (Status.ICF1>399)) //>120%
		{
			RegimError |= 1;
		}
		else
			RegimError &= ~1;
		
		if((Status.IAF2>200) || (Status.IBF2>200) || (Status.ICF2>200)) //>50%
		{
			RegimError |= 2;
		}
		else
			RegimError &= ~2;
		
		
		
		if( (Status.Calb & 0x80) && ((Status.Calb&0x60) != pr_b))
		{
			pr_b = Status.Calb & 0x60;
			
			switch( (Status.Calb & 0x0f))
			{
				case 8:
						ChangeCallB(0);
					break;
				case 9:
						ChangeCallB(1);
					break;
				case 10:
						ChangeCallB(2);
					break;
				case 11:
						ChangeCallB(3);
					break;
				case 12:
						ChangeCallB(4);
					break;
				case 13:
						ChangeCallB(5);
					break;
				default:
					break;
			}
		}
			
		IWDG_ReloadCounter();//WDT reset //409.5ms
	}
}
//==================================================================================
//==================================================================================
//==================================================================================
__inline void sys_init(void)
{
	PLL_init();
	
	EEPROM_init();
	
	GPIO_init();

	TIM1_init();

	TIM2_init();

	//TIM3_init();
	UART_init();
	
	//SPI_init();
	//WDT_init();
	
	
	SysTick_Config(HSE_Value/1000);//1 ms
	
	ADC_init();
}
//==================================================================================
void ADC_init(void)
{
	ADC_InitTypeDef sADC;
	ADCx_InitTypeDef sADCx;
	
	ADC_StructInit(&sADC);
	ADCx_StructInit(&sADCx);
	
// 	RST_CLK_ADCclkEnable(ENABLE);
// 	RST_CLK_ADCclkSelection(RST_CLK_ADCclkCPU_C1);
// 	RST_CLK_ADCclkPrescaler(RST_CLK_ADCclkDIV2);
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC, ENABLE);
	
	/* ADC Configuration */
	/* Reset all ADC settings */
	ADC_DeInit();

	
	sADC.ADC_StartDelay = 0;
	sADC.ADC_TempSensor = ADC_TEMP_SENSOR_Enable;
	sADC.ADC_TempSensorAmplifier = ADC_TEMP_SENSOR_AMPLIFIER_Enable;
	sADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
	sADC.ADC_IntVRefConversion = DISABLE;
	sADC.ADC_IntVRefTrimming = 0;
	
	ADC_Init(&sADC);

	
	sADCx.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU;
	//sADCx.ADC_SamplingMode	= ADC_SAMPLING_MODE_SINGLE_CONV;
	sADCx.ADC_SamplingMode = ADC_SAMPLING_MODE_CICLIC_CONV;
	sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
	sADCx.ADC_LevelControl = DISABLE;
	sADCx.ADC_ChannelNumber	= ADC_CH_ADC0;
	//sADCx.ADC_ChannelNumber	= ADC_CH_ADC0;
// 	sADCx.ADC_Channels		= (ADC_CH_ADC0_MSK|ADC_CH_ADC1_MSK|ADC_CH_ADC2_MSK|	\
// 								ADC_CH_ADC3_MSK|ADC_CH_ADC4_MSK|ADC_CH_ADC5_MSK|	\
// 								ADC_CH_ADC6_MSK|ADC_CH_ADC7_MSK|ADC_CH_TEMP_SENSOR_MSK);
	sADCx.ADC_VRefSource	= ADC_VREF_SOURCE_INTERNAL;
	sADCx.ADC_IntVRefSource	= ADC_INT_VREF_SOURCE_INEXACT;
	//sADCx.ADC_Prescaler		= ADC_CLK_div_2048;//4.6 ms
	//sADCx.ADC_Prescaler		= ADC_CLK_div_256;
	//sADCx.ADC_Prescaler = ADC_CLK_div_64;//0.144 ms
	sADCx.ADC_Prescaler = ADC_CLK_div_16;
	//sADCx.ADC_Prescaler = ADC_CLK_div_None;
	sADCx.ADC_DelayGo = 7;

	ADC1_Init(&sADCx);
	
ADC1_ClearOverwriteFlag();
	/* Enable ADC1 EOCIF and AWOIFEN interupts */
	ADC1_ITConfig((ADC1_IT_END_OF_CONVERSION), ENABLE);
	
NVIC_EnableIRQ(ADC_IRQn);
	/* ADC1 enable */
	ADC1_Cmd(ENABLE);

//	ADC1_Start();
}
//==================================================================================
#define ALL_PORTS_CLK ( RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF )
void GPIO_init(void)
{
	PORT_InitTypeDef PORT_InitStructure;
	
  /* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
	PORT_StructInit(&PORT_InitStructure);//reset struct	
	
 /* Configure PORTA pins */

	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_4 );
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	
	PORT_StructInit(&PORT_InitStructure);//reset struct
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|PORT_Pin_3|PORT_Pin_5|PORT_Pin_6);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTA, (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|PORT_Pin_3|PORT_Pin_5|PORT_Pin_6),Bit_SET);//close optr
	
/* Configure PORTB pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7| PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10);//КИ
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	
	PORT_StructInit(&PORT_InitStructure);//reset struct
/* Configure PORTD pin */

	PORT_InitStructure.PORT_Pin  = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|	\
									PORT_Pin_3|PORT_Pin_4|PORT_Pin_5|	\
									PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_InitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	//PORT_StructInit(&PORT_InitStructure);//reset struct
	
	
  /* Configure PORTF pin */
  	PORT_InitStructure.PORT_Pin = ( PORT_Pin_0|PORT_Pin_1 );//UART2 RX TX
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_PD 	= PORT_PD_OPEN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
}
//==================================================================================
void TIM1_init(void)
{
	TIMER_CntInitTypeDef sTIM_CntInit;
	
	TIMER_CntStructInit(&sTIM_CntInit);
	
	// TIMER1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1,ENABLE);
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER1);

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				= 0;
	sTIM_CntInit.TIMER_Prescaler                = 7999;
	sTIM_CntInit.TIMER_Period                   = 20;//10ms
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

	NVIC_EnableIRQ(Timer1_IRQn);
	TIMER_ITConfig(MDR_TIMER1,TIMER_STATUS_CNT_ARR, ENABLE);
	
  
	/* TMR1 enable */
	TIMER_Cmd (MDR_TIMER1,ENABLE);
}
//==================================================================================
void TIM2_init(void)
{
	TIMER_CntInitTypeDef sTIM_CntInit;
	
	TIMER_CntStructInit(&sTIM_CntInit);

	
	// TIMER2
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2,ENABLE);
	
/* Reset all TIMER2 settings */
	TIMER_DeInit(MDR_TIMER2);

	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
	sTIM_CntInit.TIMER_Prescaler                = 0;
	sTIM_CntInit.TIMER_Period                   = 0xFFFF;
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER2,&sTIM_CntInit);

 	NVIC_EnableIRQ(Timer2_IRQn);
 	TIMER_ITConfig(MDR_TIMER2,TIMER_STATUS_CNT_ARR, ENABLE);
	
  
	/* TMR2 enable */
	TIMER_Cmd (MDR_TIMER2,ENABLE);
}
//==================================================================================
void TIM3_init(void)
{
	TIMER_CntInitTypeDef sTIM_CntInit;

	TIMER_CntStructInit(&sTIM_CntInit);

	// TIMER3
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3,ENABLE);
	/* Reset all TIMER3 settings */
	TIMER_DeInit(MDR_TIMER3);

	TIMER_BRGInit(MDR_TIMER3,TIMER_HCLKdiv1);

  /* TIM3 configuration ------------------------------------------------*/
  /* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				= 0;
	sTIM_CntInit.TIMER_Prescaler                = 0;
	sTIM_CntInit.TIMER_Period                   = 0xFFFF;
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_EvtFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER3,&sTIM_CntInit);

	// 	NVIC_EnableIRQ(Timer3_IRQn);
	// 	TIMER_ITConfig(MDR_TIMER3,TIMER_STATUS_CNT_ARR, ENABLE);
	/* Enable TIMER3 */
	TIMER_Cmd(MDR_TIMER3,ENABLE);
}
//==================================================================================
void EEPROM_init(void)
{	
	/* Enables the clock on EEPROM */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
}

//==================================================================================
void EEPROM_save_hour(uint16_t Wdata)
{
	__disable_irq();
	EEPROM_ErasePage(EEPROM_ADD,EEPROM_Info_Bank_Select);
 	EEPROM_ProgramHalfWord(EEPROM_ADD,EEPROM_Info_Bank_Select,Wdata);
	__enable_irq();
}
//==================================================================================
void PLL_init(void)
{	
	/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while( RST_CLK_HSEstatus() == ERROR );
	
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	MDR_RST_CLK->CPU_CLOCK |= 2;//CPU_C1 set HSE
	RST_CLK_HSIcmd(DISABLE);
	/* Configure CPU_PLL clock */
	//RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv1,0);
	//RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
}
//==================================================================================
void UART_init(void)
{
	
	UART_InitTypeDef UART_InitStructure;

	UART_StructInit(&UART_InitStructure);

	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2,ENABLE);
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART2, UART_HCLKdiv1);
	UART_DeInit(MDR_UART2);

	/* Disable interrupt on UART1 */
	NVIC_DisableIRQ(UART2_IRQn);

	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 9600;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits2;
	UART_InitStructure.UART_Parity				= UART_Parity_No;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init(MDR_UART2,&UART_InitStructure);

	NVIC_EnableIRQ(UART2_IRQn);
	UART_ITConfig(MDR_UART2,UART_IT_RX, ENABLE);
	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART2,ENABLE);
}
//==================================================================================
void save_calib_data(void)
{
	uint32_t Wdata = 0;
	
	__disable_irq();
	EEPROM_ErasePage(EEPROM_ADD,EEPROM_Info_Bank_Select);//page0 4K all eeprom

	Wdata = (((calib_data[3]<<24)&0xff000000))|(((calib_data[2]<<16)&0x00ff0000))|(((calib_data[1]<<8)&0x0000ff00))|(calib_data[0]&0xff);
	EEPROM_ProgramWord(EEPROM_ADD,EEPROM_Info_Bank_Select,Wdata);
	Wdata = (((calib_data[7]<<24)&0xff000000))|(((calib_data[6]<<16)&0x00ff0000))|(((calib_data[5]<<8)&0x0000ff00))|(calib_data[4]&0xff);
	EEPROM_ProgramWord(EEPROM_ADD+4,EEPROM_Info_Bank_Select,Wdata);
	Wdata = (calib_data[8]&0xff);
	EEPROM_ProgramWord(EEPROM_ADD+8,EEPROM_Info_Bank_Select,Wdata);
	__enable_irq();
}
//==================================================================================
void restore_calib_data(void)
{
	uint8_t i = 0;
	bool save_dat = false;
	__disable_irq();
	for(i=0; i<ARR_CAL; i++)
		{
 			calib_data[i] = EEPROM_ReadByte((EEPROM_ADD+i),EEPROM_Info_Bank_Select);
 			
 			if((calib_data[i] < calib_data_min[i]) || (calib_data[i] > calib_data_max[i]))
 			{
 				calib_data[i] = calib_data_nom[i];
 				save_dat = true;
 			}
		}
	__enable_irq();	
 	if(save_dat)
 		save_calib_data();
	
}
//==================================================================================
void calk_Tbap(void)
{
	uint32_t tmp = 0;
/*
	внутр темп датчик 
	ацп 95 при -2С
	120 при +62С
	106 при 25С
*/				

	tmp = ADC_Arr[8]>>4;	
	if(tmp>95)
		Status.TBapPz = (256*tmp-24520)/100;
	else //minus
		Status.TBapPz = 0;

}
//==================================================================================
void calk_Ifid(void)
{
	unsigned int tmp1 = 0, tmp2 = 0;

	tmp1 = ADC_Arr[0];
	tmp2 = tmp1;
	tmp1 *= calib_data[0];
	tmp1 /= 18;
	tmp1 += tmp2;
	
	Status.IAF1 = tmp1;
	
	tmp1 = ADC_Arr[2];
	tmp2 = tmp1;
	tmp1 *= calib_data[1];
	tmp1 /= 18;
	tmp1 += tmp2;
	
	Status.IBF1 = tmp1;
	
	tmp1 = ADC_Arr[3];
	tmp2 = tmp1;
	tmp1 *= calib_data[2];
	tmp1 /= 18;
	tmp1 += tmp2;
	
	Status.ICF1 = tmp1;
	
	tmp1 = ADC_Arr[4];
	tmp2 = tmp1;
	tmp1 *= calib_data[3];
	tmp1 /= 18;
	tmp1 += tmp2;
	
	Status.IAF2 = tmp1;
	
	tmp1 = ADC_Arr[5];
	tmp2 = tmp1;
	tmp1 *= calib_data[4];
	tmp1 /= 18;
	tmp1 += tmp2;
	
	Status.IBF2 = tmp1;
	
	tmp1 = ADC_Arr[6];
	tmp2 = tmp1;
	tmp1 *= calib_data[5];
	tmp1 /= 18;
	tmp1 += tmp2;
	
	Status.ICF2 = tmp1;
}


//==================================================================================
__inline void Read_KN(void)//10ms
{
	uint8_t reg=0;
	static uint8_t tmp_reg=0;
	
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_5))//вкл к2  нажали на кнопку
	{
		reg |= 1<<0;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_6))//откл к2  нажали на кнопку
	{
		reg |= 1<<1;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_7))//вкл К1 нажали на кнопку
	{
		reg |= 1<<2;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_8))//откл К1  нажали на кнопку
	{
		reg |= 1<<3;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_9))//К2 вкл
	{
		reg |= 1<<4;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_10))//К1 вкл
	{
		reg |= 1<<5;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTA,PORT_Pin_4))//Kal ON
	{
		reg |= 1<<6;
	}
	
	if(tmp_reg == reg)
	{
		Status.KN_ZF = tmp_reg;//20ms
	}
	else
		tmp_reg = reg;
	
}

//==================================================================================
void send_PI23(void)
{
	//static uint8_t cnt_link=0;
	uint8_t sum = 0;

	sum += 0xAA;
	sum += NUMBER_PZF;
	sum += (uint8_t)Status.KN_ZF;
	sum += (uint8_t)Status.Error;
	sum += (uint8_t)Status.IAF1;
	sum += (uint8_t)(Status.IAF1>>8);
	sum += (uint8_t)Status.IBF1;
	sum += (uint8_t)(Status.IBF1>>8);
	sum += (uint8_t)Status.ICF1;
	sum += (uint8_t)(Status.ICF1>>8);
	sum += (uint8_t)Status.IAF2;
	sum += (uint8_t)(Status.IAF2>>8);
	sum += (uint8_t)Status.IBF2;
	sum += (uint8_t)(Status.IBF2>>8);
	sum += (uint8_t)Status.ICF2;
	sum += (uint8_t)(Status.ICF2>>8);
	
	sum = 255 - sum;
	++sum;	

	UART2_send_byte(0xAA);
	UART2_send_byte(NUMBER_PZF);
	UART2_send_byte((uint8_t)Status.KN_ZF);
	UART2_send_byte((uint8_t)Status.Error);
	UART2_send_byte((uint8_t)Status.IAF1);
	UART2_send_byte((uint8_t)(Status.IAF1>>8));
	UART2_send_byte((uint8_t)Status.IBF1);
	UART2_send_byte((uint8_t)(Status.IBF1>>8));
	UART2_send_byte((uint8_t)Status.ICF1);
	UART2_send_byte((uint8_t)(Status.ICF1>>8));
	UART2_send_byte((uint8_t)Status.IAF2);
	UART2_send_byte((uint8_t)(Status.IAF2>>8));
	UART2_send_byte((uint8_t)Status.IBF2);
	UART2_send_byte((uint8_t)(Status.IBF2>>8));
	UART2_send_byte((uint8_t)Status.ICF2);
	UART2_send_byte((uint8_t)(Status.ICF2>>8));

	UART2_send_byte(sum);
}
//==================================================================================
void Kalibrovka(void)
{
// 	unsigned char R0;
// 	
// 	if((RegS & 0x89) != (RegSOld & 0x89))
// 	{
// 		if((RegS & 0x80) && (!(RegSOld & 0x80)))
// 			if(Status.N_paramPZF<6)
// 				++Status.N_paramPZF;
// 			else 
// 				Status.N_paramPZF = 0;

// 		if((calib_data[Status.N_paramPZF] < calib_data_max[Status.N_paramPZF]) && (Status.KN_ZF&(1<<0)))
// 				++calib_data[Status.N_paramPZF];
// 			else
// 				if((calib_data[Status.N_paramPZF] > calib_data_min[Status.N_paramPZF]) && (Status.KN_ZF&(1<<2)))
// 					--calib_data[Status.N_paramPZF];

// 		
// 		if(Koef[CtInd] != ReadEeprom(CtInd+1))
// 		{

// 			R0 = Koef[CtInd];
// 			WriteEeprom((CtInd+1),R0);

// 		}		
// 		RegSOld = RegS;
// 	}
}							        
//==================================================================================
void UART2_send_byte(uint8_t byte)
{
//	uint8_t time_out_byte=0;
	
	UART_SendData(MDR_UART2,byte);

//	while(UART_GetFlagStatus (MDR_UART2, UART_FLAG_BUSY)== SET)
//	{
//		//UART_ReceiveData (MDR_UART2);
//	}
	while(UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFF));
	UART_ReceiveData ( MDR_UART2 );
}
//==================================================================================
bool Read_Data(uint8_t add_dev)
{
	uint8_t sum=0, cnt=0;

	while(cnt<(RX_FRAME_SIZE))
	{
		sum += ArrayRX_PI23[cnt];
		++cnt;
	}
	sum += 0xAA;//старт посл
	
	if(sum)//check CRC
	{
		return false;
	}
	else	
	{
		uint8_t numb = ArrayRX_PI23[1];
		
		if(numb == add_dev)
		{
			Status.Calb = (ArrayRX_PI23[2]);
			Status.F1_Off = (ArrayRX_PI23[0]>>7)&1;
			Status.F2_Off = (ArrayRX_PI23[0]>>6)&1;
			rst_alarm = (ArrayRX_PI23[0]>>5)&1;
			return true;
		}
		else
			return false;
	}
}
//==================================================================================
//==================================================================================
void SetError(void)
{
	uint8_t cnt;

	for(cnt=0;cnt<16;++cnt)
	{
		if(!ErrTime[cnt])
		{
			Status.Error |= ((uint16_t)1<<cnt);
			ALARM_ON;
		}
	}
	
	if(Status.Error&2)//>50%
	{
		OVER_K2_ON;
		K1_OFF;
		F1_ON = false;
	}
	if(Status.Error&1)//>120%
	{
		OVER_K1_ON;
		K2_OFF;
		F2_ON = false;
		K1_OFF;
		F1_ON = false;
	}
	
	if(Status.Error&8)//ТРФ
	{
		K2_OFF;
		F2_ON = false;
		K1_OFF;
		F1_ON = false;
	}
}
//==================================================================================
void ChangeCtError(void)
{
	uint8_t cnt;
	for(cnt=0;cnt<16;++cnt)
		{
		if(RegimError & (1<<cnt))
			{
			if(ErrTime[cnt])
				--ErrTime[cnt];
			}
		else
			if(ErrTime[cnt]<ErrTime0[cnt])
				++ErrTime[cnt];
		}
}
//==================================================================================
#define CAL_TIME 7
void ChangeCtTime(void)
{
uint8_t cnt;
static uint8_t	calib_time = CAL_TIME;
static bool ch_call = false;
	
	for(cnt=0; cnt<16; ++cnt)
		{
		if((RegimTime>>cnt)&1)
			{
			if(time[cnt])
				--time[cnt];
			}
		else
			time[cnt] = time0[cnt];
		}
		
	if(Status.Calb & 0x80)
	{
		if(Status.Calb & 0x60)
		{
			ch_call = true;
			calib_time = CAL_TIME;
		}
		
		if(ch_call)
			--calib_time;
		
		if(!calib_time)
		{
			calib_time = CAL_TIME;
			ch_call = false;
			save_calib_data();
		}
	}
}
//==================================================================================
void ChangeCallB(uint8_t ch_byte)
{
	if(ch_byte < ARR_CAL)
	{
		if(Status.Calb & 0x20)
		{
			if(calib_data[ch_byte]<calib_data_max[ch_byte])
				++calib_data[ ch_byte ];
		}
		else
		if(Status.Calb & 0x40)
		{
			if(calib_data[ch_byte]>calib_data_min[ch_byte])
				--calib_data[ ch_byte ];
		}
	}
}
//==================================================================================


