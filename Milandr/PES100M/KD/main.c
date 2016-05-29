/*
*
*Программа КД миландр ПЭС100М
*
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_eeprom.h"
#include <stdbool.h>
#include <string.h>
//#include <stdlib.h>


#define NUMBER_PDG	(uint8_t)0xA0
#define NUMBER_PZF	(uint8_t)0xA1


#define ST_ON		PORT_ResetBits(MDR_PORTA, (PORT_Pin_4))
#define ST_OFF		PORT_SetBits(MDR_PORTA, (PORT_Pin_4))

#define N400_ON		PORT_ResetBits(MDR_PORTA, (PORT_Pin_6))
#define N400_OFF	PORT_SetBits(MDR_PORTA, (PORT_Pin_6))

#define VOZB_ON		PORT_ResetBits(MDR_PORTE, (PORT_Pin_1))
#define VOZB_OFF	PORT_SetBits(MDR_PORTE, (PORT_Pin_1))

#define STOP_ON		PORT_ResetBits(MDR_PORTE, (PORT_Pin_7))
#define STOP_OFF	PORT_SetBits(MDR_PORTE, (PORT_Pin_7))

#define N_ISO_ON	PORT_ResetBits(MDR_PORTE, (PORT_Pin_2))
#define N_ISO_OFF	PORT_SetBits(MDR_PORTE, (PORT_Pin_2))

#define ALARM_ON	PORT_ResetBits(MDR_PORTE, (PORT_Pin_3))
#define ALARM_OFF	PORT_SetBits(MDR_PORTE, (PORT_Pin_3))
#define ALARM_TOGLE	MDR_PORTE->RXTX ^= PORT_Pin_3

#define  LIN_LOW	 	PORT_ResetBits(MDR_PORTF, (PORT_Pin_2))
#define  LIN_HIGHT	 	PORT_SetBits(MDR_PORTF, (PORT_Pin_2))
#define  LIN_READ	 	PORT_ReadInputDataBit(MDR_PORTF, (PORT_Pin_3))

#define  TEST_P	 	MDR_PORTA->RXTX ^= PORT_Pin_4

#define  TEST_MODE	 	!PORT_ReadInputDataBit(MDR_PORTA, (PORT_Pin_4))

#define MAX_MESSAGE	6

#define EEPROM_ADD	(uint32_t)0x08000000	//page0


typedef struct
{
	uint16_t	KN_DG;				//1
	uint16_t	Error;				//2
	uint16_t	UU;					//3
	uint16_t	UV;					//4
	uint16_t	UW;					//5
	uint16_t	UF;					//6
	uint16_t	IU;					//7
	uint16_t	IV;					//8
	uint16_t	IW;					//9
	uint8_t		ISL;				//10
	uint8_t		TG;					//11
	uint16_t	NDiz;				//12
	uint8_t		TBapPD;				//13
	uint8_t		Load;				//14
	uint8_t		Calb;				//15
	uint32_t	Vout;				//16
	bool		F1_Off;				//17
	bool		F2_Off;				//18
}RegS;
	
volatile RegS Status = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	

volatile bool		ADC_compl = false,
					TM=false,
					RX_Ok = false;
volatile uint16_t	delay_count = 0;
uint32_t 			RegWrk = 0,
					RegimTime = 0,
					sum_Nd = 0, sum_Niz = 0,
					tmp2=0,
					vref_50=2049;
//#define DEBUG_ON

#define RX_FRAME_SIZE	4
volatile uint8_t ArrayRX_PI23[ RX_FRAME_SIZE ];
	
	
#define ADC_COUNT_CH 9
volatile uint16_t	ADC_Arr[ ADC_COUNT_CH ],
					time[16],
					ErrTime[16],
					RegimError;
const uint16_t 		time0[16]=		{8,	4,	10,	4,	180,	20,	3,	7,	6,	0,	0,	0,	0,	0,	0,	1};
const uint16_t 		ErrTime0[16]=	{1,	1,	6,	600,	3,	3,	5,	5,	5,	15,	1,	1,	1,	1,	1,	1};//sec


#define ARR_CAL		9
uint8_t 	calib_data[ARR_CAL],
			Message[4] ;
const uint8_t		calib_data_max[ARR_CAL] = {255,	255,	255,	5,	255,	255,	255,	255,	255},
					calib_data_min[ARR_CAL] = {1,	1,		1,		1,		1,	1,		1,		1,		1},
					calib_data_nom[ARR_CAL] = {20,	20, 20,	1,		170,	170,		170,	1,		90};	//0011214
					//calib_data_nom[ARR_CAL] = {110,	110,110,	1,125,145,115,	70,	1};	//0021214

					
const char data_str[]=__DATE__;//mmm dd yyyy
					


void GPIO_init(void);
void TIM1_init(void);
void TIM2_init(void);
void TIM3_init(void);
void ADC_init(void);
void DAC_init(void);
void UART_init(void);
void PLL_init(void);
void WDT_init(void);
void EEPROM_init(void);
void CAN_init(void);
void sys_init(void);
void calk_Tbap(void);
void calk_Ugen(void);
void calk_Igen(void);
void calk_TG(void);
void calk_Ndiz(void);
void ChangeCtError(void);
void ChangeCtTime(void);
void SetError(void);
void ResetError(void);
void Read_KN(void);
void restore_calib_data(void);
void save_calib_data(void);
void send_PI23(void);
void ControlStop(void);
void ControlStarter(void);
void set_reg_work(void);
void Clear_Message(void);
void calk_RIz(void);
void UART2_send_byte(uint8_t byte);
bool Read_Data(uint8_t add_dev);
void ChangeCallB(uint8_t ch_byte);

//==================================================================================
void delay_05ms(uint16_t delay)
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
void Timer1_IRQHandler(void)//10ms
{
	static uint8_t cnt_read=0, cnt_sec=0, cnt_led=0;
	
	MDR_TIMER1->CNT = 0x0000;
	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
	 
	if(cnt_read > 46)
	{
		cnt_read = 0;
		sum_Nd = MDR_TIMER3 -> CNT;
		MDR_TIMER3 -> CNT = 0;
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
	
	if(cnt_led > 30)
	{
		cnt_led = 0;
	}
	else
		++cnt_led;
	
	Read_KN();
	
	if(RX_Ok && (!TM))
		{
		if(Read_Data(NUMBER_PDG))
			send_PI23();
		
		RX_Ok = false;
		}
}

//==================================================================================
#define DELAY_START 19
void SysTick_Handler(void)//.25 ms
{
	static bool start = false;
 	static uint8_t cnt_s = 0;
	
	if(cnt_s)
	{
		if(delay_count)
		{
			delay_count--;
		}
		cnt_s = 0;
	}
	else
		++cnt_s;
	
	sum_Niz = MDR_TIMER2 -> CNT;
	MDR_TIMER2 -> CNT = 0;
}

//==================================================================================
#define CT_AD_SUM	64
//#define	AD_SAMPL	124
#define	AD_SAMPL	70
void ADC_IRQHandler(void)//0.029ms 29 ?????? ?? ???? ?????
{
	uint8_t cnt_ch = 0;
	static uint8_t 	countAD = AD_SAMPL,
					cnt_pr=0;
	static uint32_t AdTempResult = 0,
					tmp1 =0;
	uint32_t tmp = 0,tmp12 = 0;

	tmp = ADC1_GetResult();
	cnt_ch = (tmp>>16);

	tmp &= 0xFFF;
	
	switch(cnt_ch)
	{
		case ADC_CH_ADC7:
			AdTempResult += tmp;
			if(countAD)
				{
					--countAD;
				}
				else
				{
					countAD = AD_SAMPL;
					AdTempResult /= AD_SAMPL;
					ADC_Arr[ADC_CH_ADC7] = AdTempResult;
					ADC1_SetChannel(ADC_CH_ADC0);
					ADC_compl = true;
				}
			break;
		
		default:
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
						
					ADC_Arr[cnt_ch] = tmp1;	
#else					
				{
					ADC_Arr[cnt_ch] = tmp;	
#endif
					tmp1=0;
					if((cnt_ch == 4) || (cnt_ch == 5) || (cnt_ch == 6))
					{
						if( Status.NDiz>1000 )
						{

							tmp12 = (ADC_Arr[4]*456)/(uint32_t)10;
					
							tmp = (tmp12 + 9*DAC2_GetData())/10;
						
							Status.Vout = tmp;
						
							if(tmp > 0xFFF )
								tmp = 0xFFF;

							DAC2_SetData( tmp );
						}
						else
						{
							DAC2_SetData( 0 );
							if(ADC_Arr[4]==0)
								Status.Vout = 0;
						}
					}
				
					++cnt_ch;
					ADC1_SetChannel(cnt_ch);
					
					countAD = AD_SAMPL;
				}
			break;
	}
}

//==================================================================================
//==================================================================================
//==================================================================================
int main( void )
{
PORT_InitTypeDef PORT_InitStructure;
	uint32_t cnt = 0xFFFF;
	uint32_t pr_b=0;
	
	cnt = 0xFFFF;
	while(--cnt)
	__nop();
		
	sys_init();
	
	IWDG_ReloadCounter();
	delay_05ms(50);
	
	restore_calib_data();
	
	if(TEST_MODE)
		TM=true;
		else
			TM=false;
		
	if( TM )
	//if(1)
	{
		uint16_t cnt_o = 0;
		bool up = true;
		
		while( 1 )
		{
			if( ADC_compl )
			{
				calk_Ndiz();
				calk_RIz();

				ADC_compl = false;
			}
			
			if( RX_Ok )
			{
				for(cnt=0; cnt<ADC_COUNT_CH; cnt++){
					if(cnt<4)
						UART2_send_byte((uint8_t)(ADC_Arr[cnt]>>4)*3);
					else
						UART2_send_byte((uint8_t)(ADC_Arr[cnt]>>4));
				}
				UART2_send_byte((uint8_t)Status.KN_DG);
				UART2_send_byte((uint8_t)(Status.KN_DG>>8));
				UART2_send_byte((uint8_t)sum_Nd);
				UART2_send_byte((uint8_t)(sum_Nd>>8));
				UART2_send_byte((uint8_t)sum_Niz);
				UART2_send_byte((uint8_t)(sum_Niz>>8));//15
					ST_OFF;
					N400_OFF;
					VOZB_OFF;
					STOP_OFF;
					N_ISO_OFF;
					ALARM_OFF;
				for(cnt=0; cnt<8; cnt++)
				{
				
					if((ArrayRX_PI23[0]>>cnt)&1)
						switch(cnt)
						{
							case 0:
								ALARM_ON;
								break;
							case 1:
								N_ISO_ON;
								break;
							case 2:
								STOP_ON;
								break;
							case 3:
								ST_ON;
								break;
							case 4:
								N400_ON;
								break;
							case 5:
								VOZB_ON;
								break;
							case 6:
								break;
							case 7:
								break;
						}
				}
				RX_Ok = false;
			}
			if(up)
			{
				++cnt_o;
				if(cnt_o>4094)
				{
					up = false;
				}
			}
			else
			{
				--cnt_o;
				if(!cnt)
				{
					up = true;
				}
			}
			DAC2_SetData( cnt_o );
		}
	}
	
	
	//WDT_init();
	
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTA, PORT_Pin_6,Bit_SET);
	
	if( (!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_9))&& 
		(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_10))&&
		(!PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_0)))//kalibr
		{
			Status.Calb |= 0x80;
		}
			
	while(1)
	{
		if(ADC_compl)//100 ms //36 тактов на один канал
		{
			//calk_Tbap();
			calk_Ugen();
			calk_Igen();
			calk_TG();
			calk_Ndiz();
			calk_RIz();

			ADC_compl = false;
		}
		
		
		if(Status.NDiz<1000)
		{			
			if(!(Status.Error&(1<<5)))//no ddm
				N400_OFF;
			
			RegimTime &= ~(1<<7);
		}
		else
		{
			RegimTime |= 1<<7;
			if(!time[7])
				N400_ON;
		}
			
		if( Status.KN_DG & (1<<2))// MU prov
		{
			//if(!Status.NDiz)
				RegWrk = 0x00;	
			
			//if( (Status.Calb & 0x80) && !(Status.KN_DG & 0xC04))
			if( (Status.Calb & 0x80) && ((Status.Calb&0x60) != pr_b))
			{
				pr_b = Status.Calb & 0x60;
				
				switch((Status.Calb & 0x0f))
				{
					case 0:
							ChangeCallB(7);
						break;
					case 1:
							ChangeCallB(4);
						break;
					case 2:
							ChangeCallB(5);
						break;
					case 3:
							ChangeCallB(6);
						break;
					case 4:
							ChangeCallB(0);
						break;
					case 5:
							ChangeCallB(1);
						break;
					case 6:
							ChangeCallB(2);
						break;
					case 7:
							ChangeCallB(3);
						break;
					case 14:
							ChangeCallB(8);
						break;
					default:
						break;
				}
			}
		}
		else	//DU
		{
			Status.Calb &= ~0x80;
			
			if(Status.KN_DG & (1<<6))//Pusk
			{
				RegWrk = 1;
			}
			if(Status.KN_DG & (1<<7))//STOP
			{
				if(Status.TG>70)
				{
					RegimTime |= 0x10;
					time[4] = time0[4];
					RegWrk = 0x80;
				}
				else
					RegWrk = 0x10;
			}
			set_reg_work();
		}
		
		if(Status.KN_DG & (1<<3))
		{
			RegimTime |= 1<<6;
			ResetError();
			if(!time[6])
			{
				ALARM_OFF;
			}
		}
		else
		{
			RegimTime &= ~(1<<6);
			time[6] = time0[6];
		}

		//if(Status.KN_DG & (1<<1)) // prot off

		if(Status.KN_DG & 1)
		{
			N_ISO_ON;
		}
		else
		{
			if(!(Status.Error & 0x100))
				N_ISO_OFF;
		}
		
		IWDG_ReloadCounter();//WDT reset //409.5ms
	}
}
//==================================================================================
//==================================================================================
//==================================================================================
void sys_init(void)
{
//	uint8_t cnt=255;
	PLL_init();
	
	EEPROM_init();
	
	GPIO_init();

	TIM1_init();

	TIM2_init();

	TIM3_init();
	
	UART_init();

	//WDT_init();
	SysTick_Config(HSE_Value/2000); //0.5 ms
	ADC_init();
	DAC_init();	
}
//==================================================================================
void DAC_init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DAC, ENABLE);
	DAC_DeInit();
	DAC2_Init(DAC2_AVCC);
	//DAC2_SetData(0x7FF);
	DAC2_SetData(0x00);
	DAC2_Cmd(ENABLE);
}
//==================================================================================
void ADC_init(void)
{
	ADC_InitTypeDef sADC;
	ADCx_InitTypeDef sADCx;
	
	ADC_StructInit(&sADC);
	ADCx_StructInit(&sADCx);
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
	sADCx.ADC_SamplingMode = ADC_SAMPLING_MODE_CICLIC_CONV;
	sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
	sADCx.ADC_LevelControl = DISABLE;
	sADCx.ADC_ChannelNumber	= ADC_CH_ADC0;
	sADCx.ADC_VRefSource	= ADC_VREF_SOURCE_INTERNAL;
	sADCx.ADC_IntVRefSource	= ADC_INT_VREF_SOURCE_INEXACT;
	sADCx.ADC_Prescaler = ADC_CLK_div_16;//0.029 ms
	sADCx.ADC_DelayGo = 7;//29 тактов

	ADC1_Init(&sADCx);
	
	ADC1_ClearOverwriteFlag();
	
	/* Enable ADC1 EOCIF and AWOIFEN interupts */
	ADC1_ITConfig((ADC1_IT_END_OF_CONVERSION), ENABLE);
	
	NVIC_EnableIRQ(ADC_IRQn);

	
	/* ADC1 enable */
	ADC1_Cmd(ENABLE);
	
	//ADC1_Start();
}
//==================================================================================
#define ALL_PORTS_CLK ( RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)
void GPIO_init(void)
{
	PORT_InitTypeDef PORT_InitStructure;
	
  /* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
	PORT_StructInit(&PORT_InitStructure);//reset struct	
 /* Configure PORTA pins */
	
	 /* Configure PORTA pins */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4);
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_StructInit(&PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1);
	PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	
	PORT_StructInit(&PORT_InitStructure);
	
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0|PORT_Pin_2|PORT_Pin_5|PORT_Pin_7);
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_3);
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);

