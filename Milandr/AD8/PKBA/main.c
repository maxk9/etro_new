/*
*Программа ПКБА миландр АД8
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
#include "link_pult.h"
#include "MDR32F9Qx_power.h"
#include <string.h>
//#include <stdlib.h>


#define trans_toka

#define EEPROM_ADD	(uint32_t)0x08000000	//page0

#define PORTD_PIN (GPIO_PIN(10)|GPIO_PIN(11)|GPIO_PIN(12)|GPIO_PIN(13)|GPIO_PIN(14))

#define LED_ON		PORT_SetBits(MDR_PORTB, PORT_Pin_5)
#define LED_OFF		PORT_ResetBits(MDR_PORTB, PORT_Pin_5)


const char data_str[]=__DATE__;//mmm dd yyyy

volatile RegS Status={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile uint16_t delay_count=0;

uint8_t error_pkba;
#define ADC_COUNT_CH 9
uint16_t	ADC_Arr[ADC_COUNT_CH],
			nd=0,
			delay_flash_led=0;

#define RX_FRAME_SIZE	3
#define ARR_CAL			6
volatile uint8_t ArrayRX_PKDU[RX_FRAME_SIZE],
				calib_data[6];
const uint8_t	calib_data_max[6] = {30,1,1,255,1,1},
				calib_data_min[6] = {0,0,0,235,0,0},
				calib_data_nom[6] = {20,0,0,245,0,0};
volatile bool 	ADC_compl = false,
				RX_ok = false,
				ndiz_ok = 0,
				led = false;

ADC_InitTypeDef sADC;
ADCx_InitTypeDef sADCx;
PORT_InitTypeDef PORT_InitStructure;
TIMER_CntInitTypeDef sTIM_CntInit;
TIMER_ChnInitTypeDef sTIM_ChnInit;
static UART_InitTypeDef UART_InitStructure;

volatile uint16_t time[16],RegimError;
const uint16_t time0[16] = {5,3,10,4,10,10,30,600,6,0,0,0,0,0,0,1};
const uint16_t ErrTime0[16] = {1,1,6,600,10,20,10,1,5,15,1,1,1,1,1,1};
volatile uint16_t ErrTime[16];

//==================================================================================
void GPIO_init(void);
void TIM1_init(void);
void TIM2_init(void);
void TIM3_init(void);
void ADC_init(void);
void UART_init(void);
void PLL_init(void);
void WDT_init(void);
void EEPROM_init(void);
void send_PKDU(void);
void calk_Tbap(void);
void calk_Ugen(void);
void calk_Igen(void);
void Read_KN(void);
void calk_Pm(void);
void calk_Tm(void);
bool ReadPKDU(void);
void set_reg_work(void);
void ChangeCtTime(void);
void ChangeCtError(void);
void ControlLed(void);
void ResetError(void);
void SetError(void);
void ControlKG(void);
void save_calib_data(void);
void restore_calib_data(void);
//==================================================================================
void delay_ms(uint16_t delay)
{
	delay_count=delay;
	while(delay_count);
}
//==================================================================================
void Timer1_IRQHandler(void)//100ms
{
	static uint8_t cnt_read=0,cnt_sec=0;
	uint32_t sum_Nd=0;

	
	MDR_TIMER1->CNT = 0x0000;
	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
	
	if(cnt_read>8)
	{
		cnt_read=0;
		sum_Nd=MDR_TIMER3->CNT;
		MDR_TIMER3->CNT=0;
			
			sum_Nd*=669;
			Status.NDiz=sum_Nd/1000;
			if(Status.RegWrk&0x40)
			{
				if(((Status.NDiz<2850)||(Status.NDiz>3300)))
					RegimError|=(uint16_t)1<<8;
				else
					RegimError&=~((uint16_t)1<<8);
			}
	}
	else
		++cnt_read;
	
	if(cnt_sec>11)
	{
		ChangeCtTime();
		ChangeCtError();
		SetError();
		cnt_sec=0;
	}
	else
		++cnt_sec;
	
}

void Timer2_IRQHandler(void)
{
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
		Read_KN();
	
}


void UART1_IRQHandler(void)
{
	uint8_t tmp=0;
	static bool start=false;
	static uint8_t cnt_rx_byte=0;
	
		tmp=UART_ReceiveData(MDR_UART1);
		
		if((tmp==0xAA)&&(!start))
		{
			start=true;
		}
		else
		{
			if(start)
			{
				if(!RX_ok)
					ArrayRX_PKDU[cnt_rx_byte]=tmp; 
				
				++cnt_rx_byte;
				if(cnt_rx_byte==RX_FRAME_SIZE)
				{	
					RX_ok=true;
					start=false;
					cnt_rx_byte=0;
				}
			}
		}
}

void UART2_IRQHandler(void)
{
	if(UART_GetITStatus(MDR_UART2,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART2,UART_IT_RX);
	}
}




#define	AD_SAMPL	62
void ADC_IRQHandler(void)//0.144 ms //36 тактов на один канал
{
	uint8_t cnt_ch = 0;
	static uint8_t 	countAD = AD_SAMPL+2;
	static uint32_t AdTempResult = 0;
	
	uint32_t tmp = 0;

	tmp = ADC1_GetResult();
	cnt_ch = ( tmp>>16 );

	tmp &= 0xFFF; //12 bit

	if(countAD)
	{
		--countAD;
		AdTempResult += tmp;
	}
	else
	{
		countAD = AD_SAMPL + 2;
		AdTempResult >>= 6; 
		
		switch(cnt_ch)
		{
			case ADC_CH_ADC7:
				ADC1_SetChannel( ADC_CH_TEMP_SENSOR );
				break;
			case ADC_CH_TEMP_SENSOR:
				ADC1_SetChannel( ADC_CH_ADC0 );
				cnt_ch = 8;
				break;
			default:
				ADC1_SetChannel((cnt_ch+1));
				break;
		}
		
		if( !ADC_compl )
			ADC_Arr[cnt_ch] = AdTempResult;
		
		if( cnt_ch == 8 )
			ADC_compl = true;
		
		AdTempResult = 0;
	}
}




void SysTick_Handler(void)//~1 ms
{
	static uint16_t cnt_led=0;
	
	if(delay_count)
	{
		delay_count--;
	}
	
	++cnt_led;
	++delay_flash_led;
	++time_out_byte;

	if(led)
	{
		if(cnt_led==1000)
		{
			LED_ON;
		}
		if(cnt_led==1100)
		{
			LED_OFF;
		}
		if(cnt_led==1200)
		{
			LED_ON;
		}
		if(cnt_led>1300)
		{
			LED_OFF;
			cnt_led=0;
		}
	}
	else
	{
		if(cnt_led==900)
		{
			LED_ON;
		}
		else
			if(cnt_led>1000)
			{
				LED_OFF;
				cnt_led=0;
			}
	}

}
//==================================================================================
//==================================================================================
//==================================================================================
int main()
{
	uint8_t cnt = 255;
	cnt = 255;
	while(--cnt)	
	__nop();
	
	
	PLL_init();
	GPIO_init();
	TIM1_init();
	TIM2_init();
	TIM3_init();
	UART_init();
	EEPROM_init();
	WDT_init();
	ADC_init();
	SysTick_Config(SystemCoreClock/1200);//~1 ms
	restore_calib_data();
	
  while(1)
	{
		if(ADC_compl)
		{	
			calk_Tbap();
			calk_Ugen();
			calk_Igen();
			calk_Pm();
			calk_Tm();
			Status.DT=ADC_Arr[6]>>4;
			
			
			ADC_compl=false;
		}	
		
		if(RX_ok)
		{
			if(ReadPKDU())
				send_PKDU();
			
			led=true;
			delay_flash_led=0;
			RX_ok=false;
		}
		
		if(delay_flash_led>500)
		{
			led=false;
			delay_flash_led=0;
			Status.Led2|=1<<2;
			
			if(Status.RegWrk&0x40)
				{
					PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
				}
			Status.RegWrk=0;
			Status.RegimTime=0;
			Status.Led1&=~(1<<7);
			Status.Led2&=~(0x23);
		}
		else
		{
			if((Status.RabReg0>>1)&1)
				{
					Status.Led2&=~(1<<2);
					
					if((Status.UGen>100)&&(Status.NDiz>1000)&&(!Status.RegWrk))
						Status.RegWrk=0x40;
					
					set_reg_work();
					ControlLed();
					ControlKG();
				}
			else
			{
				Status.Led2|=1<<2;
				if(Status.RegWrk&0x40)
				{
					PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
				}
				Status.RegWrk=0;
				Status.RegimTime=0;
				Status.Led1&=~(1<<7);
				Status.Led2&=~(0x23);
			}
		}
		__nop();//
		
		IWDG_ReloadCounter();
	}
}
//==================================================================================
//==================================================================================
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
	sADCx.ADC_Prescaler = ADC_CLK_div_16;

	sADCx.ADC_DelayGo = 7;

	ADC1_Init(&sADCx);
	
	ADC1_ClearOverwriteFlag();
	
	/* Enable ADC1 EOCIF and AWOIFEN interupts */
	ADC1_ITConfig((ADC1_IT_END_OF_CONVERSION), ENABLE);
	
	NVIC_EnableIRQ(ADC_IRQn);

	
	/* ADC1 enable */
	ADC1_Cmd(ENABLE);

}
//==================================================================================

