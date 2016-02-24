
//viev memory layout 0x8006010 URef


//#include <stdio.h>
//#include <math.h>
#include <stdbool.h>
//#include <1986be91.h>
#include "MDR32Fx.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_ssp.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "udp_frame.h"	//формат кадра UDP
#include "font5x7.h"

#include <stdio.h>


#define  LCD_DIS 		PORT_ResetBits(MDR_PORTD, PORT_Pin_4 )
#define  LCD_EN 		PORT_SetBits(MDR_PORTD, PORT_Pin_4 )
#define  LCD_ALL_ROW_OFF 	{PORT_ResetBits(MDR_PORTE, (PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|PORT_Pin_3|PORT_Pin_4));}

							
#define  LCD_ROW1_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_0))
#define  LCD_ROW2_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_1))
#define  LCD_ROW3_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_2))					
#define  LCD_ROW4_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_3))
#define  LCD_ROW5_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_4))

#define  LED_HL3_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_6))
#define  LED_HL4_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_7))
#define  LED_HL5_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_8))
#define  LED_HL6_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_9))
#define  LED_HL7_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_10))
#define  LED_HL8_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_11))
#define  LED_HL9_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_13))
#define  LED_HL10_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_14))

#define  LED_HL3_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_6))
#define  LED_HL4_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_7))
#define  LED_HL5_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_8))
#define  LED_HL6_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_9))
#define  LED_HL7_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_10))
#define  LED_HL8_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_11))
#define  LED_HL9_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_13))
#define  LED_HL10_ON 	PORT_ResetBits(MDR_PORTE, (PORT_Pin_14))

#define  ZUM_OFF 	PORT_ResetBits(MDR_PORTD, (PORT_Pin_12))
#define  ZUM_ON 	PORT_SetBits(MDR_PORTD, (PORT_Pin_12))

#define  ALL_LED_HL_OFF 	PORT_SetBits(MDR_PORTE, (PORT_Pin_6|PORT_Pin_7|PORT_Pin_8|PORT_Pin_9|PORT_Pin_10|PORT_Pin_11|PORT_Pin_13|PORT_Pin_14))


extern packet_To_eth PKDU_To_Eth;
extern packet_From_eth PKDU_From_Eth;

stat PKDU_Status = {0};


uint16_t RomReceiveEthernet[10];


const unsigned  char TabDecSeg[10] = {0Xfc,0X60,0Xda,0Xf2,0X66,
                                      0Xb6,0Xbe,0Xe0,0Xfe,0Xf6};

																			
const unsigned int CtError0[16] = {100,100,100,100,100,100,30,30,30,
									30,30,10,120,300,120,120};																			

uint8_t RegTransmitRS1[41],
		RamReceiveRS1[41],
		RomReceiveRS1[41][6],																						 
		ParamInd = 0;

uint16_t 	TimeWork = 0,
			Avaria = 0,
			RegimIndMain = 0,
			RegSOld = 0,
			TestTimer2 = 0,	
			Error=0,
			CtReceiveRS1,
			CtTransmitRS1,
			ControlTransmit1,
			ControlReceive1,
			RegimTr=0,																				
			Segment[10],
			CtUsart[2],
			TestUsart[10],
			CtUsart[2],											
			CtError[16],
			TestMDR[5];

const unsigned int CtReceiveRS0=40;

const unsigned int NumberBlock1=2;

unsigned int TestError[5];

volatile uint32_t	led_reg[7][5],
					index_char[16],
					IndData[5];	

bool RX_RS_OK = false;




