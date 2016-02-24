/*
*Программа демо платы миландр 
*/


//#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_ebc.h"
#include <stdbool.h>
#include "ks0108.h"
#include "leds.h"
#include "graphic.h"
#include "joystick.h"
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

//#include "5600_vg1.h"

// Kernel includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "death.h"


#define PKDU2_NIOB 1


#ifndef PKDU2_NIOB
	#define PKBA_AD8	1
#endif



#define LED1_OFF	PORT_ResetBits(MDR_PORTD, PORT_Pin_10)
#define LED1_ON		PORT_SetBits(MDR_PORTD, PORT_Pin_10)
#define LED1_TOGLE	MDR_PORTD->RXTX ^= PORT_Pin_10 	
#define LED2_TOGLE	MDR_PORTD->RXTX ^= PORT_Pin_13 

#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTE, PORT_Pin_14)
#define RS485_TX_ON		PORT_SetBits(MDR_PORTE, PORT_Pin_14)


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


volatile RegS StatusPKDU= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


volatile uint16_t delay_count=0;

#define LINK_COUNT	5	//число попыток связи

#define RX_FRAME_SIZE	19

uint8_t ArrayRX_PKBA[RX_FRAME_SIZE];

volatile bool 	ADC_compl=false,
				RX_ok=false,
				TX_st=false;

extern unsigned char uipIP_ADDR0,uipIP_ADDR1,uipIP_ADDR2,uipIP_ADDR3;

const char data_str[]=__DATE__;//mmm dd yyyy

extern struct uip_eth_addr mac;
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
void clear_Arr(u8 *arr,u8 cnt);
void UART1_send_byte(uint8_t byte);

SemaphoreHandle_t xPKBA_RX_Semaphore = NULL;
//==================================================================================
//==================================================================================
//==================================================================================

void delay_ms(uint16_t delay)
{
	delay_count=delay;
	while(delay_count);
}

// void SysTick_Handler(void)//~1ms
// {
// //	static uint16_t cnt_link=0;
// 	//static bool st=0;
// 	
// 	if(delay_count)
// 	{
// 		delay_count--;
// 	}
// 	
// 	//++time_out_byte;
// 	//++cnt_start;
// 	// vTaskIncrementTick();
// }
//==================================================================================
uint8_t cnt_rx_byte = 0;
bool start = false;// ld=0;
uint8_t tmp = 0; 

