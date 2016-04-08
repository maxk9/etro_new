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
#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_adc.h"
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

while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}


#endif







const char data_str[]=__DATE__;//mmm dd yyyy

#define EEPROM_ADD	(uint32_t)0x08000000	//page0



#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTA, PORT_Pin_2)
#define RS485_TX_ON		PORT_SetBits(MDR_PORTA, PORT_Pin_2)

#define K1_OFF			PORT_ResetBits(MDR_PORTC, PORT_Pin_1)
#define K1_ON			PORT_SetBits(MDR_PORTC, PORT_Pin_1)
#define K2_OFF			PORT_ResetBits(MDR_PORTA, PORT_Pin_5)
#define K2_ON			PORT_SetBits(MDR_PORTA, PORT_Pin_5)
#define K3_OFF			PORT_ResetBits(MDR_PORTF, PORT_Pin_3)
#define K3_ON			PORT_SetBits(MDR_PORTF, PORT_Pin_3)
#define K4_OFF			PORT_ResetBits(MDR_PORTF, PORT_Pin_5)
#define K4_ON			PORT_SetBits(MDR_PORTF, PORT_Pin_5)
#define K5_OFF			PORT_ResetBits(MDR_PORTF, PORT_Pin_6)
#define K5_ON			PORT_SetBits(MDR_PORTF, PORT_Pin_6)
#define K6_OFF			PORT_ResetBits(MDR_PORTE, PORT_Pin_0)
#define K6_ON			PORT_SetBits(MDR_PORTE, PORT_Pin_0)
#define K6_TOGLE		MDR_PORTE->RXTX ^= PORT_Pin_0	
#define K7_OFF			PORT_ResetBits(MDR_PORTE, PORT_Pin_1)
#define K7_ON			PORT_SetBits(MDR_PORTE, PORT_Pin_1)
#define K8_OFF			PORT_ResetBits(MDR_PORTE, PORT_Pin_7)
#define K8_ON			PORT_SetBits(MDR_PORTE, PORT_Pin_7)
#define K9_OFF			PORT_ResetBits(MDR_PORTB, PORT_Pin_10)
#define K9_ON			PORT_SetBits(MDR_PORTB, PORT_Pin_10)
#define K10_OFF			PORT_ResetBits(MDR_PORTC, PORT_Pin_2)
#define K10_ON			PORT_SetBits(MDR_PORTC, PORT_Pin_2)
#define K11_OFF			PORT_ResetBits(MDR_PORTA, PORT_Pin_4)
#define K11_ON			PORT_SetBits(MDR_PORTA, PORT_Pin_4)
#define K11_TOGGLE		MDR_PORTA->RXTX ^= PORT_Pin_4
#define K12_OFF			PORT_ResetBits(MDR_PORTF, PORT_Pin_0)
#define K12_ON			PORT_SetBits(MDR_PORTF, PORT_Pin_0)
#define K13_OFF			PORT_ResetBits(MDR_PORTF, PORT_Pin_1)
#define K13_ON			PORT_SetBits(MDR_PORTF, PORT_Pin_1)

#define TPR_UP_ON		PORT_ResetBits(MDR_PORTF, PORT_Pin_2)
#define TPR_UP_OFF		PORT_SetBits(MDR_PORTF, PORT_Pin_2)
#define TPR_DOWN_ON		PORT_ResetBits(MDR_PORTF, PORT_Pin_4)
#define TPR_DOWN_OFF	PORT_SetBits(MDR_PORTF, PORT_Pin_4)


volatile struct 
{
	uint16_t Ndiz;		//0
	uint16_t FreqU;		//1
	uint16_t UDG;		//2
	uint16_t UBS;		//3
	uint16_t Igen;		//4
	uint8_t DM;			//5
	uint8_t TOJ;		//6
	uint8_t TBap;		//7
	uint8_t LvlDIZ;		//8
	uint16_t RegWrk;	//9
	uint8_t Isol;		//10
	uint8_t Led;		//11
	uint16_t RegSDG;	//12
	uint16_t Error;		//13
	uint16_t RegimTime; //14
	uint16_t RegimError;//15
	bool protOFF;		//16
	uint16_t d_U;		//17
	uint16_t d_F;		//18
	uint32_t KWH;	//19
	uint32_t time;	//20
	uint8_t spt_time;	//21
} StatusDG = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0 };//{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };

						
volatile struct 
{
	uint8_t ADCF;	//0
	uint8_t ADCU;	//1
	uint16_t RegSMU;	//3
	uint8_t CurrPosLCD;	//4
} StatusMU, StatusDU;






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

						



volatile uint32_t	delay_count = 0,
					sum_Nd = 0, 
					sum_FU = 0,
					sum_Niz = 0,
					vzb = 8;
					
#define PU_MPU		0
#define PU_DPU		1


#define PDG_FRAME_SIZE	34 //aa+crc16

#define ARR_RX_MHU	7	//+crc
#define ARR_RX_PDU	ARR_RX_MHU	//+crc
//size_t fre=0;
volatile BaseType_t	RX_PU[ ARR_RX_MHU ];		

volatile bool		adc_ok = false,
					one_sec = false,
					two_sec = false,
					read_rn = false,
					alarm = false,
					read_ok = false;
					
#define ADC_COUNT_CH 9
volatile BaseType_t	ADC_Arr[ ADC_COUNT_CH ],
					time[ 16 ],
					ErrTime[ 16 ];

const uint16_t 		time0[ 16 ] =	{	5,	1,	30,	1,	180, 20, 180,10,	4,	15,	60,	60,	300,	40,	30,	1 };
const uint16_t 		ErrTime0[ 16 ]=	{	1,	1,	10,	6, 3,	3,	5,	30,	5,	10,	1,	1,	3,	5,	5,	3 };//se


uint8_t arr_rx_rn[10],
		tpl =0,
		cnt_rx_rn = 0;

#define ARR_CAL 6
volatile uint8_t calib_data[ ARR_CAL ];
const uint8_t	calib_data_max[ ARR_CAL ] = { 50,	50,	1,	255,	1,	1 },
				calib_data_min[ ARR_CAL ] = { 1,	15,	1,	235,	1,	1 },
				calib_data_nom[ ARR_CAL ] = { 1,	15,	1,	245,	1,	1 };					

void vLinkPU_SysTask( void *pvParameters );
void vLinkRN_SysTask( void *pvParameters );
void UART2_IRQHandler( void );	
void Timer1_IRQHandler( void );
void UART1_send_byte( uint8_t );
/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

void clear_Arr( u8 *arr,u8 cnt );
void UART2_send_byte( uint8_t byte );
void vSetErrorTask( void *pvParameters );
void vKeyHandlerTask( void *pvParameters );
void vSetRegWorkTask( void *pvParameters );
void vADC_calkTask( void *pvParameters );
void ChangeCtTime( void );
void ChangeCtError( void );
void SetError( void );
void calk_Tbap( void );
void calk_Pm( void );
void calk_Tm( void );
void calk_Ugen( void );
void calk_Igen( void );
void save_calib_data( void );
void restore_calib_data( void );
void calk_Dlevel( void );
void calk_Isol( void );
void calk_Ndiz( void );
bool read_rx_data( uint8_t type_PU );
bool send_pult( uint8_t type_PU );
void calk_FrU( void );
void calk_Ub( void );
void LoadRegTransmitUsart( unsigned char NumberFunction );
uint16_t calc_CRC16( uint8_t *buf, uint16_t num );
void ResetError( void );
void rtp_up( uint8_t sec_05 );
void rtp_dwn( uint8_t sec_05 );
void set_tpl(uint16_t nd);

//==================================================================================
//==================================================================================
//==================================================================================
#define	AD_SAMPL	140
#define CT_AD_SUM	64
void ADC_IRQHandler(void)//0.144 ms //36 тактов на один канал
{
	uint8_t cnt_ch = 0;
	static uint8_t 	countAD = AD_SAMPL+2;
	static uint32_t AdTempResult = 0,
					AdTempMax = 0,
					AdTempMin = 0;
	uint32_t tmp = 0;

	tmp = ADC1_GetResult();
	cnt_ch = ( tmp>>16 )&0x1F;
	tmp &= 0xFFF; //12 bit
	

	//if(cnt_ch == ADC_CH_TEMP_SENSOR)
	switch( cnt_ch )
	{
		case ADC_CH_TEMP_SENSOR:
				cnt_ch = 8;
		case ADC_CH_ADC2:	//dm
		case ADC_CH_ADC3:	//UT
		case ADC_CH_ADC4:	//VB
		case ADC_CH_ADC5:	//OJ
			
			if( countAD )
			{
				--countAD;
				AdTempResult += tmp;
			}
			if( !countAD )
			{
				AdTempResult /= AD_SAMPL; 
				ADC_Arr[ cnt_ch ] = AdTempResult;
				
				AdTempResult = 0;
			}
			
			break;
		case ADC_CH_ADC6:	//Ig
		case ADC_CH_ADC7:	//Ug 50Hz 20ms
			if( countAD )
			{
				--countAD;

				if( countAD < AD_SAMPL )
				{
					if( tmp > AdTempMax )
						AdTempMax = tmp;
						
					if( tmp < AdTempMin )
						AdTempMin = tmp;
				}
			}
			if( !countAD )
			{
				tmp = AdTempMax - AdTempMin;
					if( tmp >= 60 )
						tmp -= 60;
					else
						tmp = 0; 
					
				ADC_Arr[ cnt_ch ] = tmp;
			}
			
			break;
		
		default: break;
	}
	
	if( !countAD )
	{
		countAD = AD_SAMPL;
		
		AdTempMax = 0;
		AdTempMin = 0xffff;
		
		switch( cnt_ch )
		{
			case ADC_CH_ADC7:
				ADC1_SetChannel( ADC_CH_TEMP_SENSOR );
				break;
			case 8:
				ADC1_SetChannel( ADC_CH_ADC2 );
				adc_ok = true;
				break;
			default:
				ADC1_SetChannel((cnt_ch+1));
				break;
		}
	}
}