PORT_StructInit(&PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_6);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTA, PORT_Pin_6,Bit_SET);


	PORT_InitStructure.PORT_Pin   = (PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10);
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_7);
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);

/* Configure PORTC pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0);
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);

	PORT_StructInit(&PORT_InitStructure);
	
/* Configure PORTD pin */

	PORT_InitStructure.PORT_Pin  = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|	\
									PORT_Pin_3|PORT_Pin_4|PORT_Pin_5|	\
									PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_InitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);

/* Configure PORTE pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0);
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_InitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_6);
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1|PORT_Pin_2|PORT_Pin_3|PORT_Pin_7);
	PORT_InitStructure.PORT_PD 	= PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTE, (PORT_Pin_1|PORT_Pin_2|PORT_Pin_3|PORT_Pin_7),Bit_SET);

  /* Configure PORTF pin */
  	PORT_InitStructure.PORT_Pin = (PORT_Pin_0|PORT_Pin_1);
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_PD 	= PORT_PD_OPEN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	
	PORT_Init(MDR_PORTF, &PORT_InitStructure);


	PORT_StructInit(&PORT_InitStructure);//reset struct

	PORT_InitStructure.PORT_Pin = (PORT_Pin_3);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6);
	PORT_InitStructure.PORT_PD 	= PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTF, (PORT_Pin_4|PORT_Pin_5|PORT_Pin_6),Bit_SET);
 	//PORT_StructInit(&PORT_InitStructure);//reset struct
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
	sTIM_CntInit.TIMER_Period                   = 20;
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

	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
	
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
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_EvtFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_CH1;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER2,&sTIM_CntInit);

  
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
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_CH4;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER3,&sTIM_CntInit);

	/* Enable TIMER3 */
	TIMER_Cmd(MDR_TIMER3,ENABLE);
}
//==================================================================================
void EEPROM_init(void)
{	
//	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", \
//							"Sep", "Oct", "Nov", "Dec"};
//	uint8_t i=0,month=0;

	/* Enables the clock on EEPROM */
//	EEPROM_SetLatency(EEPROM_Latency_7);
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
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while(RST_CLK_HSEstatus() == ERROR);
   
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	MDR_RST_CLK->CPU_CLOCK |= 2;
	RST_CLK_HSIcmd(DISABLE);
}

