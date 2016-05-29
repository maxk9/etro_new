/*
*PI18 ESDA
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
#include "MDR32F9Qx_i2c.h"

#include <stdbool.h>

#include <string.h>
//#include <stdlib.h>
//#include "i2c.h"
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



//#define TEST_OFF	PORT_ResetBits( MDR_PORTA, PORT_Pin_4 )
#define TEST_OFF	(MDR_PORTA->RXTX &= (!PORT_Pin_4))
#define TEST_ON		(MDR_PORTA->RXTX  |= (PORT_Pin_4))
//#define TEST_ON		PORT_SetBits( MDR_PORTA, PORT_Pin_4 )

#define I2C_PORT	MDR_PORTC	// ÐŸÐ¾Ñ€Ñ‚ Ð½Ð° ÐºÐ¾Ñ‚Ð¾Ñ€Ð¾Ð¼ Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐµÑ‚ÑÑ I2C
#define I2C_SCL		PORT_Pin_0	// ÐŸÐ¸Ð½ SCL
#define I2C_SDA		PORT_Pin_1	// ÐŸÐ¸Ð½ SDA


#define I2C_DELAY()	{ __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop();	\
						__nop(); __nop(); __nop(); __nop(); __nop();__nop();}	// ÐžÐ±Ñ‰Ð°Ñ Ð¿Ð°ÑƒÐ·Ð° Ð½Ð° ÑˆÐ¸Ð½Ðµ

#define  LCD_DIS 		PORT_ResetBits(MDR_PORTA, PORT_Pin_1 )
#define  LCD_EN 		PORT_SetBits(MDR_PORTA, PORT_Pin_1 )
#define  LCD_ALL_ROW_OFF 	{PORT_ResetBits(MDR_PORTD, (PORT_Pin_4));	\
							PORT_ResetBits(MDR_PORTE, (PORT_Pin_0|PORT_Pin_1|PORT_Pin_6|PORT_Pin_7));}

							
#define  LCD_ROW1_ON 	PORT_SetBits(MDR_PORTD, (PORT_Pin_4))
#define  LCD_ROW2_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_6))
#define  LCD_ROW3_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_7))					
#define  LCD_ROW4_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_0))
#define  LCD_ROW5_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_1))
//size_t fre=0;
		

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

							
const uint32_t	LG19U6[]    = {0, 0  ,0   ,0   ,17*5 ,25*5 ,52*5  ,22*5 } ,
				LGNIOBII[]  = {0, 0  ,0   ,46*5,41*5,47*5,34*5  ,41*5  ,42*5 },
				KHU[]       = {0, 0  ,0   ,0   ,43*5,57*5,52*5 },
				NO[]        = {0, 0  ,0   ,0   ,46*5,38*5,51*5 },
				MASLA[]     = {0,0   ,0   ,45*5,33*5,50*5,44*5  ,33*5 },
				TOPLIVA[]   = {0, 0  ,51*5,47*5,48*5,44*5,41*5  ,35*5  ,33*5} ,
				PUSKA[]     = {0, 0  ,0   ,48*5,52*5,50*5,43*5	,33*5 },
				OBOROTOV[]  = {0, 0  ,47*5,34*5,47*5,49*5,47*5	,51*5 ,47*5 ,35*5 },
				VOZB[]		= {0, 0  ,0   ,0   ,35*5,47*5,40*5  ,34*5  ,14*5 },
				OBRAT[]		= {0, 0  ,0   ,47*5,34*5,49*5,33*5  ,51*5 },
				MOCH[]		= {0, 0  ,0   ,45*5,47*5,58*5,46*5 },
				FIRE[]		= {0, 0  ,0   ,48*5,47*5,39*5,33*5  ,49*5  },
				DOZAPRAVKA[]= {0,37*5,47*5,40*5,33*5,48*5,49*5  ,33*5  ,35*5  ,43*5 ,33*5},
				NIZKAYA[]   = {0,0   ,0   ,46*5,41*5,40*5,43*5  ,33*5  ,64*5} ,
				ISOLYACIA[] = {0,0   ,41*5,40*5,47*5,44*5,64*5  ,55*5  ,41*5  ,64*5},
				AVARIA[]    = {0,0   ,0   ,33*5,35*5,33*5,49*5  ,41*5  ,64*5} ,
				DVIGATELA[] = {0,37*5,35*5,41*5,36*5,33*5,51*5  ,38*5  ,44*5  ,64*5},
				NO_NORMA[]  = {0,0   ,46*5,38*5,0   ,46*5,47*5  ,49*5  ,45*5  ,33*5},
				NEISPRAV[]  = {0,0   ,46*5,38*5,41*5,50*5,48*5  ,49*5  ,33*5  ,35*5 ,14*5},
				VG[]        = {0,0   ,0   ,0   ,0   ,35*5,36*5 },
				VS[]        = {0,0   ,0   ,0   ,0   ,35*5,50*5 },
				FVU[]       = {0,0   ,0   ,0   ,53*5,35*5,52*5 },
				UPPA[]      = {0,0   ,0   ,0   ,52*5,48*5,48*5  ,33*5} ,
				LUKA[]      = {0,0   ,0   ,0   ,44*5,63*5,43*5  ,33*5} ,
				NA_KORP[]   = {0,0   ,46*5,33*5,0   ,43*5,47*5  ,49*5  ,48*5  ,14*5},
				GOTOV[]     = {0,0   ,0   ,36*5,47*5,51*5,47*5  ,35*5  ,46*5} ,
				WORK[]      = {0,0   ,0   ,49*5,33*5,34*5,47*5  ,51*5  ,33*5} ,
				TOJ_8[]     = {0,0   ,0   ,51*5,47*5,39*5,28*5  ,24*5  ,50*5} ,
				TOJ_37[]    = {0,0   ,0   ,51*5,47*5,39*5,28*5  ,19*5  ,23*5  ,50*5} ,
				PROTECT[]   = {0,0   ,0   ,40*5,33*5,58*5,41*5  ,51*5  ,33*5} ,
				OTKL[]      = {0,0   ,0   ,0   ,47*5,51*5,43*5  ,44*5} ,
				SYSTEM[]    = {0,0   ,50*5,41*5,50*5,51*5,38*5  ,45*5  ,60*5} ,
				SETI[]      = {0,0   ,0   ,0   ,50*5,38*5,51*5  ,41*5} ,
				MU[]        = {0,0   ,0   ,0   ,0   ,45*5,52*5 },
				DU[]        = {0,0   ,0   ,0   ,0   ,37*5,52*5 },
				OBRYV[]     = {0,0   ,0   ,47*5,34*5,49*5,60*5  ,35*5} ,
				SVYAZ_SN[]  = {0,0   ,50*5,35*5,64*5,40*5,14*5  ,57*5  ,50*5  ,46*5},
				SVYAZ_MSHU[]= {0,0   ,50*5,35*5,64*5,40*5,14*5  ,45*5  ,58*5  ,52*5},
				SVYAZ_RN[]  = {0,0   ,50*5,35*5,64*5,40*5,14*5  ,49*5  ,46*5} ,
				PEREGRUZ[]  = {0,0   ,48*5,38*5,49*5,38*5,36*5  ,49*5  ,52*5  ,40*5};		
					
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
					led_reg[12][5],
					index_char[24];

uint8_t				data_tx[14],
					data_rx[14],
					sec_05 = 0,
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
void LoadControlSum(void);		
void LoadRegTransmit(void);					

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
void scroll_mess_left( void );
void load_led_reg( void );
void show_led_data( uint8_t num_par );
void DecSegm( uint16_t R0 );
void clear_lcd( void );
bool read_rx_data( uint8_t *Arr );
void Show_led_str( const uint32_t *str, uint8_t n_str, uint8_t len );
uint16_t calc_CRC16( uint8_t *buf, uint16_t num );
uint8_t read_byte_I2C();


//==================================================================================
//==================================================================================
//==================================================================================
//==================================================================================
void delay_4ms( uint16_t delay )
{
	delay_count=delay;
	while(delay_count);
}


//==================================================================================
void Timer1_IRQHandler( void )//3.125ms
{
	static uint8_t 	cnt_row = 0;
	uint8_t reg = 12;
	
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
void I2C_IRQHandler( void )
{
	
}
//==================================================================================

int main(void)
{
	
	uint32_t pausa = 0xFFFF;
	uint8_t cnt_b = 0, i = 0, addr = 0, state = 0, read_d[10];
	bool stop = false, st_one = false;
	
	while( --pausa );
	
	prvSetupHardware();

	
	while( cnt_b < 24 )
	{
		index_char[ cnt_b ] = 6*5; //FF
		++cnt_b;
	}
	load_led_reg();
	
	sec_05 = 0; 
	while( sec_05<2 );
	clear_lcd();
	
	Show_led_str(KHU,0,7);
	Show_led_str(LGNIOBII,1,9);	

//	xTaskCreate( vIndikatorTask,  "Indikator", (configMINIMAL_STACK_SIZE), NULL, 3,( xTaskHandle * ) NULL);
//	xTaskCreate( vLink_SysTask,  "Link_Sys", (configMINIMAL_STACK_SIZE), NULL, 3,( xTaskHandle * ) NULL);

////	/* Start the scheduler. */
//	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	
	for(cnt_b=0;cnt_b<14;cnt_b++)
		data_tx[cnt_b] = cnt_b;
	
	for(cnt_b=0;cnt_b<14;cnt_b++)
		data_rx[cnt_b] = 0;
	
	while(1)
	{
		if((I2C_PORT->RXTX & I2C_SDA)==0)
		{
			I2C_DELAY();
			if(I2C_PORT->RXTX & I2C_SCL)//detect start
			{
				__disable_irq();
				LCD_ALL_ROW_OFF;
				while(I2C_PORT->RXTX & I2C_SCL);
				
				if(read_byte_I2C())
				{
					I2C_DELAY();
					for(cnt_b=0;cnt_b<14;cnt_b++)
						data_rx[cnt_b] = 0;
				}
				__enable_irq();
			}
		}
	}
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
//	UART_InitTypeDef UART_InitStructure;
	SSP_InitTypeDef SPI_InitStructure;
	TIMER_CntInitTypeDef sTIM_CntInit;
	I2C_InitTypeDef I2C_InitStructure;
