
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

																			
const uint32_t 	CtError0[16] = {100,100,100,100,100,100,30,30,30,30,30,10,120,300,120,120},
				CtTime0[16] = {1000,10,200,1000,22500,10,10,10,75000,1000,1000,37500,7500,1000,1015,1016},
				CtReceiveRS0 = 40,
				NumberBlock1 = 2,
				CtError1_0[16] = {500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500};

uint32_t RegTransmitRS1[81],
		RamReceiveRS1[81],
		RomReceiveRS1[81][3],																						 
		ParamInd = 0,
		RegimIndAvaria=0,						
		RegS=0,RegSOld=0,RegS1=0,RegKnOld=0,
		TestError[5],
		CtKn=0,
		RegSWork=0,
		CtIndAvaria[96],
		AdResult[8],
		TestAd[8],
		RIzol=0,
		Minus=0,
		UBat=0,
		IBat=0,
		FLine=0,
		ILine=0,
		ULine=0,
		ISet=0,
		Power=0,
		PowerSet=0,
		PowerGen1=0,
		FSet=0,
		USet=0,
		CtUEndL=0,
		RegimWork=0,
		CtError1[16],
		CtError2[16],
		UGen1=0,
		FGen1=0,
		IGen1=0,
		Error2=0,
		Error1=0,
		Avaria = 0,
		AvariaHigh =0,
		AvariaIndHigh = 0,
		AvariaOldHigh = 0,
		AvariaHighHigh =0,
		AvariaIndHighHigh=0,
		AvariaOldHighHigh=0,
		AvariaOld=0,
		NDiz1=0,
		NDiz2=0,
		PM1=0,
		TOG2=0,
		TGG2=0,
		TOG1=0,
		TGG1=0,
		UT1=0,
		CtError[32];
									
uint16_t 	TimeWork = 0,
			
			RegimIndMain = 0,
			TestTimer2 = 0,	
			Error=0,
			CtReceiveRS1=0,
			CtTransmitRS1=0,
			ControlTransmit1=0,
			ControlReceive1=0,
			RegimTr=0,																				
			Segment[10],
			CtUsart[2],
			TestUsart[10],
			CtUsart[2],											
			TestMDR[5];




volatile uint32_t	led_reg[7][5],
					index_char[16],
					IndData[5];	

bool RX_RS_OK = false, timer2_on=false;




const uint8_t   LGED2x100[]={62,37 ,0  ,18,54 ,17 ,16 ,16},
				OTKAZ[]    ={47,51 ,43 ,33,40},
				KDG[]      ={43,37 ,36},
				PZF[]      ={48,40 ,53},
				OK[]       ={47,43},
				Raznos[]   ={0 ,49 ,33 ,40,46 ,47 ,50},
				DG1[]	   ={0 ,0 ,37 ,36,17 },
				PM_4_0[]   ={0 ,49 ,45,0,20,14,16 },
				TOJ_105[]  ={0 ,51 ,47,39,17,16,21 },
				NO_VOZB[]  ={46 ,38,51,0,35,47,40,34 },
				NO_OBOR[]  ={46 ,38,51,0,47,34,47,34 },
				Ug1[]      ={65,36,17},
				Ig1[]      ={66,36,17},
				Fg1[]      ={67,36,17},
				Pg1[]      ={49,36,17},
				N_1[]      ={68,17},
				PM_1[]     ={49,45,17},
				Ug2[]      ={65,36,18},
				Ig2[]      ={66,36,18},
				Fg2[]      ={67,36,18},
				Pg2[]      ={49,36,18},
				N_2[]      ={68,18},
				PM_2[]     ={49,45,18},
				UC[]	   ={65,50},
				IC[]       ={66,50},
				FC[]       ={67,50},
				PC[]       ={49,50},
				FN[]       ={67,68},
				UN[]       ={65,68},
				TOJ1[]     ={51,47,39,17},
				TGJ1[]     ={51,36,39,17},
				UT_1[]     ={52,51,17},
				RISOL1[]   ={69,41,40,17},
				UPR[]    ={0 ,0,52 ,48,49 },
				INTERN[] ={41,46 ,51 ,38,49 ,46 ,38 ,51},
				
				Disel[]    ={0 ,37 ,41 ,40,38 ,44 ,64},
				
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

