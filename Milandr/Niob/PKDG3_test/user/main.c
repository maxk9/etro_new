/*
*Программа тест миландр 
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_adc.h"

#include <stdbool.h>

#include "MDR32F9Qx_power.h"
#include <string.h>





#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTA, PORT_Pin_2)
#define RS485_TX_ON 	PORT_SetBits(MDR_PORTA, PORT_Pin_2)


const unsigned  char TabDecSeg[10] = { 0Xfc,0X60,0Xda,0Xf2,0X66,
                                       0Xb6,0Xbe,0Xe0,0Xfe,0Xf6 };


#define ARR_CNT_USART_TX	30

uint8_t Arr_send_uart[ ARR_CNT_USART_TX ];
//struct 
//{
//	
//}send_usart;

const char data_str[] = __DATE__;//mmm dd yyyy

uint16_t Segment[10];

volatile uint32_t delay_count = 0, 
				  Ctrl_Key = 0;



void Read_Key( void );
extern void vLink_SysTask( void *pvParameters );
extern void UART2_IRQHandler( void );	
extern void Timer1_IRQHandler( void );
void Indicator1( unsigned int R0 );
void Indicator2( unsigned int R0 );

/*
 * The task that handles the UDP stack.  All TCP/IP processing is performed in
 * this task.
 */
//extern uint16_t RegS;

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );


void UART2_send_byte( uint8_t byte );

//==================================================================================
//==================================================================================
//==================================================================================
void Timer1_IRQHandler( void )
{

	static uint8_t cnt_anod = 0;
	uint16_t RegSSP = 0;
	
	MDR_TIMER1-> CNT = 0x0000;
	MDR_TIMER1-> STATUS &= ~( 1 << 1 );
	NVIC_ClearPendingIRQ( Timer1_IRQn );

	if( cnt_anod<7 )
		++cnt_anod;
	 else
		cnt_anod = 0;
	
	RegSSP = Segment[cnt_anod]<<8;
	switch( cnt_anod )
	{
		 case 0:	RegSSP |= 0xfe;
			 break;
		 case 1:	RegSSP |= 0xfd;
			 break;
		 case 2:	RegSSP |= 0xfb;
			 break;
		 case 3:	RegSSP |= 0xf7;
			 break;
		 case 4:	RegSSP |= 0xef;
			 break;
		 case 5:	RegSSP |= 0xdf;
			 break;
		 case 6:	RegSSP |= 0xbf;
			 break;
		 case 7:	RegSSP |= 0x7f;
			 break;
		 default: break;
	}
	 
	MDR_PORTD -> RXTX |= 0x8; //EN
	SSP_SendData( MDR_SSP1, RegSSP );
	
	while( SSP_GetFlagStatus( MDR_SSP1, SSP_FLAG_BSY ) == SET );
	
	MDR_PORTD -> RXTX &= ~0x8;//dis

}

void Timer2_IRQHandler(void)
{
	unsigned char R0=0;
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
	
}


//==================================================================================
void delay_ms( uint16_t delay )
{
	delay_count=delay;
	while(delay_count);
}

//==================================================================================
 void Read_Key( void ) 
{ // Key handling is a continuous process and as such the task 
	// is implemented using an infinite loop (as most real time 
	// tasks are). 
	static uint16_t RegKn = 0, RegKnOld = 0;
	
		if(!(MDR_PORTC->RXTX & 0x10))
			RegKn |=1;
		else
			RegKn &=0xfffe;
		
		if(!(MDR_PORTC->RXTX & 0x20))
			RegKn |=2;
		else
			RegKn &=0xfffd;
		
		if(!(MDR_PORTC->RXTX & 0x40))
			RegKn |=4;
		else
			RegKn &=0xfffb;
		
		if(!(MDR_PORTC->RXTX & 0x80))
			RegKn |=8;
		else
			RegKn &=0xfff7;

		if(!(MDR_PORTC->RXTX & 0x100))
			RegKn |=0x10;
		else
			RegKn &=0xffef;
		
		if(!(MDR_PORTC->RXTX & 0x200))
			RegKn |=0x20;
		else
			RegKn &=0xffdf;
		
		if(!(MDR_PORTC->RXTX & 0x400))
			RegKn |=0x40;
		else
			RegKn &=0xffbf;
		
		if(!(MDR_PORTC->RXTX & 0x800))
			RegKn |=0x80;
		else
			RegKn &=0xff7f;

		if(!(MDR_PORTC->RXTX & 0x8))
			RegKn |=0x100;
		else
			RegKn &=0xfeff;

		if(!(MDR_PORTC->RXTX & 0x1000))
			RegKn |=0x200;
		else
			RegKn &=0xfdff;
		
		if(!(MDR_PORTC->RXTX & 0x2000))
			RegKn |=0x400;
		else
			RegKn &=0xfbff;
		
		if(!(MDR_PORTC->RXTX & 0x4000))
			RegKn |=0x800;
		else
			RegKn &=0xf7ff;
		
		if(!(MDR_PORTC->RXTX & 0x8000))
			RegKn |=0x1000;
		else
			RegKn &=0xefff;


		if(RegKnOld == RegKn)
		{
			Ctrl_Key = RegKn;
		}
		else
			RegKnOld = RegKn;
}
//==================================================================================



