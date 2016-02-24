/*
*Программа медиаконвертера миландр 
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_eeprom.h"
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


#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTE, PORT_Pin_3)
#define RS485_TX_ON		PORT_SetBits(MDR_PORTE, PORT_Pin_3)



const char data_str[]=__DATE__;//mmm dd yyyy

/* The time between cycles of the 'check' functionality (defined within the
tick hook. */
#define mainCHECK_DELAY						( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* Size of the stack allocated to the uIP task. */
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 3 )


/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY )




//volatile RegS StatusPKDU= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

volatile uint16_t delay_count = 0;

#define LINK_COUNT	5	//число попыток связи

#define RX_FRAME_SIZE	3

uint8_t ArrayRX_PKBA[RX_FRAME_SIZE];

volatile bool 	ADC_compl = false,
				RX_ok = false,
				TX_st = false;

extern unsigned char uipIP_ADDR0,uipIP_ADDR1,uipIP_ADDR2,uipIP_ADDR3;
extern struct uip_eth_addr mac;


SemaphoreHandle_t xUSART_RX_Semaphore = NULL;
QueueHandle_t xUSART_Queue;

/*
 * The task that handles the uIP stack.  All TCP/IP processing is performed in
 * this task.
 */
extern void vuIP_Task( void *pvParameters );


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
void UART1_send_byte(uint8_t byte);

//==================================================================================
//==================================================================================
//==================================================================================
void delay_ms(uint16_t delay)
{
	delay_count=delay;
	while(delay_count);
}

uint8_t cnt_rx_byte = 0;
bool start = false;// ld=0;
uint8_t tmp = 0; 