//CLK
/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig( RST_CLK_HSE_ON );
	
	while( RST_CLK_HSEstatus() == ERROR );

	
	/* Configure CPU_PLL clock */
	RST_CLK_CPU_PLLconfig ( RST_CLK_CPU_PLLsrcHSEdiv1,RST_CLK_CPU_PLLmul4 );	//64Mhz
	RST_CLK_CPU_PLLcmd( ENABLE );
	
	while( RST_CLK_CPU_PLLstatus() == ERROR);
	
	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
	
	
	RST_CLK_CPU_PLLuse( ENABLE);
	
	RST_CLK_HSIcmd( DISABLE );


/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd( ALL_PORTS_CLK, ENABLE );

//I2C
//15.5 KHz
	//adress = 4
	//Acknowledge ON
	//Enable
	I2C_StructInit( &I2C_InitStructure );
	
	RST_CLK_PCLKcmd( RST_CLK_PCLK_I2C, ENABLE );
	
	PORT_StructInit( &PORT_InitStructure );//reset struct
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_1 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_OPEN;
	//PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	PORT_SetBits(I2C_PORT,I2C_SDA);
	
	PORT_StructInit( &PORT_InitStructure );//reset struct
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 );
	//PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);
//	
//	//I2C_InitStructure.I2C_ClkDiv = 107;	//53.76 KHz
//	I2C_InitStructure.I2C_ClkDiv = 98;	//59.52 KHz
//	I2C_InitStructure.I2C_Speed = I2C_SPEED_UP_TO_400KHz;