//==================================================================================
void UART_init(void)
{
	
	UART_InitTypeDef UART_InitStructure;

	UART_StructInit(&UART_InitStructure);

	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2,ENABLE);

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
	UART_Cmd(MDR_UART2,ENABLE);
}
//==================================================================================
void WDT_init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_IWDG,ENABLE);
	IWDG_WriteAccessEnable();
	IWDG_SetPrescaler(IWDG_Prescaler_4);//409.5ms
	IWDG_SetReload(0xFFF);
	IWDG_Enable();
	IWDG_ReloadCounter();
}
//==================================================================================
void calk_TG(void) 
{
	static uint32_t sum_tmp=0, cnt_sum=0;
	uint32_t tmp = 0;
	unsigned int tmp1 = 0, tmp2 = 0;
	
	
	sum_tmp += ADC_Arr[7];
	++cnt_sum;
	if(cnt_sum>127)
	{
		cnt_sum =0;
		sum_tmp >>= 7;
		tmp1 = sum_tmp;
		if(tmp1 > 510)
		//tmp1 = ADC_Arr[7]>>3;
		//if(tmp1 > 453)
			tmp =0;
		else
		{
			tmp = tmp1*34;
			tmp = 15372 - tmp;
			tmp /= 100;
		}
		
		tmp1 = tmp;
		tmp2 = tmp1;
		tmp1 *= calib_data[7];
		tmp1 >>= 8;
		tmp1 += tmp2;

			
		Status.TG = tmp1;
	}

		if(Status.TG>105)
			{
				RegimError |= 1<<4;
			}
		else		
			RegimError &= ~(1<<4);

}