#define ALL_PORTS_CLK ( RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF )
void GPIO_init(void)
{
  /* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
	PORT_StructInit(&PORT_InitStructure);//reset struct	
 /* Configure PORTA pins */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|	\
									PORT_Pin_3|PORT_Pin_4|PORT_Pin_5);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTA, (PORT_Pin_1|PORT_Pin_2|PORT_Pin_3|PORT_Pin_4|PORT_Pin_5),Bit_SET);
	
 	
	PORT_InitStructure.PORT_Pin = (PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
  /* Configure PORTB pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_5);
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_6|PORT_Pin_7|PORT_Pin_8|PORT_Pin_10);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	PORT_ResetBits(MDR_PORTB, PORT_Pin_All);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_9);
	PORT_InitStructure.PORT_PD 	= PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTB, PORT_Pin_9,Bit_SET);
	
	  /* Configure PORTC pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	PORT_ResetBits(MDR_PORTC, PORT_Pin_All);
	
	  /* Configure PORTD pin */

	PORT_InitStructure.PORT_Pin  = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|	\
									PORT_Pin_3|PORT_Pin_4|PORT_Pin_5|	\
									PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_InitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
/* Configure PORTE pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|	\
									PORT_Pin_3|PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_PD 	= PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTE, (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|PORT_Pin_3|PORT_Pin_6|PORT_Pin_7),Bit_SET);
	
  /* Configure PORTF pin */
  	PORT_InitStructure.PORT_Pin = (PORT_Pin_0|PORT_Pin_1);//UART2 RX TX
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MDR_PORTF, &PORT_InitStructure);
}
//==================================================================================
void TIM1_init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1,ENABLE);
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER1);

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
	sTIM_CntInit.TIMER_Prescaler                = 7999;//100ms
	sTIM_CntInit.TIMER_Period                   = 82;
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
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2,ENABLE);
	