int main( void )
{
	uint32_t pausa = 0xFFFF;
	//bool cntl=0;
	//POWER_detector_init();
	
	while(--pausa);
	
	
	prvSetupHardware();
	Segment[0] = 0xFF;
	Segment[1] = 0xFF;
	Indicator1(123);
	Indicator2(456);
	
	while(1)
	{
		
	}
}
//==================================================================================
//==================================================================================
//==================================================================================
#define ALL_PORTS_CLK (	RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF )

void prvSetupHardware( void )
{
	PORT_InitTypeDef PORT_InitStructure;
	UART_InitTypeDef UART_InitStructure;
	SSP_InitTypeDef SPI_InitStructure;
	TIMER_CntInitTypeDef sTIM_CntInit;
//	TIMER_ChnInitTypeDef sTIM_ChnInit;
	
/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd( ALL_PORTS_CLK, ENABLE );
//CLK
/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig( RST_CLK_HSE_ON );
	
	while( RST_CLK_HSEstatus() == ERROR );

	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
	MDR_RST_CLK -> CPU_CLOCK |= 2;//CPU_C1 set HSE
	RST_CLK_HSIcmd( DISABLE );
			

//SPI
 	RST_CLK_PCLKcmd( RST_CLK_PCLK_SSP1, ENABLE );

	/* Reset all SSP settings */
	SSP_DeInit( MDR_SSP1 );	
	
	SSP_BRGInit( MDR_SSP1,SSP_HCLKdiv8 );
	

	/* SSP1 MASTER configuration ------------------------------------------------*/
	SSP_StructInit ( &SPI_InitStructure );
	
	SPI_InitStructure.SSP_SCR = 0x00;
	SPI_InitStructure.SSP_CPSDVSR = 10;
	SPI_InitStructure.SSP_Mode = SSP_ModeMaster;
	SPI_InitStructure.SSP_WordLength = SSP_WordLength16b;
	SPI_InitStructure.SSP_SPH = SSP_SPH_1Edge;
	SPI_InitStructure.SSP_SPO = SSP_SPO_Low;
	SPI_InitStructure.SSP_FRF = SSP_FRF_SPI_Motorola;
	SPI_InitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;

	SSP_Init( MDR_SSP1,&SPI_InitStructure );
	  /* Enable SSP1 */
	SSP_Cmd( MDR_SSP1, ENABLE );
	
	
//Timer1
	
	// TIMER1
	RST_CLK_PCLKcmd( RST_CLK_PCLK_TIMER1, ENABLE );
	/* Reset all TIMER1 settings */
	TIMER_DeInit( MDR_TIMER1 );

	TIMER_BRGInit( MDR_TIMER1, TIMER_HCLKdiv1 );

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				= 0;
	sTIM_CntInit.TIMER_Prescaler                = 150;//
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


	
	// TIMER2
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2,ENABLE);
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER2);

	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				= 0;
	sTIM_CntInit.TIMER_Prescaler                = 0xf;//
	sTIM_CntInit.TIMER_Period                   = 0xffff;
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit ( MDR_TIMER2,&sTIM_CntInit );

	NVIC_EnableIRQ( Timer2_IRQn );
	TIMER_ITConfig( MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE );
	
  
	/* TMR1 enable */
	TIMER_Cmd ( MDR_TIMER2, ENABLE );
