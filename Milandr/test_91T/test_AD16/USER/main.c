/*
*Программа AD16 KDG миландр 
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

#include "ks0108.h"
#include "leds.h"
#include "graphic.h"
#include "joystick.h"

#include "MDR32F9Qx_power.h"
#include <string.h>
//#include <stdlib.h>

#include "types.h"


// Kernel includes
#include "FreeRTOSConfig.h"



#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "timers.h"
#include "semphr.h"

//#include "death.h"


#define DEBUG_ON 1

#ifdef DEBUG_ON

#include <stdio.h>
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
   if (DEMCR & TRCENA) {

while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}


#endif

const char data_str[]=__DATE__;//mmm dd yyyy




#define LED1_OFF	PORT_ResetBits(MDR_PORTD, PORT_Pin_10)
#define LED1_ON		PORT_SetBits(MDR_PORTD, PORT_Pin_10)
#define LED1_TOGLE	MDR_PORTD->RXTX ^= PORT_Pin_10 	
#define LED2_TOGLE	MDR_PORTD->RXTX ^= PORT_Pin_13 

#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTE, PORT_Pin_14)
#define RS485_TX_ON		PORT_SetBits(MDR_PORTE, PORT_Pin_14)


#define ARR_RX_MHU	3
//size_t fre=0;
volatile uint8_t	RX_MHU[ARR_RX_MHU];

volatile uint16_t 	delay_count=0,
					sum_Nd = 0;

volatile bool		ADC_compl = false,
					RX_Ok = false,
					calibr_on = false,
					one_sec = false;
					
					

/*-----------------------------------------------------------*/
xSemaphoreHandle 	xTime_Err_Semaphore = NULL,
					xUSART_RX_Semaphore = NULL;
/* The queue used to send messages to the LCD task. */
QueueHandle_t xLCDQueue;
/*-----------------------------------------------------------*/


/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

/*
 * Configures the high frequency timers - those used to measure the timing
 * jitter while the real time kernel is executing.
 */
extern void vSetupHighFrequencyTimer( void );



void clear_Arr(u8 *arr,u8 cnt);
void UART1_send_byte(uint8_t byte);
void vSetErrorTask( void *pvParameters );
void vKeyHandlerTask( void *pvParameters );
void vSetRegWork( void *pvParameters );
void vLink_SysTask( void *pvParameters );
void vSetRegWork( void *pvParameters );
void vLCDTask( void *pvParameters );
//==================================================================================
//==================================================================================
//==================================================================================