/* Reset all TIMER2 settings */
	TIMER_DeInit(MDR_TIMER2);

	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
	sTIM_CntInit.TIMER_Prescaler                = 4000;//50ms
	sTIM_CntInit.TIMER_Period                   = 82;
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
	
  
	/* TMR1 enable */
	TIMER_Cmd (MDR_TIMER2,ENABLE);
}
//==================================================================================
void TIM3_init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3,ENABLE);
	/* Reset all TIMER3 settings */
	TIMER_DeInit(MDR_TIMER3);

  TIMER_BRGInit(MDR_TIMER3,TIMER_HCLKdiv1);

  /* TIM3 configuration ------------------------------------------------*/
  /* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
  sTIM_CntInit.TIMER_Prescaler                = 0;
  sTIM_CntInit.TIMER_Period                   = 0xFFFF;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_EvtFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_ETR;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TIMER3,&sTIM_CntInit);
  
  TIMER_Cmd(MDR_TIMER3,ENABLE);
}
//==================================================================================
void EEPROM_init(void)
{
	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", \
							"Sep", "Oct", "Nov", "Dec"};
	uint8_t i=0,month=0;

	/* Enables the clock on EEPROM */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	EEPROM_SetLatency(EEPROM_Latency_0);
	EEPROM_ReadHalfWord(EEPROM_ADD+4,EEPROM_Info_Bank_Select);
	
	for (i = 0; i < 12; i++)
	  {
		if (!strncmp(data_str, months[i],3))
		{
		  month = i + 1;
		}
	  }		
		if(data_str[4]>='0')
			Status.reg1=(data_str[4]-'0')*10+(data_str[5]-'0');
		else
			Status.reg1=data_str[5]-'0';
		
 	Status.reg0=month;
}
//==================================================================================
void PLL_init(void)
{
	/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while(RST_CLK_HSEstatus() == ERROR);
	
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	MDR_RST_CLK->CPU_CLOCK |= 2;
	
	RST_CLK_HSIcmd(DISABLE);
}