//==================================================================================
void calk_Igen(void)
{
//	unsigned int tmp1 = 0, tmp2 = 0;

//	tmp1 = ADC_Arr[4];
//	tmp2 = tmp1;
//	tmp1 *= calib_data[4];
//	tmp1 >>= 7;
//	tmp1 += tmp2;
//	
//	Status.IU = tmp1;
//	
//	tmp1 = ADC_Arr[5];
//	tmp2 = tmp1;
//	tmp1 *= calib_data[5];
//	tmp1 >>= 7;
//	tmp1 += tmp2;
//	
//	Status.IV = tmp1;
//	
//	tmp1 = ADC_Arr[6];
//	tmp2 = tmp1;
//	tmp1 *= calib_data[6];
//	tmp1 >>= 7;
//	tmp1 += tmp2;
//	
//	Status.IW = tmp1;
//	
//	
	
	static uint32_t cnt_u=0,
					tmp1U = 0, 
					tmp1V = 0, 
					tmp1W = 0, 
					tmp2 = 0;
	uint32_t tmp1=0;
	++cnt_u;
	
	tmp1 = ADC_Arr[4];
	tmp2 = tmp1;
	tmp1 *= calib_data[4];
	tmp1 >>= 5;
	tmp1 += tmp2;
	
	tmp1U += tmp1;
	
	tmp1 = ADC_Arr[5];
	tmp2 = tmp1;
	tmp1 *= calib_data[5];
	tmp1 >>= 5;
	tmp1 += tmp2;
	
	tmp1V += tmp1;
	
	tmp1 = ADC_Arr[6];
	tmp2 = tmp1;
	tmp1 *= calib_data[6];
	tmp1 >>= 5;
	tmp1 += tmp2;
	
	tmp1W += tmp1;
	
	
	if(cnt_u>63)
	{
		cnt_u=0;
		tmp1U >>= 6;
		tmp1V >>= 6;
		tmp1W >>= 6;

		Status.IU = tmp1U;
		Status.IV = tmp1V;
		Status.IW = tmp1W;
	}
	
}