const uint8_t   LGED2x100[]={62,37 ,0  ,18,54 ,17 ,16 ,16},
				OTKAZ[]    ={47,51 ,43 ,33,40},
				KDG[]      ={43,37 ,36},
				PZF[]      ={48,40 ,53},
				OK[]       ={47,43},
				Raznos[]   ={0 ,49 ,33 ,40,46 ,47 ,50},
				Peregrev[] ={48,38 ,49 ,38,36 ,49 ,38 ,35},
				Disel[]    ={0 ,37 ,41 ,40,38 ,44 ,64},
				Ug[]       ={0 ,0  ,0  ,65,36},
				Ig[]       ={0 ,0  ,0  ,66,36},
				Ig_na_10A[]={66,36 ,0  ,46,33 ,17 ,16 ,33},
				n_norm[]   ={46,38 ,0  ,46,47 ,49 ,45},
				Overload[] ={48,38 ,49 ,38,36 ,49 ,52 ,40},
				error[]    ={0 ,47 ,57 ,41,34 ,43 ,33},
				RN[]       ={0 ,0  ,0  ,49,46},
				NO[]       ={0 ,0  ,46 ,38,51},
				MASLA[]    ={0 ,45 ,33 ,50,44 ,33},
				DM_MASLA[] ={0 ,37 ,45 ,28,1  ,43 ,36},
				Topliv[]   ={51,47 ,48 ,44,41 ,35 ,33},
				PUSK[]     ={0 ,0  ,48 ,52,50 ,43},
				OG_b100[]  ={0 ,51 ,47 ,39,30 ,17 ,16 ,16},
				OG_b105[]  ={0 ,51 ,47 ,39,30 ,17 ,16 ,21},
				OSTANOV[]  ={47,50 ,51 ,33,46 ,47 ,35},
				Rashol[]   ={49,33 ,50 ,54,47 ,44 ,33 ,39},
				NO_STOP[]  ={0 ,0  ,46 ,38,51 ,0  ,47 ,50 ,51,33,46,47 ,35,33},
				NO_Vozb[]  ={0 ,0  ,0  ,46,38 ,51 ,0  ,35 ,47,40,34,14},
				TOLKO[]    ={0 ,51 ,47 ,44,61 ,43 ,47},
				PDU[]      ={0 ,0  ,48 ,37,52},
				SWECH[]    ={0 ,0  ,50 ,35,38 ,56 ,33},
				ON[]       ={0 ,0  ,0  ,35,43 ,44},
				OFF[]      ={0 ,0  ,35 ,60,43 ,44},
				PJD[]      ={0 ,0  ,0  ,48,39 ,37},
				DZF[]      ={0 ,0  ,0  ,37,40 ,53},
				SPT[]      ={50,48 ,51},
				uOJ[]      ={0 ,52 ,49 ,0 ,47 ,39},
				Progrev[]  ={48,49 ,47 ,36,49 ,38 ,35},
				WORK[]     ={0 ,49 ,33 ,34,47 ,51 ,33},
				FIRE[]     ={0 ,48 ,47 ,39,33 ,49},
				GOTOV[]    ={0 ,36 ,47 ,51,47 ,35},
				NIZK[]     ={0 ,46 ,41 ,40,43 ,33 ,64},
				ISOL[]     ={0 ,41 ,40 ,47,44 ,14},
				BLOK[]     ={0 ,0  ,34 ,44,47 ,43},
				PRTC[]     ={0 ,40 ,33 ,58,41 ,51},
				VOZD[]     ={0 ,35 ,47 ,40,37},
				FILTR[]    ={0 ,53 ,41 ,44,61 ,51 ,49};	






//void UART2_send_byte(uint8_t);
void LoadLED_Reg( uint8_t ,uint8_t );
void LoadRegTransmit( void );
void load_led_reg( void );
void show_led_data( uint8_t num_par );
void clear_lcd( void );
void Show_led_str( const uint8_t *str, uint8_t n_str, uint8_t len );
void DecSegm( uint16_t R0 );

//=======================================================================================
//=======================================================================================
//=======================================================================================

/*void vLink_SysTask( void *pvParameters ) //процесс связи
{ 
	uint8_t i = 0;
	
	for( ;; ) 
	{ 
		vTaskDelay( 10 );
		
		
		if(RX_RS_OK)//приняты данные
		{
			vTaskDelay( 100 );
			UART_ITConfig(MDR_UART2,UART_IT_RX, DISABLE);
			LoadRegTransmit();
			RS485_TX_ON;
			i = 11;
			
			while(i>1)
			{
				--i;
				UART2_send_byte(RegTransmitRS1[i]);
			}
			
			__disable_irq();
			UART2_send_byte(RegTransmitRS1[0]);
			RS485_TX_OFF;
			__enable_irq();
			
			UART_ITConfig(MDR_UART2,UART_IT_RX, ENABLE);
			RX_RS_OK = false;
		}
	}
}*/
//=======================================================================================