void    ControlRegimInd(void)
{
	if((!Avaria)&&(!AvariaHigh)&&(!AvariaHighHigh)&&(!RegimIndMain))
	RegimIndMain=1;
	if(RegimIndMain>1)
	RegimIndMain=1;
	if((Avaria !=AvariaOld)||(AvariaHigh !=AvariaOldHigh)||(AvariaHighHigh !=AvariaOldHighHigh))
	{
		RegimIndMain=0;
		AvariaOld=Avaria;
		AvariaOldHigh=AvariaHigh;
		AvariaOldHighHigh=AvariaHighHigh;
	}


	else if((RegS & 0x400)&&(!(RegSOld & 0x400)))

	{
		if(!RegimIndMain)
		RegimIndMain=1;
		else
		RegimIndMain=0;

		RegSOld |=0x400;
	}
	if(!(RegS & 0x400))
	RegSOld &=0xfffffbff;
}
//=======================================================================================
//void IndicatorParam(void)
//{

//PKDU_To_Eth.TOG = RomReceiveRS1[37][1];
//PKDU_To_Eth.DM = RomReceiveRS1[37][3];
//PKDU_To_Eth.I_L = RomReceiveRS1[37][4];

//}

void Indicator3(unsigned int R0,unsigned char R5)
{
	static unsigned short R1;
	static unsigned int R2;
	static unsigned int R3;

	R1=R0;
	if(R1>999)
		R1=999;
	R3=0;
	while(R1>=100)
	{
		R1 -=100;
		++R3;
	}
	R2=0;
	while(R1>=10)
	{
		R1 -=10;
		++R2;
	}

	index_char[R5]=(R3+16)*5;
	index_char[R5+1]=(R2+16)*5;
	index_char[R5+2]=(R1+16)*5;
}

void Indicator1(unsigned int R0,unsigned char R5)
{
	static unsigned int R1;
	static unsigned int R2;
	static unsigned int R3;
	static unsigned int R4;

	R1=R0;
	if(R1>9999)
	R1=9999;
	R4=0;
	while(R1>=1000)
	{
		R1 -=1000;
		++R4;
	}

	R3=0;
	while(R1>=100)
	{
		R1 -=100;
		++R3;
	}
	R2=0;
	while(R1>=10)
	{
		R1 -=10;
		++R2;
	}
	index_char[R5]=(R4+16)*5;
	index_char[R5+1]=(R3+16)*5;
	index_char[R5+2]=(R2+16)*5;
	index_char[R5+3]=(R1+16)*5;
}

void Indicator3_(unsigned int R0,unsigned char R5)
{
	static unsigned short R1;
	static unsigned int R2;
	static unsigned int R3;


	R1=R0;
	if(R1>999)
	R1=999;
	R3=0;
	while(R1>=100)
	{
		R1 -=100;
		++R3;
	}
	R2=0;
	while(R1>=10)
	{
		R1 -=10;
		++R2;
	}

	index_char[R5]=(R3+16)*5;
	index_char[R5+1]=(R2+16)*5;
	index_char[R5+2]=14*5;
	index_char[R5+3]=(R1+16)*5;
}
void Indicator2_(unsigned int R0,unsigned char R5)
{
	static unsigned short R1;
	static unsigned short R2;



	R1=R0;
	if(R1>99)
	R1=99;

	R2=0;
	while(R1>=10)
	{
		R1 -=10;
		++R2;
	}


	index_char[R5]=(R2+16)*5;
	index_char[R5+1]=14*5;
	index_char[R5+2]=(R1+16)*5;
}