//	I2C_Init( &I2C_InitStructure );
//		
////	NVIC_EnableIRQ( I2C_IRQn );
////	I2C_ITConfig( ENABLE );
//	
//	I2C_Cmd( ENABLE );
	
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
	sTIM_CntInit.TIMER_Period                   = 100;//1.56 ms
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
	
	/************************ UART2 Initialization *************************/
//	
//	RST_CLK_PCLKcmd( RST_CLK_PCLK_UART2, ENABLE );
//	PORT_StructInit( &PORT_InitStructure );//reset struct

//	//PORT_InitStructure.PORT_Pin = PORT_Pin_5|PORT_Pin_6;//RX TX
//	PORT_InitStructure.PORT_Pin = ( PORT_Pin_0 | PORT_Pin_1 );//RX TX
//	PORT_InitStructure.PORT_FUNC = PORT_FUNC_OVERRID;
//	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
//	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
//	PORT_Init( MDR_PORTF, &PORT_InitStructure );

//	
//	/* Set the HCLK division factor = 1 for UART1,UART2*/
//	UART_BRGInit( MDR_UART2, UART_HCLKdiv1 );
//	UART_DeInit( MDR_UART2 );
//	
//	/* Disable interrupt on UART2 */
//    NVIC_DisableIRQ( UART2_IRQn );
//	
//	/* Initialize UART_InitStructure */
//	UART_InitStructure.UART_BaudRate			= 115200;
//	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
//	UART_InitStructure.UART_StopBits			= UART_StopBits2;
//	UART_InitStructure.UART_Parity				= UART_Parity_No;
//	UART_InitStructure.UART_FIFOMode			= UART_FIFO_ON;
//	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