//=======================================================================================
void ControlRegimInd(void)
{
	static unsigned int AvariaOld = 0;
	
//	if((!Avaria)&&(!RegimIndMain))
//		RegimIndMain = 1;
//	
//	if(RegimIndMain > 1)
//		RegimIndMain = 1;
//	
//	if(Avaria != AvariaOld)
//	{
//		RegimIndMain = 0;
//		AvariaOld = Avaria;
//	}
//	else 
//	if((RegS & 0x40)&&(!(RegSOld & 0x40)))
//	{
//		if(!RegimIndMain)
//			RegimIndMain = 1;
//		else
//			RegimIndMain = 0;

//		RegSOld |= 0x40;
//	}
//	
//	if(!(RegS & 0x40))
//		RegSOld &= 0xffbf;
}
//=======================================================================================
void IndicatorParam(void)
{
//	static unsigned int R0;
//	static unsigned int R1;

// 	switch(ParamInd)
// 	{
// 		case 0:	R0=CtUsart[0];
// 				R1=TestUsart[5];
// 				Segment[0]=0x20;
// 				Segment[1]=0;

// 		break;
// 		case 1:	R0=TestUsart[0];
// 				R1=0 & 0xff;
// 				Segment[0]=0x10;

// 		break;
// 		case 2:	R0=0>>8;
// 				R1=0 & 0xff;
// 				Segment[0]=0x8;

// 		break;

// 		case 3:	R0=0;
// 				R1=0;
// 				Segment[0]=0x4;

// 		break;

// 		case 4:	R0=0>>1;
// 				R1=0>>1;
// 				Segment[0]=0x2;

// 		break;

// 		case 5:	R0=0;
// 				R1=0>>2;
// 				Segment[0]=0x1;

// 		break;
// 		case 6:	R0=0>>2;
// 				R1=0;
// 				Segment[0]=0;
// 				Segment[1]=0x20;

// 		break;
// 		case 7:	R0=RegS & 0xff;
// 				R1=RegS>>8;
// 				Segment[1]=0x10;
// 		break;
// 		case 8:	R0=RegS & 0xff;
// 				R1=RegS>>8;
// 				Segment[1]=0x8;
// 		break;
// 		case 9:	R0=RegS & 0xff;
// 				R1=RegS>>8;
// 				Segment[1]=0x4;
// 		break;
// 		case 10:	R0=RegS & 0xff;
// 					R1=RegS>>8;
// 					Segment[1]=0x2;
// 		break;
// 		case 11:	R0=RegS & 0xff;
// 					R1=RegS>>8;
// 					Segment[1]=0x1;
// 		break;
// 		case 12:	R0=RegS & 0xff;
// 					R1=RegS>>8;
// 					Segment[1]=0x80;
// 		break;
// 		case 13:	R0=RegS & 0xff;
// 					R1=RegS>>8;
// 					Segment[1]=0x40;
// 		break;
// 		case 14:	R0=RegS & 0xff;
// 					R1=RegS>>8;
// 					Segment[0]=0x80;
// 					Segment[1]=0;
// 		break;
// 		case 15:	R0=RegS & 0xff;
// 					R1=RegS>>8;
// 					Segment[0]=0x40;
// 		break;

// 		default:break;
// 	}
// R0 = 0;
// R1 = 0;
//	Segment[0] = (uint8_t)(RegS>>8);
//	Segment[1] = (uint8_t)RegS;
//	Indicator1((uint8_t)PKDU_From_Eth.DG_Cmd);
//	Indicator1(RomReceiveRS1[37][3]);//(TestMDR[0]);//RomReceiveRS1[37][1]);//MDR_TIMER1->CNT>>8);//(uint8_t)PKDU_Status.DG_Cmd);
//	Indicator2(RomReceiveRS1[37][1]);//TestMDR[1]);//CtError[1]);//(uint8_t)PKDU_Status.DG_Fn);	
//	Indicator2((uint8_t)PKDU_Status.DG_Fn);
PKDU_To_Eth.TOG = RomReceiveRS1[37][1];
PKDU_To_Eth.DM = RomReceiveRS1[37][3];
PKDU_To_Eth.I_L = RomReceiveRS1[37][4];

}
//=======================================================================================
void ControlParamInd(void)
{
//	if(RegimIndMain == 1)
//	{
//		if((RegS & 0x80) && (!(RegSOld & 0x80)))
//		{
//		if(ParamInd<15)
//			++ParamInd;
//		else
//			ParamInd = 0;
//		
//		RegSOld |= 0x80;
//		}
//	}
//	
//	if(!(RegS & 0x80))
//		RegSOld &= 0xff7f;

}

