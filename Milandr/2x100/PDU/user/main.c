/*
*Программа ПДУ миландр 
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_ebc.h"
#include "MDR32F9Qx_ssp.h"
#include <stdbool.h>

#include "MDR32F9Qx_power.h"
#include <string.h>
//#include <stdlib.h>

#include "types.h"

// Kernel includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "death.h"

#include "udp_frame.h"




const char data_str[]=__DATE__;//mmm dd yyyy




extern const char  font5x7[];
extern uint8_t uipIP_ADDR0,uipIP_ADDR1,uipIP_ADDR2,uipIP_ADDR3;
extern struct uip_eth_addr mac;

//SemaphoreHandle_t xPKBA_RX_Semaphore = NULL;


extern void vSmirnov_Task( void *pvParameters );
extern void vLink_SysTask( void *pvParameters );
extern void UART2_IRQHandler(void);	
extern void Timer1_IRQHandler(void);
/*
 * The task that handles the UDP stack.  All TCP/IP processing is performed in
 * this task.
 */
extern void vUDP_Task( void *pvParameters );

extern void clear_lcd( void );

extern uint32_t RegS,RegSOld,RegS1,RegKnOld, CtKn,RegSWork;

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

/*
 * Configures the high frequency timers - those used to measure the timing
 * jitter while the real time kernel is executing.
 */
extern void vSetupHighFrequencyTimer( void );

/*
 * Hook functions that can get called by the kernel.
 */
void vApplicationStackOverflowHook( TaskHandle_t *pxTask, signed char *pcTaskName );
void clear_Arr(u8 *arr,u8 cnt);
void UART2_send_byte(uint8_t byte);

//==================================================================================
//==================================================================================
//==================================================================================
//==================================================================================
// void vKeyHandlerTask( void *pvParameters ) 
//{ // Key handling is a continuous process and as such the task 
//	// is implemented using an infinite loop (as most real time 
//	// tasks are). 
//	static uint32_t RegKn = 0;//, RegKnOld = 0;
//	
//	for( ;; ) 
//	{ 
//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_6))
//			RegKn |=1;
//		else
//			RegKn &=0xFFfffe;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_7))
//			RegKn |=2;
//		else
//			RegKn &=0xFFfffd;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_8))
//			RegKn |=4;
//		else
//			RegKn &=0xFFfffb;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_9))
//			RegKn |=8;
//		else
//			RegKn &=0xFFfff7;

//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_10))
//			RegKn |=0x10;
//		else
//			RegKn &=0xFFffef;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_11))
//			RegKn |=0x20;
//		else
//			RegKn &=0xFFffdf;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_4))
//			RegKn |=0x40;
//		else
//			RegKn &=0xFFffbf;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_5))
//			RegKn |=0x80;
//		else
//			RegKn &=0xFFff7f;

//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_8))
//			RegKn |=0x100;
//		else
//			RegKn &=0xFFfeff;

