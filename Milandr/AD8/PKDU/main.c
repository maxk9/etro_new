/*
*Ïנמדנאללא ÏÄÊÓ ÀÄ8
מע24.06.14
ןנמרטעû 
0370514
0400614
0380514
0360614
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_ssp.h"
#include <stdbool.h>
#include "link_pult.h"



#define PORTD_PIN (GPIO_PIN(10)|GPIO_PIN(11)|GPIO_PIN(12)|GPIO_PIN(13)|GPIO_PIN(14))

#define EEPROM_ADD	(uint32_t)0x08000000

#define ALARM_ON		PORT_SetBits(MDR_PORTA, PORT_Pin_4)
#define ALARM_OFF		PORT_ResetBits(MDR_PORTA, PORT_Pin_4)

#define REG_EN		PORT_SetBits(MDR_PORTB, PORT_Pin_9)
#define REG_DIS		PORT_ResetBits(MDR_PORTB, PORT_Pin_9)

#define TIME_OUT_LINK_ST	100

RegS StatusPKBA;

bool 	RX_ok = false,
		TX_st = false;
		
#define RX_FRAME_SIZE	19

uint8_t ArrayRX_PKBA[RX_FRAME_SIZE];

uint8_t IndData[5];

const uint8_t Table[10] = {	0X3f,0X6,0X5b,0X4f,0X66,
							0X6d,0X7d,0X7,0X7f,0X6f };
const uint8_t TableZap[10] = {	0Xbf,0X86,0Xdb,0Xcf,0Xe6,
								0Xed,0Xfd,0X87,0Xff,0Xef };
const uint8_t TabSimvol0[6] = { 0x3f,0x71,0x39,0x3e,0x30,0x76 };
const uint8_t TabSimvol1[2] = { 0x63,0x73 };

volatile uint16_t Segment[7];


#define LINK_COUNT	10

uint16_t  maska_err=0;
volatile uint16_t cnt_start=0;

volatile uint16_t 	delay_count=0;

SSP_InitTypeDef SPI_InitStructure;
PORT_InitTypeDef PORT_InitStructure;
TIMER_CntInitTypeDef sTIM_CntInit;
TIMER_ChnInitTypeDef sTIM_ChnInit;
static UART_InitTypeDef UART_InitStructure;

struct
{
	uint8_t t1;
	uint8_t t2;
	uint8_t t3;
	uint8_t t4;
}time;

struct 
{
volatile bool error;
volatile	uint8_t byte1;
volatile	uint8_t byte2;
volatile	uint8_t SchowPar;
volatile 	uint16_t StatusKN;
volatile 	uint16_t Hour;
volatile 	uint8_t Min;
}PKDU;

//==================================================================================
void GPIO_init(void);
void TIM1_init(void);
void TIM2_init(void);
void TIM3_init(void);
void UART_init(void);
void PLL_init(void);
void WDT_init(void);
void EEPROM_init(void);
void read_Kn(void);
void ShowParam(void);
void LoadLED_Reg(uint8_t R1,uint8_t anod);
void link_PKBA(void);
void show_ver(void);
void DecSegm(uint16_t R0);
void link_PKBA(void);
void Set_Error(void);
void ControlZvonok(void);
void EEPROM_save_hour(uint16_t Wdata);
void write_hour(void);
//==================================================================================
void delay_ms(uint16_t delay)
{
	delay_count=delay;
	while(delay_count);
}
//==================================================================================
void Timer1_IRQHandler(void)
{
	MDR_TIMER1->CNT = 0x0000;
	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
	
	read_Kn();
}

void Timer2_IRQHandler(void)
{
	static uint8_t cnt_anod=0;
	
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
	
	if(cnt_anod<6)
		++cnt_anod;
	 else
		cnt_anod=0;
	 
	LoadLED_Reg(Segment[cnt_anod],cnt_anod);
	
}

void Timer3_IRQHandler(void)
{
 if(TIMER_GetITStatus(MDR_TIMER3,TIMER_STATUS_CNT_ARR) == SET)
	{
		TIMER_ClearITPendingBit(MDR_TIMER3,TIMER_STATUS_CNT_ARR);
	}
}

void UART1_IRQHandler(void)
{
	static uint8_t cnt_rx_byte=0;
	static bool start=false;
	uint8_t tmp=0; 
	
	
	if(UART_GetITStatus(MDR_UART1,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
		tmp=UART_ReceiveData(MDR_UART1);
		
		if((tmp==0xAA) && (!start))
		{
			start=true;
		}
		else
		{
			if(start)
			{
				if(!RX_ok)
					ArrayRX_PKBA[cnt_rx_byte]=tmp; 
					
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
	MDR_UART1->RSR_ECR = 0;	
}

void UART2_IRQHandler(void)
{
	if(UART_GetITStatus(MDR_UART2,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART2,UART_IT_RX);
	}	
}


void SysTick_Handler(void)
{
	static uint32_t link_tmr = 0,
					cnt_sec = 0;
	
	if(delay_count)
	{
		delay_count--;
	}
	
	if( link_tmr < TIME_OUT_LINK_ST )
		++link_tmr;
	else
	{
		link_tmr = 0;
		TX_st = true;
	}
	
	++time_out_byte;
	
	if( cnt_sec > 58000 )
	{
		cnt_sec = 0;
		++PKDU.Min;
	}
	else
		++cnt_sec;
}
//==================================================================================
//==================================================================================
//==================================================================================
int main()
{
	bool ver = 0;
	uint8_t cnt_link = 0;
	
	PLL_init();
	GPIO_init();
	TIM1_init();
	TIM2_init();
	TIM3_init();
	UART_init();
	WDT_init();
	EEPROM_init();
	
	SysTick_Config(SystemCoreClock/800);//~10 ms
	
	
	time.t1=5;
	time.t2=5;
	time.t3=5;
	time.t4=5;
	delay_ms(100);
	
	Segment[0]=0xFF;
	Segment[1]=0xFF;
	Segment[2]=0xFF;
	Segment[3]=0xFF;
	Segment[4]=0xFF;
	Segment[5]=0xFF;
	Segment[6]=0x7F;
	ALARM_ON;
	delay_ms(100);
	ALARM_OFF;
	delay_ms(1000);
	//
	//
	
  while(1)
	{
		if( TX_st )
		{
			TX_st = 0;
			
			if( cnt_link < LINK_COUNT )
			{
				link_PKBA();
				++cnt_link;
			}
			else
			{
				PKDU.error = true;
				cnt_link = 0;
			}
		}
		
		if( RX_ok && !PKDU.error )
		{
			uint8_t cnt_byte = 0, sum = 0;
			
			while( cnt_byte < RX_FRAME_SIZE )
			{
				sum += ArrayRX_PKBA[cnt_byte];
				++cnt_byte;
			}
			sum += 0xAA;
			
			if( !sum )
			{
				StatusPKBA.reg0 = ArrayRX_PKBA[0];
				StatusPKBA.reg1 = ArrayRX_PKBA[1];
				StatusPKBA.Error = (ArrayRX_PKBA[3]<<8) | ArrayRX_PKBA[2];
				StatusPKBA.RabReg0 = ArrayRX_PKBA[4];
				StatusPKBA.UGen = (ArrayRX_PKBA[6]<<8) | ArrayRX_PKBA[5];
				StatusPKBA.IGen = (ArrayRX_PKBA[8]<<8) | ArrayRX_PKBA[7];
				StatusPKBA.DT = ArrayRX_PKBA[9];
				StatusPKBA.DM = ArrayRX_PKBA[10];
				StatusPKBA.TM = ArrayRX_PKBA[11];
				StatusPKBA.NDiz = (ArrayRX_PKBA[13]<<8) | ArrayRX_PKBA[12];
				StatusPKBA.TBapEx = ArrayRX_PKBA[14];
				StatusPKBA.TBapIn = ArrayRX_PKBA[15];
				StatusPKBA.Led1 = ArrayRX_PKBA[16];
				StatusPKBA.Led2 = ArrayRX_PKBA[17];

				if( !ver )
				{
					ver = true;
					show_ver();
					delay_ms( 1500 );
					Segment[ 0 ] = 0;
					Segment[ 1 ] = 0;
					Segment[ 2 ] = 0;
					Segment[ 3 ] = 0;
					Segment[ 4 ] = 0;
				}
			cnt_link=0;
			}
			RX_ok=0;
		}
		
		__nop();
		
		Set_Error();
		
		if(!PKDU.error)
			ShowParam();
		
		if((PKDU.StatusKN>>4)&1)
		{
			maska_err = 0;
			StatusPKBA.Error=0;
			PKDU.error=false;
			cnt_link = 0;
		}
		
		if(((PKDU.StatusKN>>5)&1) && ((PKDU.StatusKN>>6)&1))
		{
			Segment[0] = 0xFF;
			Segment[1] = 0xFF;
			Segment[2] = 0xFF;
			Segment[3] = 0xFF;
			Segment[4] = 0xFF;
			Segment[5] = 0xFF;
			Segment[6] = 0x7F;
			
			while(((PKDU.StatusKN>>5)&1)&&((PKDU.StatusKN>>6)&1))
				IWDG_ReloadCounter();
		}
		
		ControlZvonok();
		
		write_hour();
		IWDG_ReloadCounter();
	}
}
//==================================================================================
//==================================================================================
//==================================================================================
void read_Kn(void)
{
	static uint16_t	tmp_reg=0;
	uint16_t Reg2=0;
		
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_0))
		Reg2|=(1<<0);
	
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_1))
		Reg2|=(1<<1);
	
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_2))
		Reg2|=(1<<2);
	
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_3))
		Reg2|=(1<<3);
	
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_4))
		Reg2|=(1<<4);
	
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_5))
		Reg2|=(1<<5);
	
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_6))
		Reg2|=(1<<6);
	
	if(!PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_7))
		Reg2|=(1<<7);
	
	if(!PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_5))
		Reg2|=(uint16_t)(1<<8);
	
	if(!PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_6))
		Reg2|=(uint16_t)(1<<9);
	
	if(tmp_reg==Reg2)
	{
		PKDU.StatusKN=tmp_reg;
	}
	else
		tmp_reg=Reg2;
}

//==================================================================================

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
					   RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
					   RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)
void GPIO_init(void)
{
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
	PORT_StructInit(&PORT_InitStructure);
 /* Configure PORTA pins */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin = (PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	
	/* Configure PORTB pin */

  	PORT_InitStructure.PORT_Pin   = (PORT_Pin_5);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	  /* Configure PORTC pin */

  	PORT_InitStructure.PORT_Pin   = (PORT_Pin_8|PORT_Pin_9|PORT_Pin_10);
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	/* Configure PORTD pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|	\
									PORT_Pin_3|PORT_Pin_4|PORT_Pin_5|	\
									PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_OE	= PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	
	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
  /* Configure PORTF pin */
  	PORT_InitStructure.PORT_Pin = (PORT_Pin_0|PORT_Pin_1);
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin = (PORT_Pin_2);
	PORT_InitStructure.PORT_OE	= PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_6);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	
	RS485_TX_OFF;

}
//==================================================================================
void TIM1_init(void)
{
	// TIMER1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1,ENABLE);
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER1);

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
	sTIM_CntInit.TIMER_Prescaler                = 7999;
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
	// TIMER2
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2,ENABLE);
	/* Reset all TIMER2 settings */
	TIMER_DeInit(MDR_TIMER2);

	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	/* TIM2 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
	sTIM_CntInit.TIMER_Prescaler                = 194;
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
	
  
	/* TMR2 enable */
	TIMER_Cmd (MDR_TIMER2,ENABLE);
}
//==================================================================================
void TIM3_init(void)
{
	// TIMER3
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER3,ENABLE);
	/* Reset all TIMER3 settings */
  TIMER_DeInit(MDR_TIMER3);

  TIMER_BRGInit(MDR_TIMER3,TIMER_HCLKdiv1);

  /* TIM3 configuration ------------------------------------------------*/
  /* Initializes the TIMERx Counter ------------------------------------*/
  sTIM_CntInit.TIMER_Prescaler                = 0x10;
  sTIM_CntInit.TIMER_Period                   = 0x200;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_ETR;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TIMER3,&sTIM_CntInit);
  
	NVIC_EnableIRQ(Timer3_IRQn);
	TIMER_ITConfig(MDR_TIMER3,TIMER_STATUS_CNT_ARR, ENABLE);
    /* Enable TIMER3 */
  TIMER_Cmd(MDR_TIMER3,ENABLE);
  
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
	RST_CLK_PCLKcmd( RST_CLK_PCLK_UART1, ENABLE );
	UART_BRGInit( MDR_UART1, UART_HCLKdiv1 );
	UART_DeInit( MDR_UART1 );
	NVIC_DisableIRQ( UART1_IRQn );
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate		= 9600;
	UART_InitStructure.UART_WordLength		= UART_WordLength8b;
	UART_InitStructure.UART_StopBits		= UART_StopBits2;
	UART_InitStructure.UART_Parity			= UART_Parity_No;
	UART_InitStructure.UART_FIFOMode		= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	UART_Init( MDR_UART1, &UART_InitStructure );

	NVIC_EnableIRQ( UART1_IRQn );
	UART_ITConfig( MDR_UART1, UART_IT_RX, ENABLE );
	UART_Cmd( MDR_UART1,ENABLE );
}

