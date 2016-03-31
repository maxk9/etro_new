/*
*Программа AD16 KDU миландр 
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_ssp.h"
#include <stdbool.h>


#include <string.h>
//#include <stdlib.h>

#include "font5x7.h"

#include "types.h"

// Kernel includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define DEBUG_ON 0

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

while (ITM_Port32(0) == 0){}
    ITM_Port8(0) = ch;
  }
  return(ch);
}


#endif




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



#define RS485_TX_OFF	PORT_ResetBits( MDR_PORTA, PORT_Pin_2 )
#define RS485_TX_ON		PORT_SetBits( MDR_PORTA, PORT_Pin_2 )

#define  LCD_DIS 		PORT_ResetBits(MDR_PORTA, PORT_Pin_3 )
#define  LCD_EN 		PORT_SetBits(MDR_PORTA, PORT_Pin_3 )
#define  LCD_ALL_ROW_OFF 	{PORT_ResetBits(MDR_PORTA, (PORT_Pin_1 | PORT_Pin_5));	\
							PORT_ResetBits(MDR_PORTB, (PORT_Pin_5|PORT_Pin_6|PORT_Pin_7));}

							
#define  LCD_ROW1_ON 	PORT_SetBits(MDR_PORTA, (PORT_Pin_1))
#define  LCD_ROW2_ON 	PORT_SetBits(MDR_PORTA, (PORT_Pin_5))
#define  LCD_ROW3_ON 	PORT_SetBits(MDR_PORTB, (PORT_Pin_5))					
#define  LCD_ROW4_ON 	PORT_SetBits(MDR_PORTB, (PORT_Pin_6))
#define  LCD_ROW5_ON 	PORT_SetBits(MDR_PORTB, (PORT_Pin_7))
//size_t fre=0;
#define  LED_PJD_ON 	PORT_ResetBits(MDR_PORTB, (PORT_Pin_9))		
#define  LED_PJD_OFF 	PORT_SetBits(MDR_PORTB, (PORT_Pin_9))
							
#define  LED_ALARM_ON 	PORT_ResetBits(MDR_PORTF, (PORT_Pin_2))							
#define  LED_ALARM_OFF 	PORT_SetBits(MDR_PORTF, (PORT_Pin_2))
#define  LED_ALARM_TOGGLE 	MDR_PORTF->RXTX ^= PORT_Pin_2	
							
#define  LED_FIRE_ON 	PORT_ResetBits(MDR_PORTF, (PORT_Pin_4))
#define  LED_FIRE_OFF 	PORT_SetBits(MDR_PORTF, (PORT_Pin_4))
#define  LED_FIRE_TOGGLE 	MDR_PORTF->RXTX ^= PORT_Pin_4							
							
#define  LED_ISOL_ON 	PORT_SetBits(MDR_PORTB, (PORT_Pin_10))								
#define  LED_ISOL_OFF 	PORT_ResetBits(MDR_PORTB, (PORT_Pin_10))
#define  LED_ISOL_TOGGLE	MDR_PORTB->RXTX ^= PORT_Pin_10							

#define  ZUM_ON 	PORT_SetBits(MDR_PORTF, (PORT_Pin_6))								
#define  ZUM_OFF 	PORT_ResetBits(MDR_PORTF, (PORT_Pin_6))
		

//CRC16_TABLE_LOCATION poly 0xA001.
const uint8_t high[ 256 ]=
{ /*1 	2 		3 	4 		5 	6 		7 	8 		9 	10 		11 	12 		13 	14 		15 	16 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,	/* 1 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 2 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 3 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,	/* 4 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 5 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,	/* 6 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,	/* 7 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 8 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 9 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,	/* 10 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,	/* 11 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 12 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,	/* 13 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 14 */
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,	/* 15 */
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40		/* 16 */
};