//==================================================================================
void UART_init(void)
{
	  
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1,ENABLE);
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_DeInit(MDR_UART1);
	
    NVIC_DisableIRQ(UART1_IRQn);
	
  UART_InitStructure.UART_BaudRate				= 9600;
  UART_InitStructure.UART_WordLength			= UART_WordLength8b;
  UART_InitStructure.UART_StopBits				= UART_StopBits2;
  UART_InitStructure.UART_Parity				= UART_Parity_No;
  UART_InitStructure.UART_FIFOMode				= UART_FIFO_OFF;
  UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;


  UART_Init(MDR_UART1,&UART_InitStructure);

	NVIC_EnableIRQ(UART1_IRQn);
	UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);

	UART_Cmd(MDR_UART1,ENABLE);
}
//==================================================================================
void WDT_init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_IWDG,ENABLE);
	IWDG_WriteAccessEnable();
	IWDG_SetPrescaler(IWDG_Prescaler_4);
	IWDG_SetReload(0xFFF);
	IWDG_Enable();
	IWDG_ReloadCounter();
}
//==================================================================================
void set_reg_work(void)
{
	static uint8_t cnt_pusk=0;
	
	if(Status.RegWrk&1)
	{
		if((Status.UGen<100) && (Status.NDiz<1000))
		{
			Status.Led2 |= 1;
			if(Status.TM>35) //TM >+5C
			{
				if(!(Status.RegWrk&2))
				{
					PORT_ResetBits(MDR_PORTE, (PORT_Pin_1));//TPN
					Status.RegimTime |= 1;
					Status.RegWrk |= 2;
					Status.Error |= 1<<14;
				}
			}
			else
			{
				Status.RegWrk |= 0x20;
				Status.RegWrk &= ~1;
				Status.RegimTime |= 1<<6;
				PORT_ResetBits(MDR_PORTE, (PORT_Pin_3));//PV
				PORT_ResetBits(MDR_PORTA, (PORT_Pin_2));//MEN
			}
		}	
	}
	if(Status.RegWrk&2)
	{
		if(!time[0])
		{
			Status.Error &=~(1<<14);
			PORT_SetBits(MDR_PORTE,(PORT_Pin_1));
			Status.RegimTime&=~1;
			if((Status.UGen<100)&&(Status.NDiz<1000))
				PORT_ResetBits( MDR_PORTE, (PORT_Pin_6) );//ST ON
			
			Status.RegimTime |=2;
			Status.RegWrk |=4;
			cnt_pusk=0;
		}
	}
	if(Status.RegWrk&4)
	{
		if(cnt_pusk<3)
		{
			if(!time[1])
			{
				PORT_SetBits(MDR_PORTE, (PORT_Pin_6));
				Status.RegimTime|=4;
				Status.RegimTime&=~2;
				time[1]=time0[1];
			}
			if(!time[2])
			{
				if((Status.UGen>100)||(Status.NDiz>1000))
				{
					Status.RegWrk &= ~2;
					Status.RegWrk &= ~4;
					Status.RegWrk &= ~1;
					Status.RegWrk |= 0x40;
				}
				else
				{
					if((Status.UGen<100) && (Status.NDiz<1000))
						PORT_ResetBits(MDR_PORTE, (PORT_Pin_6));
					
					Status.RegimTime |= 2;
					Status.RegimTime &= ~4;
					time[2] = time0[2];
					++cnt_pusk;
				}
			}
		}
		else
		{
			PORT_SetBits(MDR_PORTE,(PORT_Pin_6));
			Status.RegimTime=0;
			Status.Error|=1;
			Status.RegWrk=0;
		}
	}
	if(Status.RegWrk&8)
	{
	}
	if((Status.RegWrk&0x10))
	{ 
		Status.Error &= ~(1<<14);
		Status.Led2 &= ~1;
		Status.RegWrk &= ~0x6F;
		Status.RegimTime &= ~0x15F;
		Status.RegimTime |= 1<<5;
		PORT_SetBits(MDR_PORTE,(PORT_Pin_1));
		PORT_SetBits(MDR_PORTE,(PORT_Pin_6));
		PORT_SetBits(MDR_PORTE,(PORT_Pin_7));
		PORT_ResetBits(MDR_PORTA,(PORT_Pin_5));
		PORT_SetBits(MDR_PORTE, (PORT_Pin_3));
		PORT_SetBits(MDR_PORTA, (PORT_Pin_2));
		PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
		PORT_SetBits(MDR_PORTE, (PORT_Pin_2));
		PORT_SetBits(MDR_PORTA, (PORT_Pin_1));
		PORT_SetBits(MDR_PORTE, (PORT_Pin_0));
		Status.Load = 0;
		
		Status.Led1 &= ~(1<<7);
		
		if(!time[5])
		{
			if( Status.NDiz || (Status.UGen>100))
			{
				PORT_ResetBits(MDR_PORTA, (PORT_Pin_3));
				RegimError = 0;
				Status.Error |= 2;
				Status.RegimTime &= ~(1<<5);
				Status.RegWrk &= ~(1<<4);
			}
			else
			{
				PORT_SetBits(MDR_PORTA,(PORT_Pin_5));
				Status.Led2 &= ~2;
				Status.RegWrk = 0;
				Status.RegimTime = 0;
				
				RegimError=0;
			}
		}
		else
			Status.Led2|=2;
			
	}
	if(Status.RegWrk&0x20)//TM< +5C
	{
		if(!time[6])
		{
			PORT_SetBits(MDR_PORTE, (PORT_Pin_3));//PV
			PORT_SetBits(MDR_PORTA, (PORT_Pin_2));//MEN
			Status.RegimTime &= ~(1<<6);
			time[6] = time0[6];
			
			if(!(Status.RegWrk&2))
			{
				PORT_ResetBits(MDR_PORTE, (PORT_Pin_1));//TPN
				Status.RegimTime |= 1;
				Status.RegWrk |= 2;
				Status.Error |= 1<<14;
			}
		}
		if(Status.NDiz)
		{
			PORT_SetBits(MDR_PORTE, (PORT_Pin_6));//ST OFF
			Status.RegWrk &= ~2;
			Status.RegWrk &= ~4;
			Status.RegWrk &= ~0x20;
			Status.RegWrk |= 0x40;
		}
	}
	if(Status.RegWrk&0x40)
	{
		if(Status.TM>90)
		{
			PORT_ResetBits(MDR_PORTE, (PORT_Pin_2));
		}
		if(Status.TM<80)
		{
			PORT_SetBits(MDR_PORTE, (PORT_Pin_2));
		}
		if(!(Status.Error&0x1FC))
		{
			PORT_SetBits(MDR_PORTA, (PORT_Pin_3));
			PORT_ResetBits(MDR_PORTE, (PORT_Pin_0));
		}
		else
		{
			if(!Status.protOFF)
				Status.Load&=~1;
			
			PORT_SetBits(MDR_PORTE, (PORT_Pin_0));
		}
		
		Status.RegimTime|=1<<8;
		
		Status.Led2&=~1;
	}
	if(Status.RegWrk&0x80)
	{
		Status.Led2&=~1;
		Status.Led2|=2;
		
		PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
		PORT_SetBits(MDR_PORTA, (PORT_Pin_1));
		PORT_SetBits(MDR_PORTE, (PORT_Pin_0));
		PORT_ResetBits(MDR_PORTE, (PORT_Pin_2));
		
		if((!time[7])||(Status.TM<125))
		{
			Status.RegWrk=0x10;
		}
	}
}
//==================================================================================
void send_PKDU(void)
{
	uint8_t sum=0;

	sum=0;
	sum=Status.reg0+Status.reg1+(uint8_t)Status.Error+(uint8_t)(Status.Error>>8)+Status.RabReg0+(uint8_t)(Status.UGen)+(uint8_t)(Status.UGen>>8);
	sum+=(uint8_t)Status.IGen+(uint8_t)(Status.IGen>>8)+Status.DT+Status.DM+Status.TM+(uint8_t)Status.NDiz;
	sum+=(uint8_t)(Status.NDiz>>8)+Status.TBapEx+Status.TBapIn+Status.Led1+Status.Led2;
	sum+=0xAA;
	sum=255-sum;
	++sum;

	UART_ITConfig(MDR_UART1,UART_IT_RX, DISABLE);
	RS485_TX_ON;
	UART1_send_byte(0xAA);
	UART1_send_byte(Status.reg0);					
	UART1_send_byte(Status.reg1);					
	UART1_send_byte((uint8_t)Status.Error);			
	UART1_send_byte((uint8_t)(Status.Error>>8));	
	UART1_send_byte(Status.RabReg0);				
	UART1_send_byte((uint8_t)(Status.UGen));		
	UART1_send_byte((uint8_t)(Status.UGen>>8));		
	UART1_send_byte((uint8_t)Status.IGen);			
	UART1_send_byte((uint8_t)(Status.IGen>>8));		
	UART1_send_byte( Status.DT );					
	UART1_send_byte( Status.DM );					
	UART1_send_byte( Status.TM );					
	UART1_send_byte((uint8_t) Status.NDiz );		
	UART1_send_byte((uint8_t)( Status.NDiz>>8 )); 	
	UART1_send_byte( Status.TBapEx );				
	UART1_send_byte( Status.TBapIn );				
	UART1_send_byte( Status.Led1 );					
	UART1_send_byte( Status.Led2 );					
	__disable_irq();
	UART1_send_byte( sum );							
	__enable_irq();

	RS485_TX_OFF;

	UART_ClearITPendingBit( MDR_UART1,UART_IT_RX );
	UART_ITConfig( MDR_UART1,UART_IT_RX, ENABLE );

}