//	/* Configure UART1 parameters*/
//	UART_Init( MDR_UART2,&UART_InitStructure );

//	MDR_UART2->IFLS = (0x04<<3);//7/8 full FIFO

//	NVIC_EnableIRQ( UART2_IRQn );
//	UART_ITConfig( MDR_UART2, UART_IT_RX, ENABLE );
//	/* Enables UART2 peripheral */
//	//UART_Cmd( MDR_UART1, ENABLE );
//	UART_Cmd( MDR_UART2, DISABLE );
	
//PORT
	
	PORT_StructInit( &PORT_InitStructure );//reset struct	
 /* Configure PORTA pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_1 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTA, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_4 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTA, &PORT_InitStructure );

 /* Configure PORTB pins */
//	PORT_InitStructure.PORT_Pin   = (PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_9 | PORT_Pin_10);
//	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
//	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
//	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
//	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
//	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

//	PORT_Init(MDR_PORTB, &PORT_InitStructure);

 /* Configure PORTC pins */
PORT_StructInit( &PORT_InitStructure );//reset struct	

 /* Configure PORTD pins */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
 /* Configure PORTE pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_6 | PORT_Pin_7);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);

 /* Configure PORTF pins */
// 	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_2 | PORT_Pin_4  | PORT_Pin_6 );
//	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
//	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
//	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
//	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
//	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
//	
//	PORT_Init(MDR_PORTF, &PORT_InitStructure);
//	
//	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_3 | PORT_Pin_5 );
//	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
//	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
//	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
//	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
//	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

//	PORT_Init(MDR_PORTF, &PORT_InitStructure);
	
}