const uint8_t low[ 256 ]=
{ /*1	2		3	4		5	6		7	8		9	10		11	12		13	14		15	16 */
	 0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2, 0xc6, 0x06, 0x07, 0xc7, 0x05, 0xc5, 0xc4, 0x04,	/* 1 */
	 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e, 0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09, 0x08, 0xc8,	/* 2 */
	 0xd8, 0x18, 0x19, 0xd9, 0x1b, 0xdb, 0xda, 0x1a, 0x1e, 0xde, 0xdf, 0x1f, 0xdd, 0x1d, 0x1c, 0xdc,	/* 3 */
	 0x14, 0xd4, 0xd5, 0x15, 0xd7, 0x17, 0x16, 0xd6, 0xd2, 0x12, 0x13, 0xd3, 0x11, 0xd1, 0xd0, 0x10,	/* 4 */
	 0xf0, 0x30, 0x31, 0xf1, 0x33, 0xf3, 0xf2, 0x32, 0x36, 0xf6, 0xf7, 0x37, 0xf5, 0x35, 0x34, 0xf4,	/* 5 */
	 0x3c, 0xfc, 0xfd, 0x3d, 0xff, 0x3f, 0x3e, 0xfe, 0xfa, 0x3a, 0x3b, 0xfb, 0x39, 0xf9, 0xf8, 0x38,	/* 6 */
	 0x28, 0xe8, 0xe9, 0x29, 0xeb, 0x2b, 0x2a, 0xea, 0xee, 0x2e, 0x2f, 0xef, 0x2d, 0xed, 0xec, 0x2c,	/* 7 */
	 0xe4, 0x24, 0x25, 0xe5, 0x27, 0xe7, 0xe6, 0x26, 0x22, 0xe2, 0xe3, 0x23, 0xe1, 0x21, 0x20, 0xe0,	/* 8 */
	 0xa0, 0x60, 0x61, 0xa1, 0x63, 0xa3, 0xa2, 0x62, 0x66, 0xa6, 0xa7, 0x67, 0xa5, 0x65, 0x64, 0xa4,	/* 9 */
	 0x6c, 0xac, 0xad, 0x6d, 0xaf, 0x6f, 0x6e, 0xae, 0xaa, 0x6a, 0x6b, 0xab, 0x69, 0xa9, 0xa8, 0x68,	/* 10 */
	 0x78, 0xb8, 0xb9, 0x79, 0xbb, 0x7b, 0x7a, 0xba, 0xbe, 0x7e, 0x7f, 0xbf, 0x7d, 0xbd, 0xbc, 0x7c,	/* 11 */
	 0xb4, 0x74, 0x75, 0xb5, 0x77, 0xb7, 0xb6, 0x76, 0x72, 0xb2, 0xb3, 0x73, 0xb1, 0x71, 0x70, 0xb0,	/* 12 */
	 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,	/* 13 */
	 0x9c, 0x5c, 0x5d, 0x9d, 0x5f, 0x9f, 0x9e, 0x5e, 0x5a, 0x9a, 0x9b, 0x5b, 0x99, 0x59, 0x58, 0x98, 	/* 14 */
	 0x88, 0x48, 0x49, 0x89, 0x4b, 0x8b, 0x8a, 0x4a, 0x4e, 0x8e, 0x8f, 0x4f, 0x8d, 0x4d, 0x4c, 0x8c, 	/* 15 */
	 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40 	/* 16 */
};

							
const uint16_t  OTKAZ[]    ={47*5,51*5 ,43*5 ,33*5,40*5},
				KDG[]      ={43*5,37*5 ,36*5},
				PZF[]      ={48*5,40*5 ,53*5},
				OK[]       ={47*5,43*5},
				Raznos[]   ={0   ,49*5 ,33*5 ,40*5,46*5 ,47*5 ,50*5},
				Peregrev[] ={48*5,38*5 ,49*5 ,38*5,36*5 ,49*5 ,38*5 ,35*5},
				Disel[]    ={0   ,37*5 ,41*5 ,40*5,38*5 ,44*5 ,64*5},
				Ug[]       ={0   ,0    ,0    ,65*5,36*5},
				Ig[]       ={0   ,0    ,0    ,66*5,36*5},
				Ig_na_10A[]={66*5,36*5 ,0    ,46*5,33*5 ,17*5 ,16*5 ,33*5},
				n_norm[]   ={46*5,38*5 ,0    ,46*5,47*5 ,49*5 ,45*5},
				Overload[] ={48*5,38*5 ,49*5 ,38*5,36*5 ,49*5 ,52*5 ,40*5},
				error[]    ={0   ,47*5 ,57*5 ,41*5,34*5 ,43*5 ,33*5},
				RN[]       ={0   ,0    ,0    ,49*5,46*5},
				NO[]       ={0   ,0    ,46*5 ,38*5,51*5},
				MASLA[]    ={0   ,45*5 ,33*5 ,50*5,44*5 ,33*5},
				DM_MASLA[] ={0   ,37*5 ,45*5 ,28*5,1*5  ,43*5 ,36*5},
				Topliv[]   ={51*5,47*5 ,48*5 ,44*5,41*5 ,35*5 ,33*5},
				PUSK[]     ={0   ,0    ,48*5 ,52*5,50*5 ,43*5},
				OG_b100[]  ={0   ,51*5 ,47*5 ,39*5,30*5 ,17*5 ,16*5 ,16*5},
				OG_b105[]  ={0   ,51*5 ,47*5 ,39*5,30*5 ,17*5 ,16*5 ,21*5},
				OSTANOV[]  ={47*5,50*5 ,51*5 ,33*5,46*5 ,47*5 ,35*5},
				Rashol[]   ={49*5,33*5 ,50*5 ,54*5,47*5 ,44*5 ,33*5 ,39*5},
				NO_STOP[]  ={0   ,0    ,46*5 ,38*5,51*5 ,0    ,47*5 ,50*5 ,51*5,33*5,46*5,47*5 ,35*5,33*5},
				NO_Vozb[]  ={0   ,0    ,0    ,46*5,38*5 ,51*5 ,0    ,35*5 ,47*5,40*5,34*5,14*5},
				TOLKO[]    ={0   ,51*5 ,47*5 ,44*5,61*5 ,43*5 ,47*5},
				MHU[]      ={0   ,0    ,58*5 ,45*5,52*5},
				SWECH[]    ={0   ,0    ,50*5 ,35*5,38*5 ,56*5 ,33*5},
				ON[]       ={0   ,0    ,0    ,35*5,43*5 ,44*5},
				OFF[]      ={0   ,0    ,35*5 ,60*5,43*5 ,44*5},
				PJD[]      ={0   ,0    ,0    ,48*5,39*5 ,37*5},
				DZF[]      ={0   ,0    ,0    ,37*5,40*5 ,53*5},
				SPT[]      ={50*5,48*5 ,51*5},
				uOJ[]      ={0   ,52*5 ,49*5 ,0   ,47*5 ,39*5},
				Progrev[]  ={48*5,49*5 ,47*5 ,36*5,49*5 ,38*5 ,35*5},
				WORK[]     ={0   ,49*5 ,33*5 ,34*5,47*5 ,51*5 ,33*5},
				FIRE[]     ={0   ,48*5 ,47*5 ,39*5,33*5 ,49*5},
				GOTOV[]    ={0   ,36*5 ,47*5 ,51*5,47*5 ,35*5},
				NIZK[]     ={0   ,46*5 ,41*5 ,40*5,43*5 ,33*5 ,64*5},
				ISOL[]     ={0   ,41*5 ,40*5 ,47*5,44*5 ,14*5},
				BLOK[]     ={0   ,0    ,34*5 ,44*5,47*5 ,43*5},
				PRTC[]     ={0   ,40*5 ,33*5 ,58*5,41*5 ,51*5},
				VOZD[]     ={0   ,35*5 ,47*5 ,40*5,37*5},
				FILTR[]    ={0   ,53*5 ,41*5 ,44*5,61*5 ,51*5 ,49*5};
					
					
volatile struct 
{
	uint32_t Ndiz;	//0
	uint32_t FreqU;	//1
	uint32_t UDG;		//2
	uint32_t UBS;		//3
	uint32_t Igen;	//4
	uint32_t DM;		//5
	uint32_t TOJ;		//6
	uint32_t TBap;	//7
	uint32_t LvlDIZ;	//8
	uint32_t RegWrk;	//9
	uint32_t Isol;	//10
	uint32_t Led;		//11
	uint32_t RegSDG;	//12
	uint32_t Error;	//13
	uint32_t RegimTime; //14
	uint32_t RegimError;//15
	uint32_t KWH;
}StatusDG = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

volatile struct 
{
	uint8_t ADCF;	//0
	uint8_t ADCU;	//1
	uint16_t RegSW;	//3
	uint8_t CurrPosLCD;	//4
	uint8_t SPT_Time;
}StatusDU = {0, 0, 0, 0, 30};

#define PDG_FRAME_SIZE	(uint32_t)34 //aa+crc16
#define RX_SIZE	(uint32_t)(PDG_FRAME_SIZE*2-4) 

volatile struct 
{
	uint8_t ArrayRX[(RX_SIZE)];
	uint8_t count;
	uint8_t tail;
	uint8_t head;
}DataUsart={0,0,0,0};


#define PU_MPU		0
#define PU_DPU		1

#define ADC_COUNT_CH 3
volatile uint32_t	ADC_Arr[ADC_COUNT_CH];

volatile uint32_t	IndData[5];							
							
volatile uint32_t	delay_count = 0,
					led_reg[7][5],
					index_char[16];

uint8_t				sec_05 = 0,
					//led_sec = 0,
					status_link = 0;

volatile bool		adc_ok = false,
					two_sec = false,
					read_ok = false;

//SemaphoreHandle_t xPKBA_RX_Semaphore = NULL;

void vLink_SysTask( void *pvParameters );
void vIndikatorTask( void *pvParameters );
void vKeyHandlerTask( void *pvParameters );
void vADC_CalkTask( void *pvParameters );

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

/*
 * Hook functions that can get called by the kernel.
 */
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName );
void clear_Arr( u8 *arr,u8 cnt );
void UART1_send_byte( uint8_t byte );
void scroll_mess_left(void);
void load_led_reg(void);
void show_led_data(uint8_t num_par);
void DecSegm(uint16_t R0);
void OK_PDG(void);
void clear_lcd(void);
void err_KDG(void);
void only_hmu(void);
bool read_rx_data(uint8_t *Arr);
void Show_led_str(const uint16_t *str, uint8_t n_str, uint8_t len);
uint16_t calc_CRC16( uint8_t *buf, uint16_t num );


//==================================================================================
//==================================================================================
//==================================================================================
//==================================================================================
void delay_4ms(uint16_t delay)
{
	delay_count=delay;
	while(delay_count);
}