//==================================================================================
void calk_Ugen(void)
{
	static uint32_t cnt_u=0,
					tmp1U = 0, 
					tmp1V = 0, 
					tmp1W = 0, 
					tmp1F = 0, tmp2 = 0;
	uint32_t tmp1=0;
	++cnt_u;
	
	tmp1 = ADC_Arr[0]>>1;
	tmp2 = tmp1;
	tmp1 *= calib_data[0];
	tmp1 >>= 8;
	tmp1 += tmp2;
	
	tmp1U += tmp1;
	
	tmp1 = ADC_Arr[1]>>1;
	tmp2 = tmp1;
	tmp1 *= calib_data[1];
	tmp1 >>= 8;
	tmp1 += tmp2;
	
	tmp1V += tmp1;
	
	tmp1 = ADC_Arr[2]>>1;
	tmp2 = tmp1;
	tmp1 *= calib_data[2];
	tmp1 >>= 8;
	tmp1 += tmp2;
	
	tmp1W += tmp1;
	
	tmp1 = ADC_Arr[3]>>1;
	tmp2 = tmp1;
	tmp1 *= calib_data[3];
	tmp1 >>= 8;
	tmp1 += tmp2;
	
	tmp1F += tmp1;
	
	if(cnt_u>63)
	{
		cnt_u=0;
		tmp1U >>= 6;
		tmp1V >>= 6;
		tmp1W >>= 6;
		tmp1F >>= 6;
		Status.UU = (Status.UU + 99*tmp1U)/100;
		Status.UV = (Status.UV + 99*tmp1V)/100;
		Status.UW = (Status.UW + 99*tmp1W)/100;
		Status.UF = (Status.UF + 99*tmp1F)/100;
	}
}