void IndicatorParam(void)
{

	switch(ParamInd)
	{
	case 0:
		Show_led_str(Ug1,0,3);
		Indicator3(UGen1,5);
	
		Show_led_str(Ig1,1,3);
		Indicator3(IGen1,13);
		load_led_reg();
	
		break;
	case 1:
		Show_led_str(Fg1,0,3);
		Indicator3(FGen1,5);
	
		Show_led_str(Pg1,1,3);
		Indicator3(PowerGen1,13);
		load_led_reg();

		break;
	case 2:
		Show_led_str(N_1,0,2);
		Indicator1(NDiz1, 4);

		Show_led_str(PM_1,1,3);
		Indicator2_(PM1, 13);
		load_led_reg();
	
		break;
	case 3:
		Show_led_str(TOJ1,0,4);
		Indicator1(TOG1, 4);
	
		Show_led_str(TGJ1,1,4);
		Indicator3(TGG1, 13);
		load_led_reg();

		break;
	case 4:
		Show_led_str(UT_1,0,3);
		Indicator3(UT1, 5);
	
		Show_led_str(RISOL1,1,4);
		Indicator2_(CtError[2], 13);
		load_led_reg();
		break;


	case 10:
		Show_led_str(Ug2,0,3);
		Indicator3(RegS, 5);
	
		Show_led_str(Ig2,1,3);
		Indicator3(RegS, 13);
		load_led_reg();


		break;
	case 11:
		Show_led_str(Fg2,0,3);
		Indicator3(RegS, 5);
	
		Show_led_str(Pg2,1,3);
		Indicator3(RegS, 13);
		load_led_reg();

		break;
	case 12:
		Show_led_str(N_2,0,3);
		Indicator1(NDiz2, 4);
	
		Show_led_str(PM_2,1,3);
		Indicator3(RegS, 13);
		load_led_reg();

		break;
	case 20:
		Show_led_str(UC,0,2);
		Indicator3(USet, 5);
	
		Show_led_str(IC,1,2);
		Indicator3(ISet, 13);
		load_led_reg();

		break;
	case 21:
		Show_led_str(FC,0,2);
		Indicator3_(FSet, 4);
	
		Show_led_str(PC,1,2);
		Indicator3(PowerSet, 13);
		load_led_reg();

		break;
	case 22:
		Show_led_str(FN,0,2);
		Indicator3_(FLine, 4);
	
		Show_led_str(UN,1,2);
		Indicator3(ULine, 13);
		load_led_reg();

		break;



	default:break;
	}

}

//=======================================================================================
void ControlParamInd(void)
{
	static unsigned int R0;
	R0=ParamInd;
	if(RegimIndMain==1)
	{
		if((RegS & 0x10000)&&(!(RegSOld & 0x10000)))

		{
			if(R0>10)
			R0=0;
			else if(R0<10)
			++R0;
			else
			R0=0;
			RegSOld |=0x10000;
			ParamInd=R0;
		}
		else if((RegS & 0x20000)&&(!(RegSOld & 0x20000)))

		{
			if(R0>10)
			R0=10;
			else if(R0)
			--R0;
			else
			R0=10;
			RegSOld |=0x20000;
			ParamInd=R0;
		}
		else if((RegS & 0x10)&&(!(RegSOld & 0x10)))

		{
			if(R0<10)
			R0=10;
			else if(R0>20)
			R0=20;
			else if(R0<20)
			++R0;
			else
			R0=10;
			RegSOld |=0x10;
			ParamInd=R0;
		}
		else if((RegS & 0x20)&&(!(RegSOld & 0x20)))

		{
			if(R0<10)
			R0=10;
			else if(R0>20)
			R0=20;
			else if(R0>10)
			--R0;
			else
			R0=20;
			RegSOld |=0x20;
			ParamInd=R0;
		}
		else if((RegS & 0x100)&&(!(RegSOld & 0x100)))

		{
			if(R0<20)
			R0=20;

			else if(R0<30)
			++R0;
			else
			R0=20;

			RegSOld |=0x100;
			ParamInd=R0;
		}

	}
	if(!(RegS & 0x10000))
	RegSOld &=0xfffeffff;
	if(!(RegS & 0x20000))
	RegSOld &=0xfffdffff;
	if(!(RegS & 0x10))
	RegSOld &=0xffffffef;
	if(!(RegS & 0x20))
	RegSOld &=0xffffffdf;
	if(!(RegS & 0x100))
	RegSOld &=0xfffffeff;


}
//=======================================================================================