#define	AD_SAMPL	62
void ADC_IRQHandler(void)//0.144 ms //36 тактов на один канал
{
	uint8_t cnt_ch = 0;
	//static bool ADC_compl = false;
	static uint8_t 	countAD = AD_SAMPL+2;
	static uint32_t AdTempResult = 0;
//	static portBASE_TYPE xTaskWoken;
	uint32_t tmp = 0;

	tmp = ADC1_GetResult();
	cnt_ch = ( tmp>>16 );

	tmp &= 0xFFF; //12 bit
	
//	xTaskWoken = pdFALSE;
	
	if( countAD )
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
			case ADC_CH_ADC1:
				ADC1_SetChannel( ADC_CH_TEMP_SENSOR );
				break;
			case ADC_CH_TEMP_SENSOR:
				ADC1_SetChannel( ADC_CH_ADC0 );
				cnt_ch = 2;
				break;
			default:
				ADC1_SetChannel((cnt_ch+1));
				break;
		}
		
		ADC_Arr[cnt_ch] = AdTempResult>>4;
		
		if( cnt_ch == 2 )
		{
			adc_ok = true;
//			if( xADC_cmpl_Semaphore != NULL )
//			{
//				xSemaphoreGiveFromISR( xADC_cmpl_Semaphore, &xTaskWoken );
//			}
		}
		AdTempResult = 0;
	}
	
//	if( xTaskWoken != pdFALSE )
//    {
//        // We can force a context switch here. 
//		portYIELD();
//    }
}
//==================================================================================
void Timer1_IRQHandler(void)//3.125ms
{
	static uint8_t 	cnt_row = 0;
	uint8_t reg = 7;
	
	MDR_TIMER1 -> CNT = 0x0000;
	MDR_TIMER1 -> STATUS &= ~( 1 << 1 );
	NVIC_ClearPendingIRQ( Timer1_IRQn );
	
	LCD_DIS;
	LCD_ALL_ROW_OFF;
	
	while( reg )
	{
		--reg;
		SSP_SendData( MDR_SSP1, (uint16_t)led_reg[reg][cnt_row] );
		while( !SSP_GetFlagStatus( MDR_SSP1, SSP_FLAG_TNF ) );
	}
	while( SSP_GetFlagStatus( MDR_SSP1, SSP_FLAG_BSY ) );
	
	LCD_EN;
	
	switch( cnt_row )
	{
		case 0:
			LCD_ROW1_ON;
			break;
		case 1:
			LCD_ROW2_ON;
			break;
		case 2:
			LCD_ROW3_ON;
			break;
		case 3:
			LCD_ROW4_ON;
			break;
		case 4:
			LCD_ROW5_ON;
			break;
		default: break;
	}


	if( cnt_row < 4 )
		++cnt_row;
	else
		cnt_row = 0;
	
	if( delay_count )
		--delay_count;
}
//==================================================================================
void Timer2_IRQHandler( void )//0.5s
{
	static uint8_t 	cnt_05sec=0;
//	static portBASE_TYPE xTaskWoken;
	
	MDR_TIMER2 -> CNT = 0x0000;
	MDR_TIMER2 -> STATUS &= ~( 1 << 1 );
	NVIC_ClearPendingIRQ( Timer2_IRQn );
	
	if(sec_05)
	{
		two_sec = true;
	}
	
	++sec_05;
	
	if(( StatusDG.Led>>0 )&1 )
	{
		LED_PJD_ON;
	}
	else
		LED_PJD_OFF;
	
	if( (StatusDG.Error || (!status_link))||(StatusDG.TOJ>129))
	{
		LED_ALARM_TOGGLE;
		ZUM_ON;
	}
	else
	{
		LED_ALARM_OFF;
		if(!(StatusDG.RegWrk&1))
			ZUM_OFF;
	}
		
	if(( StatusDG.Led>>2 )&1)
	{
		//LED_FIRE_ON;
		LED_FIRE_TOGGLE;
	}
	else
		LED_FIRE_OFF;
	
	if(( StatusDG.Led>>3 )&1)
	{
		LED_ISOL_TOGGLE;
	}
	else
		LED_ISOL_OFF;
	
	if(cnt_05sec > 1)
	//if(cnt_sec > 99)
	{
		if((StatusDG.Error>>15)&1)
		{
			if(StatusDU.SPT_Time)
				--StatusDU.SPT_Time;
		}
		else
			StatusDU.SPT_Time=30;
		//ChangeCtTime();
		//ChangeCtError();
//xSemaphoreGiveFromISR(xTime_Err_Semaphore, &xTaskWoken);
		cnt_05sec = 0;	
		
	}
	else
		++cnt_05sec;
	
//	if( xTaskWoken != pdFALSE )
//    {
//        // We can force a context switch here. 
//		portYIELD();
//    }
}
//==================================================================================
void UART1_IRQHandler( void )//RX 14 byte
{
	uint32_t tmp = 0;//,cnt = 0;
	static bool start = false;
//	static uint32_t cnt_rx_byte = 0;
//	static portBASE_TYPE xTaskWoken;
	
//	xTaskWoken = pdFALSE;
	
//	PORT_SetBits( MDR_PORTB, PORT_Pin_10 );
	
	while( !UART_GetFlagStatus( MDR_UART1, UART_FLAG_RXFE ) )
	{
		DataUsart.ArrayRX[ DataUsart.tail ] = (uint8_t)UART_ReceiveData( MDR_UART1 );

		++DataUsart.count;
		
		if( DataUsart.tail >= (RX_SIZE-1) )
			{
				DataUsart.tail = 0;
			}
			else
				++DataUsart.tail;
	}
//		if( !read_ok )
//		{
//			if( !start )
//			{
//				if( tmp == 0xAA )
//				{
//					start = true;
//					cnt_rx_byte = 0; //__nop();
//				}
//			}
//			else
//			{
//				DataUsart.ArrayRX[ cnt_rx_byte ] = tmp; 

//				if( cnt_rx_byte >= (PDG_FRAME_SIZE-1) )
//				{
//					cnt_rx_byte = 0;
//					start = false;
//					read_ok = true;
//				//PORT_ResetBits( MDR_PORTB, PORT_Pin_10 );
//				//xSemaphoreGiveFromISR( xUSART_RX_Semaphore, &xTaskWoken );
//				}
//				else
//					++cnt_rx_byte;
//			}
//		}
	//	++cnt;

	
//	PORT_ResetBits( MDR_PORTB, PORT_Pin_10 );
//	if( xTaskWoken != pdFALSE )
//    {
//        // We can force a context switch here. 
//		portYIELD();
//    }
}
//==================================================================================

int main(void)
{
	uint32_t pausa = 0xFFFF;
	uint8_t cnt_b=0;
	
	while(--pausa);
	
	prvSetupHardware();
	
	LED_PJD_ON;
	LED_ALARM_ON;
	LED_FIRE_ON;
	LED_ISOL_ON;
	
	ZUM_ON;
	
	while( cnt_b < 16 )
	{
		index_char[ cnt_b ] = 6*5; //FF
		++cnt_b;
	}
	load_led_reg();
	
	sec_05 = 0; 
	while( sec_05<2 );
	clear_lcd();
	
	LED_PJD_OFF;
	LED_ALARM_OFF;
	LED_FIRE_OFF;
	LED_ISOL_OFF;
	
	ZUM_OFF;
	
	xTaskCreate( vADC_CalkTask,  "ADCCalk", configMINIMAL_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL);
	xTaskCreate( vIndikatorTask,  "Indikator", configMINIMAL_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL);
	xTaskCreate( vLink_SysTask,  "Link_Sys", (configMINIMAL_STACK_SIZE), NULL, 3,( xTaskHandle * ) NULL);
	xTaskCreate( vKeyHandlerTask,  "Key", (configMINIMAL_STACK_SIZE), NULL, 3,( xTaskHandle * ) NULL);

//		
	UART_Cmd( MDR_UART1, ENABLE );
//	/* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	while(1)
	{}
}
//==================================================================================
//==================================================================================
//==================================================================================
#define ALL_PORTS_CLK	(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF )

void prvSetupHardware( void )
{
	PORT_InitTypeDef PORT_InitStructure;
	UART_InitTypeDef UART_InitStructure;
	SSP_InitTypeDef SPI_InitStructure;
	TIMER_CntInitTypeDef sTIM_CntInit;
	ADC_InitTypeDef sADC;
	ADCx_InitTypeDef sADCx;
	
	ADC_StructInit( &sADC );
	ADCx_StructInit( &sADCx );
	
//CLK
/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig( RST_CLK_HSE_ON );
	
	while( RST_CLK_HSEstatus() == ERROR );
	
	//EEPROM_SetLatency( EEPROM_Latency_3 ); //до 75Mhz
	
	/* Configure CPU_PLL clock */
	RST_CLK_CPU_PLLconfig ( RST_CLK_CPU_PLLsrcHSEdiv1,RST_CLK_CPU_PLLmul4 );
	RST_CLK_CPU_PLLcmd( ENABLE );
	
	while( RST_CLK_CPU_PLLstatus() == ERROR);
	
	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
	
	
	
	RST_CLK_CPU_PLLuse( ENABLE);
	
	RST_CLK_HSIcmd( DISABLE );


/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd( ALL_PORTS_CLK, ENABLE );

//ADC
	
	RST_CLK_PCLKcmd( RST_CLK_PCLK_ADC, ENABLE );
	
	/* ADC Configuration */
	/* Reset all ADC settings */
	ADC_DeInit();

	sADC.ADC_StartDelay = 0;
	sADC.ADC_TempSensor = ADC_TEMP_SENSOR_Enable;
	sADC.ADC_TempSensorAmplifier = ADC_TEMP_SENSOR_AMPLIFIER_Enable;
	sADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
	sADC.ADC_IntVRefConversion = DISABLE;
	sADC.ADC_IntVRefTrimming = 0;
	
	ADC_Init( &sADC );

	
	sADCx.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU;
	sADCx.ADC_SamplingMode = ADC_SAMPLING_MODE_CICLIC_CONV;
	sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
	sADCx.ADC_LevelControl = DISABLE;
	sADCx.ADC_ChannelNumber	= ADC_CH_ADC0;
	sADCx.ADC_VRefSource	= ADC_VREF_SOURCE_INTERNAL;
	sADCx.ADC_IntVRefSource	= ADC_INT_VREF_SOURCE_INEXACT;
	//sADCx.ADC_Prescaler		= ADC_CLK_div_2048;//4.6 ms
	//sADCx.ADC_Prescaler		= ADC_CLK_div_256;
	//sADCx.ADC_Prescaler = ADC_CLK_div_64;//0.144 ms
	//sADCx.ADC_Prescaler = ADC_CLK_div_32;//0.07 ms
	sADCx.ADC_Prescaler = ADC_CLK_div_64;

	sADCx.ADC_DelayGo = 7;

	ADC1_Init(&sADCx);
	
	ADC1_ClearOverwriteFlag();
	
	/* Enable ADC1 EOCIF and AWOIFEN interupts */
	ADC1_ITConfig((ADC1_IT_END_OF_CONVERSION), ENABLE);
	
	NVIC_EnableIRQ(ADC_IRQn);

	
	/* ADC1 enable */
	ADC1_Cmd(ENABLE);	
	
//SPI
	PORT_StructInit( &PORT_InitStructure );//reset struct

	PORT_InitStructure.PORT_Pin = ( PORT_Pin_0 | PORT_Pin_1 );//spi
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init( MDR_PORTF, &PORT_InitStructure );

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
	RST_CLK_PCLKcmd( RST_CLK_PCLK_TIMER2, ENABLE );
	/* Reset all TIMER1 settings */
	TIMER_DeInit( MDR_TIMER2 );

	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	/* TIM2 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				= 0;
//	sTIM_CntInit.TIMER_Prescaler                = 1000;// CLK = TIMER_CLK/(TIMER_Prescaler + 1)
//	sTIM_CntInit.TIMER_Period                   = 16000;
	sTIM_CntInit.TIMER_Prescaler                = 1000;// CLK = TIMER_CLK/(TIMER_Prescaler + 1)
	sTIM_CntInit.TIMER_Period                   = 32000;
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
	TIMER_Cmd ( MDR_TIMER2,ENABLE );
//UART1
	
	/************************ UART1 Initialization *************************/
	
	RST_CLK_PCLKcmd( RST_CLK_PCLK_UART1, ENABLE );
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
	
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit( MDR_UART1, UART_HCLKdiv1 );
	UART_DeInit( MDR_UART1 );
	
	/* Disable interrupt on UART1 */
    NVIC_DisableIRQ( UART1_IRQn );
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 115200;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits2;
	UART_InitStructure.UART_Parity				= UART_Parity_No;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_ON;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init( MDR_UART1,&UART_InitStructure );

	MDR_UART1->IFLS = (0x04<<3);//7/8 full FIFO

	NVIC_EnableIRQ( UART1_IRQn );
	UART_ITConfig( MDR_UART1, UART_IT_RX, ENABLE );
	/* Enables UART2 peripheral */
	//UART_Cmd( MDR_UART1, ENABLE );
	UART_Cmd( MDR_UART1, DISABLE );
	
//PORT
	
	PORT_StructInit( &PORT_InitStructure );//reset struct	
 /* Configure PORTA pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_1 | PORT_Pin_3 | PORT_Pin_5);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTA, &PORT_InitStructure );

 /* Configure PORTB pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_9 | PORT_Pin_10);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);

 /* Configure PORTC pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);

 /* Configure PORTD pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
 /* Configure PORTE pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_6 | PORT_Pin_7);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);

 /* Configure PORTF pins */
 	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_2 | PORT_Pin_4  | PORT_Pin_6 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	
	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_3 | PORT_Pin_5 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
}