//=======================================================================================

void LoadRegTransmit(void)
{
	static unsigned int R1;
	uint32_t RegS = PKDU_Status.Ctrl_BTN;
//	static unsigned int R2;
	
	RegTransmitRS1[40] = 0xff;
	RegTransmitRS1[39] = 0xff;
	
	RegTransmitRS1[38] = NumberBlock1;
	R1=RegS>>8;

	RegTransmitRS1[37] = PKDU_From_Eth.DG_Cmd;
	R1=RegS;
	RegTransmitRS1[6] = R1 & 0xff;
	R1=RomReceiveEthernet[5];
	RegTransmitRS1[5] = R1 & 0xff;
	R1=RomReceiveEthernet[4];
	RegTransmitRS1[4] = R1 & 0xff;
	R1=RomReceiveEthernet[3];
	RegTransmitRS1[3] = R1 & 0xff;
	R1=RomReceiveEthernet[2];
	RegTransmitRS1[2] = R1 & 0xff;
	R1=RomReceiveEthernet[1];
	RegTransmitRS1[1] = R1 & 0xff;

}

void TransmitUsart(void)
{
	unsigned int R3;
	LoadRegTransmit();

	RegimTr = 1;
	MDR_PORTD->RXTX |=0x4;
	R3 = 50;
	while(R3)
		--R3;
	MDR_UART2->CR |= 0x100;
	MDR_UART2->DR = 0xff;//NumberBlock1;
	MDR_UART2->ICR |= 0x20;//Reset Int Transmit
	CtTransmitRS1 = CtReceiveRS0;

	ControlTransmit1=0;//NumberBlock1;
	ControlReceive1=0;

	if(TestUsart[4])
		--TestUsart[4];
	else
		TestUsart[4]=100;
}

//=======================================================================================
/*void UART2_IRQHandler(void)
{
	uint8_t tmp = 0;
	static uint8_t cnt_RX = 0;
	
	if(UART_GetITStatus(MDR_UART2,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART2,UART_IT_RX);
		tmp = UART_ReceiveData(MDR_UART2);
		
		if(!RX_RS_OK) //предыдущие данные обработаны
		{
			RamReceiveRS1[cnt_RX] = tmp;
			++cnt_RX;
			
			if(cnt_RX == 10)
			{
				cnt_RX = 0;
				RX_RS_OK = true;
			}
		}
	}
	
	portYIELD_FROM_ISR( NULL );
}*/