//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_9))
//			RegKn |=0x200;
//		else
//			RegKn &=0xFFfdff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_10))
//			RegKn |= 0x400;
//		else
//			RegKn &= 0xFFfbff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_11))
//			RegKn |= 0x800;
//		else
//			RegKn &= 0xFFf7ff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_3))
//			RegKn |= 0x1000;
//		else
//			RegKn &= 0xFFefff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_13))
//			RegKn |= 0x2000;
//		else
//			RegKn &= 0xFFDfff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_12))
//			RegKn |= 0x4000;
//		else
//			RegKn &= 0xFFbfff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_15))
//			RegKn |= 0x8000;
//		else
//			RegKn &= 0xFF7fff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_14))
//			RegKn |= 0x10000;
//		else
//			RegKn &= 0xFeFfff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_5))
//			RegKn |= 0x20000;
//		else
//			RegKn &= 0xFdFfff;
//		
//		if(!PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_3))
//			RegKn |= 0x40000;
//		else
//			RegKn &= 0xFbFfff;
//		
////		if(RegKnOld == RegKn)
////		{
//			PKDU_Status.Ctrl_BTN = RegKn;
////		}
////		else
////			RegKnOld = RegKn;
//		
//		vTaskDelay( 50 );
//	} 
//}

 void vKeyHandlerTask( void *pvParameters ) 
{ // Key handling is a continuous process and as such the task 
	// is implemented using an infinite loop (as most real time 
	// tasks are). 
	static uint32_t RegKn = 0;//, RegKnOld = 0;
	
	for( ;; ) 
	{ 
		unsigned char R0;
		RegSOld=RegS;
		RegKnOld=RegKn;


		if(!(MDR_PORTC->RXTX & 0x10))
		RegKn |=1;
		else
		RegKn &=0xffffe;
		if(!(MDR_PORTC->RXTX & 0x20))
		RegKn |=2;
		else
		RegKn &=0xffffd;
		if(!(MDR_PORTD->RXTX & 0x8))
		RegKn |=4;
		else
		RegKn &=0xffffb;
		if(!(MDR_PORTD->RXTX & 0x20))
		RegKn |=0x8;
		else
		RegKn &=0xffff7;

		if(!(MDR_PORTC->RXTX & 0x100))
		RegKn |=0x10;
		else
		RegKn &=0xfffef;
		if(!(MDR_PORTC->RXTX & 0x200))
		RegKn |=0x20;
		else
		RegKn &=0xfffdf;
		if(!(MDR_PORTC->RXTX & 0x400))
		RegKn |=0x40;
		else
		RegKn &=0xffbf;
		if(!(MDR_PORTC->RXTX & 0x800))
		RegKn |=0x80;
		else
		RegKn &=0xfff7f;

		if(!(MDR_PORTC->RXTX & 0x8))
		RegKn |=0x100;
		else
		RegKn &=0xffeff;

		if(!(MDR_PORTC->RXTX & 0x1000))
		RegKn |=0x200;
		else
		RegKn &=0xfdff;
		if(!(MDR_PORTC->RXTX & 0x2000))
		RegKn |=0x400;
		else
		RegKn &=0xffbff;
		if(!(MDR_PORTC->RXTX & 0x4000))
		RegKn |=0x800;
		else
		RegKn &=0xff7ff;
		if(!(MDR_PORTC->RXTX & 0x8000))
		RegKn |=0x1000;
		else
		RegKn &=0xfefff;
		if(!(MDR_PORTD->RXTX & 0x40))
		RegKn |=0x2000;
		else
		RegKn &=0xfdfff;
		if(!(MDR_PORTD->RXTX & 0x80))
		RegKn |=0x4000;
		else
		RegKn &=0xfbfff;
		if(!(MDR_PORTD->RXTX & 0x100))
		RegKn |=0x8000;
		else
		RegKn &=0xf7fff;
		if(!(MDR_PORTD->RXTX & 0x200))
		RegKn |=0x10000;
		else
		RegKn &=0xeffff;
		if(!(MDR_PORTD->RXTX & 0x400))
		RegKn |=0x20000;
		else
		RegKn &=0xdffff;
		if(!(MDR_PORTD->RXTX & 0x800))
		RegKn |=0x40000;
		else
		RegKn &=0xbffff;
		if(!(MDR_PORTE->RXTX & 0x8000))
		RegKn |=0x80000;
		else
		RegKn &=0x7ffff;

		if(RegKnOld==RegKn)
		RegS=RegKn;


		for(R0=0;R0<=18;++R0)
		{
			if((RegS & (1<<R0))&&(!(RegSOld & (1<<R0))))
			{
				RegSWork |=(1<<R0);
				CtKn=30;
			}

		}

		if(!CtKn)
		RegSWork=0;
		
		vTaskDelay( 50 );
	} 
}

int main(void)
{
	uint32_t pausa = 0xFFFF;
	
	while(--pausa);
	
	
	prvSetupHardware();
	
	clear_lcd();
	
	xTaskCreate( vUDP_Task, "UDP", (configMINIMAL_STACK_SIZE*3), NULL, 2,( xTaskHandle * ) NULL );
//	xTaskCreate( vIndikatorTask,  "Indikator", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
	
//	xTaskCreate( vLink_SysTask,  "Link_Sys", (configMINIMAL_STACK_SIZE), NULL, 2,( xTaskHandle * ) NULL);
	xTaskCreate( vKeyHandlerTask,  "Key", (configMINIMAL_STACK_SIZE), NULL, 2,( xTaskHandle * ) NULL);
	xTaskCreate( vSmirnov_Task,  "Sm_TSK", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);

	/* The suicide tasks must be created last as they need to know how many
	tasks were running prior to their creation in order to ascertain whether
	or not the correct/expected number of tasks are running at any given time. */
//    vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

	/* Configure the high frequency interrupt used to measure the interrupt
	jitter time. */
//	vSetupHighFrequencyTimer();

	/* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	
	
	while(1);
}
//==================================================================================
//==================================================================================
//==================================================================================
#define ALL_PORTS_CLK (	RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

void prvSetupHardware( void )
{
	
	PORT_InitTypeDef PORT_InitStructure;
	UART_InitTypeDef UART_InitStructure;
	TIMER_CntInitTypeDef sTIM_CntInit;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	EBC_InitTypeDef EBC_InitStructure;
	SSP_InitTypeDef SPI_InitStructure;

	TIMER_ChnStructInit(&sTIM_ChnInit);
	
	
/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while(RST_CLK_HSEstatus() == ERROR);

	/* Configure CPU_PLL clock */
	RST_CLK_CPU_PLLconfig( RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul4 );//64mhz
	RST_CLK_CPU_PLLcmd( ENABLE );
	
	while( RST_CLK_CPU_PLLstatus() == ERROR );
	

	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
	
	RST_CLK_CPU_PLLuse(ENABLE);
	
	RST_CLK_HSIcmd(DISABLE);

	/* Enables the clock on EEPROM */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);

/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd( ALL_PORTS_CLK, ENABLE );
	