//==================================================================================
void POWER_detector_init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_POWER,ENABLE);
	POWER_DeInit();
	POWER_PVDlevelConfig(PWR_PVDlevel_3V0);
}
//==================================================================================
void calk_Tbap(void)
{
	uint8_t tmp=0;
		
#define V_25 106
#define Average_Slope 6
	tmp=ADC_Arr[8]>>4;	
	if(tmp>95)
		Status.TBapIn = (256*(uint16_t)tmp-24520)/100;
	else 
		Status.TBapIn = 0;
	
	tmp=ADC_Arr[5]>>4;		
	if(tmp<75)
	{
		Status.TBapEx=(8404-78*(uint16_t)tmp)/100;
	}
	else
	{
		if(tmp<154)
		{
		Status.TBapEx=(4910-32*(uint16_t)tmp)/100;
		}
		else
			Status.TBapEx=0;
	}
	
	if(Status.TBapEx>85)
	{
		RegimError|=((uint16_t)1<<9);
	}
	else
		RegimError&=~((uint16_t)1<<9);
}
//==================================================================================
void calk_Pm(void)
{
	static uint16_t cnt_msr=0;
	static uint32_t sum_Pmg=0;
	

	sum_Pmg+=ADC_Arr[4]>>4;
	
	if(cnt_msr>30)
	{
		cnt_msr=0;
		sum_Pmg>>=5;
		
		if(sum_Pmg>32)
			sum_Pmg-=31;
		else
			sum_Pmg=0;
		
		sum_Pmg*=8;
		sum_Pmg/=10;
		
		if(sum_Pmg>=99)
			Status.DM=99;
		else
			Status.DM=sum_Pmg;
		
		if((Status.NDiz>2000)&&(Status.DM<4))
		{
			RegimError |= 1<<4;
		}
		else
		{
			RegimError &= ~(1<<4);
		}
	}
	else
		++cnt_msr;

}
//==================================================================================
void calk_Tm(void)
{
	static uint16_t cnt_msr=0;
	static uint32_t sum_Tmg=0;
	
	sum_Tmg+=ADC_Arr[3]>>4;
	
	if(cnt_msr>62)
	{
		cnt_msr=0;
		sum_Tmg>>=6;
		
		if(sum_Tmg>115)
		{
			sum_Tmg*=215;
			sum_Tmg/=100;
			sum_Tmg-=248;
			sum_Tmg+=calib_data[0];
		}
		else
			sum_Tmg=0;
		
		Status.TM=sum_Tmg;
		
		if(Status.NDiz>2000)
		{
			if(Status.TM>140)
				{
					RegimError|=1<<6;
				}
			else		
				RegimError&=~(1<<6);
		}
	}
	else
		++cnt_msr;
}