#define RX_FRAME_SIZE	3
void UART1_IRQHandler( void )
{
	uint8_t tmp = 0;
	static bool start = false;
	static uint8_t cnt_rx_byte = 0;
//	static portBASE_TYPE xTaskWoken;
	
	//xTaskWoken = pdFALSE;
	
	while(!UART_GetFlagStatus( MDR_UART1, UART_FLAG_RXFE ))
	{
		tmp = (uint8_t)UART_ReceiveData( MDR_UART1 );
		
		if( !read_ok )
		{
			if(!start)
			{
				if( tmp == 0x55 )
				{
					start = true;
					//__nop();
				}
			}
			else
			{
				RX_PU[ cnt_rx_byte ] = tmp; 

				if( cnt_rx_byte >= (ARR_RX_MHU-1) )
				{
					cnt_rx_byte = 0;
					start = false;
					read_ok = true;
		//PORT_ResetBits( MDR_PORTB, PORT_Pin_10 );
		//			xSemaphoreGiveFromISR( xUSART_RX_Semaphore, &xTaskWoken );
				}
				else
					++cnt_rx_byte;
			}
		}
	//	++cnt;
	}
//	 if( xTaskWoken != pdFALSE )
//    {
//        // We can force a context switch here. 
//		portYIELD();
//    }
}
//==================================================================================
void UART2_IRQHandler( void )
{
//	uint8_t tmp = 0;
//	static bool start = false;
//	static uint8_t cnt_rx_byte = 0;
//	static portBASE_TYPE xTaskWoken;
	arr_rx_rn[ cnt_rx_rn ] = UART_ReceiveData( MDR_UART2 );
	
	if(arr_rx_rn[ 0 ] == 11)
	{
		if( cnt_rx_rn < 9 )
			++cnt_rx_rn;
	}
	//tmp = 
}
//==================================================================================
void Timer2_IRQHandler( void )//500ms
{
	static uint8_t cnt_sec=0, cnt=0;
	uint32_t tmp=0;
	static bool sw_chan=false;
	
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~( 1 << 1 );
	NVIC_ClearPendingIRQ( Timer2_IRQn );
	 
	
//	if( !sw_chan )
//	{
//		if(StatusDG.RegWrk & 0x40)
//		{
//			TIMER_CntEventSourceConfig( MDR_TIMER3, TIMER_EvSrc_CH4 );
//			sw_chan = true;
//		}
//		else
//			sum_FU = 0;
//		
		sum_Nd = TIMER_GetCounter( MDR_TIMER3 );
//		StatusDG.Ndiz = (sum_Nd*100)/132;// 470 Om
		tmp = (sum_Nd*100)/144;// 3.3K Om
		StatusDG.Ndiz = tmp;
		MDR_TIMER3 -> CNT = 0;
//	}
//	else
//	{
//		if( cnt > 2 )
//		{
//			cnt = 0;
//			sw_chan = false;
//			sum_FU = TIMER_GetCounter( MDR_TIMER3 );
//			TIMER_CntEventSourceConfig( MDR_TIMER3, TIMER_EvSrc_CH1 );
//			MDR_TIMER3 -> CNT = 0;
//		}
//		else
//			++cnt;
//	}
	
	sum_Niz = MDR_TIMER1 -> CNT;
	MDR_TIMER1 -> CNT = 0;
	
	
	if( cnt_sec > 1 )
	{
		cnt_sec = 0;
		one_sec = true;
		if(PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_1))
			++StatusDG.time;
		
		if(StatusDG.Error&0x8000)
		{
			if(StatusDG.spt_time)
				--StatusDG.spt_time;
			else
				K8_ON;
		}
	}
	else
		++cnt_sec;
	
	if(alarm)
			K6_TOGLE;
	else
	{
		if(!(StatusDG.RegWrk&1))
		K6_OFF;
	}
}
//==================================================================================
//==================================================================================
//void Timer3_IRQHandler( void )//10ms
//{
//	static uint32_t tmp_sum_nd= 0, cnt = 0;
//	
//	MDR_TIMER3->CNT = 0x0000;
//	MDR_TIMER3->STATUS &= ~( 1 << 1 );
//	NVIC_ClearPendingIRQ( Timer3_IRQn );
//	
//	//if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_2))
//	if(cnt<127)
//	{
//		++cnt;
//		tmp_sum_nd += MDR_TIMER3 ->CCR1;
//		MDR_TIMER3 ->CCR1 = 0;
//	}
//	else
//	{
//		cnt = 0;
//		tmp_sum_nd >>= 7;
//		sum_Nd = tmp_sum_nd;
//	}
//}
//==================================================================================
void delay_ms( uint16_t delay )
{
	delay_count=delay;
	while(delay_count);
}

//==================================================================================
//==================================================================================
//==================================================================================
int main( void )
{
	uint32_t pausa = 0xFFFF;
	//bool cntl=0;
	//POWER_detector_init();
	
	while( --pausa );
	
	__disable_irq();
	prvSetupHardware();
	
	restore_calib_data();

	//K7_ON;
	StatusDG.spt_time = 28;
	ResetError();
	
	xTaskCreate( vSetRegWorkTask,  "SetRegWork", configMINIMAL_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL );
	xTaskCreate( vSetErrorTask,  "SetError", configMINIMAL_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL );
	xTaskCreate( vLinkPU_SysTask,  "LinkPU_Sys", (configMINIMAL_STACK_SIZE*2), NULL, 3,( xTaskHandle * ) NULL );
	xTaskCreate( vLinkRN_SysTask,  "LinkRN_Sys", configMINIMAL_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL );
	xTaskCreate( vKeyHandlerTask,  "Key", configMINIMAL_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL );
	xTaskCreate( vADC_calkTask,  "ADC_clk", configMINIMAL_STACK_SIZE, NULL, 3,( xTaskHandle * ) NULL );
	
	__enable_irq();
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
	TIMER_CntInitTypeDef sTIM_CntInit;
	ADC_InitTypeDef sADC;
	ADCx_InitTypeDef sADCx;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	
	ADC_StructInit(&sADC);
	ADCx_StructInit(&sADCx);
	TIMER_ChnStructInit(&sTIM_ChnInit);
//CLK
	
///* Enable HSE clock oscillator */
//	RST_CLK_HSEconfig( RST_CLK_HSE_ON );
//	
//	while( RST_CLK_HSEstatus() == ERROR );

//	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
//	MDR_RST_CLK -> CPU_CLOCK |= 2;//CPU_C1 set HSE
//	RST_CLK_HSIcmd( DISABLE );


/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while(RST_CLK_HSEstatus() == ERROR);
	
//	EEPROM_SetLatency(EEPROM_Latency_2); //до 75Mhz	

	/* Configure CPU_PLL clock */
	RST_CLK_CPU_PLLconfig( RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul4 );
	RST_CLK_CPU_PLLcmd( ENABLE );
	
	while( RST_CLK_CPU_PLLstatus() == ERROR );
	


	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
	
	RST_CLK_CPU_PLLuse(ENABLE);
	
	RST_CLK_HSIcmd(DISABLE);

	/* Enables the clock on EEPROM */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);

/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd( ALL_PORTS_CLK, ENABLE );
	
//ADC
	
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
	sADCx.ADC_ChannelNumber	= ADC_CH_ADC2;
	sADCx.ADC_VRefSource	= ADC_VREF_SOURCE_INTERNAL;
	sADCx.ADC_IntVRefSource	= ADC_INT_VREF_SOURCE_INEXACT;
	//sADCx.ADC_Prescaler		= ADC_CLK_div_2048;//4.6 ms
	//sADCx.ADC_Prescaler		= ADC_CLK_div_256;
	//sADCx.ADC_Prescaler = ADC_CLK_div_64;//0.144 ms
	//sADCx.ADC_Prescaler = ADC_CLK_div_32;//0.07 ms
	//sADCx.ADC_Prescaler = ADC_CLK_div_128;//500 KHz Fcore =64M
	sADCx.ADC_Prescaler = ADC_CLK_div_256;//250 KHz Fcore =64M

	sADCx.ADC_DelayGo = 7;//36 тактов на канал

	ADC1_Init(&sADCx);
	
	ADC1_ClearOverwriteFlag();
	
	/* Enable ADC1 EOCIF and AWOIFEN interupts */
	ADC1_ITConfig((ADC1_IT_END_OF_CONVERSION), ENABLE);
	
	NVIC_EnableIRQ(ADC_IRQn);

	
	/* ADC1 enable */
	ADC1_Cmd(ENABLE);
	