//==================================================================================
void vIndikatorTask( void *pvParameters )
{
//	bool first_link_ok = false,
//		first_link_err = false;
//	uint8_t tmp = 0;
	uint8_t num_mes = 19;
//			i = 0; 
	
	
	for( ;; )
	{
		
		switch( num_mes)
		{
			case 0:
				Show_led_str(NO,0,7);
				Show_led_str(MASLA,1,8);		
				break;
			case 1:
				Show_led_str(NO,0,7);
				Show_led_str(TOPLIVA,1,9);	
				break;
			case 2:
				Show_led_str(DOZAPRAVKA,0,11);
				Show_led_str(MASLA,1,8);		
				break;
			case 3:
				Show_led_str(DOZAPRAVKA,0,11);
				Show_led_str(TOPLIVA,1,9);	
				break;
			case 4:
				Show_led_str(NIZKAYA,0,9);
				Show_led_str(ISOLYACIA,1,10);		
				break;
			case 5:
				Show_led_str(AVARIA,0,9);
				Show_led_str(DVIGATELA,1,10);		
				break;
			case 6:
				index_char[5] = 65*5;	//U
				Show_led_str(NO_NORMA,1,10);		
				break;
			case 7:
				Show_led_str(NEISPRAV,0,11);
				Show_led_str(VG,1,7);		
				break;
			case 8:
				Show_led_str(NEISPRAV,0,11);
				Show_led_str(VS,1,7);		
				break;
			case 9:
				index_char[5] = 65*5;	//U
				Show_led_str(NA_KORP,1,10);		
				break;
			case 10:
				Show_led_str(GOTOV,0,9);
				Show_led_str(SYSTEM,1,9);		
				break;
			case 11:
				Show_led_str(NEISPRAV,0,11);
				Show_led_str(SETI,1,8);		
				break;
			case 12:
				Show_led_str(GOTOV,0,9);
				Show_led_str(MU,1,7);
				break;
			case 13:
				Show_led_str(GOTOV,0,9);
				Show_led_str(DU,1,7);	
				break;
			case 14:
				Show_led_str(OBRYV,0,8);
				Show_led_str(SVYAZ_SN,1,10);		
				break;
			case 15:
				Show_led_str(OBRYV,0,8);
				Show_led_str(SVYAZ_MSHU,1,10);		
				break;
			case 16:
				Show_led_str(OBRYV,0,8);
				Show_led_str(SVYAZ_RN,1,9);		
				break;
			case 17:
				Show_led_str(PEREGRUZ,0,10);
				break;
			case 18:
				Show_led_str(KHU,0,7);
				Show_led_str(LG19U6,1,8);		
				break;
			case 19:
				Show_led_str(KHU,0,7);
				Show_led_str(LGNIOBII,1,9);		
				break;
			case 20:
				Show_led_str(NO,0,7);
				Show_led_str(PUSKA,1,8);		
				break;
			case 21:
				Show_led_str(NO,0,7);
				Show_led_str(OBOROTOV,1,10);		
				break;
			case 22:
				Show_led_str(NO,0,7);
				Show_led_str(VOZB,1,9);		
				break;
			case 23:
				Show_led_str(OBRAT,0,8);
				Show_led_str(MOCH,1,7);		
				break;
			case 24:
				Show_led_str(FIRE,0,8);
				//Show_led_str(,1,9);		
				break;
			case 25:
				Show_led_str(NEISPRAV,0,11);
				Show_led_str(FVU,1,7);		
				break;
			case 26:
				Show_led_str(WORK,0,9);
				//Show_led_str(,1,9);		
				break;
			case 27:
				Show_led_str(NEISPRAV,0,11);
				Show_led_str(UPPA,1,8);		
				break;
			case 28:
				Show_led_str(NEISPRAV,0,11);
				Show_led_str(LUKA,1,8);		
				break;
			case 29:
				Show_led_str(TOJ_8,0,9);
				
				break;
			case 30:
				Show_led_str(TOJ_37,0,10);
				break;
			case 31:
				Show_led_str(PROTECT,0,9);
				Show_led_str(OTKL,1,8);			
				break;
			default: 
				break;
		}
		
//		if(num_mes<31)
//			++num_mes;
//		else
//			num_mes =0 ;
		
		vTaskDelay(500);
		clear_lcd();
	}
}
//==================================================================================
#define ADRESS		0x04
#define TRANS_COUNT 10