void calk_Ugen(void)
{
	static uint8_t cnt_msr=0;
	static uint32_t sum_Ug=0;
	uint32_t tmp=0, Ug=0;
	
	tmp=ADC_Arr[7]>>2;
	Ug=(tmp*1000)/2290;
	sum_Ug+=Ug;
	
	if(cnt_msr>14)
	{
		cnt_msr=0;
		sum_Ug>>=4;

		sum_Ug=(sum_Ug*calib_data[3])/calib_data_nom[3];
		
		if(sum_Ug>999)
			Status.UGen=999;
		else
			Status.UGen=sum_Ug;
		
		if(((Status.UGen<250)||(Status.UGen>300))&&(Status.NDiz>2500))
			{
				RegimError|=1<<2;
			}
			else
				RegimError&=~(1<<2);
	}
	else
		++cnt_msr;
}

//==================================================================================
#ifdef trans_toka
void calk_Igen(void)
{
	static uint16_t cnt_msr=0;
	static uint32_t sum_Ig=0;
	uint16_t tmp_I=0;
	
	sum_Ig+=ADC_Arr[0]>>2;
	
	if(cnt_msr>99)
	{
		cnt_msr = 0;
		sum_Ig /= 97;
		if(sum_Ig>2)
		{
			tmp_I = sum_Ig*311;
			tmp_I -= 930;
			tmp_I /= 100;
		}
		else
			tmp_I=0;
		
		Status.IGen=tmp_I;	
	}
	else
	{
		++cnt_msr;
	}
	
	if(Status.RegWrk&0x40)
	{
		if((Status.IGen>280)&&(Status.IGen<=290))
		{
			RegimError|=1<<3;
		}
		else
		{
			RegimError&=~(1<<3);
		
			if(Status.IGen>290)
			{
				RegimError|=1<<5;
			}
			else
			{
				RegimError&=~(1<<5);
			}
		}
	}
}
#endif
//==================================================================================
void Read_KN(void)
{
	uint8_t reg=0;
	static uint8_t tmp_reg=0;
	
	if(PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_1))
	{
		reg|=1<<0;
	}
	if(PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_10))
	{
		reg|=1<<1;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_8))
	{
		reg|=1<<2;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_7))
	{
		reg|=1<<3;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_6))
	{
		reg|=1<<4;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_0))
	{
		reg|=1<<5;
	}
	if(!PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_2))
	{
		reg|=1<<6;
	}
	
	if(tmp_reg==reg)
	{
		Status.RabReg0=tmp_reg;
		
		if((Status.RabReg0>>5)&1)
		{
			Status.Led1|=1;
			Status.Error |=0x40;
			if((Status.RegWrk&0x40)||(Status.RegWrk&4))
				Status.RegWrk=0x10;
		}
		
		if((Status.RabReg0>>6)&1)
		{
			Status.Led1 |= 2;
			Status.Error |= 0x10;
			PORT_ResetBits(MDR_PORTA, (PORT_Pin_3));
			if((Status.RegWrk&0x40) || (Status.RegWrk&4))
			{
				Status.RegWrk=0x10;
			}
		}
		
		if((Status.RabReg0&1))
			RegimError|=(uint16_t)1<<7;
		else
			RegimError&=~(1<<7);
		
		if(Status.RabReg0&0x10)
			Status.RegWrk=0x10;
	}
	else
		tmp_reg=reg;
	
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

	for(cnt=0;cnt<16;++cnt)
	{
		if(!ErrTime[cnt])
		{
			if(!(Status.RegWrk&0x10))
			{
				Status.Error |=((uint16_t)1<<cnt);
			
				switch(cnt)
				{
					case 2:
						Status.Led2|=1<<3;
						if((!Status.protOFF)&&(Status.RegWrk&0x40))
							Status.RegWrk=0x10;
						break;
					case 3:
							Status.Led2|=1<<4;
							
							if(!Status.protOFF)
							{
								PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
								if(Status.TM<125)
									Status.RegWrk=0x10;
								else
								{
									Status.RegWrk&=~0x40;
									Status.RegWrk|=1<<7;
									Status.RegimTime|=1<<7;
								}	
							}
						break;
					case 4:
						Status.Led1|=1<<1;
							Status.RegWrk=0x10;	
						PORT_ResetBits(MDR_PORTA, (PORT_Pin_3));
						break;
					case 5:
							Status.Led2|=1<<4;
							
							if(!Status.protOFF)
							{
								PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
							}
						break;
					case 6:
						Status.Led1|=1;
						Status.RegWrk&=~0x40;
						Status.RegWrk|=1<<7;
						Status.RegimTime|=1<<7;
						break;
					case 7:
						Status.Led1|=1<<5;
						if(!Status.protOFF)
							Status.RegWrk=0x10;
						break;
					case 8:
						Status.Led1|=1<<3;
						break;
					case 9:
						Status.Led1|=1<<4;
						break;
					default: break;
				}
			}
		}
	}
}
//==================================================================================
void ControlKG(void)
{
	if(!PORT_ReadInputDataBit(MDR_PORTA, (PORT_Pin_4)))
	{
		Status.Led2|=1<<5;
	}
	else
		Status.Led2&=~(1<<5);
	
	if(((Status.Error&0x1FC)||(Status.TM<70))&&(!Status.protOFF))
	{
		PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
		Status.Load&=~1;
		if(Status.Error&0x1FC)
		{
			Status.Led1&=~(1<<7);
			PORT_SetBits(MDR_PORTA, (PORT_Pin_1));
		}
	}
	else
		if(Status.RegWrk&0x40)
		{
			if((Status.TM>70)||(Status.protOFF))
			{
				if(!time[8])
				{
					if(Status.Load&1)
					{
						PORT_ResetBits(MDR_PORTA, (PORT_Pin_4));
						PORT_SetBits(MDR_PORTA, (PORT_Pin_1));
						Status.Led1&=~(1<<7);
					}
					else
					{
						PORT_SetBits(MDR_PORTA, (PORT_Pin_4));
						PORT_ResetBits(MDR_PORTA, (PORT_Pin_1));
						Status.Led1|=1<<7;
					}
				}
			}
		}
}