void UART1_IRQHandler(void)
{

	//static signed BaseType_t xHigherPriorityTaskWoken; 
	
	//xHigherPriorityTaskWoken = pdFALSE;
	
	if(UART_GetITStatus(MDR_UART1,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
		tmp=UART_ReceiveData(MDR_UART1);

#ifdef 	PKBA_AD8		
		if((tmp==0xAA) && (!start))//start byte
		{
			start=true;
		}
		else
		{
			if(start)
			{
				if(!RX_ok)//предыдущие данные обработаны
					ArrayRX_PKBA[cnt_rx_byte]=tmp; 
					
				++cnt_rx_byte;
				if(cnt_rx_byte==RX_FRAME_SIZE)
				{	
					RX_ok=true;
					start=false;
					cnt_rx_byte=0;
					xSemaphoreGiveFromISR( xPKBA_RX_Semaphore,NULL );
				}
				
// 				if(ld)
// 					LED1_ON;
// 				else
// 					LED1_OFF;
// 				
// 				ld=!ld;
			}
		}
#endif
#ifdef 	PKDU2_NIOB
		if(!RX_ok)//предыдущие данные обработаны
		{
			ArrayRX_PKBA[cnt_rx_byte] = tmp; 
			++cnt_rx_byte;
			
			if(cnt_rx_byte == 11)
			{
				cnt_rx_byte=0;
				RX_ok = true;
				xSemaphoreGiveFromISR( xPKBA_RX_Semaphore,NULL );
			}
		}
#endif
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
		LED2_TOGLE;
	//[Suspend waiting for a key press]
	//[Process the key press] 
	} 
}
//==================================================================================
 void vLinkPKBATask( void *pvParameters ) 
{ 
	uint8_t sum=0,cnt_link=0,
			strlen[20];
	//LED1_TOGLE;
	/* Create the semaphore used by the ISR to wake this task. */
	xPKBA_RX_Semaphore = xSemaphoreCreateBinary();

	//StatusPKDU.reg1 |= 1; //manual load
	
	for( ;; ) 
	{
//		LED1_TOGLE;
		xSemaphoreTake( xPKBA_RX_Semaphore, configTICK_RATE_HZ/2 );//.5s
		
		if(RX_ok)
		{
#ifdef 	PKBA_AD8
			
			uint8_t cnt_byte=0;
			sum=0;
			while(cnt_byte<RX_FRAME_SIZE)
			{
				sum+=ArrayRX_PKBA[cnt_byte];
				++cnt_byte;
			}
			sum+=0xAA;
			
			if(!sum)
			{
				uint16_t tmp = 0;
			//link_PKBA(StatusPKBA);
				
	
				
				tmp=(ArrayRX_PKBA[8]<<8)|ArrayRX_PKBA[7];
				StatusPKDU.IGen = tmp;
				sprintf(strlen,"Tok=%4d |",tmp);
				GLCD_GoTo(0,2);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
				
				tmp=ArrayRX_PKBA[11];
				
				if(tmp>29)
				{
					tmp -= 30;
					StatusPKDU.TM = tmp;
					sprintf(strlen,"TM = %3dC|",tmp);
				}
				else
				{
					tmp = 30-tmp;
					StatusPKDU.TM = tmp;
					sprintf(strlen,"TM =-%dC |",tmp);
				}
				GLCD_GoTo(0,3);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
				
				tmp=ArrayRX_PKBA[10];
				StatusPKDU.DM = tmp;
				sprintf(strlen,"DM = %1d.%1d |",tmp/10,tmp%10);
				GLCD_GoTo(0,4);
				GLCD_WriteString( strlen );
				clear_Arr(strlen,20);
				
				StatusPKDU.TBapEx = ArrayRX_PKBA[14];				
				sprintf(strlen,"TBEx=%dC |",ArrayRX_PKBA[14]);
				GLCD_GoTo(0,5);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
				
				StatusPKDU.TBapIn = ArrayRX_PKBA[15];
				sprintf(strlen,"TBIn=%dC |",ArrayRX_PKBA[15]);
				GLCD_GoTo(0,6);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
				
				StatusPKDU.day = ArrayRX_PKBA[1];
				StatusPKDU.mnth = ArrayRX_PKBA[0];
										//		reg1				reg0
				sprintf(strlen,"Date:%d/%d",ArrayRX_PKBA[1],ArrayRX_PKBA[0]);
				
				GLCD_GoTo(60,2);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
				
				tmp = (ArrayRX_PKBA[6]<<8)|ArrayRX_PKBA[5];
				StatusPKDU.UGen = tmp;
				sprintf(strlen,"Ug = %2d.%1dV",tmp/10,tmp%(tmp/10));
				
				GLCD_GoTo(60,3);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
				
				
				StatusPKDU.knz = ArrayRX_PKBA[4];
				sprintf(strlen,"Kn%1d%1d%1d%1d%1d%1d%1d%1d",(ArrayRX_PKBA[4]>>7)&1,(ArrayRX_PKBA[4]>>6)&1,
															(ArrayRX_PKBA[4]>>5)&1,(ArrayRX_PKBA[4]>>4)&1,
															(ArrayRX_PKBA[4]>>3)&1,(ArrayRX_PKBA[4]>>2)&1,
															(ArrayRX_PKBA[4]>>1)&1,(ArrayRX_PKBA[4]>>0)&1);
				GLCD_GoTo(60,4);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
			
				tmp=(ArrayRX_PKBA[13]<<8)|ArrayRX_PKBA[12];
				StatusPKDU.NDiz = tmp;
				sprintf(strlen,"Fdv= %4d",tmp);
				GLCD_GoTo(60,5);
				GLCD_WriteString(strlen);
				clear_Arr(strlen,20);
							
				GLCD_GoTo(0,1);
				GLCD_WriteString("TX PKBA: Ok    ");
				StatusPKDU.Error &= !1;	


				cnt_link=0;
			}
			else 
			{
				GLCD_GoTo(0,1);
				GLCD_WriteString("TX PKBA: CRC err ");
				StatusPKDU.Error |= 1;
			}
#endif
#ifdef 	PKDU2_NIOB
			cnt_link=0;
			GLCD_GoTo(0,2);
			GLCD_WriteString("RX PKDU2: Ok      ");
#endif
			RX_ok=0;
		}

		
		vTaskDelay( 300 );
		
		if(cnt_link<LINK_COUNT)
		{
#ifdef PKDU2_NIOB	
			StatusPKDU.reg0 = 1;			
			StatusPKDU.reg1 = 2;
			sum = StatusPKDU.reg0 + StatusPKDU.reg1 + 3 + 4 + 5 + 6 + 7 + 8 + 0xAA;
#endif
#ifdef PKBA_AD8			
			sum = StatusPKDU.reg0 + StatusPKDU.reg1 + 0xAA;
#endif
			sum = 255 - sum;
			++sum;	
//			sprintf(strlen,"%02X %02X %02X %02X",0xAA,StatusPKDU.reg0,StatusPKDU.reg1,sum);
// 			GLCD_GoTo(0,1);
// 			GLCD_WriteString(strlen);
			//NVIC_DisableIRQ(UART1_IRQn);
			UART_ITConfig(MDR_UART1, UART_IT_RX, DISABLE);
			RS485_TX_ON;
			UART1_send_byte(0xAA);
			UART1_send_byte(StatusPKDU.reg0);
			UART1_send_byte(StatusPKDU.reg1);
#ifdef PKDU2_NIOB
			UART1_send_byte(3);
			UART1_send_byte(4);
			UART1_send_byte(5);
			UART1_send_byte(6);
			UART1_send_byte(7);
			UART1_send_byte(8);
#endif
			__disable_irq();
			UART1_send_byte(sum);
			RS485_TX_OFF;
			__enable_irq();
			
			//while(GetKey()==DOWN);

			UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);
				
			++cnt_link;
#ifdef PKDU2_NIOB	
			GLCD_GoTo(0,1);
			sprintf(strlen,"TX PKDU2: %2d cnt   ",cnt_link);
 			GLCD_WriteString(strlen);		
#endif
			if(StatusPKDU.reg0!=0)
				StatusPKDU.reg0=0;
			
		}
		else
		{
// 			LCD_CLS();
#ifdef PKBA_AD8
			GLCD_GoTo(0,1);
			GLCD_WriteString("TX PKBA:");		
 			sprintf(strlen,"%2d cnt   ",cnt_link);
 			GLCD_WriteString(strlen);
			GLCD_GoTo(0,2);
 			GLCD_WriteString("RX PKBA: Error ");
			cnt_link = 0;
			StatusPKDU.Error |= 1;
#endif
			
#ifdef PKDU2_NIOB
			GLCD_GoTo(0,1);
			GLCD_WriteString("TX PKDU2:");		
 			sprintf(strlen,"%2d cnt   ",cnt_link);
 			GLCD_WriteString(strlen);
			GLCD_GoTo(0,2);
 			GLCD_WriteString("RX PKDU2: Error ");
			cnt_link = 0;
			StatusPKDU.Error |= 1;
#endif
		}

	} 
}

 void vGLCD_Task( void *pvParameters ) 
{ 
	for( ;; ) 
	{
	GLCD_GoTo(0,1);
	//GLCD_WriteString("TX PKBA: 1");
	vTaskDelay( 1000 );
	GLCD_GoTo(0,1);
	//GLCD_WriteString("TX PKBA: 0");
	vTaskDelay( 2000 );
// 		[Suspend waiting for 2ms since the start of the previous cycle]
// 		[Sample the input] 
// 		[Filter the sampled input] 
// 		[Perform control algorithm] 
// 		[Output result] 
	} 
}
//==================================================================================
//==================================================================================
//==================================================================================
int main(void)
{
	uint8_t strlen[20];
	//bool cntl=0;
	//POWER_detector_init();
	
			//sum=0;
	
	prvSetupHardware();
	
	RS485_TX_OFF;
	
	SysTick_Config( SystemCoreClock/8000 );//~1 ms
	
	GLCD_ClearScreen();
	
	sprintf( strlen,"IP: %d.%d.%d.%d",uipIP_ADDR0,uipIP_ADDR1,uipIP_ADDR2,uipIP_ADDR3 );
	GLCD_GoTo( 0,0 );
	GLCD_WriteString( strlen );
	GLCD_GoTo( 0,1 );
	//GLCD_WriteString("TX PKBA:");
	GLCD_GoTo( 0,2 );
	//GLCD_WriteString("RX PKBA:");
// 	GLCD_GoTo(0,3);
// 	GLCD_WriteString("|                   |");
// 	GLCD_GoTo(0,4);
// 	GLCD_WriteString("|  en.radzio.dxp.pl |");
// 	GLCD_GoTo(0,5);
// 	GLCD_WriteString("|  STM32 Cortex-M3  |");
// 	GLCD_GoTo(0,6);
// 	GLCD_WriteString("|  microcontrollers |");
// 	GLCD_GoTo(0,7);
// 	GLCD_WriteString("+-------------------+");
	
	xTaskCreate( vuIP_Task, "uIP", mainBASIC_WEB_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL );
 	xTaskCreate( vLinkPKBATask,  "LinkPKBA", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
 	//xTaskCreate( vKeyHandlerTask,  "Key", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
	//xTaskCreate( vGLCD_Task,  "GLCD", configMINIMAL_STACK_SIZE, NULL, 2,( xTaskHandle * ) NULL);
	
	/* The suicide tasks must be created last as they need to know how many
	tasks were running prior to their creation in order to ascertain whether
	or not the correct/expected number of tasks are running at any given time. */
//   vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

	/* Configure the high frequency interrupt used to measure the interrupt
	jitter time. */
	//vSetupHighFrequencyTimer();

	/* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	while(1){};
}
//==================================================================================
//==================================================================================
//==================================================================================


void prvSetupHardware( void )
{
	SSP_InitTypeDef SPI_InitStructure;
	PORT_InitTypeDef PORT_InitStructure;
	UART_InitTypeDef UART_InitStructure;
//	EBC_InitTypeDef EBC_InitStruct;
	
//CLK
// 	MDR_RST_CLK->HS_CONTROL = 0x00000001;
// 	while((MDR_RST_CLK->CLOCK_STATUS&0x00000004)!=0x00000004){}
// 		
// //PLL On
// 	MDR_RST_CLK->PLL_CONTROL = 0x00000904; // PLL K_mul = 9 (clk = 80 MHz)
// 	while((MDR_RST_CLK->CLOCK_STATUS & 0x00000002) != 0x00000002){} //wait PLL
// 	MDR_EEPROM->CMD = 0x0000018;	//Delay = 3
// 	MDR_RST_CLK->CPU_CLOCK = 0x00000106;
	

/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while(!RST_CLK_HSEstatus());
	
	RST_CLK_HSIcmd(DISABLE);
	
	//CPU_C2
	// Pll config - 8*10 = 80
	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);
	RST_CLK_CPU_PLLuse(ENABLE);
	RST_CLK_CPU_PLLcmd(ENABLE);
	while (SUCCESS != RST_CLK_CPU_PLLstatus()) ;
	
	//CPU_C3
	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	
	MDR_RST_CLK->CPU_CLOCK |=2;//CPU_C1 set HSE

//PORT
	
#define ALL_PORTS_CLK ( RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

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

	//Configures the External Bus.

	/* Enables the RTCHSE clock on the EBC */
// 	RST_CLK_PCLKcmd(RST_CLK_PCLK_EBC, ENABLE);

// 	EBC_StructInit(&EBC_InitStruct);

// 	/* Configure External Bus */
// 	EBC_InitStruct.EBC_WaitState = EBC_WAIT_STATE_18HCLK;
// 	EBC_InitStruct.EBC_Mode = EBC_MODE_ROM;

// 	EBC_Init(&EBC_InitStruct);
	
	GLCD_Initialize();
/************************ UART1 Initialization *************************/
	PORT_StructInit(&PORT_InitStructure);//reset struct

	PORT_InitStructure.PORT_Pin = PORT_Pin_12 | PORT_Pin_13;//RX TX
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin = PORT_Pin_14;//EN RS485
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_InitStructure.PORT_PD = PORT_PD_OPEN;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);

//UART1
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_DeInit(MDR_UART1);
	
	/* Disable interrupt on UART1 */
    NVIC_DisableIRQ(UART1_IRQn);
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 38400;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	
#ifdef PKBA_AD8
 	UART_InitStructure.UART_StopBits			= UART_StopBits2; //PKBA
 	UART_InitStructure.UART_Parity				= UART_Parity_No;
#endif

#ifdef PKDU2_NIOB
	UART_InitStructure.UART_StopBits			= UART_StopBits1;//PKDU2 niobyi
	UART_InitStructure.UART_Parity				= UART_Parity_Even;
#endif
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init(MDR_UART1,&UART_InitStructure);

	NVIC_EnableIRQ(UART1_IRQn);
	UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);
	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART1,ENABLE);
	