void UART2_IRQHandler(void)
{
unsigned int R0;
unsigned int R1;
unsigned int R2;

        if(TestUsart[5])
			--TestUsart[5];
        else
			TestUsart[5]=110;


	R1=MDR_UART2->MIS & 0xfff;


	R0= MDR_UART2->DR & 0xfff;
		
	if(R1 & 0x780)//Int Error
		{

		MDR_UART2->ICR |=0xfff;//reset int
		MDR_UART2->RSR_ECR=0xff;//Reset Error
		CtReceiveRS1=CtReceiveRS0;
		CtTransmitRS1=CtReceiveRS0;
		MDR_PORTD->RXTX &=0xfb;
		return;
		}
	else if(R1 & 0x10)
	{
		R0 &=0xff;
		MDR_UART2->ICR |=0xfff;//reset int
		MDR_UART2->RSR_ECR=0xff;//Reset Error

		if(RegimTr)
		{

			if(CtTransmitRS1>1)
			{

				--CtTransmitRS1;
				MDR_UART2->DR=RegTransmitRS1[CtTransmitRS1];
				if(CtTransmitRS1>=(CtReceiveRS0-1))
					ControlTransmit1=0;
				else
					ControlTransmit1 +=RegTransmitRS1[CtTransmitRS1];

			}
			else if(CtTransmitRS1==1)
			{

				--CtTransmitRS1;
				MDR_UART2->DR=ControlTransmit1;

			}
			else
			{
				++TestUsart[0];
				if(TestUsart[0]>999)
					TestUsart[0]=0;
				RegimTr=0;

				MDR_PORTD->RXTX &=0xfb;
				CtUsart[0]=10;//300;//32;
				CtTransmitRS1=CtReceiveRS0;
				CtError[2]=CtError0[1];
				CtError[7]=CtError0[6];
			}
		}

		else
		//Receive far

		{
			if(!CtReceiveRS1)
			{
				if(TestUsart[1])
					--TestUsart[1];
				else
					TestUsart[1]=5000;
			}
			TestUsart[3]=CtReceiveRS1;

			RamReceiveRS1[CtReceiveRS1]=R0;
			if(CtReceiveRS1==CtReceiveRS0)
			{

				if(R0==0xff)
					--CtReceiveRS1;
				else
					CtReceiveRS1=CtReceiveRS0;
				return;
			}
			else if(CtReceiveRS1==(CtReceiveRS0-1))
			{
				if(R0==0xff)
				{
					--CtReceiveRS1;
					ControlReceive1=0;
				}
				else
					CtReceiveRS1=CtReceiveRS0;
				return;
			}
			else if((CtReceiveRS1)&&(CtReceiveRS1<=(CtReceiveRS0-2)))
			{

				--CtReceiveRS1;
				ControlReceive1 +=R0;


			}
			else if(!CtReceiveRS1)
			{

				CtReceiveRS1=CtReceiveRS0;
				TestUsart[2]=RamReceiveRS1[8];
				if(TestUsart[8])
					--TestUsart[8];
				else
					TestUsart[8]=5000;

				if((ControlReceive1& 0xff)!=R0)
				{
					TestError[0]=R0;
					TestError[1]=ControlReceive1 & 0xff;
				}
				else
				{
					TestError[2]=R0;
					TestError[3]=ControlReceive1 & 0xff;
				}

				if((ControlReceive1 & 0xff)==R0)
				{
													
					TestMDR[0]=RamReceiveRS1[37];
					TestMDR[1]=RamReceiveRS1[38];																					

					if(RamReceiveRS1[38]==1)
					{
																	
					for(R2=0;R2<=40;++R2)
						RomReceiveRS1[R2][1]=RamReceiveRS1[R2];																								
					CtUsart[0]=2;
					CtError[1]=CtError0[1];
					CtError[6]=CtError0[6];
					}
					else if(RamReceiveRS1[38]==5)
					{
					for(R2=0;R2<=40;++R2)
						RomReceiveRS1[R2][5]=RamReceiveRS1[R2];																									
					CtUsart[0]=4;
					CtError[5]=CtError0[1];
					CtError[10]=CtError0[6];
					}
					else if(RamReceiveRS1[38]==4)
					{
					for(R2=0;R2<=40;++R2)
						RomReceiveRS1[R2][4]=RamReceiveRS1[R2];																								
					CtUsart[0]=6;
					CtError[4]=CtError0[1];
					CtError[9]=CtError0[6];
					}

					else if(RamReceiveRS1[38]==3)
					{
					for(R2=0;R2<=40;++R2)
						RomReceiveRS1[R2][3]=RamReceiveRS1[R2];																									
					CtUsart[0]=8;
					CtError[3]=CtError0[1];
					CtError[8]=CtError0[6];
					}
				}
			}
		}
	}
}









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
	
	
	portYIELD_FROM_ISR( NULL );
}




void Timer2_IRQHandler(void)
{
	unsigned char R0=0;
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
	
//	if(!(MDR_PORTD->RXTX & 4))//Transmit =off
//		CtError[11]=CtError0[11];

//	if(Error & 0x800)
//		{
//		MDR_PORTD->RXTX &=0xfb;
//		//InitUsart9600();

//		CtReceiveRS1=CtReceiveRS0;
//		CtTransmitRS1=CtReceiveRS0;
//		CtUsart[0]=0x10;
//		Error &=0xf7ff;

//		CtError[11]=CtError0[11];

//		}
//	else if((Error & 0x40)&&(Error & 0x100)&&(Error & 0x200)&&(Error & 0x400))
//		{
//		//InitUsart9600();

//		CtReceiveRS1=CtReceiveRS0;
//		CtTransmitRS1=CtReceiveRS0;
//		CtUsart[0]=0x10;
//		Error &=0xf8bf;
//		CtError[6]=CtError0[6];
//		CtError[8]=CtError0[6];
//		CtError[9]=CtError0[6];
//		CtError[10]=CtError0[6];
//		}


//	for(R0=1;R0<=11;++R0)
//		{
//		if(CtError[R0])
//			--CtError[R0];
//		}


//	if(CtUsart[0])
//		--CtUsart[0];
//	else
//	{
//	// if(PORTD->RXTX & 0x1)
//	TransmitUsart();
//	CtUsart[0]=10;//64;//32;
//	}
//	
//	if(TestTimer2)
//		--TestTimer2;
//	else
//		TestTimer2=200;	

	portYIELD_FROM_ISR( NULL );
}