//==================================================================================
void ControlLed(void)
{
	bool ti=0;
	
	Status.RegimTime|=0x8000;
	if(!time[15])	
	{
		time[15]=time0[15];

		if(!(Status.Error&0x28))
		{
		if((Status.IGen>280)&&(Status.IGen<291))
			ti=true;
		else
			Status.Led2&=~(1<<4);
		
		Status.Led2^=(uint8_t)((Status.Error&3)|(ti<<4));
		}
		else
			Status.Led2^=(uint8_t)((Status.Error&3));
					
		if((Status.NDiz>2000)&&(!(Status.Error&0x40)))
		{
			if((Status.TM>=135)&&(Status.TM<=140))
				Status.Led1^=1;
			else
				Status.Led1&=~1;
		}
	}
}

//==================================================================================
bool ReadPKDU(void)
{
	uint8_t sum=0,cnt=0;

	while(cnt<(RX_FRAME_SIZE))
	{
		sum+=ArrayRX_PKDU[cnt];
		++cnt;
	}
	sum+=0xAA;
	
	if(sum)
	{
		error_pkba |= 1;
		return false;
	}
	else	
	{
		uint8_t numb=(ArrayRX_PKDU[1]>>4)&0x0F;
		
		if((ArrayRX_PKDU[0]>>4)&1)
			ResetError();
		
		if((ArrayRX_PKDU[0]>>7)&1)
		{
			Status.Led2|=(1<<6);
			Status.protOFF=true;
		}
		else
		{
			Status.Led2&=~(1<<6);
			Status.protOFF=false;
		}
		
		if(((ArrayRX_PKDU[0]>>1)&1)||(Status.RabReg0&0x40))
		{
			if(	(Status.RegWrk&1)||
				(Status.RegWrk&0x40)||
				(Status.RegWrk&0x20)||
				(Status.RegWrk&0x80))
			{
				if(Status.TM<125)
				{
					Status.RegWrk|=(1<<4);
				}
				else
				{
					Status.RegWrk&=~0x40;
					Status.RegWrk|=1<<7;
					Status.RegimTime|=1<<7;
				}
			}
		}
		
		
		if(((ArrayRX_PKDU[0]&1)||(ArrayRX_PKDU[1]&2))&&(!(Status.RegWrk&0x50)))
			Status.RegWrk|=1;
		
		if((Status.RegWrk&0x40)&&((Status.TM>70)||(Status.protOFF)))
		{
			if(!(ArrayRX_PKDU[1]&1))
			{
				Status.Load=1;
			}
			else
			{
				if((ArrayRX_PKDU[0]&4)||(Status.RabReg0&0x4))
					Status.Load=1;
				else
					if((ArrayRX_PKDU[0]&8)||(Status.RabReg0&0x8))
						Status.Load=0;
			}
		}

		if((ArrayRX_PKDU[0]&0x40)&&(ArrayRX_PKDU[0]&0x10))
		if((!(Status.RabReg0&2))&&(numb==1))
		{
			Status.calibr_on=true;
			Status.Error |=1<<14;
		}
		
		if(((ArrayRX_PKDU[0]&0x20)||(ArrayRX_PKDU[0]&0x10))&& Status.calibr_on)
		{
			if(numb<ARR_CAL)
			{
				if((calib_data[numb]<calib_data_max[numb])&&(ArrayRX_PKDU[0]&0x20))
						++calib_data[numb];
					else
						if((calib_data[numb]>calib_data_min[numb])&&(ArrayRX_PKDU[0]&0x10))
							--calib_data[numb];
			}
			
			ArrayRX_PKDU[0]&=~0x30;
		}
		
		return true;
	}
}
//==================================================================================
void ChangeCtTime(void)
{
uint8_t cnt;
	#define CAL_TIME 30
static uint8_t	calib_time=CAL_TIME;
	
	for(cnt=0;cnt<16;++cnt)
		{
		if((Status.RegimTime>>cnt)&1)
			{
			if(time[cnt])
				--time[cnt];
			}
		else
			time[cnt]=time0[cnt];
		}
		
	if(Status.calibr_on)
	{
		--calib_time;
		if(!calib_time)
		{
			calib_time=CAL_TIME;
			Status.calibr_on=false;
			save_calib_data();
			Status.Error &=~(1<<14);
		}
	}

}
//==================================================================================
void ResetError(void)
{
	Status.Error&=0x10;
	RegimError=0;
	Status.Led1&=0x82;
	Status.Led2&=0x67;
}
//==================================================================================
void save_calib_data(void)
{
	uint32_t Wdata=0;
	
	__disable_irq();
	EEPROM_ErasePage(EEPROM_ADD,EEPROM_Info_Bank_Select);//page0 4K all eeprom

	Wdata=(((calib_data[3]<<24)&0xff000000))|(((calib_data[2]<<16)&0x00ff0000))|(((calib_data[1]<<8)&0x0000ff00))|(calib_data[0]&0xff);
	EEPROM_ProgramWord(EEPROM_ADD,EEPROM_Info_Bank_Select,Wdata);
	Wdata=((((calib_data[5]<<8)&0x0000ff00))|((calib_data[4]&0x000000ff)))&0x0000ffff;
	EEPROM_ProgramWord(EEPROM_ADD+4,EEPROM_Info_Bank_Select,Wdata);
	__enable_irq();
}
//==================================================================================
void restore_calib_data(void)
{
	uint8_t i=0;
	bool save_dat=false;
	
	for(i=0;i<ARR_CAL;++i)
		{
		calib_data[i]=EEPROM_ReadByte(EEPROM_ADD+i,EEPROM_Info_Bank_Select);
			if((calib_data[i]<calib_data_min[i])||(calib_data[i]>calib_data_max[i]))
			{
				calib_data[i]=calib_data_nom[i];
				save_dat=true;
			}
		}
		
	if(save_dat)
		save_calib_data();
	
}