//Timer1
	
	// TIMER1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1,ENABLE);
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER1);

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);


	/* TIM1 configuration ------------------------------------------------*/
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
	TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

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
	sTIM_CntInit.TIMER_Prescaler                = 10000;//
	sTIM_CntInit.TIMER_Period                   = 4000;//0.5s
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit( MDR_TIMER2, &sTIM_CntInit );

	NVIC_EnableIRQ( Timer2_IRQn );
	TIMER_ITConfig( MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE );
	
	/* TMR2 enable */
	TIMER_Cmd ( MDR_TIMER2, ENABLE );
	
	
	// TIMER3
	RST_CLK_PCLKcmd( RST_CLK_PCLK_TIMER3,ENABLE );
	/* Reset all TIMER3 settings */
	TIMER_DeInit( MDR_TIMER3 );

	TIMER_BRGInit( MDR_TIMER3,TIMER_HCLKdiv1 );

  /* TIM3 configuration ------------------------------------------------*/
  
	//sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_CAPTURE;
//  	sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL1;
//	sTIM_ChnInit.TIMER_CH_Prescaler = TIMER_CH_Prescaler_div_4;
//	sTIM_ChnInit.TIMER_CH_FilterConf = TIMER_Filter_2FF_at_TIMER_CLK;
	sTIM_ChnInit.TIMER_CH_EventSource = TIMER_CH_EvSrc_PE;

	TIMER_ChnInit( MDR_TIMER3, &sTIM_ChnInit);
	
  /* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				= 0;
//	sTIM_CntInit.TIMER_Prescaler                = 64;	//64 mhz 1MHz 0.001 ms
//	sTIM_CntInit.TIMER_Period                   = 5000;//5ms
	sTIM_CntInit.TIMER_Prescaler                = 0;	//64 mhz 
	sTIM_CntInit.TIMER_Period                   = 0xFFFF;//
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_EvtFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_CH1;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	
	TIMER_CntInit( MDR_TIMER3, &sTIM_CntInit );
	
//	NVIC_EnableIRQ( Timer3_IRQn );
//	TIMER_ITConfig( MDR_TIMER3, TIMER_STATUS_CNT_ARR, ENABLE );
	
	/* Enable TIMER3 */
	TIMER_Cmd( MDR_TIMER3, ENABLE );
	
	
	
//UART1
	
	/************************ UART1 Initialization *************************/
	PORT_StructInit( &PORT_InitStructure );//reset struct
	RST_CLK_PCLKcmd( RST_CLK_PCLK_UART1, ENABLE );
	
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
	RST_CLK_PCLKcmd( RST_CLK_PCLK_UART2, ENABLE );

	PORT_InitStructure.PORT_Pin = ( PORT_Pin_0 | PORT_Pin_1 );//RX TX
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	
	PORT_Init( MDR_PORTD, &PORT_InitStructure );


	
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit( MDR_UART2, UART_HCLKdiv1 );
	UART_DeInit( MDR_UART2 );
	
	/* Disable interrupt on UART2 */
    NVIC_DisableIRQ( UART2_IRQn );
	
	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 9600;//9605;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits2;
	UART_InitStructure.UART_Parity				= UART_Parity_No;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART2 parameters*/
	UART_Init( MDR_UART2, &UART_InitStructure );

	NVIC_EnableIRQ( UART2_IRQn );
	UART_ITConfig( MDR_UART2,UART_IT_RX, ENABLE );
	/* Enables UART2 peripheral */
	UART_Cmd( MDR_UART2, ENABLE );
	
//PORT
	
	PORT_StructInit( &PORT_InitStructure );//reset struct	
 /* Configure PORTA pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTA, &PORT_InitStructure );
PORT_ResetBits(MDR_PORTA, (PORT_Pin_0 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5) );

	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_1 ); //tmr1 ch1
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTA, &PORT_InitStructure );
	
 /* Configure PORTB pins */
 	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_6 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTB, &PORT_InitStructure );
PORT_ResetBits( MDR_PORTB, (PORT_Pin_6 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10) );

	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_5 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTB, &PORT_InitStructure );
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_7 );//tmr 3 ch4 Freq Ug
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTB, &PORT_InitStructure );
	
 /* Configure PORTC pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTC, &PORT_InitStructure );

	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_1 | PORT_Pin_2 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTC, &PORT_InitStructure );
PORT_ResetBits( MDR_PORTC, (PORT_Pin_1 | PORT_Pin_2) );
 /* Configure PORTD pins */
	
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_InitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

	PORT_Init( MDR_PORTD, &PORT_InitStructure );
	
 /* Configure PORTE pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_7 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);
PORT_ResetBits(MDR_PORTE, (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_7) );

	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_3 | PORT_Pin_6 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_2 );//tmr3 ch1
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_OVERRID;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	
 /* Configure PORTF pins */
	PORT_InitStructure.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 );
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init( MDR_PORTF, &PORT_InitStructure);
	PORT_ResetBits( MDR_PORTF, (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_3 | PORT_Pin_5 | PORT_Pin_6) );
	PORT_SetBits( MDR_PORTF, (PORT_Pin_2 | PORT_Pin_4) );
}