//EBC
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EBC, ENABLE);
	
	EBC_StructInit(&EBC_InitStructure);
	
	EBC_InitStructure.EBC_Mode = EBC_MODE_RAM;
	EBC_InitStructure.EBC_Cpol = EBC_CPOL_POSITIVE;
	//EBC_InitStructure.EBC_WaitState = EBC_WAIT_STATE_10HCLK;//EBC_WAIT_STATE_18HCLK
	EBC_InitStructure.EBC_WaitState = EBC_WAIT_STATE_18HCLK;

	EBC_Init(&EBC_InitStructure);

//SPI
	PORT_StructInit( &PORT_InitStructure );//reset struct

	PORT_InitStructure.PORT_Pin = ( PORT_Pin_13 | PORT_Pin_15 );//spi
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init( MDR_PORTB, &PORT_InitStructure );

 	RST_CLK_PCLKcmd( RST_CLK_PCLK_SSP1, ENABLE );

	/* Reset all SSP settings */
	SSP_DeInit( MDR_SSP1 );	
	
	SSP_BRGInit( MDR_SSP1, SSP_HCLKdiv8 );
	

	/* SSP1 MASTER configuration ------------------------------------------------*/
	SSP_StructInit (&SPI_InitStructure);
	
	SPI_InitStructure.SSP_SCR = 0x00;
	SPI_InitStructure.SSP_CPSDVSR = 10;
	SPI_InitStructure.SSP_Mode = SSP_ModeMaster;
	SPI_InitStructure.SSP_WordLength = SSP_WordLength16b;
	SPI_InitStructure.SSP_SPH = SSP_SPH_2Edge;
	SPI_InitStructure.SSP_SPO = SSP_SPO_Low;
	SPI_InitStructure.SSP_FRF = SSP_FRF_SPI_Motorola;
	SPI_InitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;

	SSP_Init( MDR_SSP1, &SPI_InitStructure );
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
	sTIM_CntInit.TIMER_Prescaler                = 1000;// CLK = TIMER_CLK/(TIMER_Prescaler + 1)
	sTIM_CntInit.TIMER_Period                   = 200;//3.125 ms
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit ( MDR_TIMER1, &sTIM_CntInit );

	NVIC_EnableIRQ( Timer1_IRQn );
	TIMER_ITConfig( MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE );
	
  
	/* TMR1 enable */
	TIMER_Cmd ( MDR_TIMER1, ENABLE );
	
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
	TIMER_CntInit (MDR_TIMER2,&sTIM_CntInit);

	NVIC_EnableIRQ(Timer2_IRQn);
	TIMER_ITConfig(MDR_TIMER2,TIMER_STATUS_CNT_ARR, ENABLE);
	
  
	/* TMR1 enable */
	TIMER_Cmd (MDR_TIMER2,ENABLE);
//UART2
	
	/************************ UART2 Initialization *************************/
	PORT_StructInit(&PORT_InitStructure);//reset struct

	//PORT_InitStructure.PORT_Pin = PORT_Pin_5|PORT_Pin_6;//RX TX
	PORT_InitStructure.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1);//RX TX
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MDR_PORTD, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin = PORT_Pin_2;//EN RS485
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_InitStructure.PORT_PD = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
	RS485_TX_OFF;
	
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2,ENABLE);
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART2, UART_HCLKdiv1);
	UART_DeInit(MDR_UART2);
	
	/* Disable interrupt on UART2 */
    NVIC_DisableIRQ(UART2_IRQn);
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 9600;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits1;
	UART_InitStructure.UART_Parity				= UART_Parity_Even;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART2 parameters*/
	UART_Init(MDR_UART2,&UART_InitStructure);

	NVIC_EnableIRQ(UART2_IRQn);
	UART_ITConfig(MDR_UART2,UART_IT_RX, ENABLE);
	/* Enables UART2 peripheral */
	UART_Cmd(MDR_UART2,ENABLE);
	
//PORT
	
	PORT_StructInit(&PORT_InitStructure);//reset struct	
 /* Configure PORTA pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_All);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_MAIN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);

	PORT_StructInit(&PORT_InitStructure);//reset struct	
	
 /* Configure PORTB pins */
//	PORT_InitStructure.PORT_Pin   = (PORT_Pin_12 | PORT_Pin_14);
//	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
//	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
//	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
//	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
//	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

//	PORT_Init(MDR_PORTB, &PORT_InitStructure);

 /* Configure PORTC pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1 | PORT_Pin_2);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_MAIN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_3 |PORT_Pin_4 |  \
									PORT_Pin_5 | 	\
									PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 |	\
									PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 |	\
									PORT_Pin_14 | PORT_Pin_15);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);

 /* Configure PORTD pins */
	PORT_InitStructure.PORT_Pin   = (	PORT_Pin_3 | PORT_Pin_5 | PORT_Pin_6 | 	\
										PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | 	\
										PORT_Pin_10 | PORT_Pin_11 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTD, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_4 | PORT_Pin_12  );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTD, &PORT_InitStructure );
	
 /* Configure PORTE pins */
 	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_15 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTE, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_12);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_MAIN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 \
									| PORT_Pin_4);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTE, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 \
									| PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_13 | PORT_Pin_14 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_OPEN;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTE, &PORT_InitStructure );

 /* Configure PORTF pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_All & (~(PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_15)));
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_MAIN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init( MDR_PORTF, &PORT_InitStructure );
}