void UART1_IRQHandler(void)
{
	static u8 cntb = 0;
	 BaseType_t xTaskWoken;

	
	if(UART_GetITStatus(MDR_UART1,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
		tmp = UART_ReceiveData(MDR_UART1);
//		RS485_TX_ON;
//		UART1_send_byte(tmp);
//		RS485_TX_OFF;
		
#if( DEBUG_ON == 1 )	
//if(cntb > 10 )		
//{
//	printf("%c[1;1f%c[J", 27, 27); //ClearScreen
//	cntb = 0;
//}
//else 
//	++cntb;

//		printf("%d 0x%X\n",cntb, tmp); 
#endif
		if((tmp == 0xAA) && (!start))//start byte
		{
			start = true;
		}
		else
		{
			if(start)
			{
				if(!RX_ok)//предыдущие данные обработаны
					ArrayRX_PKBA[cnt_rx_byte] = tmp; 
					
				++cnt_rx_byte;
				if(cnt_rx_byte == RX_FRAME_SIZE)
				{	
					start = false;
					cnt_rx_byte = 0;
					xSemaphoreGiveFromISR( xUSART_RX_Semaphore, &xTaskWoken );
					//xQueueSendFromISR( xUSART_Queue, &ArrayRX_PKBA, xTaskWoken );
				}
			}
		}
	}
	MDR_UART1->RSR_ECR = 0;	
	    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is 
    port specific. */
   portYIELD_FROM_ISR( NULL );
}
//==================================================================================
 void vKeyHandlerTask( void *pvParameters ) 
{ // Key handling is a continuous process and as such the task 
	// is implemented using an infinite loop (as most real time 
	// tasks are). 
	for( ;; ) 
	{

		vTaskDelay( 1000 );
		//LED2_TOGLE;
	//[Suspend waiting for a key press]
	//[Process the key press] 
	}
}

//==================================================================================
#if( DEBUG_ON == 1 )

void vDebugTask( void *pvParameters )
{ 
	char stat_buff[100];
	u8 	i = 0, j = 0;

	for( ;; )
	{
		
#if( configGENERATE_RUN_TIME_STATS == 1 )

			for( i=0; i<100; i++ )
				stat_buff[i] = 0;
			
			vTaskList(stat_buff);
			//vTaskGetRunTimeStats( stat_buff );
			
			i = 99;
			while(!stat_buff[i])
				i--;
			
			printf("%c[1;1f%c[J", 27, 27); //ClearScreen
			
			j=0;
			while( j<i )
			{
				putchar( stat_buff[j] );
				++j;
			}
		
#endif	
		vTaskDelay( 1000 );
	} 
}
#endif
//==================================================================================
 void vLinkPKBATask( void *pvParameters ) 
{ 
	uint8_t sum = 0, cnt_byte = 0;
	//LED1_TOGLE;
	/* Create the semaphore used by the ISR to wake this task. */
	
	//xUSART_Queue = xQueueCreate( RX_FRAME_SIZE, sizeof( ArrayRX_PKBA ) );
	vSemaphoreCreateBinary( xUSART_RX_Semaphore );
	
	
	for( ;; ) 
	{
		
		while( xSemaphoreTake( xUSART_RX_Semaphore, portMAX_DELAY ) != pdPASS );

		printf("%c[1;1f%c[J", 27, 27); //ClearScreen
		printf("0x%X 0x%X 0x%X",ArrayRX_PKBA[0], ArrayRX_PKBA[1], ArrayRX_PKBA[2]);
		
		cnt_byte = 0;
		sum = 0;
		while(cnt_byte < RX_FRAME_SIZE)
		{
			sum += ArrayRX_PKBA[cnt_byte];
			++cnt_byte;
		}
		sum += 0xAA;
		
		if(!sum)
		{
			uint16_t tmp = 0;
		//link_PKBA(StatusPKBA);
			
			//StatusPKDU.IGen = (ArrayRX_PKBA[8]<<8) | ArrayRX_PKBA[7];

			//tmp = ArrayRX_PKBA[11];
			
			if(tmp>29)
			{
				tmp -= 30;
				//StatusPKDU.TM = tmp;
			}
			else
			{
				tmp = 30 - tmp;
				//StatusPKDU.TM = tmp;
			}


			sum = 1+2+0xAA;
			sum = 255-sum;
			++sum;	
//			sprintf(strlen,"%02X %02X %02X %02X",0xAA,StatusPKDU.reg0,StatusPKDU.reg1,sum);
// 			GLCD_GoTo(0,1);
// 			GLCD_WriteString(strlen);
			//NVIC_DisableIRQ(UART1_IRQn);
			UART_ITConfig(MDR_UART1,UART_IT_RX, DISABLE);
			RS485_TX_ON;
			UART1_send_byte(0xAA);
			UART1_send_byte(1);
			UART1_send_byte(2);
			__disable_irq();
			UART1_send_byte(sum);
			RS485_TX_OFF;
			__enable_irq();
		

			UART_ITConfig(MDR_UART1, UART_IT_RX, ENABLE);
		}
		else 
		{
		//	StatusPKDU.Error |= 1;
		}	
	}
}


int main(void)
{
	uint32_t pausa = 0xFFFF;
	
	while( --pausa );

	prvSetupHardware();
	
	
	//xTaskCreate( vuIP_Task, "uIP", mainBASIC_WEB_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL );
 	xTaskCreate( vLinkPKBATask,  "LinkPKBA", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
	xTaskCreate( vKeyHandlerTask,  "KeyTSK", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);

#if(DEBUG_ON)
	xTaskCreate( vDebugTask,  "DebugTask", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
#endif
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
	SSP_InitTypeDef SPI_InitStructure;
	PORT_InitTypeDef PORT_InitStructure;
	UART_InitTypeDef UART_InitStructure;

//CLK
/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig( RST_CLK_HSE_ON );
	
	while( RST_CLK_HSEstatus() == ERROR );
	
	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
	MDR_RST_CLK -> CPU_CLOCK |= 2;//CPU_C1 set HSE
	RST_CLK_HSIcmd( DISABLE );

/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd( ALL_PORTS_CLK, ENABLE );
	
//UART1
	
	/************************ UART1 Initialization *************************/
	PORT_StructInit(&PORT_InitStructure);//reset struct

	PORT_InitStructure.PORT_Pin = PORT_Pin_5|PORT_Pin_6;//RX TX
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MDR_PORTB, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin = PORT_Pin_3;//EN RS485
	PORT_InitStructure.PORT_OE =PORT_OE_OUT;
	PORT_InitStructure.PORT_PD = PORT_PD_OPEN;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	
	RS485_TX_OFF;
	
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1,ENABLE);
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_DeInit(MDR_UART1);
	
	/* Disable interrupt on UART1 */
    NVIC_DisableIRQ(UART1_IRQn);
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 38400;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits1;
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
	
	PORT_StructInit(&PORT_InitStructure);//reset struct	
 /* Configure PORTA pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1 | PORT_Pin_2 |	\
									PORT_Pin_3 | PORT_Pin_5);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTA, (PORT_Pin_1 |PORT_Pin_5 | PORT_Pin_2), Bit_SET);//DIR - out nwe - set DCS - set
	PORT_WriteBit(MDR_PORTA, (PORT_Pin_3), Bit_RESET);//
	

 /* Configure PORTF pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	//PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
//SPI

	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP1,ENABLE);
	/* Reset all SSP settings */
	SSP_DeInit(MDR_SSP1);	
	
	SSP_BRGInit(MDR_SSP1,SSP_HCLKdiv8);
	
	/* SSP1 MASTER configuration ------------------------------------------------*/
	SSP_StructInit (&SPI_InitStructure);
	
	SPI_InitStructure.SSP_SCR = 0x01;
	SPI_InitStructure.SSP_CPSDVSR = 6;
	SPI_InitStructure.SSP_Mode = SSP_ModeMaster;
	SPI_InitStructure.SSP_WordLength = SSP_WordLength8b;
	SPI_InitStructure.SSP_SPH = SSP_SPH_2Edge;
	SPI_InitStructure.SSP_SPO = SSP_SPO_Low;
	SPI_InitStructure.SSP_FRF = SSP_FRF_SPI_Motorola;
	SPI_InitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;

	SSP_Init(MDR_SSP1, &SPI_InitStructure);
	  /* Enable SSP1 */
	SSP_Cmd(MDR_SSP1, ENABLE);
}



void UART1_send_byte(uint8_t byte)
{
	UART_SendData(MDR_UART1,byte);
	//time_out_byte=0;
	//while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY) || !UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	{
// 		if(time_out_byte>3)
// 			break;
	}
	UART_ReceiveData (MDR_UART1);
	UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
// 	while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY))
// 	{
// 		UART_ReceiveData (MDR_UART1);
// 	}
}