void LoadRegTransmit(void)
{
	static unsigned int R1;
	static unsigned int R2;

	RegTransmitRS1[80]=0xff;
	RegTransmitRS1[79]=0xff;
	RegTransmitRS1[78]=NumberBlock1;
	RegTransmitRS1[77]=RegimWork & 0xff;
	RegTransmitRS1[76]=RegimWork >>8;
	RegTransmitRS1[40]=0xff;
	RegTransmitRS1[39]=0xff;
	RegTransmitRS1[38]=NumberBlock1;
	R1=RegS>>8;
	RegTransmitRS1[26]=R1 & 0xff;
	R1=RegS;
	RegTransmitRS1[27]=R1 & 0xff;
	
	//R1=RomReceiveEthernet[5];
	R1=PKDU_From_Eth.DG_Cmd;
	RegTransmitRS1[5]=R1 & 0xff;
	//R1=RomReceiveEthernet[4];
	R1=PKDU_From_Eth.DG_Cmd>>8;
	RegTransmitRS1[4]=R1 & 0xff;
	R1=RomReceiveEthernet[3];
	RegTransmitRS1[3]=R1 & 0xff;
	R1=RomReceiveEthernet[2];
	RegTransmitRS1[2]=R1 & 0xff;
	R1=RomReceiveEthernet[1];
	RegTransmitRS1[1]=0x55;//R1 & 0xff;


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
	RamReceiveRS1[CtTransmitRS1]=R0;
	if(R1 & 0x780)//Int Error
	{

		MDR_UART2->ICR |=0xfff;//reset int
		MDR_UART2->RSR_ECR=0xff;//Reset Error
		CtReceiveRS1=CtReceiveRS0;
		CtTransmitRS1=CtReceiveRS0;
		MDR_PORTD->RXTX &=0xfffb;
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

				MDR_PORTD->RXTX &=0xfffb;
				CtUsart[0]=8;//300;//32;
				CtTransmitRS1=CtReceiveRS0;
				CtError[2]=CtError0[2];
				CtError[9]=CtError0[9];
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

					for(R2=0;R2<=80;++R2)
					RomReceiveRS1[R2][RamReceiveRS1[78]]=RamReceiveRS1[R2];


					if(RamReceiveRS1[78]==1)
					{
						CtUsart[0]=4;
						CtError[1]=CtError0[1];
						CtError[11]=CtError0[11];
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
	unsigned int R0,R1;
	
	MDR_TIMER2 -> CNT = 0x0000;
	MDR_TIMER2 -> STATUS &= ~( 1 << 1 );
	NVIC_ClearPendingIRQ( Timer2_IRQn );
	
	if(CtKn)
	--CtKn;

		for(R1=0;R1<=95;++R1)
		{
		if(RegimIndAvaria==R1)
			{
			if(CtIndAvaria[R1])
				--CtIndAvaria[R1];
			}
		}
		for(R0=1;R0<=31;++R0)//prov
		{
		if(CtError[R0])
			--CtError[R0];
		}


		if(!(MDR_PORTD->RXTX & 4))//Transmit =off
			CtError[10]=CtError0[10];

		if(Error & 0xe00)
		{
			//MDR_PORTD->RXTX &=0xfffb;
			//InitUsart2();

			CtReceiveRS1=CtReceiveRS0;
			CtTransmitRS1=CtReceiveRS0;
			CtUsart[0]=8;
			Error &=0xfffff1ff;

			CtError[9]=CtError0[9];
			CtError[10]=CtError0[10];
			CtError[11]=CtError0[11];
		}


		if(CtUsart[0])
		--CtUsart[0];
		else
		{
			if(MDR_PORTD->RXTX & 0x1)
				TransmitUsart();
			CtUsart[0]=8;//64;//32;
		}


	portYIELD_FROM_ISR( NULL );
}
void ControlRegimWork(void)
{
	static unsigned int R0;
	R0=RegimWork;
	if(R0 &1)//DG1=0n
	{
		if(RegSWork & 0x2000)
		{

			R0 &=0xfffe;
			RegSWork &=0xffffdfff;
		}
	}
	else//DG1=0ff
	{
		if(RegSWork & 0x2000)
		{

			R0 |=0x1;
			RegSWork &=0xffffdfff;
		}
	}

	if(R0 & 2)//DG2=0n
	{
		if(RegSWork & 0x40000)
		{

			R0 &=0xfffd;
			RegSWork &=0xfffbffff;
		}
	}
	else//DG2=0ff
	{
		if(RegSWork & 0x40000)
		{

			R0 |=0x2;
			RegSWork &=0xfffbffff;
		}
	}
	if(R0 & 4)//Set=0n
	{
		if(RegSWork & 0x40)
		{

			R0 &=0xfffb;
			RegSWork &=0xffffffbf;
		}
	}
	else//Set=0ff
	{
		if(RegSWork & 0x40)
		{

			R0 |=0x4;
			RegSWork &=0xffffffbf;
		}
	}
	if(R0 & 8)//Rezerv=0n
	{
		if(RegSWork & 0x80)
		{

			R0 &=0xfff7;
			RegSWork &=0xffffff7f;
		}
	}
	else//Rezerv=0ff
	{
		if(RegSWork & 0x80)
		{

			R0 |=0x8;
			RegSWork &=0xffffff7f;
		}
	}
	if(R0 & 0x10)//SES=0n
	{
		if(RegSWork & 0x200)
		{

			R0 &=0xffef;
			RegSWork &=0xfffffdff;
		}
	}
	else//Rezerv=0ff
	{
		if(RegSWork & 0x200)
		{

			R0 |=0x10;
			RegSWork &=0xfffffdff;
		}
	}
	if(R0 & 0x20)//Ethernet=0n
	{
		if(RegSWork & 0x1000)
		{

			R0 &=0xffdf;
			RegSWork &=0xffffefff;
		}
	}
	else//Ethernet=0ff
	{
		if(RegSWork & 0x1000)
		{

			R0 |=0x20;
			RegSWork &=0xffffefff;
		}
	}
	if(R0 & 0x40)//FVU=0n
	{
		if(RegSWork & 0x4)
		{

			R0 &=0xffbf;
			RegSWork &=0xfffffffb;
		}
	}
	else//FVU=0ff
	{
		if(RegSWork & 0x4)
		{

			R0 |=0x40;
			RegSWork &=0xfffffffb;
		}
	}

	if(R0 & 0x80)//RIzol=0n
	{
		if(RegSWork & 0x800)
		{

			R0 &=0xff7f;
			RegSWork &=0xfffff7ff;
		}
	}
	else//RIzol=0ff
	{
		if(RegSWork & 0x800)
		{

			R0 |=0x80;
			RegSWork &=0xfffff7ff;
		}
	}


	if(RegSWork & 0x8)//Reset ZS
	R0 |=0x100;
	else
	R0 &=0xfeff;


	RegimWork=R0;
}
void    SetError(void)
{
	unsigned char R0;
	for(R0=0;R0<=31;++R0)
	{
		if(!(CtError[R0]))
		Error |=(1<<R0);
	}
	for(R0=0;R0<=15;++R0)
	{
		if(!(CtError1[R0]))
		Error1 |=(1<<R0);
	}
	if(!(CtError2[0]))
	Error2 |=1;
	if(!(CtError2[1]))
	Error2 |=2;
}
void AccountUGen1(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[64][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[65][1];
	UGen1=R0;
}
void AccountFGen1(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[62][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[63][1];
	FGen1=R0;
}
void AccountIGen1(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[60][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[61][1];
	IGen1=R0;
}
void AccountPowerGen1(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[58][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[59][1];
	PowerGen1=R0;
}



void AccountUSet(void)
{
	unsigned int R0;
	R0 = RomReceiveRS1[76][1];
	R0 <<= 8;
	R0 |= RomReceiveRS1[77][1];
	USet = R0;
}
void AccountFSet(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[74][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[75][1];
	FSet=R0;
}
void AccountFLine(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[72][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[73][1];
	FLine=R0;
}


unsigned int AccountNDiz1(void)
{
	unsigned int R0;
	/*if(Error & R1)
R0=0;
else */
	{
		R0=RomReceiveRS1[3][1];
		R0 <<=8;
		R0 |=RomReceiveRS1[2][1];
	}
	return R0;
}
unsigned int AccountNDiz2(void)
{
	unsigned int R0;
	/*if(Error & R1)
R0=0;
else */
	{
		R0=RomReceiveRS1[5][1];
		R0 <<=8;
		R0 |=RomReceiveRS1[4][1];
	}
	return R0;
}

unsigned int AccountPM(unsigned char R1)
{
	unsigned int R0;
	if(Error & R1)
	R0=0;
	else
	R0=RomReceiveRS1[29][R1];
	return R0;
}
unsigned int AccountTOG(unsigned char R1)
{
	unsigned int R0;
	if(Error & R1)
	R0=0;
	else
	R0=RomReceiveRS1[28][R1];
	return R0;
}
unsigned int AccountTGG(unsigned char R1)
{
	unsigned int R0;
	if(Error & R1)
	R0=0;
	else
	R0=RomReceiveRS1[27][R1];
	return R0;
}
unsigned int AccountUT(unsigned char R1)
{
	unsigned int R0;
	if(Error & R1)
	R0=0;
	else
	R0=RomReceiveRS1[26][R1];
	return R0;
}

void AccountULine(void)
{
	unsigned int R0;

	R0=RomReceiveRS1[70][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[71][1];

	ULine=R0;
}
void AccountISet(void)
{
	unsigned int R0;

	R0=RomReceiveRS1[68][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[69][1];

	ISet=R0;
}

void AccountPowerSet(void)
{
	unsigned int R0;

	R0=RomReceiveRS1[66][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[67][1];

	PowerSet=R0;
}




void AccountRIzol(void)
{
	unsigned int R0;

	if(!(Error & 0x2))//Link DG=well
	{
		R0=RomReceiveRS1[11][1];
		R0 <<=8;
		R0 |=RomReceiveRS1[12][1];

	}
	else
	R0=999;

	RIzol=R0;
}


void vSmirnov_Task( void *pvParameters )
{
	uint8_t cnt_b=0,strl[10];
	bool ctrl_inet=false;
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
	load_led_reg();
	vTaskDelay( 1000 );
	
//InitTimer3();
//while(1)
//{vTaskDelay( 10 );}
	while(1)
	{
		//IWDG->KR = 0xaaaa;
		//Avaria = 0x41;
		if(PKDU_Status.ERROR)//если произошла ошибка ВГ1 не отвечает и тп
		{
			clear_lcd();
			Show_led_str(error,0,7);//1 строчка ОШИБКА
			
			if(PKDU_Status.ERROR<8)
				index_char[11] = (PKDU_Status.ERROR+16)*5; //номер ошибки
			else
				index_char[11] = 6*5;
			
			load_led_reg();
			vTaskDelay( 1000 );
		}
		//отправляем в инет то же что и приняли
		PKDU_To_Eth.DG_Cmd = (uint16_t)UGen1;
		//PKDU_To_Eth.DG_res1 = (uint16_t)UGen2;
		//PKDU_To_Eth.DM = PM2;
		PKDU_To_Eth.GOM_Cmd = IGen1;
		PKDU_To_Eth.Indikator = FGen1;
		//PKDU_To_Eth.I_ab = UT2;
		PKDU_To_Eth.I_L = NDiz2;
		PKDU_To_Eth.Ris_DG = TOG1;
		PKDU_To_Eth.Ris_GOM = TOG2;
		//PKDU_To_Eth.SET_GOM_cmd = IGen2;
		PKDU_To_Eth.TOG = PM1;
		//PKDU_To_Eth.UL = FGen2;
		PKDU_To_Eth.U_ab = UT1;
		PKDU_To_Eth.U_c =  NDiz1;
		PKDU_To_Eth.U_dg = PowerGen1;
	//	PKDU_To_Eth.U_gom = PowerGen2;
		
		if(RegS&0x1000)
		{
			ctrl_inet = !ctrl_inet;
			if(ctrl_inet)
			{
				Show_led_str(UPR,0,5);
				Show_led_str(INTERN,1,8);	
				load_led_reg();
				LED_HL8_ON;
			}
			else
				LED_HL8_OFF;
			while(RegS&0x1000)
				vTaskDelay( 10 );
		}
			
//		RegS = PKDU_Status.Ctrl_BTN;
//		if(RegS_old != RegS)//кнопки на дисплее
//		{
//			RegS_old = RegS;
//			
//			sprintf((char *)strl,"%08X",RegS);

//			for(cnt_b=0;cnt_b<8;cnt_b++)
//			{
//				if(strl[cnt_b]<'9')
//					index_char[cnt_b] = (strl[cnt_b]-'0'+16)*5;
//				else
//					index_char[cnt_b] = 6*5;
//			}
//			load_led_reg();
//		}
	

//		
//			ReadKn();
			ControlRegimWork();
			ControlRegimInd();
			/*if((!RegimIndMain)&&(Avaria))
IndicatorAvaria();
else if((!RegimIndMain)&&(AvariaHigh))
IndicatorAvaria();
else if((!RegimIndMain)&&(AvariaHighHigh))
IndicatorAvaria();
else */
			{
				ControlParamInd();
				if(!ctrl_inet)
				IndicatorParam();
				else
				{
					RegimWork = PKDU_From_Eth.DG_Cmd; //16 bit
					sprintf((char *)strl,"%08X",RegimWork);

					for(cnt_b=0;cnt_b<8;cnt_b++)
					{
						if(strl[cnt_b]<'9')
							index_char[cnt_b] = (strl[cnt_b]-'0'+16)*5;
						else
							index_char[cnt_b] = 6*5;
					}
					load_led_reg();
				}
			}
			NDiz1=AccountNDiz1();
			NDiz2=AccountNDiz2();
			PM1=AccountPM(2);
			TOG1=AccountTOG(2);
			TGG1=AccountTGG(2);
			UT1=AccountUT(2);
			AccountUSet();
			AccountFSet();
			AccountULine();
			AccountFLine();
			AccountISet();

			AccountPowerSet();//
			AccountULine();

			AccountRIzol();

			AccountUGen1();
			AccountFGen1();
			AccountIGen1();
			AccountPowerGen1();


			SetError();
		
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