//==================================================================================
void vIndikatorTask( void *pvParameters )
{
	bool first_link_ok = false,
		first_link_err = false;
	uint8_t tmp = 0;
	uint8_t cnt_mu = 60,
			i = 0; 
	
	
	for( ;; )
	{
		++cnt_mu;
		if( status_link )
		{
			first_link_err = false;
			if(!first_link_ok)
			{
				first_link_ok = true;
				OK_PDG();
				vTaskDelay( 1000 );
			}
		}
		else
		{
			first_link_ok = false;
			if(!first_link_err)
			{
				first_link_err = true;
				err_KDG();
				StatusDG.Led = 2;
			}
		}
		
		if( first_link_ok )
		{
			if(!(StatusDG.RegWrk&0x95)&&(!(StatusDG.Error&0x8000)))
			{
				if(StatusDU.RegSW & 0x20)//sel +
				{
					cnt_mu = 0;
					if(StatusDU.CurrPosLCD < 9)
						++StatusDU.CurrPosLCD;
					else 
						StatusDU.CurrPosLCD = 0;
				}
				
				if(StatusDU.RegSW & 0x10)//sel -
				{
					cnt_mu = 0;
					if( StatusDU.CurrPosLCD )
						--StatusDU.CurrPosLCD;
					else 
						StatusDU.CurrPosLCD = 9;
				}
				
				taskENTER_CRITICAL();
				show_led_data( StatusDU.CurrPosLCD );
				taskEXIT_CRITICAL();
			}
			else
			{
				cnt_mu = 91;
			}
			while(StatusDU.RegSW & 0x30);
			
			if(cnt_mu > 90)//4s
			{
				
				if(StatusDG.RegWrk & (1<<8))
				{
					only_hmu();
					vTaskDelay( 1000 );
				}
				
				for(i=0; i < 16; i++)
				{
					if( StatusDG.Error )
					{
						if( (StatusDG.Error>>i)&1 )
						{
							switch( i )
							{
								case 0:
									clear_lcd();
									Show_led_str(Raznos,0,7);
									break;
								case 1:
									Show_led_str(NO,0,5);
									Show_led_str(PUSK,1,6);
									break;
								case 2:
									Show_led_str(Ug,0,5);
									Show_led_str(n_norm,1,7);
									break;
								case 3:
									Show_led_str(Ig,0,5);
									Show_led_str(n_norm,1,7);
									break;
								case 4:
									clear_lcd();
									Show_led_str(DM_MASLA,0,7);
									break;
								case 5:
									Show_led_str(Overload,0,8);
									Show_led_str(Ig_na_10A,1,8);
									break;
								case 6:
									Show_led_str(Peregrev,0,8);
									Show_led_str(Disel,1,7);
									break;
								case 7:
									Show_led_str(NO,0,5);
									Show_led_str(Topliv,1,7);
									break;
								case 8:
									Show_led_str(NIZK,0,7);
									Show_led_str(uOJ,1,6);
									break;
								case 9:
									Show_led_str(error,0,7);
									Show_led_str(RN,1,5);
									break;
								case 10:
									Show_led_str(NO,0,5);
									Show_led_str(Topliv,1,7);
									break;
								case 11:
									Show_led_str(NO,0,5);
									Show_led_str(OSTANOV,1,11);
									break;
								case 12:
									Show_led_str(NIZK,0,7);
									Show_led_str(ISOL,1,6);
								//LED_ISOL_ON;
									break;
								case 13:
									Show_led_str(VOZD,0,5);
									Show_led_str(FILTR,1,7);
									break;
								case 14:
									Show_led_str(NO,0,5);
									Show_led_str(MASLA,1,6);
									break;
								case 15:
									Show_led_str(FIRE,0,6);
									Show_led_str(SPT,1,3);
									DecSegm( StatusDU.SPT_Time );
									index_char[11] = IndData[3]*5;
									index_char[12] = IndData[4]*5;
									index_char[13] = 50*5;//s
									index_char[14] = 38*5;//e
									index_char[15] = 43*5;//k
									load_led_reg();
									break;
							}
							vTaskDelay( 1000 );
						}
					}
//					else
//					{
//						ZUM_OFF;
//						//LED_ISOL_ON;
//					}
					
					if( (StatusDG.RegWrk>>i)&1 )
					{
						switch( i )
						{
							case 0:
								clear_lcd();
								Show_led_str(PUSK,0,6);
								ZUM_ON;
								vTaskDelay( 1000 );
							
								break;
							case 1:
								Show_led_str(SWECH,0,7);
								Show_led_str(ON,1,6);
								vTaskDelay( 1000 );
								break;
							case 2:
								ZUM_OFF;
								clear_lcd();
								Show_led_str(PUSK,0,6);
								
								vTaskDelay( 1000 );
								
								break;
							case 3:
								Show_led_str(PJD,0,6);
								Show_led_str(WORK,1,7);
								vTaskDelay( 1000 );
								break;
							case 4:
								clear_lcd();
								Show_led_str(OSTANOV,0,7);
								vTaskDelay( 1000 );
								break;
							case 5:
								clear_lcd();
								Show_led_str(Progrev,0,7);
								vTaskDelay( 1000 );
								break;
							case 6:
								clear_lcd();
								Show_led_str(WORK,0,7);
								vTaskDelay( 1000 );
								break;
							case 7:
								
								clear_lcd();
								Show_led_str(Rashol,0,8);
				
								tmp = StatusDG.TOJ - 30;
								DecSegm( tmp );
								index_char[10] = IndData[2]*5;
								index_char[11] = IndData[3]*5;
								index_char[12] = IndData[4]*5;
								index_char[13] = 50*5;//C
							load_led_reg();
								vTaskDelay( 1000 );
								break;
							case 9:
							clear_lcd();
							Show_led_str(PJD,0,6);
							Show_led_str(PUSK,1,8);
								vTaskDelay( 1000 );
								break;
							default:
								break;
						}
					}
				}
				if(!(StatusDG.RegWrk & 0x80))
				{
					if(StatusDG.TOJ > 129)
					{
						ZUM_ON;
						clear_lcd();
						Show_led_str(OG_b100,0,8);
						vTaskDelay( 1000 );
					}
					if(StatusDG.TOJ > 134)
					{
						ZUM_ON;
						clear_lcd();
						Show_led_str(OG_b105,0,8);
						vTaskDelay( 1000 );
					}
				}
				if(StatusDU.RegSW&8)
				{
					clear_lcd();
					Show_led_str(BLOK,0,6);
					Show_led_str(PRTC,1,6);
					vTaskDelay( 1000 );
				}
				if( !(StatusDG.RegWrk) && (!(StatusDG.Error)) )	
				{
					clear_lcd();
					Show_led_str(GOTOV,0,6);
					vTaskDelay( 1000 );
				}
				cnt_mu = 0;
			}
		}
		
		vTaskDelay( 50 );
	}
}
//==================================================================================
void vADC_CalkTask( void *pvParameters )
{
 //	xADC_cmpl_Semaphore = xSemaphoreCreateBinary();
 	for( ;; )
 	{
		//if( xADC_cmpl_Semaphore != NULL )adc_ok
		if( adc_ok )
		{
			//xSemaphoreTake( xADC_cmpl_Semaphore, portMAX_DELAY );
			StatusDU.ADCF = ADC_Arr[1];
			StatusDU.ADCU = ADC_Arr[0];
			adc_ok = false;
		}
	}
}
//==================================================================================
void vLink_SysTask( void *pvParameters )
{
#define ARR_SIZE	6
 	bool	start = false;// lnk_ok = false;
	uint8_t sum=0, 
			byte[ARR_SIZE], 
			i=0, k=0, b=0,
			rx_data[PDG_FRAME_SIZE];
	

 	for( ;; )
 	{
		if( DataUsart.count >= (PDG_FRAME_SIZE) )
		{
			vTaskDelay( 4 );

			i =	DataUsart.tail;
			for(k = 0; k<(PDG_FRAME_SIZE); k++, i++ ) //search start sequence
			{
				if((i)>=RX_SIZE )
					i = 0;
				if((DataUsart.ArrayRX[i]) == 0xFF)
				{
					if((i+1)>=RX_SIZE )
					{
						if((DataUsart.ArrayRX[0]&0xFE) == 0xFE)
						{
							DataUsart.head = i;
							start = true;
						}
					}
					else
						if((DataUsart.ArrayRX[(i+1)]&0xFE) == 0xFE)
						{
							DataUsart.head = i;
							start = true;
						}
					
					if(start)	
					{
						for( k = 0; k<PDG_FRAME_SIZE; k++, i++ )
						{
							if((i)>=RX_SIZE )
								i = 0;
							
							rx_data[k] = DataUsart.ArrayRX[i];
						}
						break;
					}
				}
			}
			DataUsart.count = 0;
			
			if( start )
			{
				start = false;
				sec_05 = 0;
				if( read_rx_data( rx_data ) == true )
				{
					status_link = 1;
					
					sum = 0; 
					i =0;
					while( i<ARR_SIZE )
					{
						byte[i] = 0;
						++i;
					}
					
					taskENTER_CRITICAL();
					
					byte[0] = (uint8_t) StatusDU.RegSW;
					byte[1] = (uint8_t) (StatusDU.RegSW>>8);
					byte[2] = (uint8_t) StatusDU.ADCF;
					byte[3] = (uint8_t) StatusDU.ADCU;
					byte[4] = (uint8_t) StatusDU.CurrPosLCD;
					
					taskEXIT_CRITICAL();
					
					sum = 2 + byte[0] + byte[1] + byte[2] + byte[3] + byte[4] + 0x55;
					//sum = 2 + 3 + 4 + 5 + 0x55;
					sum = 255 - sum;
					++sum;	
					
					UART_ITConfig( MDR_UART1, UART_IT_RX, DISABLE );
					
					RS485_TX_ON;
					UART1_send_byte( 0x55 );
					UART1_send_byte( 2 );
					UART1_send_byte( byte[0] );
					UART1_send_byte( byte[1] );
					UART1_send_byte( byte[2] );
					UART1_send_byte( byte[3] );
					UART1_send_byte( byte[4] );

					//__disable_irq();
					UART1_send_byte( sum );
					while( UART_GetFlagStatus( MDR_UART1, UART_FLAG_BUSY ));
					RS485_TX_OFF;
					
					//__enable_irq();
					
					while(!UART_GetFlagStatus( MDR_UART1, UART_FLAG_RXFE ))
					{
						UART_ReceiveData( MDR_UART1 );
					}
					
					UART_ClearITPendingBit( MDR_UART1, UART_IT_RX );
					
					UART_ITConfig( MDR_UART1, UART_IT_RX, ENABLE );
				}
			}
		}
		
		if(sec_05>1)//err timeout link
		{
			sec_05 = 0;
			status_link = 0;
		}
	}
}
//==================================================================================
void vKeyHandlerTask( void *pvParameters )
{ // Key handling is a continuous process and as such the task 
	// is implemented using an infinite loop (as most real time 
	// tasks are). 
	static uint16_t RegKn = 0, RegKnOld = 0;
//	char stat_buff[150];
	u8 i=0, j=0;
	vTaskDelay( 150 );
	for( ;; )
	{
		
#if( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( DEBUG_ON == 1 ) )
		if( sec_05>1 )
		{	
			char stat_buff[150];
			u8 i=0, j=0;
			
			sec_05 = 0;
			
			printf( "%c[1;1f%c[J", 27, 27 ); //ClearScreen
			
			for( i=0; i<150; i++ )
				stat_buff[i] = 0;
			
//			vTaskGetRunTimeStats( stat_buff );
//			
//			i = 149;
//			while( !stat_buff[i] )
//				i--;
//			
//			
//			
//			j=0;
//			while( j<i )
//			{
//				putchar( stat_buff[j] );
//				++j;
//			}
//			
//			for( i=0; i<150; i++ )
//				stat_buff[i] = 0;
//			
//			putchar( '\r' );
//			putchar( '\n' );
			
			vTaskList(stat_buff);
			
			i = 149;
			while( !stat_buff[i] )
				i--;
			
			j=0;
			while( j<i )
			{
				putchar( stat_buff[j] );
				++j;
			}
		}
#endif			
		
		if(!PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_0))//pusk
			RegKn |= 1;
		else
			RegKn &= 0xfffe;
		
		if(!PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_1))//stop
			RegKn |= 2;
		else
			RegKn &=0xfffd;
		
		if(!PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_2))//uki
			RegKn |= 4;
		else
			RegKn &= 0xfffb;
		
		if(!PORT_ReadInputDataBit( MDR_PORTE,PORT_Pin_3 ))//protect
			RegKn |= 8;
		else
			RegKn &= 0xfff7;

		if(!PORT_ReadInputDataBit( MDR_PORTE,PORT_Pin_6 ))//select UP
			RegKn |= 0x10;
		else
			RegKn &= 0xffef;
		
		if(!PORT_ReadInputDataBit( MDR_PORTE,PORT_Pin_7 ))//select DOWN/reset
			RegKn |= 0x20;
		else
			RegKn &= 0xffdf;
		
		if(!PORT_ReadInputDataBit( MDR_PORTF, PORT_Pin_3 ))//LOAD ON
			RegKn |= 0x40;
		else
			RegKn &= 0xffbf;

		if(!PORT_ReadInputDataBit( MDR_PORTF, PORT_Pin_5 ))//FIRE extinguish
			RegKn |= 0x80;
		else
			RegKn &= 0xff7f;
		
		if(PORT_ReadInputDataBit( MDR_PORTC, PORT_Pin_0 ))//FIRE detect
			RegKn |= 0x100;
		else
			RegKn &= 0xfeff;

		if( RegKnOld == RegKn )
		{
			StatusDU.RegSW = RegKn;
		}
		else
			RegKnOld = RegKn;
		
		vTaskDelay( 50 );
	}
}
//==================================================================================
void scroll_mess_left(void)
{
	uint8_t digit=0, row=0, tmp_reg = 0;
	
	tmp_reg = led_reg[0][0];
	
	while(digit<18)
	{
		row = 0;
		while(row < 4)
		{
			led_reg[digit][row] = led_reg[digit][row + 1];
			++row;
		}
		
		led_reg[digit][row] = led_reg[digit+1][0];
		
		++digit;
	}
	led_reg[digit][0] = led_reg[digit][1];
	led_reg[digit][1] = led_reg[digit][2];
	led_reg[digit][2] = led_reg[digit][3];
	led_reg[digit][3] = led_reg[digit][4];
	led_reg[digit][4] = tmp_reg;
}
//==================================================================================
void load_led_reg(void)
{
	uint8_t reg = 0, 
			row = 0,
			digit = 0,
			bit8 = 0,
			bit16 = 0;
	
	
	while( row < 5 )
	{
		reg = 0;
		digit = 0;
		while( reg < 7 )
		//while( reg < 6 )
		{
			led_reg[reg][row] = 0;
			
			bit16 = 0;
			while( (bit16 < 16) && (reg < 7))
			//while( (bit16 < 16) && (reg < 6))
			{
				while( bit8 < 7 )
				{
					++bit8;	
					//led_reg[reg][row] |= ((uint16_t)((tmp >> bit8)&1)) << bit16;
					led_reg[reg][row] |= ((uint16_t)((font5x7[index_char[digit] + row] >> (8-bit8))&1)) << bit16;
					
					if( bit16<15 )
						++bit16;
					else
					{
						bit16 = 0;
						++reg;
						if(reg < 7)
							led_reg[reg][row] = 0;
					}
				}
				bit8 = 0;
				++digit;
			}
			++reg;
		}
		++row;
	}
}
//==================================================================================
void show_led_data( uint8_t num_par )
{
	uint8_t tmp = 0;
	
//	clear_lcd();
	
	switch(num_par)
	{
		case 0:	//ДМ
			index_char[0] = 37*5;//д
			index_char[1] = 45*5;//м
			index_char[2] = 0;
			DecSegm( StatusDG.DM );
			if(!IndData[3])
				index_char[3] = (IndData[3]+16)*5;
			else
				index_char[3] = (IndData[3])*5;
			
			index_char[4] = 14*5;//.
			index_char[5] = IndData[4]*5;
			index_char[6] = 43*5;//k
			index_char[7] = 36*5;//Г
			//TOJ
			index_char[8] = 51*5;//T
			index_char[9] = 47*5;//O
			index_char[10] = 39*5;//J
			index_char[11] = 0;
			
			if( StatusDG.TOJ < 30 )
			{
				tmp = 30 - StatusDG.TOJ;
				DecSegm( tmp );
				index_char[12] = 13*5;
				index_char[13] = IndData[3]*5;
				index_char[14] = IndData[4]*5;
				index_char[15] = 50*5;//C
			}
			else
			{
				tmp = StatusDG.TOJ - 30;
				DecSegm( tmp );
				index_char[12] = IndData[2]*5;
				index_char[13] = IndData[3]*5;
				index_char[14] = IndData[4]*5;
				index_char[15] = 50*5;//C
			}
		
			break;
		case 1:	
			//TOJ
			index_char[0] = 51*5;//T
			index_char[1] = 47*5;//O
			index_char[2] = 39*5;//J
			index_char[3] = 0;
		
			if( StatusDG.TOJ < 30 )
			{
				tmp = 30 - StatusDG.TOJ;
				DecSegm( tmp );
				index_char[4] = 13*5;
				index_char[5] = IndData[3]*5;
				index_char[6] = IndData[4]*5;
				index_char[7] = 50*5;//C
			}
			else
			{
				tmp = StatusDG.TOJ - 30;
				DecSegm( tmp );
				index_char[4] = IndData[2]*5;
				index_char[5] = IndData[3]*5;
				index_char[6] = IndData[4]*5;
				index_char[7] = 50*5;//C
			}
			//NDIZ
			index_char[8] = 68*5;//N
			index_char[9] = 0;
			DecSegm( StatusDG.Ndiz );
			index_char[10] = IndData[1]*5;
			index_char[11] = IndData[2]*5;
			index_char[12] = IndData[3]*5;
			index_char[13] = IndData[4]*5;
			index_char[14] = 47*5;//O
			index_char[15] = 34*5;//б

			break;
		case 2:
			//NDIZ
			index_char[0] = 68*5;//N
			index_char[1] = 0;
			DecSegm( StatusDG.Ndiz );
			index_char[2] = IndData[1]*5;
			index_char[3] = IndData[2]*5;
			index_char[4] = IndData[3]*5;
			index_char[5] = IndData[4]*5;
			index_char[6] = 47*5;//O
			index_char[7] = 34*5;//B
		
			//Ug
			index_char[8] = 65*5;//U
			index_char[9] = 36*5;//g
			index_char[10] = 0;
			index_char[11] = 0;
			DecSegm( StatusDG.UDG );
			index_char[12] = IndData[2]*5;
			index_char[13] = IndData[3]*5;
			index_char[14] = IndData[4]*5;
			index_char[15] = 35*5;//B
			
			break;
		case 3:
			//Ug
			index_char[0] = 65*5;//U
			index_char[1] = 36*5;//g
			index_char[2] = 0;
			index_char[3] = 0;
			DecSegm( StatusDG.UDG );
			index_char[4] = IndData[2]*5;
			index_char[5] = IndData[3]*5;
			index_char[6] = IndData[4]*5;
			index_char[7] = 35*5;//B
			
			//IH
			index_char[8] = 66*5;//I
			index_char[9] = 36*5;//g
			index_char[10] = 0;
			index_char[11] = 0;
			DecSegm( StatusDG.Igen );
			index_char[12] = IndData[2]*5;
			index_char[13] = IndData[3]*5;
			index_char[14] = IndData[4]*5;
			index_char[15] = 33*5;//A
			
			break;
		case 4:
			//IH
			index_char[0] = 66*5;//I
			index_char[1] = 36*5;//g
			index_char[2] = 0;
			index_char[3] = 0;
			DecSegm( StatusDG.Igen );
			index_char[4] = IndData[2]*5;
			index_char[5] = IndData[3]*5;
			index_char[6] = IndData[4]*5;
			index_char[7] = 33*5;//A
		
			//FG
			index_char[8] = 67*5;//F
			index_char[9] = 36*5;//H
			DecSegm( StatusDG.FreqU );
			index_char[10] =0;
			index_char[11] = 0;
			if(!IndData[3])
				index_char[12] = 0;
			else
				index_char[12] = IndData[3]*5;
			
			//index_char[12] = 14*5;//.
			index_char[13] = IndData[4]*5;
			index_char[14] = 36*5;//G
			index_char[15] = 55*5;//Z
			
			break;
		case 5:
			//FG
			index_char[0] = 67*5;//F
			index_char[1] = 36*5;//H
			DecSegm( StatusDG.FreqU );
			index_char[2] = 0;
		index_char[3] = 0;
			if(!IndData[3])
				index_char[4] = 0;
			else
				index_char[4] = IndData[3]*5;
			
			//index_char[4] = 14*5;//.
			index_char[5] = IndData[4]*5;
			index_char[6] = 36*5;//G
			index_char[7] = 55*5;//Z
		
			//Ub
			index_char[8] = 65*5;//U
			index_char[9] = 34*5;//B
			index_char[10] = 0;
			DecSegm( StatusDG.UBS );
			index_char[11] = IndData[2]*5;
				
			if(!IndData[2] && !IndData[3])
				index_char[12] = (IndData[3]+16)*5;
			else
				index_char[12] = IndData[3]*5;
			
			index_char[13] = 14*5;//.
			index_char[14] = IndData[4]*5;
			index_char[15] = 35*5;//B

			break;
		case 6:
			//Ub
			index_char[0] = 65*5;//U
			index_char[1] = 34*5;//B
			index_char[2] = 0;
			DecSegm( StatusDG.UBS );
			index_char[3] = IndData[2]*5;
		
			if(!IndData[2] && !IndData[3])
				index_char[4] = (IndData[3]+16)*5;
			else
				index_char[4] = IndData[3]*5;
			
			index_char[5] = 14*5;//.
			
			index_char[6] = IndData[4]*5;
			index_char[7] = 35*5;//B
		
			//BE
			index_char[8] = 35*5;//B
			index_char[9] = 62*5;//E
			index_char[10] = 0;
			DecSegm( StatusDG.KWH );
			index_char[11] = IndData[0]*5;
			index_char[12] = IndData[1]*5;
			index_char[13] = IndData[2]*5;
			index_char[14] = IndData[3]*5;
			index_char[15] = IndData[4]*5;

			break;
		case 7:
			//ве
			index_char[0] = 35*5;//в
			index_char[1] = 62*5;//е
			index_char[2] = 0;
			DecSegm( StatusDG.KWH );
			index_char[3] = IndData[0]*5;
			index_char[4] = IndData[1]*5;
			index_char[5] = IndData[2]*5;
			index_char[6] = IndData[3]*5;
			index_char[7] = IndData[4]*5;
		
			index_char[8] = 69*5;//risol
			index_char[9] = 41*5;
			index_char[10] = 40*5;
			if(StatusDG.Isol == 200)
				index_char[11] = 30*5;
			else
				index_char[11] = 0;
			DecSegm(  StatusDG.Isol );
			index_char[12] = IndData[2]*5;
			index_char[13] = IndData[3]*5;
			index_char[14] = IndData[4]*5;
			index_char[15] = 43*5;//k
			break;
			
		case 8:
			//risol
			index_char[0] = 69*5;//R
			index_char[1] = 41*5;//И
			index_char[2] = 40*5;//З
			if(StatusDG.Isol == 200)
				index_char[3] = 30*5;
			else
				index_char[3] = 0;
			DecSegm( StatusDG.Isol );
			index_char[4] = IndData[2]*5;
			index_char[5] = IndData[3]*5;
			index_char[6] = IndData[4]*5;
			index_char[7] = 43*5;//k
		
			index_char[8] = 51*5;//ТПл
			index_char[9] = 48*5;
			index_char[10] = 44*5;
			index_char[11] = 0;
			DecSegm( StatusDG.LvlDIZ );
			index_char[12] = IndData[2]*5;
			index_char[13] = IndData[3]*5;
			index_char[14] = IndData[4]*5;
			index_char[15] = 5*5;//%
			break;
		case 9:
			//тпл
			index_char[0] = 51*5;//тпл
			index_char[1] = 48*5;
			index_char[2] = 44*5;
			index_char[3] = 0;
			DecSegm( StatusDG.LvlDIZ );
			index_char[4] = IndData[2]*5;
			index_char[5] = IndData[3]*5;
			index_char[6] = IndData[4]*5;
			index_char[7] = 5*5;//%
		
			index_char[8] = 37*5;//д
			index_char[9] = 45*5;//м
			index_char[10] = 0;
			DecSegm( StatusDG.DM );
			if(!IndData[3])
				index_char[11] = (IndData[3]+16)*5;
			else
				index_char[11] = (IndData[3])*5;
			index_char[12] = 14*5;
			index_char[13] = IndData[4]*5;
			index_char[14] = 43*5;//k
			index_char[15] = 36*5;//Г
			break;
		
		default: break;
	}
	
	load_led_reg();
}
//==================================================================================
void DecSegm( uint16_t R0 )
{
 	uint16_t R1=0;

	R1=R0/10000;
	if(!R1)
		IndData[0]=0;
	else
		IndData[0]=R1+16;
	
	R1=R0%10000;
	R0=R1/1000;
	
	if(!IndData[0] && !R0)
		IndData[1]=0;
	else
		IndData[1]=R0+16;
	
	R0=R1%1000;
	R1=R0/100;
	
	if((!IndData[0] && !IndData[1])&& !R1)
		IndData[2]=0;
	else
		IndData[2]=R1+16;
	
	R1=R0%100;
	R0=R1/10;
	
	if((!IndData[0] && !IndData[1]) && (!IndData[2] && !R0))
		IndData[3]=0;
	else
		IndData[3]=R0+16;
	
	R1=R1%10;
	
	IndData[4]=R1+16;
	
}
//==================================================================================
void OK_PDG(void)
{
	uint8_t cnt_b = 0, i = 0;
	
	while( cnt_b<16 )
	{
		index_char[ cnt_b ] = 0;
		++cnt_b;
	}
	cnt_b = 1;
	
	while( cnt_b < 4 )
	{
		index_char[ cnt_b ] = KDG[ i ];
		++cnt_b;
		++i;
	}

	++cnt_b;
	i = 0;
	while( cnt_b < 7 )
	{
		index_char[cnt_b] = OK[i];
		++cnt_b;
		++i;
	}

	load_led_reg();
}
//==================================================================================
void clear_lcd(void)
{
	uint8_t reg=0, row=0, cnt_b=0;
	
	while(cnt_b<16)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	
	while(reg<7)
	{
		row=0;
		while(row<5)
		{
			led_reg[reg][row] = 0;
			++row;
		}
		++reg;
	}
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
void err_KDG(void)
{
	uint8_t cnt_b=0, i = 0;
	
	while(cnt_b<16)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	
	cnt_b=1;
	while(cnt_b<6)
	{
		index_char[cnt_b] = OTKAZ[i];
		++cnt_b;
		++i;
	}

	cnt_b=10;
	i = 0;
	while(cnt_b<13)
	{
		index_char[cnt_b] = KDG[i];
		++cnt_b;
		++i;
	}

	load_led_reg();
}
//==================================================================================
void only_hmu(void)
{
	uint8_t cnt_b=0, i = 0;
	
	while(cnt_b<16)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	
	cnt_b=0;
	while(cnt_b<7)
	{
		index_char[cnt_b] = TOLKO[i];
		++cnt_b;
		++i;
	}

	cnt_b=8;
	i = 0;
	while(cnt_b<13)
	{
		index_char[cnt_b] = MHU[i];
		++cnt_b;
		++i;
	}

	load_led_reg();
}
//==================================================================================
void UART1_send_byte(uint8_t byte)
{
	UART_SendData(MDR_UART1,byte);
	//time_out_byte=0;
	//while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	//while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY) || !UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	//while(UART_GetFlagStatus (MDR_UART1, UART_FLAG_BUSY))
	while(UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFF));

	UART_ReceiveData (MDR_UART1);
	
	UART_ClearITPendingBit( MDR_UART1,UART_IT_RX );
}