//#define RX_FRAME_SIZE	3
void UART1_IRQHandler(void)
{
	uint8_t tmp = 0;
	static bool start = false;
	static uint8_t cnt_rx_byte = 0;
	static portBASE_TYPE xTaskWoken;
	
	tmp = UART_ReceiveData(MDR_UART1);
	
	if((tmp == 0xAA) && (!start))
	{
		start = true;
	}
	else
	{
		if(start)
		{
//			if(!RX_Ok)
			RX_MHU[cnt_rx_byte] = tmp; 
//			
			++cnt_rx_byte;
			if(cnt_rx_byte == ARR_RX_MHU)
			{
				xSemaphoreGiveFromISR(xUSART_RX_Semaphore, &xTaskWoken);
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
	static portBASE_TYPE xTaskWoken;
	
	MDR_TIMER1 -> CNT = 0x0000;
	MDR_TIMER1 -> STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
	 
	if(cnt_read > 46)
	{
		cnt_read = 0;
		sum_Nd = MDR_TIMER3 -> CNT;
		MDR_TIMER3 -> CNT = 0;
	}
	else
		++cnt_read;
	
	
	if(cnt_sec > 199)
	//if(cnt_sec > 99)
	{
		//ChangeCtTime();
		//ChangeCtError();
		xSemaphoreGiveFromISR(xTime_Err_Semaphore, &xTaskWoken);
		cnt_sec = 0;	
		one_sec = true;
	}
	else
		++cnt_sec;
	
	if(cnt_led > 30)
	{
		//ALARM_TOGLE;
		cnt_led = 0;
	}
	else
		++cnt_led;
	
	//Read_KN();
}
//==================================================================================
void Timer2_IRQHandler(void)//10ms
{	
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
}
//==================================================================================



int main(void)
{
	uint32_t pausa = 0xFFFF;
	static char strk1[mainMAX_MSG_LEN], strk2[mainMAX_MSG_LEN];
	
	xLCDMessage xMessage;
	

	while(--pausa);
	
	prvSetupHardware();
	
	//printf("ABDNMSMJHKC\n\r"); //debug out
	xLCDQueue = xQueueCreate( mainLCD_QUEUE_SIZE, sizeof( xLCDMessage ) );


	xTaskCreate( vLCDTask, "LCD", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL );
	xTaskCreate( vSetRegWork,  "SRegWrk", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
	xTaskCreate( vSetErrorTask,  "SetErr", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
	
	xUSART_RX_Semaphore = xSemaphoreCreateBinary();
	
	xTaskCreate( vLink_SysTask,  "LinkSys", (configMINIMAL_STACK_SIZE), NULL, 2,( xTaskHandle * ) xUSART_RX_Semaphore);
	xTaskCreate( vKeyHandlerTask,  "Key", (configMINIMAL_STACK_SIZE), NULL, 2,( xTaskHandle * ) NULL);


	sprintf(strk1,"Date: %s",__DATE__ );
	xMessage.pcMessage = strk1;
	xMessage.str = 0;
	/* Send the message to the LCD gatekeeper for display. */
	xQueueSend( xLCDQueue, &xMessage, portMAX_DELAY );
	
	sprintf(strk2,"Time: %s",__TIME__ );
	xMessage.pcMessage = strk2;
	xMessage.str = 1;
	/* Send the message to the LCD gatekeeper for display. */
	xQueueSend( xLCDQueue, &xMessage, portMAX_DELAY );
		
		
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
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF )

void prvSetupHardware( void )
{
	PORT_InitTypeDef PORT_InitStructure;
	UART_InitTypeDef UART_InitStructure;
	//SSP_InitTypeDef SPI_InitStructure;
	TIMER_CntInitTypeDef sTIM_CntInit;
//	TIMER_ChnInitTypeDef sTIM_ChnInit;
	

//CLK
/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while(RST_CLK_HSEstatus() == ERROR);

	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	MDR_RST_CLK -> CPU_CLOCK |= 2;//CPU_C1 set HSE
	RST_CLK_HSIcmd(DISABLE);
			
/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
	PORT_StructInit(&PORT_InitStructure);//reset struct	
/************************ LCD Initialization *************************/

  /* Configure PORTA pins for data transfer to/from LCD */
	PORT_InitStructure.PORT_Pin = LCD_DATA_BUS_8;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_OE = PORT_OE_IN;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);

	/* Configure PORTE pin4 and pin5 for LCD crystals control */
	PORT_InitStructure.PORT_Pin = KS0108_CS1 | KS0108_CS2 | KS0108_RS;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin = KS0108_EN | KS0108_RW | KS0108_RES;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	
	PORT_SetBits(MDR_PORTA, LCD_DATA_BUS_8);
	PORT_SetBits(MDR_PORTE, KS0108_CS1 | KS0108_CS2 | KS0108_RS);
	PORT_SetBits(MDR_PORTC, KS0108_EN | KS0108_RW | KS0108_RES);

//Timer1
	
	// TIMER1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1,ENABLE);
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER1);

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

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

	/* TIM2 configuration ------------------------------------------------*/
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
	
	/* TMR2 enable */
	TIMER_Cmd (MDR_TIMER2,ENABLE);
	
	
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
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_CH1;
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
	
	
	
/************************ UART1 Initialization *************************/
	PORT_StructInit(&PORT_InitStructure);//reset struct

	PORT_InitStructure.PORT_Pin = PORT_Pin_12|PORT_Pin_13;//RX TX
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin = PORT_Pin_14;//EN RS485
	PORT_InitStructure.PORT_OE =PORT_OE_OUT;
	PORT_InitStructure.PORT_PD = PORT_PD_OPEN;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	RS485_TX_OFF;
//UART1
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1,ENABLE);
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_DeInit(MDR_UART1);
	
	/* Disable interrupt on UART1 */
    NVIC_DisableIRQ(UART1_IRQn);
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 38400;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits1;//PKDU2 niobyi
	UART_InitStructure.UART_Parity				= UART_Parity_No;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init(MDR_UART1,&UART_InitStructure);

	NVIC_EnableIRQ(UART1_IRQn);
	UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);
	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART1,ENABLE);
	
	
//PORT
/************************ Joystick Initialization *************************/

  /* Configure PORTC pins 10..14 for input to handle joystick events */
	PORT_StructInit(&PORT_InitStructure);//reset struct
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 |
									PORT_Pin_13 | PORT_Pin_14 );
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

	PORT_Init( MDR_PORTC, &PORT_InitStructure );