//void main(void)
void vSmirnov_Task( void *pvParameters )
{
	uint8_t cnt_b=0,strl[10];
	uint32_t RegS=0, RegS_old=1;
	
// 	#ifdef DEBUG
// 		debug();
// 	#endif
	
	
	while( cnt_b < 16 )
	{
		index_char[ cnt_b ] = 6*5; //FF
		++cnt_b;
	}
	load_led_reg();
	
	ZUM_ON;
	LED_HL3_ON;
	LED_HL4_ON;
	LED_HL5_ON;
	LED_HL6_ON;
	LED_HL7_ON;
	LED_HL8_ON;
	LED_HL9_ON;
	LED_HL10_ON;
	
	vTaskDelay( 1000 );
	clear_lcd();
	
	ZUM_OFF;
	LED_HL3_OFF;
	LED_HL4_OFF;
	LED_HL5_OFF;
	LED_HL6_OFF;
	LED_HL7_OFF;
	LED_HL8_OFF;
	LED_HL9_OFF;
	LED_HL10_OFF;
	
	ALL_LED_HL_OFF;
	

	
	Show_led_str(LGED2x100,0,8);
	Show_led_str(PDU,1,5);	
	vTaskDelay( 1000 );
	
//InitTimer3();

	while(1)
	{
		//IWDG->KR = 0xaaaa;
		//Avaria = 0x41;
		if(PKDU_Status.ERROR)
		{
			clear_lcd();
			Show_led_str(error,0,7);
			
			if(PKDU_Status.ERROR<8)
				index_char[11] = (PKDU_Status.ERROR+16)*5;
			else
				index_char[11] = 6*5;
			
			load_led_reg();
			vTaskDelay( 1000 );
		}
		//PKDU_Status.DG_Cmd = 0x00;
		RegS = PKDU_Status.Ctrl_BTN;
		
		
		
		//отправляем в инет то же что и приняли
		PKDU_To_Eth.DG_Cmd = PKDU_From_Eth.DG_Cmd;
		//кое какие кнопки
		PKDU_To_Eth.DG_res1 = (uint16_t)PKDU_Status.Ctrl_BTN;
		
		
		
		
		
		
		
		
		
		if(RegS_old != RegS)//кнопки на дисплее
		{
			RegS_old = RegS;
			
			sprintf((char *)strl,"%08X",RegS);

			for(cnt_b=0;cnt_b<8;cnt_b++)
			{
				if(strl[cnt_b]<'9')
					index_char[cnt_b] = (strl[cnt_b]-'0'+16)*5;
				else
					index_char[cnt_b] = 6*5;
			}
			load_led_reg();
		}
		
		
		
		
		
		
		
		
		
		ControlRegimInd();
		
		ControlParamInd();
		
		IndicatorParam();
		
		
		
		if( TimeWork < 9999 )
			++TimeWork;
		else
			TimeWork = 0;
		
		vTaskDelay( 10 );
	}
}
//==================================================================================

void Show_led_str( const uint8_t *str, uint8_t n_str, uint8_t len )
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
		while( cnt_b < len )
		{
			index_char[ cnt_b ] =(uint32_t)str[ cnt_b ]*5;
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
		while( i < len )
		{
			index_char[ cnt_b ] = (uint32_t)str[i]*5;
			++cnt_b;
			++i;
		}
	}

	load_led_reg();
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
		{
			led_reg[reg][row] = 0;
			
			bit16 = 0;
			while( (bit16 < 16) && (reg < 7))
			{
				while( bit8 < 7 )
				{
					++bit8;	
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
/*void UART2_send_byte(uint8_t byte)
{
	UART_SendData(MDR_UART2,byte);
	//time_out_byte=0;
	//while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	while(UART_GetFlagStatus (MDR_UART2,UART_FLAG_BUSY) || !UART_GetFlagStatus (MDR_UART2,UART_FLAG_RXFF))
	{
// 		if(time_out_byte>3)
// 			break;
	}
	UART_ReceiveData (MDR_UART2);
	UART_ClearITPendingBit(MDR_UART2,UART_IT_RX);
// 	while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY))
// 	{
// 		UART_ReceiveData (MDR_UART1);
// 	}
}*/
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