//SPI
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP1,ENABLE);
	PORT_StructInit(&PORT_InitStructure);//reset struct
	/* Configure SSP1 pins: FSS, CLK, RXD, TXD */

	/* Configure PORTB pins  */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);//clk rx tx
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_11 | PORT_Pin_12);//FSS
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;	

	PORT_Init(MDR_PORTB, &PORT_InitStructure);
	
	PORT_SetBits(MDR_PORTB,(PORT_Pin_11 | PORT_Pin_12)); //SET 11 and 12 BITS OF PORTB
	
	/* Reset all SSP settings */
	SSP_DeInit(MDR_SSP1);	
	
	SSP_BRGInit(MDR_SSP1,SSP_HCLKdiv1);//ssp enabl
	
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

	SSP_Init(MDR_SSP1,&SPI_InitStructure);
	  /* Enable SSP1 */
	SSP_Cmd(MDR_SSP1, ENABLE);
	
	while(SSP_GetFlagStatus(MDR_SSP1,SSP_FLAG_RNE) == SET)	//Purge Rx FIFO
		SSP_ReceiveData(MDR_SSP1);
	
	
/************************ Joystick Initialization *************************/

  /* Configure PORTC pins 10..14 for input to handle joystick events */
	PORT_StructInit(&PORT_InitStructure);//reset struct
	
  PORT_InitStructure.PORT_Pin   = (PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 |
                                  PORT_Pin_13 | PORT_Pin_14);
  PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

  PORT_Init(MDR_PORTC, &PORT_InitStructure);

/************************ LEDs Initialization *************************/
	PORT_StructInit(&PORT_InitStructure);//reset struct
  /* Configure PORTD pins 10..14 for output to switch LEDs on/off */
  PORT_InitStructure.PORT_Pin   = LEDs_PINs;
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTD, &PORT_InitStructure);

  /* All LEDs switch off */
  PORT_ResetBits(MDR_PORTD, LEDs_PINs);
}




//-----------------------------------------------------------
void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}

//-----------------------------------------------------------
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}

//-----------------------------------------------------------
void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that 
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
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

void clear_Arr(u8 *arr,u8 cnt)
{
	while(cnt)
	{
		arr[cnt]=0;
		cnt--;
	}
}