//==================================================================================
__inline void Read_KN(void)
{
	uint16_t reg=0;
	static uint16_t tmp_reg=0;
	
	if(PORT_ReadInputDataBit(MDR_PORTA,PORT_Pin_0))
	{
		reg |= 1<<0;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_6))
	{
		reg |= 1<<1;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_0))// MU prov
	{
		reg |= 1<<2;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_8))
	{
		reg |= 1<<3;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTA,PORT_Pin_2))
	{
		reg |= 1<<4;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTA,PORT_Pin_7))
	{
		reg |= 1<<5;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_5))
	{
		reg |= 1<<6;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_6))
	{
		reg |= 1<<7;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTA,PORT_Pin_3))
	{
		reg |= (uint16_t)1<<8;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTA,PORT_Pin_5))
	{
		reg |= (uint16_t)1<<9;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_9))
	{
		reg |= (uint16_t)1<<10;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_10))
	{
		reg |= (uint16_t)1<<11;
	}

	if(tmp_reg == reg)
	{
		Status.KN_DG = tmp_reg;
		Status.Calb &= ~0x60;
		Status.Calb |= ((Status.KN_DG & 0xC00)>>5)&0xFF;
	}
	else
		tmp_reg = reg;
	
	if(Status.KN_DG&(1<<4))
	{
		if(!(Status.KN_DG&(1<<1)))
			RegimError |= (1<<4);
		else
			ALARM_ON;
	}
	else
		RegimError &= ~(1<<4);
	
	if(Status.KN_DG&(1<<5) && (Status.NDiz > 400))
		RegimError |= (1<<5);
	else
		RegimError &= ~(1<<5);
		
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
void SetError(void)
{
	uint8_t cnt;

	for(cnt=0; cnt<16; ++cnt)
	{
		if(!ErrTime[cnt])
		{
			Status.Error |= ((uint16_t)1<<cnt);
			ALARM_ON;
		}
	}	
	
	if(Status.Error)
	{
		
		if(RegWrk&0x40)
		{
			if(Status.Error & 0x20) 
				RegWrk = 0x10;
			else
				if((Status.Error & 0x10) && (!(Status.KN_DG & (1<<1))))
					RegWrk=0x80;
				else
					if(Status.Error & 0x40) 
					{
						
					}
					else
					if(Status.Error & 0x80)
					{
						
					}
		}
		
		if(!(RegWrk&0x10))
		{
			
		}	
		if(Status.Error & 0x100)
		{
			N_ISO_ON;
		}
	}
}
//==================================================================================
#define CAL_TIME 30
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
void ResetError(void)
{
	uint8_t cnt_bit = 0;
	
	Status.Error = 0;
	RegimError = 0;
	STOP_OFF;
	N_ISO_OFF;
	
	while( cnt_bit<16 )
	{
		ErrTime[cnt_bit] = ErrTime0[cnt_bit];
		++cnt_bit;
	}
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
void calk_Ndiz(void)
{
	uint32_t tmp1=0, tmp2=0;
	
	//tmp1 = (sum_Nd*100)/126;
	tmp1 = (sum_Nd*100)/139;
	tmp2 = tmp1;
	tmp1 *= calib_data[8];
	tmp1 >>= 7;
	tmp1 += tmp2;

	Status.NDiz = tmp1;
	
	if(Status.NDiz > 1878)
	{
		time[0] = time0[0];
		RegimError |= (uint16_t)1<<2;
	}
	else
	
		RegimError &= ~((uint16_t)1<<2);
	
	if( RegWrk&0x40 )
	{
		if( Status.NDiz < 400 )
			RegimError |= ( uint16_t )1<<3;
		else
			RegimError &= ~(( uint16_t )1<<3);
	}
}
//==================================================================================
void calk_Tbap(void)
{
	uint32_t tmp = 0;


	tmp = ADC_Arr[8]>>4;	
	if(tmp>95)
		Status.TBapPD = (256*tmp-24520)/100;
	else 
		Status.TBapPD = 0;
}

//==================================================================================
void send_PI23(void)
{
	uint8_t sum = 0;

	sum += 0xAA;
	sum += NUMBER_PDG;
	sum += (uint8_t)Status.KN_DG;
	sum += (uint8_t)(Status.KN_DG>>8);
	sum += (uint8_t)(Status.Error&0xFF);
	sum += (uint8_t)Status.UU;
	sum += (uint8_t)(Status.UU>>8);
	sum += (uint8_t)Status.UV;
	sum += (uint8_t)(Status.UV>>8);
	sum += (uint8_t)Status.UW;
	sum += (uint8_t)(Status.UW>>8);
	sum += (uint8_t)Status.UF;
	sum += (uint8_t)(Status.UF>>8);
	sum += (uint8_t)Status.IU;
	sum += (uint8_t)(Status.IU>>8);
	sum += (uint8_t)Status.IV;
	sum += (uint8_t)(Status.IV>>8);
	sum += (uint8_t)Status.IW;
	sum += (uint8_t)(Status.IW>>8);
	sum += (uint8_t)Status.ISL;
	sum += (uint8_t)Status.TG;
	sum += (uint8_t)Status.NDiz;
	sum += (uint8_t)(Status.NDiz>>8);
	sum += (uint8_t)RegWrk;
	sum += (uint8_t)(RegWrk>>8);
	sum += (uint8_t)Status.Calb;
	
	sum = 255 - sum;
	++sum;	

	
	UART2_send_byte(0xAA);
	UART2_send_byte(NUMBER_PDG);
	UART2_send_byte((uint8_t)Status.KN_DG);
	UART2_send_byte((uint8_t)(Status.KN_DG>>8));
	UART2_send_byte((uint8_t)(Status.Error&0xFF));
	UART2_send_byte((uint8_t)Status.UU);
	UART2_send_byte((uint8_t)(Status.UU>>8));
	UART2_send_byte((uint8_t)Status.UV);
	UART2_send_byte((uint8_t)(Status.UV>>8));
	UART2_send_byte((uint8_t)Status.UW);
	UART2_send_byte((uint8_t)(Status.UW>>8));
	UART2_send_byte((uint8_t)Status.UF);
	UART2_send_byte((uint8_t)(Status.UF>>8));
	UART2_send_byte((uint8_t)Status.IU);
	UART2_send_byte((uint8_t)(Status.IU>>8));
	UART2_send_byte((uint8_t)Status.IV);
	UART2_send_byte((uint8_t)(Status.IV>>8));
	UART2_send_byte((uint8_t)Status.IW);
	UART2_send_byte((uint8_t)(Status.IW>>8));
	UART2_send_byte((uint8_t)Status.ISL);
	UART2_send_byte((uint8_t)Status.TG);
	UART2_send_byte((uint8_t)Status.NDiz);
	UART2_send_byte((uint8_t)(Status.NDiz>>8));
	UART2_send_byte((uint8_t)RegWrk);
	UART2_send_byte((uint8_t)(RegWrk>>8));
	UART2_send_byte((uint8_t)Status.Calb);

	UART2_send_byte(sum);
}
//==================================================================================
void set_reg_work(void)
{
	static uint8_t cnt_pusk = 0;
	
	if(RegWrk == 1)
	{
		if(Status.NDiz<400)
		{
			RegimTime |= 1<<1;
			RegWrk = 4;
			cnt_pusk = 0;
			ST_ON;
		}	
		else
			RegWrk &= ~1;
	}
	
	
	if(RegWrk&2)
	{

	}
	
	if(RegWrk == 4)
	{
		if(cnt_pusk<3)
		{
			if(!time[1])
			{
				ST_OFF;
				RegimTime |= 4;
				RegimTime &= ~2;
				time[1] = time0[1];
				if((Status.NDiz>400))
				{
					VOZB_ON;
				}
			}
			if(!time[2])
			{
				if((Status.NDiz>400))
				{
					ST_OFF;
					RegWrk &= ~2;
					RegWrk &= ~4;
					RegWrk &= ~1;
					RegWrk |= 0x40;
					RegimTime &= ~(1<<2);
				}
				else
				{
					if(!(Status.NDiz))
						ST_ON;
					
					RegimTime |= 2;
					RegimTime &= ~4;
					time[2] = time0[2];
					++cnt_pusk;
				}
			}
		}
		else
		{
			ST_OFF;
			time[2] = time0[2];
			RegimTime = 4;
			Status.Error |= 1;
			RegWrk = 0;
			ALARM_ON;
		}
	}
	

	
	if((RegWrk&0x10))
	{
		//DAC2_SetData(0);
 		RegWrk &= ~0x6F;
 		RegimTime |= 1<<5;
 		VOZB_OFF;
 		ST_OFF;

 		STOP_ON;
 		
		if(!time[5])
		{
			if(Status.NDiz > 400)
			{
				ALARM_ON;
				RegimError = 0;
				Status.Error |= 2;
				RegimTime &= ~(1<<5);
				RegWrk = 0;
			}
			else
			{
				STOP_OFF;
				RegWrk = 0;
				RegimTime = 0;
				RegimError = 0;
			}
		}
	}
	if(RegWrk&0x20)
	{
		
	}
	if(RegWrk&0x40)
	{
		if(!(Status.Error & 0x1B0))
		{
			if((Status.UU<30) && (Status.UV<30) && (Status.UW<30))
			{
				RegimError |= (1<<6);
			}
			else
			{
				RegimError &= ~(1<<6);
				if(((Status.UU<205)||(Status.UV<205)||(Status.UW<205)||
					(Status.UU>245)||(Status.UV>245)||(Status.UW>245))&&
					(RegWrk == 0x40))
				{
					RegimError |= 1<<7;
				}
				else
					RegimError &= ~(1<<7);
			}
			VOZB_ON;
 			//	DAC2_SetData(Status.Vout);
		}
	}
	if(RegWrk&0x80)
	{
		//DAC2_SetData(0);
		if((Status.TG>70))
		{
			RegimTime |= 1<<4;
			if(!time[4])
			{
				RegWrk=0x10;
				RegimTime &= ~(1<<4);
				time[5] = time0[5];
			}
		}
		else
		{
			RegimTime |= 4;
			if(!time[2])
			{
				RegWrk=0x10;
				RegimTime &= ~(1<<4);
				time[5] = time0[5];
			}
		}
	}
	

	if(!RegWrk)
	{
		if((Status.Error == 0) && (Status.NDiz>1000))
		{
			RegWrk = 0x40;
		}
	}
}

//==================================================================================
void Clear_Message(void)
{
	uint8_t cnt = 0;
	
	while(cnt<MAX_MESSAGE)
	{
		Message[cnt] = 0;
		cnt++;
	}
}
//==================================================================================
void calk_RIz(void)
{
	Status.ISL = sum_Niz;
	if(Status.ISL > 10)
		RegimError |= (1<<8);
	else
		RegimError &= ~(1<<8);
}
//==================================================================================
void UART2_send_byte(uint8_t byte)
{
	
	UART_SendData(MDR_UART2,byte);
//	while(UART_GetFlagStatus (MDR_UART2, UART_FLAG_BUSY)== SET)
//	{
//		UART_ReceiveData (MDR_UART2);
//	}
	while(UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFF));
	UART_ReceiveData ( MDR_UART2 );
}
//==================================================================================
bool Read_Data(uint8_t add_dev)
{
	uint8_t sum=0,cnt=0;

	while(cnt<(RX_FRAME_SIZE))
	{
		sum+=ArrayRX_PI23[cnt];
		++cnt;
	}
	sum+=0xAA;
	if(sum)
	{
		return false;
	}
	else	
	{
		uint8_t numb = ArrayRX_PI23[1];
		
		if(numb == add_dev)
		{
			Status.F1_Off = (ArrayRX_PI23[0]>>7)&1;
			Status.F2_Off = (ArrayRX_PI23[0]>>6)&1;
			Status.Calb &= 0xF0;
			Status.Calb |= (ArrayRX_PI23[2]&0x0F);
			return true;
		}
		else
			return false;
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