void WDT_init(void)
{
	IWDG_WriteAccessEnable();
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(0xFFF);
	IWDG_Enable();
	IWDG_ReloadCounter();
}
//==================================================================================

void EEPROM_init(void)
{	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	PKDU.Hour = EEPROM_ReadHalfWord(EEPROM_ADD,EEPROM_Info_Bank_Select);
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
void write_hour(void)
{
	
	if(PKDU.Min>58)
	{
		PKDU.Min=0;
		
		if(PKDU.Hour>9999)
			PKDU.Hour=0;
		else
			++PKDU.Hour;
		
		EEPROM_save_hour(PKDU.Hour);
	}
	
	if(PKDU.SchowPar==6)
	{
		if(!((PKDU.StatusKN>>8)&1)&&
			((PKDU.StatusKN>>2)&1)&&
			((PKDU.StatusKN>>3)&1))
		{
			PKDU.Hour=0;
			EEPROM_save_hour(0);
			ShowParam();
		}
		while(!((PKDU.StatusKN>>8)&1)&&
			((PKDU.StatusKN>>2)&1)&&
			((PKDU.StatusKN>>3)&1))
		{
			IWDG_ReloadCounter();
		}			
	}
}
//==================================================================================
void link_PKBA(void)
{
	uint8_t sum=0;
	PKDU.byte1=(PKDU.StatusKN)&0xFF;
	PKDU.byte2=((PKDU.StatusKN>>8)&3)|((PKDU.SchowPar<<4)&0xF0);
	sum=PKDU.byte1+PKDU.byte2+0xAA;
	sum=255-sum;
	++sum;	

	UART_ITConfig(MDR_UART1,UART_IT_RX, DISABLE);
	RS485_TX_ON;
	UART1_send_byte(0xAA);
	UART1_send_byte(PKDU.byte1);
	UART1_send_byte(PKDU.byte2);
	__disable_irq();
	UART1_send_byte(sum);
	RS485_TX_OFF;
	__enable_irq();

	UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);
}
//==================================================================================
void LoadLED_Reg(uint8_t R1,uint8_t anod)
{
	uint8_t R0;
	
	REG_EN;
	
	for(R0=0;R0<8;++R0)
	{
		if(R1 & 1)
			PORT_SetBits(MDR_PORTB, PORT_Pin_10);
		else
			PORT_ResetBits(MDR_PORTB, PORT_Pin_10);
		
		R1 >>= 1;
		
		PORT_ResetBits(MDR_PORTB, PORT_Pin_8);
		__nop();
		__nop();
		PORT_SetBits(MDR_PORTB, PORT_Pin_8);
	}
	
	R1=1<<anod;
	
	for(R0=0;R0<8;++R0)
	{
		if(R1 & 1)
			PORT_ResetBits(MDR_PORTB, PORT_Pin_10);
		else
			PORT_SetBits(MDR_PORTB, PORT_Pin_10);
		
		R1 >>= 1;
		
		PORT_ResetBits(MDR_PORTB, PORT_Pin_8);
		__nop();
		__nop();
		PORT_SetBits(MDR_PORTB, PORT_Pin_8);
	}
	
	REG_DIS;
}