/************************ LEDs Initialization *************************/
	PORT_StructInit( &PORT_InitStructure );//reset struct
  /* Configure PORTD pins 10..14 for output to switch LEDs on/off */
	PORT_InitStructure.PORT_Pin   = LEDs_PINs;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTD, &PORT_InitStructure );

  /* All LEDs switch off */
	PORT_ResetBits(MDR_PORTD, LEDs_PINs);
}



//==================================================================================

void vLCDTask( void *pvParameters )
{
	xLCDMessage xMessage;
	
	/* Initialise the LCD and display a startup message. */
	GLCD_Initialize();

	taskENTER_CRITICAL();
	GLCD_ClearScreen();
	taskEXIT_CRITICAL();
	
	for( ;; )
	{
		/* Wait for a message to arrive that requires displaying. */
		while( xQueueReceive( xLCDQueue, &xMessage, portMAX_DELAY ) != pdPASS );

		/* Display the message.  Print each message to a different position. */
		GLCD_PutString(xMessage);
	}
}
//==================================================================================
void vKeyHandlerTask( void *pvParameters )
{ // Key handling is a continuous process and as such the task 
	// is implemented using an infinite loop (as most real time 
	// tasks are). 
	static uint16_t RegKn = 0, RegKnOld = 0;
	char stat_buff[150];
	u8 i=0, j=0;
	
	for( ;; )
	{
		
#if(( configGENERATE_RUN_TIME_STATS == 1 ) && (DEBUG_ON == 1 ))
		if( one_sec )
		{
			i=0; 
			j=0;
			
			one_sec = false;
			
			for( i=0; i<150; i++ )
				stat_buff[i] = 0;
			
			//vTaskList(stat_buff);
			vTaskGetRunTimeStats( stat_buff );
			
			i = 99;
			while( !stat_buff[i] )
				i--;
			
			printf("%c[1;1f%c[J", 27, 27); //ClearScreen
			
			while( j<i )
			{
				putchar(stat_buff[j]);
				++j;
			}
		}
#endif		
		
		if(!(MDR_PORTC -> RXTX & 0x10))
			RegKn |= 1;
		else
			RegKn &= 0xfffe;
		
		if(!(MDR_PORTC -> RXTX & 0x20))
			RegKn |= 2;
		else
			RegKn &=0xfffd;
		
		if(!(MDR_PORTC -> RXTX & 0x40))
			RegKn |= 4;
		else
			RegKn &= 0xfffb;
		
		if(!(MDR_PORTC -> RXTX & 0x80))
			RegKn |= 8;
		else
			RegKn &= 0xfff7;

		if(!(MDR_PORTC -> RXTX & 0x100))
			RegKn |= 0x10;
		else
			RegKn &= 0xffef;
		
		if(!(MDR_PORTC->RXTX & 0x200))
			RegKn |= 0x20;
		else
			RegKn &= 0xffdf;
		
		if(!(MDR_PORTC -> RXTX & 0x400))
			RegKn |= 0x40;
		else
			RegKn &= 0xffbf;
		
		if(!(MDR_PORTC -> RXTX & 0x800))
			RegKn |= 0x80;
		else
			RegKn &= 0xff7f;

		if(!(MDR_PORTC -> RXTX & 0x8))
			RegKn |= 0x100;
		else
			RegKn &= 0xfeff;

		if(!(MDR_PORTC -> RXTX & 0x1000))
			RegKn |= 0x200;
		else
			RegKn &= 0xfdff;
		
		if(!(MDR_PORTC -> RXTX & 0x2000))
			RegKn |= 0x400;
		else
			RegKn &= 0xfbff;
		
		if(!(MDR_PORTC -> RXTX & 0x4000))
			RegKn |= 0x800;
		else
			RegKn &= 0xf7ff;
		
		if(!(MDR_PORTC -> RXTX & 0x8000))
			RegKn |= 0x1000;
		else
			RegKn &= 0xefff;


		if(RegKnOld == RegKn)
		{
			//RegS = RegKn;
			//PKDU_Status.Ctrl_BTN = RegS;
		}
		else
			RegKnOld = RegKn;
		
		vTaskDelay( 50 );
	}
}
//==================================================================================
void vSetErrorTask( void *pvParameters )
{
 	//uint8_t sum=0,cnt_link=0;
 	
 	/* Create the semaphore used by the ISR to wake this task. */
	vSemaphoreCreateBinary( xTime_Err_Semaphore );
 	
 	for( ;; )
 	{
		//if( xTime_Err_Semaphore != NULL )
		{
			xSemaphoreTake( xTime_Err_Semaphore, portMAX_DELAY );//s
			
		}
 	}
}
//==================================================================================
void vLink_SysTask( void *pvParameters )
{
 	uint8_t sum=0, i=0;
 	static char strk1[mainMAX_MSG_LEN];
	static uint8_t k=0;
	xLCDMessage xMessage;
 	/* Create the semaphore used by the ISR to wake this task. */
	
 	for( ;; )
 	{
		if( xSemaphoreTake( xUSART_RX_Semaphore, ( TickType_t )5 ) == pdTRUE )
        {
			
			//printf("%c[1;1f%c[J", 27, 27); //ClearScreen
			for(i=0; i < mainMAX_MSG_LEN; i++)
				strk1[i] = 0;
			
			sprintf(strk1,"0x%02X 0x%02X 0x%02X k=%d", RX_MHU[0], RX_MHU[1], RX_MHU[2], k);
			
			//printf(strk1);
			xMessage.pcMessage = strk1;
			xMessage.pos = 0;
			xMessage.str = 2;
			/* Send the message to the LCD gatekeeper for display. */
			xQueueSend( xLCDQueue, &xMessage, portMAX_DELAY );
			
			k =0;
			
			xSemaphoreGive( xUSART_RX_Semaphore );
		}
		else //time out
		{
			++k;
			//printf("%d",k);
			//printf("Err! no link!");
			xMessage.pcMessage = "Err! no link!";
			xMessage.pos = 0;
			xMessage.str = 2;
			/* Send the message to the LCD gatekeeper for display. */
			xQueueSend( xLCDQueue, &xMessage, portMAX_DELAY );
			
			sum = 3 + 4 + 0xAA;
			sum = 255 - sum;
			++sum;
			
			UART_ITConfig(MDR_UART1, UART_IT_RX, DISABLE);
			RS485_TX_ON;
			UART1_send_byte(0xAA);
			UART1_send_byte(3);
			UART1_send_byte(4);
			__disable_irq();
			UART1_send_byte(sum);
			RS485_TX_OFF;
			__enable_irq();
			
			UART_ITConfig(MDR_UART1, UART_IT_RX, ENABLE);
		}
	}
}

//==================================================================================
void vSetRegWork( void *pvParameters )
{
 	//uint8_t sum=0,cnt_link=0;
 	
 	/* Create the semaphore used by the ISR to wake this task. */
	//vSemaphoreCreateBinary( xTime_Err_Semaphore );
 	
 	for( ;; )
 	{
		vTaskDelay( 50 );
	}
}
//==================================================================================
void UART1_send_byte(uint8_t byte)
{
	UART_SendData(MDR_UART1,byte);
	//time_out_byte=0;
	//while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	//while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY) || !UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY))
	{
	}
	UART_ReceiveData (MDR_UART1);
	UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
}
//==================================================================================
	
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	printf("%c[1;1f%c[J", 27, 27); //ClearScreen
	printf("Stack over"); //
	for( ;; );
}	
//==================================================================================	
	
	
	
	
	
	
	
	
	
	