void vLink_SysTask( void *pvParameters )
{
#define ARR_SIZE	6
 	//bool	start = false;// lnk_ok = false;
	uint8_t //sum=0, 
			//byte[ARR_SIZE], 
			addr=0,
			i=0,// k=0, b=0,
			rx_data[TRANS_COUNT];

 	for( ;; )
 	{
//		//vTaskDelay( 4 );
		//search start condition
		if(PORT_ReadInputDataBit(I2C_PORT,I2C_SDA) == 0)
		{
			taskENTER_CRITICAL();
			
			I2C_DELAY();
			if(PORT_ReadInputDataBit(I2C_PORT,I2C_SCL))
			{
				while (PORT_ReadInputDataBit(I2C_PORT,I2C_SCL));
				addr=0;
				//detect start
				i=0;
				while(i<8)
				{
					while (!PORT_ReadInputDataBit(I2C_PORT,I2C_SCL));
					
					if(PORT_ReadInputDataBit(I2C_PORT,I2C_SDA))
						addr |= 1<<i;
					
					while (PORT_ReadInputDataBit(I2C_PORT,I2C_SCL));
					++i;
				}
				//if(addr == 4) //send ACK
				{
					while (!PORT_ReadInputDataBit(I2C_PORT,I2C_SCL));
					
					PORT_ResetBits(I2C_PORT,I2C_SDA);
					
					while (PORT_ReadInputDataBit(I2C_PORT,I2C_SCL));
					I2C_DELAY();
					while (!PORT_ReadInputDataBit(I2C_PORT,I2C_SCL));
					PORT_SetBits(I2C_PORT,I2C_SDA);
				}
			}
			taskEXIT_CRITICAL();
		}
	}
}