//==================================================================================
void vKeyHandlerTask( void *pvParameters )
{ // Key handling is a continuous process and as such the task 
	// is implemented using an infinite loop (as most real time 
	// tasks are). 
	uint16_t RegKn = 0, RegKnOld = 0, dat =0;
	uint8_t cnt_del = 0;
#define ON_24V		PORT_SetBits(MDR_PORTB, PORT_Pin_6)
#define OFF_24V		PORT_ResetBits(MDR_PORTB, PORT_Pin_6)

#define ON_B3		PORT_SetBits(MDR_PORTA, PORT_Pin_0)
#define OFF_B3		PORT_ResetBits(MDR_PORTA, PORT_Pin_0)

#define ON_B4		PORT_SetBits(MDR_PORTA, PORT_Pin_3)
#define OFF_B4		PORT_ResetBits(MDR_PORTA, PORT_Pin_3)	
	
#define ON_B5		PORT_SetBits(MDR_PORTB, PORT_Pin_8)
#define OFF_B5		PORT_ResetBits(MDR_PORTB, PORT_Pin_8)

#define ON_B6		PORT_SetBits(MDR_PORTB, PORT_Pin_9)
#define OFF_B6		PORT_ResetBits(MDR_PORTB, PORT_Pin_9)
	
	
	for( ;; )
	{
		
#if( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( DEBUG_ON == 1 ) )
//		if( one_sec )
//		{	
//			char stat_buff[150];
//			u8 i=0, j=0;
//			
//			one_sec = false;
//			
//			for( i=0; i<150; i++ )
//				stat_buff[i] = 0;
//			
//			vTaskGetRunTimeStats( stat_buff );
//			
//			i = 149;
//			while( !stat_buff[i] )
//				i--;
//			
//			printf( "%c[1;1f%c[J", 27, 27 ); //ClearScreen
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
//			
//			vTaskList(stat_buff);
//			
//			i = 149;
//			while( !stat_buff[i] )
//				i--;
//			
//			j=0;
//			while( j<i )
//			{
//				putchar( stat_buff[j] );
//				++j;
//			}
//		}
#endif		
		OFF_24V;
		
		switch( dat )
		{
			case 0: 
				ON_24V;
				ON_B3;
			
				vTaskDelay( 2 );
			
				if(PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_0))
					RegKn |= 1;
				else
					RegKn &= 0xfffe;
			
				if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_3))
					RegKn |= 2;
				else
					RegKn &=0xfffd;
				
				if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_6))
					RegKn |= 4;
				else
					RegKn &= 0xfffb;
				
				OFF_24V;
				OFF_B3;
				break;
			
			case 1: 
				ON_24V;
				ON_B4;
			
				vTaskDelay( 2 );
			
				if(PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_0))
					RegKn |= 8;
				else
					RegKn &= 0xfff7;
			
				if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_3))
					RegKn |= 0x10;
				else
					RegKn &=0xffef;
				
				if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_6))
					RegKn |= 0x20;
				else
					RegKn &= 0xffdf;
				
				OFF_24V;
				OFF_B4;
				break;	
				
			case 2: 
					//ON_24V;
					ON_B5;
				
					vTaskDelay( 5 );
				
					if(PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_0))
						RegKn |= 0x40;
					else
						RegKn &= 0xffbf;
				
					if(!PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_3))
						RegKn |= 0x80;
					else
						RegKn &=0xff7f;
					
					if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_6))
						RegKn |= 0x100;
					else
						RegKn &= 0xfeff;
					
					//OFF_24V;
					OFF_B5;
					
				break;	
				
			case 3: 
					//ON_24V;
					ON_B6;
				
					vTaskDelay( 5 );
				
					if(!PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_0))
						RegKn |= 0x200;
					else
						RegKn &= 0xfdff;
				
					if(!PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_3))
						RegKn |= 0x400;
					else
						RegKn &=0xfbff;
					
					if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_6))
						RegKn |= 0x800;
					else
						RegKn &= 0xf7ff;
					
					//OFF_24V;
					OFF_B6;
				break;	
		}
		
		if( dat < 3 )
		{
			++dat;
		}
		else
			dat = 0;
		
		if( RegKnOld == RegKn )
		{
			StatusDG.RegSDG = RegKn;
			if(( StatusDG.RegSDG & 0xC0 ) || ( StatusDU.RegSMU & 0x180 ))//SPT _ON
			{
				StatusDG.RegimError |= 1<<15;
				//StatusDG.spt_time = 20;
				//StatusDG.Error |= 0x8000;
				//alarm = true;
				//StatusDG.Led |= 1<<2;
				//StatusDG.RegWrk |= 0x10;
				//K1_OFF;
			}
			else
			{
				//StatusDG.Led &= ~(1<<2);
				//alarm = false;
				//StatusDG.spt_time = 30;
			//	StatusDG.Error &= ~0x8000;
				StatusDG.RegimError &= ~(1<<15);
			}
			
			if(StatusDG.RegSDG & 0x100)//DZV
			{
				StatusDG.RegimError |= 1<<13;
			}
			else
				StatusDG.RegimError &= ~(1<<13);
			
			if(StatusDG.RegSDG & 0x200)//lvl  OJ
			{
				if(StatusDG.TOJ>5)
					StatusDG.RegimError |= 1<<8;
			}
			else
				StatusDG.RegimError &= ~(1<<8);
			
			if(StatusDG.RegSDG & 0x400)//lvl M 
			{
				if(StatusDG.TOJ>5)
					StatusDG.RegimError |= 1<<14;
			}
			else
				StatusDG.RegimError &= ~(1<<14);
			
//			if(StatusDG.RegSDG & 0x80)//FIRE
//			{
//				StatusDG.RegimError |= 1<<15;
//			}
//			else
//				StatusDG.RegimError &= ~(1<<15);
		}
		else
			RegKnOld = RegKn;
		
		vTaskDelay( 50 );
		

		if((StatusMU.RegSMU & 0x20) || (StatusDU.RegSMU & 0x20))//res err
		{
			++cnt_del;	
			if( cnt_del > 40 ) //2 sec
			{
				cnt_del = 0;
				ResetError();
			}
		}
		else
			cnt_del = 0;
	}
}
//==================================================================================
void vADC_calkTask( void *pvParameters )
{ // Key handling is a continuous process and as such the task 
	// is implemented using an infinite loop (as most real time 
	// tasks are). 
	//static BaseType_t RegKn = 0, RegKnOld = 0;
	uint32_t p_m=0, tmp_kwh = 0;
	
	for( ;; )
	{
		if( adc_ok )
		{
			calk_Tbap();
			calk_Pm();
			calk_Tm();
			calk_Ugen();
			calk_Igen();
			calk_Dlevel();
			calk_Ndiz();
			calk_Isol();
			calk_FrU();
			calk_Ub();
			

			p_m = ((StatusDG.Igen * StatusDG.UDG * 3)/1730);
			
			if( StatusDG.time>1799 )//30min
			{
				tmp_kwh += (p_m)/2;
				if(StatusDG.KWH < tmp_kwh)
				{
					StatusDG.KWH += tmp_kwh;
					save_calib_data();
				}
				StatusDG.time = 0;
			}
			adc_ok = false;
		}
	}
}
//==================================================================================
void vSetErrorTask( void *pvParameters )
{
 	uint8_t cnt;
 	
 	/* Create the semaphore used by the ISR to wake this task. */
 //	xTime_Err_Semaphore = xSemaphoreCreateBinary();
	ResetError();
 	for( ;; )
 	{
		vTaskDelay( 1000 );
		
		ChangeCtTime();
		ChangeCtError();

		for( cnt=0;cnt<16;++cnt )
		{
			if( !ErrTime[cnt] )
			{
				if( !(StatusDG.RegWrk&0x10))
				{
					StatusDG.Error |= ((uint16_t)1<<cnt);
					StatusDG.Led |= 2;
					
					//K6_ON;
					switch( cnt )
					{
						case 0:
							StatusDG.RegWrk &= ~0x40;
							StatusDG.RegWrk |= 1<<7;//rashol
							StatusDG.RegimTime |= 1<<7;
							K1_OFF;
							alarm = true;
							break;
						case 1:
							StatusDG.RegWrk &= ~0x40;
							break;
						case 2:
								
								if(( !StatusDG.protOFF ) && ( StatusDG.RegWrk&0x40 ))//PRot
								{	
									if(StatusDG.UDG<322)
										K1_OFF;
									
									//StatusDG.RegWrk=0x10;
									alarm = true;
								}
							break;
						case 3:
//							if( !StatusDG.protOFF )//PRot
//							{
//								K1_OFF;
//								if( StatusDG.TOJ < 125 )
//									StatusDG.RegWrk = 0x10;
//								else
//								{
//									StatusDG.RegWrk &= ~0x40;
//									StatusDG.RegWrk |= 1<<7;//rashol
//									StatusDG.RegimTime |= 1<<7;
//								}
//							}
							alarm = true;
							break;
						case 4:
								StatusDG.RegWrk &= 0x100;
								StatusDG.RegWrk |= 0x10;	
								alarm = true;
							//PORT_ResetBits( MDR_PORTA, (PORT_Pin_3) );//DG авар
							break;
						case 5:
							alarm = true;
							if(!StatusDG.protOFF)//PRot
							{
								K1_OFF;
							}
							break;
						case 6:
							if(!StatusDG.protOFF)
							{
								StatusDG.RegWrk &= 0x100;
								StatusDG.RegWrk |= 1<<7;//rashol
								StatusDG.RegimTime |= 1<<7;
							}
							alarm = true;
							break;
						case 7:
//								if(!StatusDG.protOFF)//PRot
//									StatusDG.RegWrk=0x10;
						alarm = true;
							break;
						case 8: //levl OJ
							alarm = true;
							break;
						case 9:
							break;
						case 12://isol
							StatusDG.Led |= 1<<3;
							alarm = true;
							if(StatusDG.Isol <=15)
								K1_OFF;
							break;
						case 13:
							alarm = true;
							break;
						case 14://lvl maslo
							alarm = true;
							if(StatusDG.RegWrk&0x40)
							{
								StatusDG.RegWrk &= 0x100;
								StatusDG.RegWrk |= 1<<7;//rashol
								StatusDG.RegimTime |= 1<<7;
							}
							break;
						case 15://FIRE
							K1_OFF;
							StatusDG.RegWrk &=0x100;
							StatusDG.RegWrk |=0x10;
						K13_OFF;//swech
						K12_OFF;//nasos
						K11_OFF;//regim
						K10_OFF;//PDT
						StatusDG.Led |= 1<<2;
						alarm = true;
							break;
						
						default: break;
					}
				}
			}
		}
 	}
}

//==================================================================================
#define ST_OFF	K3_OFF	
void calk_Ndiz( void )
{
//	StatusDG.Ndiz = sum_Nd/101;
	//StatusDG.Ndiz = sum_Nd*100/132;
	//StatusDG.Ndiz = sum_Nd*100/132;//142
	

//	if( StatusDG.RegWrk & 4 )//ST on
//		{
	if( StatusDG.Ndiz > 150 )//zapusk
		{
			ST_OFF;
		}
	//	}
	if( (StatusDG.Ndiz > 3200) && (StatusDG.RegWrk&0x60) ) //raznos
	{
		StatusDG.RegimError |= (uint16_t)1<<0;
	}
	else
		StatusDG.RegimError &= ~((uint16_t)1<<0);
	
	if( StatusDG.RegWrk&0x60 )
	{
		if( StatusDG.Ndiz < 200 )
			StatusDG.RegimError |= (uint16_t)1<<1;
		else
			StatusDG.RegimError &= ~((uint16_t)1<<1);
	}
	else
			StatusDG.RegimError &= ~((uint16_t)1<<1);
}
//==================================================================================
#define ST_ON	K3_ON	

#define N400_ON		
#define N400_OFF	

#define VOZB_ON	vzb = 4	
#define VOZB_OFF vzb = 8

#define STOP_ON		
#define STOP_OFF	