//UART1
	
	/************************ UART1 Initialization *************************/
	PORT_StructInit( &PORT_InitStructure );//reset struct

	//PORT_InitStructure.PORT_Pin = PORT_Pin_5|PORT_Pin_6;//RX TX
	PORT_InitStructure.PORT_Pin = ( PORT_Pin_6 | PORT_Pin_7 );//RX TX
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init( MDR_PORTA, &PORT_InitStructure );

	PORT_InitStructure.PORT_Pin = PORT_Pin_2;//EN RS485
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_InitStructure.PORT_PD = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_Init( MDR_PORTA, &PORT_InitStructure );
	
	RS485_TX_OFF;
	
	
	RST_CLK_PCLKcmd( RST_CLK_PCLK_UART1, ENABLE );
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit( MDR_UART1, UART_HCLKdiv1 );
	UART_DeInit( MDR_UART1 );
	
	/* Disable interrupt on UART1 */
    NVIC_DisableIRQ( UART1_IRQn );
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 9600;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits1;
	UART_InitStructure.UART_Parity				= UART_Parity_Even;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init( MDR_UART1, &UART_InitStructure );

	NVIC_EnableIRQ( UART1_IRQn );
	UART_ITConfig( MDR_UART1, UART_IT_RX, ENABLE );
	/* Enables UART1 peripheral */
	UART_Cmd( MDR_UART1, ENABLE );


//UART2
	
	/************************ UART2 Initialization *************************/
	PORT_StructInit( &PORT_InitStructure );//reset struct

	PORT_InitStructure.PORT_Pin = ( PORT_Pin_0 | PORT_Pin_1 );//RX TX
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	
	PORT_Init( MDR_PORTD, &PORT_InitStructure );
	
	RST_CLK_PCLKcmd( RST_CLK_PCLK_UART2, ENABLE );
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit( MDR_UART2, UART_HCLKdiv1 );
	UART_DeInit( MDR_UART2 );
	
	/* Disable interrupt on UART2 */
    NVIC_DisableIRQ( UART2_IRQn );
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 9600;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits1;
	UART_InitStructure.UART_Parity				= UART_Parity_Even;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART2 parameters*/
	UART_Init( MDR_UART2, &UART_InitStructure );

	NVIC_EnableIRQ( UART2_IRQn );
	UART_ITConfig( MDR_UART2, UART_IT_RX, ENABLE );
	/* Enables UART2 peripheral */
	UART_Cmd( MDR_UART2, ENABLE );




//PORT
	
	PORT_StructInit( &PORT_InitStructure );//reset struct	
 /* Configure PORTA pins */
 
 	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_1 ); //PORT_Pin_6
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTA, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5  ); //PORT_Pin_6 - схем косяк
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTA, &PORT_InitStructure );

	PORT_StructInit( &PORT_InitStructure );//reset struct	
	
 /* Configure PORTB pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTB, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_7 );//TMR3_CH4
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTB, &PORT_InitStructure );

 /* Configure PORTC pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_2 ); //прг работа
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;

	PORT_Init( MDR_PORTC, &PORT_InitStructure );

PORT_StructInit( &PORT_InitStructure );//reset struct	

	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 ); //TWI
	//PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	//PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init( MDR_PORTC, &PORT_InitStructure );
	
 /* Configure PORTD pins */
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_2 ); //TMR3_CH2
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	
	PORT_Init( MDR_PORTD, &PORT_InitStructure );
	
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTD, &PORT_InitStructure );
	
 /* Configure PORTE pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_6 | PORT_Pin_7 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTE, &PORT_InitStructure );

 /* Configure PORTF pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_5 | PORT_Pin_6 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTF, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1  );//SPI
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init( MDR_PORTF, &PORT_InitStructure );
}

void Indicator1( unsigned int R0 )
 {
	 unsigned int R1;
	 unsigned int R2;
	 unsigned int R3;
	// static unsigned int R4;

	 R1 = R0;
	 if( R1 > 999 )
		R1 = 999;
	 R3 = 0;
	 while( R1 >= 100 )
		{
			R1 -= 100;
			++R3;
		}
	 R2 = 0;
	 while( R1 >= 10 )
		{
			R1 -= 10;
			++R2;
		}
	 Segment[7] = TabDecSeg[R3];
	 Segment[6] = TabDecSeg[R2];
	 Segment[5] = TabDecSeg[R1];
 }
 //=======================================================================================
void Indicator2( unsigned int R0 )
 {
	 unsigned int R1;
	 unsigned int R2;
	 unsigned int R3;
	// static unsigned int R4;

	R1=R0;
	if(R1>999)
		R1 = 999;
	R3=0;
	while(R1 >= 100)
		{
		R1 -= 100;
		++R3;
		}
	R2=0;
	while(R1 >= 10)
		{
		R1 -= 10;
		++R2;
		}
	Segment[4] = TabDecSeg[R3];
	Segment[3] = TabDecSeg[R2];
	Segment[2] = TabDecSeg[R1];
 }