//==================================================================================
void scroll_mess_left( void )
{
//	uint8_t	reg = 0, 
//			row = 0,
//			bit8 = 0,
//			bit16 = 0;
	//uint32_t tmp_led_reg[12][5];
//	row = 0;
//	while( row < 5 )
//	{
//		reg = 0;
//		while( reg < 12 )
//		{
//			tmp_led_reg[reg][row]=0;
//			++reg;
//		}
//		++row;
//	}
//	
//	row = 5;
//	while( row )
//	{
//		reg = 0;
//		while( reg < 12 )
//		{
//			led_reg[reg][row-1] = led_reg[reg][row];
//			++reg;	
//		}
//		--row;
//	}
}
//==================================================================================
void load_led_reg( void )
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
		while( reg < 11 )
		{
			led_reg[reg][row] = 0;
			
			bit16 = 0;
			while((bit16 < 16) && (reg < 11))
			{
				while( bit8 < 7 )
				{
					++bit8;	
					//led_reg[reg][row] |= ((uint16_t)((tmp >> bit8)&1)) << bit16;
					led_reg[ reg ][ row ] |= ((uint16_t)((font5x7[index_char[ digit ] + row] >> (8-bit8))&1)) << bit16;
					
					if( bit16 < 15 )
						++bit16;
					else
					{
						bit16 = 0;
						++reg;
						if( reg < 12 )
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
		case 0:	//ÄÌ
			index_char[0] = 37*5;//ä
			index_char[1] = 45*5;//ì
			index_char[2] = 0;
			DecSegm( StatusDG.DM );
			if(!IndData[3])
				index_char[3] = (IndData[3]+16)*5;
			else
				index_char[3] = (IndData[3])*5;
			
			index_char[4] = 14*5;//.
			index_char[5] = IndData[4]*5;
			index_char[6] = 43*5;//k
			index_char[7] = 36*5;//Ã
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
			index_char[15] = 34*5;//á

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
			//âå
			index_char[0] = 35*5;//â
			index_char[1] = 62*5;//å
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
			index_char[1] = 41*5;//È
			index_char[2] = 40*5;//Ç
			if(StatusDG.Isol == 200)
				index_char[3] = 30*5;
			else
				index_char[3] = 0;
			DecSegm( StatusDG.Isol );
			index_char[4] = IndData[2]*5;
			index_char[5] = IndData[3]*5;
			index_char[6] = IndData[4]*5;
			index_char[7] = 43*5;//k
		
			index_char[8] = 51*5;//ÒÏë
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
			//òïë
			index_char[0] = 51*5;//òïë
			index_char[1] = 48*5;
			index_char[2] = 44*5;
			index_char[3] = 0;
			DecSegm( StatusDG.LvlDIZ );
			index_char[4] = IndData[2]*5;
			index_char[5] = IndData[3]*5;
			index_char[6] = IndData[4]*5;
			index_char[7] = 5*5;//%
		
			index_char[8] = 37*5;//ä
			index_char[9] = 45*5;//ì
			index_char[10] = 0;
			DecSegm( StatusDG.DM );
			if(!IndData[3])
				index_char[11] = (IndData[3]+16)*5;
			else
				index_char[11] = (IndData[3])*5;
			index_char[12] = 14*5;
			index_char[13] = IndData[4]*5;
			index_char[14] = 43*5;//k
			index_char[15] = 36*5;//Ã
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
void clear_lcd( void )
{
	uint8_t reg=0, row=0, cnt_b=0;
	
	while(cnt_b<24)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	
	while(reg<12)
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
//==================================================================================
void UART1_send_byte( uint8_t byte )
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
bool read_rx_data( uint8_t *Arr )
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
void Show_led_str( const uint32_t *str, uint8_t n_str, uint8_t len )
{
	uint8_t cnt_b = 0, i = 0;
	
	if( n_str == 0 )
	{
		while( cnt_b < 12 )
		{
			index_char[ cnt_b ] = 0;
			++cnt_b;
		}
		
		cnt_b = 0;
		while( cnt_b < len )
		{
			index_char[ cnt_b ] = str[ cnt_b ];
			++cnt_b;
		}
	}
	else
	{
		cnt_b = 12;
		while( cnt_b < 24 )
		{
			index_char[ cnt_b ] = 0;
			++cnt_b;
		}
		cnt_b = 12;
		while( i < len )
		{
			index_char[ cnt_b ] = str[i];
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
 /* ñíà÷àëà `lo`, ïîòîì `hi` */
 return ((uint16_t)CRC_lo<<8)|CRC_hi;
}

//==================================================================================
uint8_t read_byte_I2C()
{
	uint8_t bit = 0, byte = 0, r_adr=0;
	uint32_t cnt_p=0, cnt_p_cpy=0 ;
	bool read=false;
	
	while( bit < 8 )
	{
		cnt_p=0;
		while(!( I2C_PORT->RXTX & I2C_SCL ))
		{
			++cnt_p;
		}
	//	TEST_ON;
		
		if(( I2C_PORT->RXTX & I2C_SDA ))
			r_adr |= 1<<(7-bit);
		
		if( bit == 7 )
		{
			if((r_adr==4) || (r_adr==5)) //addr=2 +rw
			{
				while( I2C_PORT -> RXTX & I2C_SCL );
				TEST_ON;
				I2C_PORT -> RXTX &= !I2C_SDA;//ACK
				cnt_p *= 2;
				break;
			}
			else
			{
				I2C_PORT -> RXTX |= I2C_SDA;
				TEST_OFF;
				return 0;
			}
		}
		else
			while( I2C_PORT -> RXTX & I2C_SCL )
			{
				++cnt_p;
			}
		I2C_DELAY();//1us
		TEST_OFF;
		++bit;
	}
	
	
	cnt_p_cpy = cnt_p;
	while(cnt_p)
	{
		--cnt_p;
		if((I2C_PORT->RXTX & I2C_SCL))
		{
			break;
		}
	}
	
	while( cnt_p )
	{
		--cnt_p;
		if(!(I2C_PORT->RXTX & I2C_SCL))
		{
			break;
		}
	}
	I2C_PORT -> RXTX |= I2C_SDA;
	TEST_OFF;

	if( !cnt_p )
	{
		return 0;
	}
	
	
	if( r_adr == 4 )//write
	{
		while(byte<11)
		{
			bit = 0;
			while( bit < 9 )
			{
				cnt_p = cnt_p_cpy*2;
				while( cnt_p )
				{
					--cnt_p;
					if(I2C_PORT->RXTX & I2C_SCL)
					{
						break;
					}
				}
				if( !cnt_p )
				{
					I2C_PORT -> RXTX |= I2C_SDA;
					return 0;
				}
				
				TEST_ON;
				
				if(( I2C_PORT->RXTX & I2C_SDA ) && ( bit != 8 ))
					data_rx[byte] |= 1<<(7-bit);
				
				while( cnt_p )
				{
					--cnt_p;
					if(!(I2C_PORT->RXTX & I2C_SCL))
					{
						break;
					}
				}
				
				++bit;
				
				if( bit == 8 )
				{
					I2C_PORT -> RXTX &= !I2C_SDA;//ACK
				}
				else
				{
					I2C_PORT -> RXTX |= I2C_SDA;
				}
				
				TEST_OFF;
				
				if( !cnt_p )
				{
					I2C_PORT -> RXTX |= I2C_SDA;
					return 0;
				}
			}
			++byte;
		}
	}
	else//(r_adr==5) //read
	{
		byte=0;
		while(byte<13)
		{
			bit = 0;
			while( bit<9 )
			{
				cnt_p = cnt_p_cpy*2;
				if(bit != 8)
				{
					if(((data_tx[byte]>>(7-bit))&1))
						I2C_PORT -> RXTX |= I2C_SDA;
					else
						I2C_PORT -> RXTX &= !I2C_SDA;
				}
				else
				{
					I2C_PORT -> RXTX |= I2C_SDA;
				}
					
				
				while(cnt_p)
				{
					--cnt_p;
					if((I2C_PORT->RXTX & I2C_SCL))
					{
						break;
					}
				}
				
				while( cnt_p )
				{
					--cnt_p;
					if(!(I2C_PORT->RXTX & I2C_SCL))
					{
						break;
					}
				}
				++bit;
			}
			++byte;
		}
	}
	
	
	
	
	
	
	
	
	
	
	I2C_PORT -> RXTX |= I2C_SDA;
	
	return true;
}


//==================================================================================
void LoadControlSum(void)
{
//	unsigned char R0;
//	RegTransmit[0]=0;
//	for(R0=1;R0<=10;++R0)
//		RegTransmit[0] +=RegTransmit[R0];
}  
//==================================================================================
void LoadRegTransmit(void)
{
	//   unsigned char R0;
	//	++TestTwi;    
	//    for(R0=1;R0<=10;++R0)
//	RegTransmit[7]=TestTWI;
//	RegTransmit[6]=3;
//	RegTransmit[5]=4;
//	RegTransmit[4]=15;
//	RegTransmit[3]=16;
//	RegTransmit[2]=17;
//	RegTransmit[1]=RegS;//TestTWI;
	/*    RegTransmit[7]=RegTransmitUsart[7];
	RegTransmit[6]=RegTransmitUsart[8];
	RegTransmit[5]=RegTransmitUsart[9];
	RegTransmit[4]=RegTransmitUsart[10];
	RegTransmit[3]=RegTransmitUsart[11];
	RegTransmit[2]=RegTransmitUsart[12];*/
}
//==================================================================================