void vSetRegWorkTask( void *pvParameters )
{
	uint8_t cnt_pusk = 0;
	uint32_t tmp_F =0;
	for( ;; )
	{
//		if( (tmp_F > StatusDG.d_F))
//			{
//				TPR_DOWN_ON;
//				vTaskDelay(((tmp_F - StatusDG.d_F)* 16));
//				TPR_DOWN_OFF;
//				tmp_F = StatusDG.d_F;
//			}
//		if( (tmp_F < StatusDG.d_F))
//			{
//				TPR_UP_ON;
//				vTaskDelay(((StatusDG.d_F - tmp_F)* 16));
//				TPR_UP_OFF;
//				tmp_F = StatusDG.d_F;
//			}
			
			
			
		if( tpl & 1 )
		{
			tpl = 0;
			//K13_ON;//swech
			TPR_UP_ON;
			vTaskDelay( 1000 );
			TPR_UP_OFF;
		}
		if( tpl & 2 )
		{
			tpl = 0;
		//	K13_OFF;//swech
			TPR_DOWN_ON;
			vTaskDelay( 200 );
			TPR_DOWN_OFF;
		}
		if( tpl & 4 )
		{
			tpl = 0;
		//	K12_ON;//nasos
			TPR_DOWN_ON;
			vTaskDelay( 2000 );
			TPR_DOWN_OFF;
		}
		if( tpl & 0x8 )
		{
			tpl = 0;
		//	K12_OFF;//nasos
		}
		if( tpl & 0x40 )
		{
			tpl = 0;
		//	K9_OFF;
		}
		
		
		if( StatusDG.RegWrk & 1 )//start
		{
			if( StatusDG.Ndiz < 100 )//ostan
			{
				K6_ON;
				TPR_DOWN_ON;
				vTaskDelay( 3000 );
				K6_OFF;
				vTaskDelay( 3000 );
				TPR_DOWN_OFF;
				vTaskDelay( 10 );
				K7_ON;
				
					TPR_UP_ON;
					vTaskDelay( 500 );
					TPR_UP_OFF;
				
				StatusDG.RegimTime |= (1<<9);
				StatusDG.RegWrk &= ~1;
				if(!(StatusDG.RegWrk & 0x10))
				{
					StatusDG.RegWrk |= 4;//CT on
					cnt_pusk = 0;
					
					time[9] = time0[9];
					if(!StatusDG.Ndiz)
						ST_ON;
				}
			}
			else
				StatusDG.RegWrk &= ~1;
		}
		
		
		if( StatusDG.RegWrk & 2 )//swech_on
		{
			StatusDG.RegimTime |= 1<<12;
			if(!time[12])
			{
				K4_OFF;
			}
			else
			{
				K4_ON;
			}
		}
		
		if( StatusDG.RegWrk & 4 )//ST on
		{
			if( cnt_pusk < 3 )
			{
				if( StatusDG.Ndiz > 200 )//zapusk
				{
					ST_OFF;
					VOZB_ON;
					
					
					if(StatusDG.TOJ < 70)//<40C
					{
						StatusDG.RegWrk &= ~((2)|(4)|(1));
						StatusDG.RegWrk |= 0x20;
					}
					else
					{
						while( (StatusDG.Ndiz < 2800) && (StatusDG.RegWrk & 4))
						{
							TPR_UP_ON;
							vTaskDelay( 400 );
							TPR_UP_OFF;
							vTaskDelay( 2000 );
						}
						StatusDG.RegWrk &= ~((2)|(4)|(1));
						StatusDG.RegWrk |= 0x40;
					}
					
					StatusDG.RegimTime &= ~((1<<2)|(1<<1)|(1<<9));
				}
				if( !time[9] )//15sec
				{
					ST_OFF;
					StatusDG.RegimTime &= ~(1<<9);//15sec
					StatusDG.RegimTime |= (1<<2);//30sec
					time[9] = time0[9];
					time[2] = time0[2];
					++cnt_pusk;
				}

				if( !time[2] )//30sec
				{
					time[2] = time0[2];

					if( !StatusDG.Ndiz )
						ST_ON;
					
					StatusDG.RegimTime |= (1<<9);
					StatusDG.RegimTime &= ~4;
					time[9] = time0[9];
				}
			}
			else
			{
				ST_OFF;
				time[2] = time0[2];
				time[9] = time0[9];
				StatusDG.RegimTime &= ~((1<<2)|(1<<1)|(1<<9));
				StatusDG.Error |= 2;
				StatusDG.RegWrk &= 0x100;
				K6_ON;
			}
		}
		
		if( StatusDG.RegWrk & 0x08 )//PJD WORK
		{
			if(StatusDG.TOJ>105)	//75C
			{
				K12_OFF;//nasos
				K10_OFF;//PDT
			}
		}
		if( StatusDG.RegWrk & 0x10 )//ostan
		{
			//StatusDG.RegWrk &= ~((2)|(4)|(1));
			StatusDG.RegWrk &= ~0xFEEF;
			//StatusDG.RegimTime |= 1<<5;
	// 		if(StatusDG.F1_Off && StatusDG.F2_Off)
			K13_OFF;//swech
			K12_OFF;//nasos
			K11_OFF;//regim
			K10_OFF;//PDT
			
			K1_OFF;
			VOZB_OFF;
			ST_OFF;
			TPR_DOWN_ON;
			vTaskDelay( 4000 );
			TPR_DOWN_OFF;
			vTaskDelay( 1000 );
			K7_OFF;
			vTaskDelay( 8000 );
			
			tmp_F = 0;			
			
		if( StatusDG.Ndiz < 400 )
			{
				//ALARM_ON;//DG авар
			//	K6_ON;
				StatusDG.RegimError = 0;
			//	StatusDG.Error |= (1<<11);
				StatusDG.RegimTime = 0;
				StatusDG.RegWrk &= 0x100;
			}
			else
			{
				StatusDG.RegWrk &= 0x100;
				StatusDG.RegimTime = 0;
				StatusDG.RegimError |= 1<<11;
				K6_ON;
// 				if(!(StatusDG.Error&(1<<5)))//no ddm
// 					N400_OFF;
// 				if(!StatusDG.Error)
// 					N400_OFF;
			}
		}
		if( StatusDG.RegWrk & 0x20 )//progr
		{
			if(StatusDG.TOJ > 70)
			{
				while( (StatusDG.Ndiz < 2800) &&(StatusDG.RegWrk & 0x20))
				{
					TPR_UP_ON;
					vTaskDelay( 400 );
					TPR_UP_OFF;
					vTaskDelay( 2000 );
				}
				StatusDG.RegWrk &= ~0x20;
				StatusDG.RegWrk |= 0x40;
			}
		}
		if( StatusDG.RegWrk & 0x40 )//gen rab
		{
			
			if((tmp_F > StatusDG.d_F))
			{
				uint8_t dlt = 0;
				dlt = tmp_F - StatusDG.d_F;
				tmp_F = StatusDG.d_F;
				
				TPR_DOWN_ON;
				vTaskDelay((dlt * 8));//1sec
				TPR_DOWN_OFF;
			}
			if((tmp_F < StatusDG.d_F)&&(StatusDG.Ndiz<3200))
			{
				uint8_t dlt = 0;
				dlt =  StatusDG.d_F - tmp_F;
				tmp_F = StatusDG.d_F;
				
				TPR_UP_ON;
				vTaskDelay((dlt * 8));
				TPR_UP_OFF;
			}

			if( !(StatusDG.Error & 0x1B0) )
			{
				VOZB_ON;
			}
		}
		if( StatusDG.RegWrk & 0x80 )//rashol
		{

			K1_OFF;
			if( StatusDG.Ndiz > 2800)
			{
				TPR_DOWN_ON;
				vTaskDelay( 500 );
				TPR_DOWN_OFF;
				vTaskDelay( 1500 );
			}
			if( StatusDG.TOJ > 110 )
			{
				StatusDG.RegimTime |= 1<<4;
				if( !time[4] )//3min 80C
				{
					StatusDG.RegWrk &= 0x100;
					StatusDG.RegWrk |= 0x10;//stop
					StatusDG.RegimTime &= ~(1<<4);
					time[5] = time0[5];
				}
			}
			else
			{
				StatusDG.RegimTime |= 1<<10;//60sec
				if( !time[10] )
				{
					StatusDG.RegWrk &= 0x100;
					StatusDG.RegWrk |= 0x10;//stop
					StatusDG.RegimTime &= ~(1<<10);
					time[10] = time0[10];
				}
			}
		}
		
		if( StatusDG.RegWrk & 0x200 )//PJD start
		{
			if(!(StatusDG.RegSDG&4))//no peregrev
			{
				K10_ON;//PDT
				K13_ON;//swech
				StatusDG.RegimTime |= 1<<13;
			}
			if( !time[13] )//40s
			{
				K12_ON;//nasos
				StatusDG.RegimTime |= 1<<14;
			}
			if((!time[14]) || ((StatusDG.RegSDG&8)))
			{
				K13_OFF;//swech
				if((StatusDG.RegSDG&8))//work
				{
					StatusDG.RegimTime &= ~0x6000;
					StatusDG.RegWrk &= ~0x200;
					StatusDG.RegWrk |= 0x8;
				}
				else 
				{
					K12_OFF;//nasos
					K10_OFF;//PDT
					StatusDG.RegWrk &= 0x100;
					time[13] = time0[13];
					time[14] = time0[14];
				}
			}
		}

	
		if( !StatusDG.RegWrk )//switch ru->du
		{
			if(( StatusDG.Error == 0 ) && ( StatusDG.Ndiz > 1000 ))//rab
			{
				StatusDG.RegWrk |= 0x40;
			}
		}
	}
}
//==================================================================================
void vLinkPU_SysTask( void *pvParameters )
{
 	uint8_t cnt = 0;
// 	static char strk1[mainMAX_MSG_LEN];
//	uint16_t k = 0;
//	xLCDMessage xMessage;
 	/* Create the semaphore used by the ISR to wake this task. */
	
	//xUSART_RX_Semaphore = xSemaphoreCreateBinary();
 	
 	for( ;; )
 	{
		//if( xSemaphoreTake( xUSART_RX_Semaphore, ( TickType_t )10 ) == pdTRUE )//receiv from slave
		//time out
		for( cnt = 0; cnt<10; cnt++ )
		{
			send_pult( PU_MPU );
			vTaskDelay( 10 );
			
			if( read_ok )//receiv from slave
			{
				read_ok = false;
				if( read_rx_data( PU_MPU ) == true )
				{
					StatusDG.RegWrk |= 1<<8;
					
					if( (StatusMU.RegSMU & 8) && (StatusDG.TOJ<40)) //swech on
						StatusDG.RegWrk |= 1<<1;
					else
					{
						K4_OFF;
						StatusDG.RegWrk &= ~(1<<1);
						StatusDG.RegimTime &= ~(1<<12);
					}
					
					if( StatusMU.RegSMU & 1 )//pusk
					{
						if(StatusDG.Ndiz>500)
						{
							StatusDG.RegWrk &= 0x100;
							StatusDG.RegWrk |= 0x40;
						}
						else
							StatusDG.RegWrk |= 1;
					}
					
					if( StatusMU.RegSMU & 0x80 )//pjd
					{
						if((!StatusDG.Ndiz) && (!(StatusDG.RegWrk&0x208)))
						{
							StatusDG.Led |=1;
							StatusDG.RegWrk |= 1<<9;//0x200
							StatusDG.RegimTime &= ~0x6000;
						}
					}
					else
					{
						if((StatusDG.RegWrk & 0x208))
						{
							StatusDG.Led &=~1;
							StatusDG.RegWrk &= ~0x308;
							K13_OFF;//swech
							K12_OFF;//nasos
							K11_OFF;//regim
							K10_OFF;//PDT
						}
					}
					if( StatusMU.RegSMU & 2 )//stop
					{
						if(StatusDG.RegWrk & 0xC0)//rab rashol
						{
							StatusDG.RegWrk &= 0x100;
							StatusDG.RegWrk |= 0x80;
						}
						else
						{
							StatusDG.RegWrk &= 0x100;
							StatusDG.RegWrk |= 0x10;
						}
					}
					if( (StatusMU.RegSMU & 0x40) && (StatusDG.RegWrk & 0x40) )//Nagr
					{
						if(!(StatusDG.Error&0x902F))
							K1_ON;
					}
					else
						K1_OFF;
					
					StatusDG.d_F = ((128-StatusMU.ADCF&0x7F));
					StatusDG.d_U = StatusMU.ADCU&0x7F;
					break;
				}
			}
		}
		
		if( cnt>9 )
			StatusDG.RegWrk &= ~(0x100);
		
		for( cnt = 0; cnt<5; cnt++ )
		{
			send_pult( PU_DPU );
			vTaskDelay( 10 );
			
			if( read_ok )//receiv from slave
			{
				read_ok = false;
				if( read_rx_data( PU_DPU ) == true )
				{	
					if(!(StatusDG.RegWrk&0x100))
					{
						K4_OFF;
						
						if( StatusDU.RegSMU & 4 ) //uki
							K9_ON;
						else
							K9_OFF;
						
						if( StatusDU.RegSMU & 0x8 )//prtect off
						{
							StatusDG.protOFF = true;
						}
						else
							StatusDG.protOFF = false;
					
						if( StatusDU.RegSMU & 1 )//pusk
						{
							if(StatusDG.Ndiz>500)
							{
								StatusDG.RegWrk &= 0x40;
								StatusDG.RegWrk |= 0x40;
							}
							else
								StatusDG.RegWrk |= 1;
						}
						if( StatusDU.RegSMU & 2 )//stop
						{
							if(StatusDG.RegWrk & 0xC0)//rab rashol
							{
								StatusDG.RegWrk &= 0x80;
								StatusDG.RegWrk |= 0x80;
							}
							else
							{
								StatusDG.RegWrk &= 0x10;
								StatusDG.RegWrk |= 0x10;
							}
						}
						if( (StatusDU.RegSMU & 0x40) && (StatusDG.RegWrk & 0x40) )//Nagr
						{
							if(!(StatusDG.Error&0x902F))
								K1_ON;
						}
						else
							K1_OFF;
						
						StatusDG.d_F = (128-(StatusDU.ADCF&0x7F));
						StatusDG.d_U = StatusDU.ADCU&0x7F;
					}
					break;
				}
			}
		}
	}
}
//==================================================================================
void vLinkRN_SysTask( void *pvParameters )
{
	uint32_t tmp_U = 0;
	uint16_t crc16 =0 ;
	uint8_t cnt = 0,
			cntb = 0,
			RegTransmitUsart[10];
	
	RegTransmitUsart[0] = 0x1;//Adres
	
	for( ;; )
 	{
		for( cnt = 0; cnt < 3; cnt++ )
		{
			
			vTaskDelay( 500 );
		
			if( cnt_rx_rn > 3 )//11 1 11 0 3 4 63 183
			{
				cnt_rx_rn = 0;
				StatusDG.RegimError &= ~(1<<9);
				ErrTime[9] = ErrTime0[9];
			}
			else
			{
				if((StatusDG.Ndiz>2700) && (StatusDG.RegWrk & 0x40))
					StatusDG.RegimError |= 1<<9;
			}

			tmp_U = ((128-StatusDG.d_U)*100)/272;
			switch( cnt )
			{
				case 0:
					RegTransmitUsart[1] = 0x6;//NumberFunction	
					RegTransmitUsart[2] = 0;//Adres H
					RegTransmitUsart[3] = 0;//Adres L
					RegTransmitUsart[4] = 0;//UNom>>8;
					//RegTransmitUsart[5] = 216;//UNod_Um=400В; 400/380 *220 - 15 = 216
					RegTransmitUsart[5] = (uint8_t)(181+tmp_U);//UNom=400В; 400/380 *220 - 15 = 216
					crc16 = calc_CRC16( RegTransmitUsart, 6 );
					RegTransmitUsart[6] = (crc16>>8);
					RegTransmitUsart[7] = (crc16&0xFF);
					
					cntb = 0;
					while( cntb < 8 )
					{
						UART2_send_byte( RegTransmitUsart[cntb] );
						cntb++;
					}

					break;
				case 1:
					RegTransmitUsart[1] = 0x6;//NumberFunction	
					RegTransmitUsart[2] = 0;//Adres H
					RegTransmitUsart[3] = 1;//Adres L
					RegTransmitUsart[4] = 0;
					RegTransmitUsart[5] = 200;//KStatizm;
					crc16 = calc_CRC16( RegTransmitUsart, 6 );
					RegTransmitUsart[6] = (crc16>>8);
					RegTransmitUsart[7] = (crc16&0xFF);
					
					cntb = 0;
					while( cntb < 8 )
					{
						UART2_send_byte( RegTransmitUsart[cntb] );
						cntb++;
					}
				
					break;

				case 2:
					RegTransmitUsart[1] = 0xf;//NumberFunction	
					RegTransmitUsart[2] = 0;//Adres H
					RegTransmitUsart[3] = 0x2;//Adres L
					RegTransmitUsart[4] = 0;//Number bits H
					RegTransmitUsart[5] = 4;//2;//Number bits L
					RegTransmitUsart[6] = 1;//Number byts 
					RegTransmitUsart[7] = vzb; //Vozb 8=oFF 4 -ON
					crc16 = calc_CRC16( RegTransmitUsart, 8 );
					RegTransmitUsart[8] = (crc16>>8);
					RegTransmitUsart[9] = (crc16&0xFF);
					
					cntb = 0;
					while( cntb < 10 )
					{
						UART2_send_byte(RegTransmitUsart[cntb]);
						cntb++;
					}

					break;

				default:break;
			}
		}
	}
}
//==================================================================================
uint16_t calc_CRC16( uint8_t *buf, uint16_t num )
{
 uint8_t CRC_lo=0, CRC_hi=0, index=0;
	
	 CRC_lo=255;
	 CRC_hi=255;
	
 while(num--)
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
//void rtp_up( uint8_t sec_05 )
//{
//	TPR_UP_ON;
//	cnt_05sec_tpr = 0;
//	while( cnt_05sec_tpr < sec_05 );
//	TPR_UP_OFF;
//}
////================================================================================
//void rtp_dwn(uint8_t sec_05)
//{
//	TPR_DOWN_ON;
//	cnt_05sec_tpr = 0;
//	while( cnt_05sec_tpr < sec_05);
//	TPR_DOWN_OFF;
//}
//==================================================================================
void ChangeCtTime( void )
{
	uint8_t cnt;
#define CAL_TIME 30
	static uint8_t calib_time = CAL_TIME;
	
	for( cnt=0; cnt<16; ++cnt )
	{
		if(( StatusDG.RegimTime >> cnt )&1)
		{
			if( time[cnt] )
				--time[ cnt ];
		}
		else
			time[ cnt ] = time0[ cnt ];
	}
		
//	if(StatusDG.calibr_on)
//	{
//		--calib_time;
//		if(!calib_time)
//		{
//			calib_time=CAL_TIME;
//			//StatusDG.calibr_on=false;
//			save_calib_data();
//			StatusDG.Error &=~(1<<14);//вкл зум
//		}
//	}
}
//==================================================================================
void ChangeCtError( void )
{
	uint8_t cnt;
	for(cnt=0;cnt<16;++cnt)
		{
		if(StatusDG.RegimError & (1<<cnt))
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
void UART1_send_byte( uint8_t byte )
{
	UART_SendData( MDR_UART1, byte );
	//time_out_byte=0;
	//while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	//while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY) || !UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	//while(UART_GetFlagStatus (MDR_UART1, UART_FLAG_BUSY))
	//while(UART_GetFlagStatus ( MDR_UART1, UART_FLAG_TXFF ))
	while(UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFF));
	UART_ReceiveData ( MDR_UART1 );

}
//==================================================================================
void UART2_send_byte( uint8_t byte )
{
	UART_SendData(MDR_UART2, byte);
	//time_out_byte=0;
	//while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	//while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY) || !UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	//while(UART_GetFlagStatus (MDR_UART2, UART_FLAG_BUSY))
	while( UART_GetFlagStatus ( MDR_UART2, UART_FLAG_TXFF ) );

	UART_ReceiveData (MDR_UART2);
	UART_ClearITPendingBit(MDR_UART2,UART_IT_RX);
}
//==================================================================================
void calk_Tbap( void )
{
	uint8_t tmp=0;
/*
	внутр темп датчик 
	ацп 95 при -2С
	120 при +62С
	106 при 25С
*/				
#define V_25 106
#define Average_Slope 6
	tmp = ADC_Arr[8]>>4;	
	if( tmp > 95 )
		StatusDG.TBap = ( 256*(uint16_t)tmp-24520 )/100;
	else //minus
		StatusDG.TBap = 0;

	
	if( StatusDG.TBap > 85 )
	{
		StatusDG.RegimError |= ( (uint16_t) 1<<8 );
	}
	else
		StatusDG.RegimError &= ~( (uint16_t) 1<<8 );
}
//==================================================================================
void calk_Pm( void )
{	
	uint16_t tmp=0;
	
	tmp = ADC_Arr[2];

	if( tmp > 253 )
	{
		tmp -= 253;
		tmp *=100;
		tmp /=  967;
	}
	else
	{
		tmp = 0;
	}
	
	if( tmp>=99 )
		StatusDG.DM = 99;
	else
		StatusDG.DM = tmp;
	
	if(( StatusDG.Ndiz>1000 ) && ( StatusDG.DM<4 ))
	{
		StatusDG.RegimError |= 1<<4;
	}
	else
	{
		StatusDG.RegimError &= ~(1<<4);
	}
}
//==================================================================================
void calk_Tm( void )
{
//	static uint16_t cnt_msr=0;
//	static uint32_t sum_Tmg=0;
	uint32_t tmp=0;
	
	tmp = ADC_Arr[5]>>4;
	if( tmp > 149 )//-30c
	{
		tmp *= 190;
		tmp -= 28404;
		tmp /= 100;
		if( tmp > 150 )
			tmp=150;
	}
	else
		tmp = 0;
	
	StatusDG.TOJ = tmp;
	
	if( StatusDG.TOJ>129 )
	{
		alarm = true;
	}
		
	if( StatusDG.Ndiz>1500 )
	{
		if( StatusDG.TOJ>134 )//110C
		{
			StatusDG.RegimError |= 1<<6;
		}
		else		
			StatusDG.RegimError &= ~(1<<6);
	}
}
//==================================================================================
void calk_Ugen( void )
{
	uint32_t tmp1 = 0, tmp2 = 0;
	
	tmp1 = ADC_Arr[7]>>3;
	tmp2 = tmp1*100;
	tmp2 /= 67;
	tmp1 *= calib_data[1];
	tmp1 >>= 8;
	tmp1 += tmp2;

	StatusDG.UDG = tmp1;
		
		//if(((StatusDG.UGen<250)||(StatusDG.UGen>305))&&(StatusDG.RegWrk&0x40))
	if((( StatusDG.UDG<351 )||( StatusDG.UDG>420 ))&&(StatusDG.RegWrk&0x40))
	//	if((( StatusDG.UDG>425 ))&&( StatusDG.Ndiz>2500 ))
		{
			StatusDG.RegimError |= 1<<2;
		}
		else
			StatusDG.RegimError &= ~(1<<2);
}
//==================================================================================
void calk_Igen( void )
{
	unsigned int tmp1 = 0, tmp2 = 0;

	tmp1 = ADC_Arr[6]>>4;
	tmp2 = tmp1*10;
	tmp2 /= 50;
	tmp1 *= calib_data[0];
	tmp1 >>= 8;
	tmp1 += tmp2;
		
	StatusDG.Igen = tmp1;
	
	if( StatusDG.RegWrk&0x40 )
	{
		if(( StatusDG.Igen>32 )&&( StatusDG.Igen<=35 ))
		{
			StatusDG.RegimError |= 1<<3;
		}
		else
		{
			StatusDG.RegimError &= ~(1<<3);
		
			if(StatusDG.Igen>35 )
			{
				StatusDG.RegimError |= 1<<5;
			}
			else
			{
				StatusDG.RegimError &= ~(1<<5);
			}
		}
	}
}
//==================================================================================
void calk_Dlevel( void )
{
	uint16_t cnt_msr = 0;
	uint32_t sum_Dl = 0;
	uint16_t tmp_Dl = 0;
	
	sum_Dl = ADC_Arr[ 3 ]>>4;//8 bit

	tmp_Dl = sum_Dl*344;
	tmp_Dl /= 100;
		
	if( tmp_Dl>100 )
		tmp_Dl = 100;
	
//	if( sum_Dl < 30 )
//	{
//		tmp_Dl = sum_Dl*333;
//		tmp_Dl /= 100;
//		
//		if( tmp_Dl>100 )
//			tmp_Dl = 100;
//	}
//	else
//		tmp_Dl = 0;
		
	
	StatusDG.LvlDIZ = tmp_Dl;	

	if(StatusDG.LvlDIZ <= 50)
	{
		if(time[11])
			K5_ON;
		else
			K5_OFF;
		StatusDG.RegimTime |= 1<<6;
		StatusDG.RegimTime |= 1<<11;
	}
	
	if(StatusDG.LvlDIZ >= 99)
	{
		K5_OFF;
	}
	
	if(!time[6])
	{
		StatusDG.RegimTime &= ~((1<<6)|(1<<11));
		time[11] = time0[11];
		time[6] = time0[6];
	}
	
	if( StatusDG.LvlDIZ <= 10 )
	{
		StatusDG.RegimError |= 1<<7;
	}
	else
	{
		StatusDG.RegimError &= ~(1<<7);
	}
}
//==================================================================================
void calk_Isol( void )
{
	uint8_t tmp = 200;
	//sum_Niz *= 100;
	if(sum_Niz > 19000)	//13kOm
		tmp = 15;
	else
	if(sum_Niz > 15600 )//100kOm
		tmp = 100;
	else
	if(sum_Niz > 14000)	//150kOm
		tmp = 150;
	else
	if(sum_Niz > 12500)	//200kOm
		tmp = 200;


	StatusDG.Isol = tmp;///101;
	if(StatusDG.Isol < 200)
		StatusDG.RegimError |= 1<<12;
	else
		StatusDG.RegimError &= ~(1<<12);
}
//==================================================================================
void calk_FrU( void )
{
	static uint32_t tmp = 0, cnt = 0;
//	if( cnt<65 )
//	{
//		++cnt;
//		//tmp += (sum_FU * 10)/53;//osn
//		tmp += (sum_FU * 10)/58;//zip
//	}
//	else
//	{
//		tmp >>= 6;
//		cnt = 0;
//		if(tmp<48)
//			tmp = 48;
//		if(tmp>52)
//			tmp = 52;
//		StatusDG.FreqU = tmp;
//	}
	if(StatusDG.RegWrk&0x40)
	{
		tmp = StatusDG.Ndiz/60;
		if(tmp<48)
			tmp = 48;
		if(tmp>52)
			tmp = 52;

		StatusDG.FreqU = tmp;
	}
}
//==================================================================================
void calk_Ub( void )
{
	StatusDG.UBS = ((ADC_Arr[4]>>4)*100)/34;
}
//==================================================================================
void save_calib_data( void )
{
	uint32_t Wdata = 0;
	
	__disable_irq();
	EEPROM_ErasePage( EEPROM_ADD, EEPROM_Info_Bank_Select );//page0 4K all eeprom
	
	Wdata = ( (calib_data[0]) & 0x000000ff );
	Wdata |= ( (calib_data[1]<<8)&0x0000ff00 );
	Wdata |= ( (calib_data[2]<<16)&0x00ff0000 );
	Wdata |= ( (calib_data[3]<<24)&0xff000000 );
	EEPROM_ProgramWord( EEPROM_ADD, EEPROM_Info_Bank_Select, Wdata );
	
	Wdata = ( (calib_data[4])&0x000000ff );
	Wdata |= (( calib_data[5]<<8)&0x0000ff00 );
	EEPROM_ProgramWord( EEPROM_ADD+4,EEPROM_Info_Bank_Select,Wdata );
	
	EEPROM_ProgramWord( EEPROM_ADD+8, EEPROM_Info_Bank_Select, StatusDG.KWH);
	
	__enable_irq();
}
//==================================================================================
void restore_calib_data( void )
{
	uint8_t i=0;
	bool save_dat=false;
	__disable_irq();
	for(i=0;i<ARR_CAL;++i)
		{
		calib_data[i]=EEPROM_ReadByte(EEPROM_ADD+i,EEPROM_Info_Bank_Select);
			if((calib_data[i]<calib_data_min[i])||(calib_data[i]>calib_data_max[i]))
			{
				calib_data[i]=calib_data_nom[i];
				save_dat=true;
			}
		}
	StatusDG.KWH = EEPROM_ReadWord(EEPROM_ADD+8,EEPROM_Info_Bank_Select);
		
	__enable_irq();
	if(save_dat)
		save_calib_data();
	
}
//==================================================================================
bool read_rx_data( uint8_t type_PU )
{
	uint8_t cnt_byte = 0, sum = 0;
			
	
	while( cnt_byte < ARR_RX_MHU )
	{
		sum += RX_PU[ cnt_byte ];
		++cnt_byte;
	}
	sum += 0x55;
	
	if( !sum )
	{
		if( type_PU == PU_MPU )
		{
			if( RX_PU[0] == 1 )
			{
				StatusMU.RegSMU = RX_PU[1];
				StatusMU.RegSMU |= ((uint16_t)RX_PU[2]<<8);
				StatusMU.ADCF = RX_PU[3];
				StatusMU.ADCU = RX_PU[4];
				StatusMU.CurrPosLCD = RX_PU[5];
				
				return true;
			}
		}
		else
		if( type_PU == PU_DPU )
		{
			if( RX_PU[0] == 2 )
			{
				StatusDU.RegSMU = RX_PU[1];
				StatusDU.RegSMU |= ((uint16_t)RX_PU[2]<<8);
				StatusDU.ADCF = RX_PU[3];
				StatusDU.ADCU = RX_PU[4];
				StatusDU.CurrPosLCD = RX_PU[5];
				
				return true;
			}
		}
	}
	return false;
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
bool send_pult( uint8_t type_PU )
{
	uint16_t sum = 0;
	uint8_t i=0;
	static uint8_t out_buf[PDG_FRAME_SIZE];
	uint32_t tmp32 = 0;
// 	static char strk1[mainMAX_MSG_LEN];
//	uint16_t k = 0;
	
	taskENTER_CRITICAL();
	tmp32 = StatusDG.KWH;
	if(tmp32>99999)
		tmp32 = 99999;
	
	out_buf[0] = (uint8_t)( 0xFF );
	out_buf[1] = (uint8_t)( 0xFE|type_PU );
	out_buf[2] = (uint8_t) StatusDG.Ndiz;
	out_buf[3] = (uint8_t)(StatusDG.Ndiz>>8);
	out_buf[4] = (uint8_t)StatusDG.FreqU;
	out_buf[5] = (uint8_t)(StatusDG.FreqU>>8);
	out_buf[6] = (uint8_t) StatusDG.UDG;
	out_buf[7] = (uint8_t)(StatusDG.UDG>>8);
	out_buf[8] = (uint8_t) StatusDG.UBS;
	out_buf[9] = (uint8_t)(StatusDG.UBS>>8);
	out_buf[10] = (uint8_t) StatusDG.Igen;	
	out_buf[11] = (uint8_t)(StatusDG.Igen>>8);
	out_buf[12] = (uint8_t) StatusDG.DM;
	out_buf[13] = (uint8_t) StatusDG.TOJ;
	out_buf[14] = (uint8_t) StatusDG.TBap;
	out_buf[15] = (uint8_t) StatusDG.LvlDIZ;
	out_buf[16] = (uint8_t) StatusDG.RegWrk;
	out_buf[17] = (uint8_t)(StatusDG.RegWrk>>8);
	out_buf[18] = (uint8_t) StatusDG.Isol;
	out_buf[19] = (uint8_t) StatusDG.Led;
	out_buf[20] = (uint8_t) StatusDG.RegSDG;
	out_buf[21] = (uint8_t)(StatusDG.RegSDG>>8);
	out_buf[22] = (uint8_t) StatusDG.Error;
	out_buf[23] = (uint8_t)(StatusDG.Error>>8);
	out_buf[24] = (uint8_t) StatusDG.RegimTime;
	out_buf[25] = (uint8_t)(StatusDG.RegimTime>>8);
	out_buf[26] = (uint8_t) StatusDG.RegimError;	
	out_buf[27] = (uint8_t)(StatusDG.RegimError>>8);
	out_buf[28] = (uint8_t) tmp32;
	out_buf[29] = (uint8_t)((tmp32>>8)&0xFF);
	out_buf[30] = (uint8_t)((tmp32>>16)&0xFF);
	out_buf[31] = (uint8_t)((tmp32>>24)&0xFF);
//	out_buf[0] = (uint8_t)( 0xFF );
//	out_buf[1] = (uint8_t)( 0xFE|type_PU );
//	for(i=2;i<(PDG_FRAME_SIZE-2);i++)
//	{
//		out_buf[i] = i ;
//	}
//	sum = 0;
//	sum += (uint8_t) StatusDG.Ndiz;		sum += (uint8_t)(StatusDG.Ndiz>>8);
//	sum += (uint8_t) StatusDG.FreqU;	sum += (uint8_t)(StatusDG.FreqU>>8);
//	sum += (uint8_t) StatusDG.UDG;		sum += (uint8_t)(StatusDG.UDG>>8);
//	sum += (uint8_t) StatusDG.UBS;		sum += (uint8_t)(StatusDG.UBS>>8);
//	sum += (uint8_t) StatusDG.Igen;		sum += (uint8_t)(StatusDG.Igen>>8);
//	sum += (uint8_t) StatusDG.DM;
//	sum += (uint8_t) StatusDG.TOJ;
//	sum += (uint8_t) StatusDG.TBap;
//	sum += (uint8_t) StatusDG.LvlDIZ;
//	sum += (uint8_t) StatusDG.RegWrk;			sum += (uint8_t)(StatusDG.RegWrk>>8);
//	sum += (uint8_t) StatusDG.Isol;
//	sum += (uint8_t) StatusDG.Led;
//	sum += (uint8_t) StatusDG.RegSDG;			sum += (uint8_t)(StatusDG.RegSDG>>8);
//	sum += (uint8_t) StatusDG.Error;			sum += (uint8_t)(StatusDG.Error>>8);
//	sum += (uint8_t) StatusDG.RegimTime;		sum += (uint8_t)(StatusDG.RegimTime>>8);	
//	sum += (uint8_t) StatusDG.RegimError;		sum += (uint8_t)(StatusDG.RegimError>>8);
//	sum += (uint8_t) tmp32;				sum += (uint8_t)((tmp32>>8)&0xFF);
//	sum += (uint8_t)((tmp32>>16)&0xFF);	sum += (uint8_t)((tmp32>>24)&0xFF);
//	sum += (uint8_t)( 0xFF );
//	sum += (uint8_t)( 0xFE|type_PU );
//	
//	sum = 255 - sum;
//	++sum;
	sum = calc_CRC16(out_buf, (PDG_FRAME_SIZE-2));
	
	UART_ITConfig( MDR_UART1, UART_IT_RX, DISABLE );
	
	RS485_TX_ON;
	
	for(i=0;i<(PDG_FRAME_SIZE-2);i++)
	{
		UART1_send_byte( out_buf[i] );
	}
	UART1_send_byte((uint8_t)sum);
	
	__disable_irq();
	UART1_send_byte((uint8_t)(sum>>8));								//28
	while( UART_GetFlagStatus (MDR_UART1, UART_FLAG_BUSY) );
	RS485_TX_OFF;
	
	__enable_irq();
//	UART1_send_byte( 0xFF );
//	UART1_send_byte( 0xFE|type_PU );
//	UART1_send_byte((uint8_t) StatusDG.Ndiz );
//	UART1_send_byte((uint8_t) (StatusDG.Ndiz>>8));
//	UART1_send_byte((uint8_t) StatusDG.FreqU );
//	UART1_send_byte((uint8_t) (StatusDG.FreqU>>8));
//	UART1_send_byte((uint8_t) StatusDG.UDG );
//	UART1_send_byte((uint8_t) (StatusDG.UDG>>8));
//	UART1_send_byte((uint8_t) StatusDG.UBS );
//	UART1_send_byte((uint8_t) (StatusDG.UBS>>8));
//	UART1_send_byte((uint8_t) StatusDG.Igen );
//	UART1_send_byte((uint8_t) (StatusDG.Igen>>8));
//	UART1_send_byte((uint8_t) StatusDG.DM );
//	UART1_send_byte((uint8_t) StatusDG.TOJ );
//	UART1_send_byte((uint8_t) StatusDG.TBap );
//	UART1_send_byte((uint8_t) StatusDG.LvlDIZ );
//	UART1_send_byte((uint8_t) StatusDG.RegWrk );
//	UART1_send_byte((uint8_t) (StatusDG.RegWrk>>8));
//	UART1_send_byte((uint8_t) StatusDG.Isol );
//	UART1_send_byte((uint8_t) StatusDG.Led );
//	UART1_send_byte((uint8_t) StatusDG.RegSDG );
//	UART1_send_byte((uint8_t) (StatusDG.RegSDG>>8));
//	UART1_send_byte((uint8_t) StatusDG.Error );
//	UART1_send_byte((uint8_t) (StatusDG.Error>>8));
//	UART1_send_byte((uint8_t) StatusDG.RegimTime );
//	UART1_send_byte((uint8_t) (StatusDG.RegimTime>>8));
//	UART1_send_byte((uint8_t) StatusDG.RegimError );
//	UART1_send_byte((uint8_t) (StatusDG.RegimError>>8));	//27
//	UART1_send_byte((uint8_t) (tmp32&0xFF));
//	UART1_send_byte((uint8_t) ((tmp32>>8)&0xFF));
//	UART1_send_byte((uint8_t) ((tmp32>>16)&0xFF));
//	UART1_send_byte((uint8_t) ((tmp32>>24)&0xFF));

		
//	__disable_irq();
//	UART1_send_byte( sum );								//28
//	while( UART_GetFlagStatus (MDR_UART1, UART_FLAG_BUSY) );
//	RS485_TX_OFF;
//	
//	__enable_irq();
	
	while(!UART_GetFlagStatus( MDR_UART1, UART_FLAG_RXFE ))
	{
		UART_ReceiveData ( MDR_UART1 );
	}
	
	UART_ClearITPendingBit( MDR_UART1, UART_IT_RX );
	UART_ITConfig( MDR_UART1, UART_IT_RX, ENABLE );
	
	taskEXIT_CRITICAL();
		
//		++k;
	return true;
}


//==================================================================================
void ResetError( void )
{
	uint8_t cnt_bit = 0;
	
	StatusDG.Error = 0;
	StatusDG.RegimError = 0;
	alarm = false;
	StatusDG.Led &= 1;
	K6_OFF;
	K8_OFF;
	StatusDG.spt_time = 28;
	while( cnt_bit<16 )
	{
		ErrTime[ cnt_bit ] = ErrTime0[ cnt_bit ];
		++cnt_bit;
	}
}

//==================================================================================
void set_tpl(uint16_t nd)
{
	if( StatusDG.Ndiz < (nd-2) )
	{
		TPR_UP_ON;
		vTaskDelay( 100 );
		TPR_UP_OFF;
		vTaskDelay( 1500 );
	}
	if( StatusDG.Ndiz > (nd+30) )
	{
		TPR_DOWN_ON;
		vTaskDelay( 100 );
		TPR_DOWN_OFF;
		vTaskDelay( 1500 );
	}
}

//==================================================================================