//==================================================================================
bool read_rx_data(uint8_t *Arr)
{
	uint32_t cnt_byte = 0, sum = 0;
	uint32_t tmp32 = 0;
	
	sum = calc_CRC16(Arr,(PDG_FRAME_SIZE-2));
//	while( cnt_byte < PDG_FRAME_SIZE )
//	{
//		sum += (uint8_t)(Arr[cnt_byte]);
//		++cnt_byte;
//	}
	//sum += 0xAA;
	
//	DataUsart.tail -= PDG_FRAME_SIZE;
//	DataUsart.count -= PDG_FRAME_SIZE;
	
	//if((!sum) && ((Arr[1]&0xFE)==0xFE))
	if(sum == (uint16_t)(Arr[PDG_FRAME_SIZE-2]|(Arr[PDG_FRAME_SIZE-1]<<8)))
//if(0)
	{									//26
		__disable_irq();
		StatusDG.Ndiz = Arr[2]|((uint16_t)Arr[3]<<8);
		StatusDG.FreqU = Arr[4] |((uint16_t)Arr[5]<<8);
		StatusDG.UDG = (Arr[6])|((uint16_t)Arr[7]<<8);
		StatusDG.UBS = Arr[8]|((uint16_t)Arr[9]<<8);
		StatusDG.Igen = (Arr[10])|((uint16_t)Arr[11]<<8);
		StatusDG.DM = Arr[12];
		StatusDG.TOJ = Arr[13];
		StatusDG.TBap = Arr[14];
		StatusDG.LvlDIZ = Arr[15];
		StatusDG.RegWrk = (Arr[16])|((uint16_t)Arr[17]<<8);
		StatusDG.Isol = Arr[18];
		StatusDG.Led = Arr[19];
		StatusDG.RegSDG = (Arr[20])|((uint16_t)Arr[21]<<8);
		StatusDG.Error = (Arr[22])|((uint16_t)Arr[23]<<8);
		StatusDG.RegimTime = (Arr[24])|((uint16_t)Arr[25]<<8);
		StatusDG.RegimError = (Arr[26])|((uint16_t)Arr[27]<<8);
		tmp32 = (Arr[28]) | ((uint32_t)Arr[29]<<8) | ((uint32_t)Arr[30]<<16) | ((uint32_t)Arr[31]<<24);
		if(tmp32>99999)
			tmp32 = 99999;
		StatusDG.KWH = tmp32;
		__enable_irq();

		if( (Arr[1]&1) == PU_DPU )
		{
			return true;
		}
	}
	
	return false;
}
//==================================================================================
void Show_led_str(const uint16_t *str, uint8_t n_str, uint8_t len)
{
	uint8_t cnt_b = 0, i = 0;
	
	if( n_str == 0 )
	{
		while( cnt_b < 8 )
		{
			index_char[ cnt_b ] = 0;
			++cnt_b;
		}
		
		cnt_b = 0;
		while(cnt_b < len)
		{
			index_char[cnt_b] = str[cnt_b];
			++cnt_b;
		}
	}
	else
	{
		cnt_b = 8;
		while( cnt_b < 16 )
		{
			index_char[ cnt_b ] = 0;
			++cnt_b;
		}
		cnt_b = 8;
		while(i < len)
		{
			index_char[cnt_b] = str[i];
			++cnt_b;
			++i;
		}
	}

	load_led_reg();
}
//==================================================================================
uint16_t calc_CRC16( uint8_t *buf, uint16_t num )
{
 uint8_t CRC_lo = 0, CRC_hi = 0, index = 0;
	
	 CRC_lo = 255;
	 CRC_hi = 255;
	
 while( num-- )
	{
		index = CRC_lo ^ (*buf);
		CRC_lo = CRC_hi ^ (high[index]);
		CRC_hi = low[index];
		buf++;
	}
 /* сначала `lo`, потом `hi` */
 return ((uint16_t)CRC_lo<<8)|CRC_hi;
}

//==================================================================================



//==================================================================================