//==================================================================================
void ControlZvonok(void)
{
	uint16_t tmp=0;
	
	tmp=((uint16_t)StatusPKBA.Error<<1)|PKDU.error;
	
	if((PKDU.StatusKN>>5)&1)
	{
		maska_err=tmp;
	}
	
	if(tmp>maska_err)
		ALARM_ON;
	else
		ALARM_OFF;
}
//==================================================================================
void show_ver(void)
{
	DecSegm(StatusPKBA.reg1);
	Segment[0]=Table[IndData[3]];
	Segment[1]=Table[IndData[4]];
	Segment[2]=0x52;
	DecSegm(StatusPKBA.reg0);
	Segment[3]=Table[IndData[3]];
	Segment[4]=Table[IndData[4]];
}
//==================================================================================
void DecSegm(uint16_t R0)
{
 	uint16_t R1=0;

	R1=R0/10000;
	IndData[0]=R1;
	R1=R0%10000;
	R0=R1/1000;
	IndData[1]=R0;
	R0=R1%1000;
	R1=R0/100;
	IndData[2]=R1;
	R1=R0%100;
	R0=R1/10;
	IndData[3]=R0;
	R1=R1%10;
	IndData[4]=R1;	
	
}
//==================================================================================
void Set_Error(void)
{
	
	if(PKDU.error)
	{
		Segment[0]=0x40;
		Segment[1]=0x7d;
		Segment[2]=0x77;
		Segment[3]=0x37;
		Segment[4]=0x40;
		Segment[5]=0x40;
		Segment[6]=0;
	}

}
//==================================================================================
void ShowParam(void)
{
	static bool tmp_KN=false;
	uint16_t tmp=0;
	
	if(!tmp_KN)
	{
		if((PKDU.StatusKN>>6)&1)
		{
			tmp_KN=true;
		
			if(PKDU.SchowPar<6)
				++PKDU.SchowPar;
			else
				PKDU.SchowPar=0;
		}
	}
	else 
		if(!((PKDU.StatusKN>>6)&1))
			tmp_KN=false;
	
	switch(PKDU.SchowPar)
	{
		case 0:
			tmp=StatusPKBA.TM;
			if(tmp>29)
				{
					tmp-=30;
					DecSegm(tmp);
					Segment[2]=Table[IndData[2]];
				}
			else
				{
					tmp=30-tmp;
					DecSegm(tmp);
					Segment[2]=0x40;
				}
			Segment[0]=TabSimvol0[0];
			Segment[1]=TabSimvol1[0];
				
			Segment[3]=Table[IndData[3]];
			Segment[4]=Table[IndData[4]];
			break;
		case 1:
			DecSegm(StatusPKBA.DM);
			Segment[0]=TabSimvol0[0];
			Segment[1]=TabSimvol1[1];
			
			Segment[2]=0;
			Segment[3]=TableZap[IndData[3]];
			Segment[4]=Table[IndData[4]];
			break;
		case 2:
			DecSegm(StatusPKBA.TBapEx);
			Segment[0]=TabSimvol0[2];
			Segment[1]=TabSimvol1[0];
		
			Segment[2]=Table[IndData[2]];
			Segment[3]=Table[IndData[3]];
			Segment[4]=Table[IndData[4]];
			break;
		case 3:
			DecSegm(StatusPKBA.UGen);
			Segment[0]=TabSimvol0[3];
			Segment[1]=0;
			
			Segment[2]=Table[IndData[2]];
			Segment[3]=TableZap[IndData[3]];
			Segment[4]=Table[IndData[4]];
			break;
		case 4:
			DecSegm(StatusPKBA.IGen);
			Segment[0]=TabSimvol0[4];
			Segment[1]=0;
		
			Segment[2]=Table[IndData[2]];
			Segment[3]=Table[IndData[3]];
			Segment[4]=Table[IndData[4]];
			break;
		case 5:
			DecSegm(StatusPKBA.NDiz);
			Segment[0]=TabSimvol0[1];
		
			Segment[1]=Table[IndData[1]];
			Segment[2]=Table[IndData[2]];
			Segment[3]=Table[IndData[3]];
			Segment[4]=Table[IndData[4]];
			break;
		case 6:
			DecSegm(PKDU.Hour);
			Segment[0]=TabSimvol0[5];
		
			Segment[1]=Table[IndData[1]];
			Segment[2]=Table[IndData[2]];
			Segment[3]=Table[IndData[3]];
			Segment[4]=Table[IndData[4]];
			break;

		default: break;
	}
	Segment[5]=StatusPKBA.Led1;
	Segment[6]=StatusPKBA.Led2;
}
//==================================================================================
