
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




packet_To_eth PKDU_To_Eth = {0x101,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
packet_From_eth PKDU_From_Eth = {0,0,0,0,0,0,0,0,0};

stat PKDU_Status = {0};


uint16_t RomReceiveEthernet[10];


const unsigned  char TabDecSeg[10] = {0Xfc,0X60,0Xda,0Xf2,0X66,
                                      0Xb6,0Xbe,0Xe0,0Xfe,0Xf6};

uint8_t RegTransmitRS1[41],
		RamReceiveRS1[41],
		RomReceiveRS1[41][6],																						 
		ParamInd = 0;

uint32_t 	RegS = 0, 
			TimeWork = 0,
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
			CtIndAvaria[32],
			AvariaInd=0,
			TestMDR[5],
			RIzol=0,
			UBat=0,
			IBat=0,
			FLine=0,
			ILine=0,
			ULine=0,
			ISet=0,
			Power=0,
			FSet=0,
			USet=0,
			RegimIndAvaria=0,
			Error1=0,
			CtIndAvaria0=60,
			CtError1[16],
			Error2=0,
			CtError2[16],
			RegimError2=0,
			RegimError1=0,
			ErrorPKDU2=0,
			Minus=0;

const unsigned int CtReceiveRS0=40,
					CtError0[16]={100,100,100,100,100,100,30,30,30,30,30,10,120,300,120,120},
					CtError1_0[16]={500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500},
					CtError2_0[16]={200,1000,500,500,500,500,500,500,500,500,500,500,500,500,500,500},
					NumberBlock1=2;

unsigned int	TestError[5];



bool RX_RS_OK = false;


//void UART2_send_byte(uint8_t);
void LoadLED_Reg(uint8_t ,uint8_t);
void LoadRegTransmit(void);

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


void Indicator1(unsigned int R0)
 {
	 static unsigned int R1;
	 static unsigned int R2;
	 static unsigned int R3;
	// static unsigned int R4;

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
	if((!ParamInd) && (Minus))
		Segment[7]=0x2;
	else
	 Segment[7] = TabDecSeg[R3];
	 Segment[6] = TabDecSeg[R2];
	 Segment[5] = TabDecSeg[R1];
 }
 //=======================================================================================
void Indicator2(unsigned int R0)
 {
	 static unsigned int R1;
	 static unsigned int R2;
	 static unsigned int R3;
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
 
 void Indicator33(unsigned int R0)
{
	static unsigned int R1;
	static unsigned int R2;
	static unsigned int R3;
	static unsigned int R4;

	R1=R0;
	if(R1>99)
	R1=99;
	R3=0;
	while(R1>=10)
	{
		R1 -=10;
		++R3;
	}
	R2=0;

	Segment[3]=TabDecSeg[R3];
	Segment[2]=TabDecSeg[R1];
}


//=======================================================================================
void ControlRegimInd(void)
{
	static unsigned int AvariaOld = 0;
	
	if((!Avaria)&&(!RegimIndMain))
		RegimIndMain = 1;
	
	if(RegimIndMain > 1)
		RegimIndMain = 1;
	
	if(Avaria != AvariaOld)
	{
		RegimIndMain = 0;
		AvariaOld = Avaria;
	}
	else 
	if((RegS & 0x40)&&(!(RegSOld & 0x40)))
	{
		if(!RegimIndMain)
			RegimIndMain = 1;
		else
			RegimIndMain = 0;

		RegSOld |= 0x40;
	}
	
	if(!(RegS & 0x40))
		RegSOld &= 0xffbf;
}
//=======================================================================================
void IndicatorParam(void)
{
	static unsigned int R0;
	static unsigned int R1;

	switch(ParamInd)
	{
	case 0:R0=RomReceiveRS1[14][1];//TOG Power;//Avaria & 0xff;//USet;//AvariaInd;//CtError[6];//CtUsart[0];
		if(R0>=50)
		{
			R0 -=50;
			Minus=0;
		}
		else
		{
			R0=50-R0;
			Minus=1;
		}
		
		
		R1=RomReceiveRS1[13][1];//PM;//Avaria>>8;//ULineSet;//RegimIndAvaria;//RomReceiveRS1[37][1];//RegS>>8;//CtError[7];//TestUsart[5];
		Segment[0] &=0x3f;
		Segment[1] &=0xbf;
		Segment[1] |=0x80;

		break;
	case 1:R0=ILine;//Avaria>>16;//ISet;//CtError[8];//R0=TestUsart[0];
		R1=RIzol;//RomReceiveRS1[12][1];//RIzol FLine;//Avaria>>24;//PowerSet;//CtError[9];//PeriodUI[1] & 0xff;
		Segment[0] &=0xbf;
		Segment[1] &=0x3f;
		Segment[0] |=0x80;
		
			
		break;
	case 2:R0=FLine;//CtError[10];//PeriodUI[2]>>8;
		R1=ULine;//CtError[11];//RomReceiveRS1[8][1];

		Segment[0] &=0x3f;
		Segment[1] &=0x7f;
		Segment[1] |=0x40;

		break;

	case 3:R0=IBat;//RomReceiveRS1[35][5];// IPZA AdResult[3]>>1;
		R1=UBat;//AdResult[4]>>2;


		Segment[0] &=0x7f;
		Segment[1] &=0x3f;
		Segment[0] |=0x40;
		
		
		break;

//	case 4:R0=AdResult[6]>>1;
//		R1=AdResult[7]>>1;
//		Segment[0]=0x2;

//		break;

//	case 5:R0=CtUEndL;
//		R1=TestAd[2]>>2;
//		Segment[0]=0x1;

//		break;
//	case 6:R0=TestAd[1]>>2;
//		R1=TestAd[0];
//		Segment[0]=0;
//		Segment[1]=0x20;

//		break;
	case 7:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[1]=0x10;
		break;
	case 8:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[1]=0x8;
		break;
	case 9:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[1]=0x4;
		break;
	case 10:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[1]=0x2;
		break;
	case 11:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[1]=0x1;
		break;
	case 12:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[1]=0x80;
		break;
	case 13:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[1]=0x40;
		break;
	case 14:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[0]=0x80;
		Segment[1]=0;
		break;
	case 15:R0=RegS & 0xff;
		R1=RegS>>8;
		Segment[0]=0x40;
		break;


	default:break;
	}
	Indicator1(R0);
	Indicator2(R1);


	// Segment[0] = (uint8_t)(RegS>>8);
	// Segment[1] = (uint8_t)RegS;
	// Indicator1((uint8_t)PKDU_From_Eth.DG_Cmd);
// //	Indicator1(RomReceiveRS1[37][3]);//(TestMDR[0]);//RomReceiveRS1[37][1]);//MDR_TIMER1->CNT>>8);//(uint8_t)PKDU_Status.DG_Cmd);
	// Indicator2(RomReceiveRS1[37][1]);//TestMDR[1]);//CtError[1]);//(uint8_t)PKDU_Status.DG_Fn);	
// //	Indicator2((uint8_t)PKDU_Status.DG_Fn);
// PKDU_To_Eth.TOG = RomReceiveRS1[37][1];
// PKDU_To_Eth.DM = RomReceiveRS1[37][3];
// PKDU_To_Eth.I_L = RomReceiveRS1[37][4];

}
//=======================================================================================
void ControlParamInd(void)
{
if(RegimIndMain==1)
	{
		if((RegS & 0x80)&&(!(RegSOld & 0x80)))

		{
			if(ParamInd<3)
			++ParamInd;
			else
			ParamInd=0;
			RegSOld |=0x80;
		}
	}
	
	// if(!(RegS & 0x100))
	// Segment[0] |=1;//Ethernet
	// else
	// Segment[0] &=0xfe;//
	
	if(!(RegS & 0x80))
	RegSOld &=0xff7f;
}

//=======================================================================================
void IndParKDG(unsigned char CtAnod)
{
	unsigned int R0;
	//unsigned int R1;
	unsigned int R2, RegSSP = 0;
	
	R0 = 2;
	while(R0)
		--R0;
	
	R2 = CtAnod>>1;
	R2 &= 0x7;
	RegSSP = Segment[R2];
	RegSSP = RegSSP<<8;
	RegSSP &= 0xff00;

	switch(R2)
	{

		case 0:RegSSP |= 0xfe;

		break;
		case 1:RegSSP |= 0xfd;

		break;
		case 2:RegSSP |= 0xfb;
		break;
		case 3:RegSSP |= 0xf7;
		break;
		case 4:RegSSP |= 0xef;
		break;
		case 5:RegSSP |= 0xdf;
		break;
		case 6:RegSSP |= 0xbf;
		break;
		case 7:RegSSP |= 0x7f;
		break;

		default: break;
	}
	
	MDR_SSP1->DR = RegSSP;
	//MDR_SSP1->CR1 |= 0x2;//enable work
}
//=======================================================================================
void SetError(void)
{
	unsigned char R0;
	for(R0=0;R0<=11;++R0)
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
//=======================================================================================
	
// void ResetError(void)
// {
// 	unsigned char R0;
// 		
// 	Error=0;
// 	for(R0=0; R0<=15; ++R0)
// 		CtError[R0] = CtError0[R0];
// 		
// 	Error1=0;
// 	for(R0=0; R0<=15; ++R0)
// 		CtError1[R0] = CtError1_0[R0];
// 		
// 	RegimError1 = 0;
// 		
// 	Error2 = 0;
// 	for(R0=0; R0<=15; ++R0)
// 		CtError2[R0] = CtError2_0[R0];
// 		
// 	RegimError2 = 0;
// 	RomReceiveRS1[5][3] = 0;
// 	ErrorPKDU2 = 0;
// }
//=======================================================================================
void LoadRegTransmit(void)
{
	static unsigned int R1;
//	static unsigned int R2;
	
	RegTransmitRS1[40] = 0xff;
	RegTransmitRS1[39] = 0xff;
	
	RegTransmitRS1[38] = NumberBlock1;
	RegTransmitRS1[37] = PKDU_From_Eth.DG_Cmd;
	R1=RegS>>8;
	RegTransmitRS1[26]=R1 & 0xff;
	R1=RegS;
	RegTransmitRS1[27]=R1 & 0xff;
	
	// R1=RegS;
	// RegTransmitRS1[6] = R1 & 0xff;
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
													
					for(R2=0;R2<=40;++R2)
						RomReceiveRS1[R2][RamReceiveRS1[38]]=RamReceiveRS1[R2];
					
					if(RamReceiveRS1[38]==1)
					{
						CtUsart[0]=2;
						CtError[1]=CtError0[1];
						CtError[6]=CtError0[6];
						Error &=0xffbd;
					}
					if(RamReceiveRS1[38]==5)
					{
						CtUsart[0]=4;
						CtError[5]=CtError0[1];
						CtError[10]=CtError0[6];
						Error &=0xfbdf;
					}
					if(RamReceiveRS1[38]==4)
					{
						CtUsart[0]=6;
						CtError[4]=CtError0[1];
						CtError[9]=CtError0[6];
						Error &=0xfdef;
					}

					if(RamReceiveRS1[38]==3)
					{
						CtUsart[0]=8;
						CtError[3]=CtError0[1];
						CtError[8]=CtError0[6];
						Error &=0xfef7;                                                
					}
				}
			}
		}
	}
}









void Timer1_IRQHandler(void)
{

	static uint8_t cnt_anod = 0;
	uint16_t RegSSP = 0;
	
	MDR_TIMER1->CNT = 0x0000;
	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);

	if(cnt_anod<7)
		++cnt_anod;
	 else
		cnt_anod=0;
	
	RegSSP = Segment[cnt_anod]<<8;
	switch(cnt_anod)
	 {
		 case 0:	RegSSP |=0xfe;
			 break;
		 case 1:	RegSSP |=0xfd;
			 break;
		 case 2:	RegSSP |=0xfb;
			 break;
		 case 3:	RegSSP |=0xf7;
			 break;
		 case 4:	RegSSP |=0xef;
			 break;
		 case 5:	RegSSP |=0xdf;
			 break;
		 case 6:	RegSSP |=0xbf;
			 break;
		 case 7:	RegSSP |=0x7f;
			 break;
		 default: break;
	 }
	 
	MDR_PORTD -> RXTX |= 0x8; //EN
	SSP_SendData(MDR_SSP1,RegSSP);
	
	while(SSP_GetFlagStatus(MDR_SSP1,SSP_FLAG_BSY) == SET);
	
	MDR_PORTD->RXTX &= ~0x8;//dis
// 	if(cnt_anod<8)
// 	{
// 		
// 	
// 		SSP_SendData(MDR_SSP1,0xFF01);
// 		++cnt_anod;
// 	}
// 	else
// 	{
// 		cnt_anod = 0;
// 		
// 	}
	 
// 	 cnt_anod = 15;
// 	 while(--cnt_anod)
// 		__nop();
	 
	 
// 	if(!(cnt_anod & 1))
// 	{
// 		MDR_PORTD->RXTX |= 0x8;
// 		//IndParKDG(cnt_anod);
// 		SSP_SendData(MDR_SSP1,0xFF01);
// 		//MDR_SSP1->DR = 0;
// 	}
// 	else
// 	{
// 		//MDR_SSP1->CR1 &= 0xfffd;//denable work
// 		MDR_PORTD->RXTX &= 0xfff7;
// 	}
	portYIELD_FROM_ISR( NULL );
}

void Timer2_IRQHandler(void)
{
	unsigned char R0=0, R1=0;
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
	
	for(R1=0;R1<=31;++R1)
	{
		if(RegimIndAvaria==R1)
		{
			if(CtIndAvaria[R1])
			--CtIndAvaria[R1];
		}
	}
	
	
	if(!(MDR_PORTD->RXTX & 4))//Transmit =off
		CtError[11]=CtError0[11];

	if(Error & 0x800)
		{
		MDR_PORTD->RXTX &=0xfb;
		//InitUsart9600();

		CtReceiveRS1=CtReceiveRS0;
		CtTransmitRS1=CtReceiveRS0;
		CtUsart[0]=0x10;
		Error &=0xf7ff;

		CtError[11]=CtError0[11];

		}
	else if((Error & 0x40)&&(Error & 0x100)&&(Error & 0x200)&&(Error & 0x400))
		{
		//InitUsart9600();

		CtReceiveRS1=CtReceiveRS0;
		CtTransmitRS1=CtReceiveRS0;
		CtUsart[0]=0x10;
		Error &=0xf8bf;
		CtError[6]=CtError0[6];
		CtError[8]=CtError0[6];
		CtError[9]=CtError0[6];
		CtError[10]=CtError0[6];
		}


	for(R0=1;R0<=11;++R0)
		{
		if(CtError[R0])
			--CtError[R0];
		}


	if(CtUsart[0])
		--CtUsart[0];
	else
	{
	// if(PORTD->RXTX & 0x1)
	TransmitUsart();
	CtUsart[0]=10;//64;//32;
	}
	
	if(TestTimer2)
		--TestTimer2;
	else
		TestTimer2=200;	


}

void AccountUSet(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[19][4];
	R0 <<=8;
	R0 |=RomReceiveRS1[20][4];
	USet=R0;
}
void AccountULine(void)
{
	unsigned int R0;
	if(!(Error & 0x10))//Link KRP=well
	{
		R0=RomReceiveRS1[23][4];
		R0 <<=8;
		R0 |=RomReceiveRS1[24][4];

	}
	else if(!(Error & 0x2))//Link DG=well
	{
		R0=RomReceiveRS1[23][1];
		R0 <<=8;
		R0 |=RomReceiveRS1[24][1];

	}
	else if(!(Error & 0x8))//Link Gom=well
	{
		R0=RomReceiveRS1[23][3];
		R0 <<=8;
		R0 |=RomReceiveRS1[24][3];

	}
	else
	R0=0;
	ULine=R0;
}
void AccountFLine(void)
{
	unsigned int R0;
	if(!(Error & 0x10))//Link KRP=well
	{
		R0=RomReceiveRS1[15][4];
		R0 <<=8;
		R0 |=RomReceiveRS1[16][4];

	}
	else if(!(Error & 0x2))//Link DG=well
	{
		R0=RomReceiveRS1[15][1];
		R0 <<=8;
		R0 |=RomReceiveRS1[16][1];

	}
	else if(!(Error & 0x8))//Link Gom=well
	{
		R0=RomReceiveRS1[15][3];
		R0 <<=8;
		R0 |=RomReceiveRS1[16][3];

	}
	else
	R0=0;
	FLine=R0;
}

void AccountILine(void)
{
	unsigned int R0;
	if(RomReceiveRS1[36][4] & 1)//KSet=on
	{
		R0=RomReceiveRS1[22][4];


	}
	else if(RomReceiveRS1[36][1] & 1)//KGen=on
	{
		R0=RomReceiveRS1[22][1];


	}
	else if(RomReceiveRS1[36][3] & 1)//KGom=on

	{
		R0=RomReceiveRS1[22][3];


	}
	else
	R0=0;
	ILine=R0;
}

void AccountPower(void)
{
	unsigned int R0;
	if(RomReceiveRS1[36][4] & 1)//KSet=on
	{
		R0=RomReceiveRS1[21][4];


	}
	else if(RomReceiveRS1[36][1] & 1)//KGen=on
	{
		R0=RomReceiveRS1[21][1];


	}
	if(RomReceiveRS1[36][3] & 1)//KGom=on

	{
		R0=RomReceiveRS1[21][3];


	}
	Power=R0;
}

void AccountUBat(void)
{
	unsigned int R0;
	if(!(Error & 0x10))//Link KRP=well
	{
		R0=RomReceiveRS1[13][4];
		R0 <<=8;
		R0 |=RomReceiveRS1[14][4];

	}
	else
	R0=0;

	UBat=R0;
}

void AccountIBat(void)
{
	unsigned int R0;
	if(!(Error & 0x20))//Link PZA=well
	{
		R0=RomReceiveRS1[35][5];
	}
	else
	R0=0;

	IBat=R0;
}

void AccountRIzol(void)
{
	unsigned int R0;
	unsigned int R1;
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

void AccountFSet(void)
{
	unsigned int R0;
	R0=RomReceiveRS1[17][4];
	R0 <<=8;
	R0 |=RomReceiveRS1[18][4];
	FSet=R0;
}

void ResetError(void)
{
	unsigned char R0;
	Error=0;
	for(R0=0;R0<=15;++R0)
	CtError[R0]=CtError0[R0];
	Error1=0;
	for(R0=0;R0<=15;++R0)
	CtError1[R0]=CtError1_0[R0];
	RegimError1=0;
	Error2=0;
	for(R0=0;R0<=15;++R0)
	CtError2[R0]=CtError2_0[R0];
	RegimError2=0;
	RomReceiveRS1[5][3]=0;
	ErrorPKDU2=0;

}

void    IndicatorAvaria(void)
{
	unsigned char R0;
	unsigned int R1;
	unsigned int R2;
	if(!AvariaInd)
	AvariaInd=Avaria;

	for(R0=0;R0<=31;++R0)
	{
		if(AvariaInd & (1<<R0))
		{
			RegimIndAvaria=R0;
			if(CtIndAvaria[R0])
			break;
			else

			{
				AvariaInd &=~(1<<R0);
				CtIndAvaria[R0]=CtIndAvaria0;
			}
		}

	}
	for(R0=0;R0<=31;++R0)
	{
		if(!(AvariaInd & (1<<R0)))
		CtIndAvaria[R0]=CtIndAvaria0;
	}

	switch(RegimIndAvaria)
	{
	case 0:R0=1;
		Segment[4]=0xee;
		R1=1;
		break;
	case 1:R0=2;
		Segment[4]=0xee;
		R1=1;
		break;
	case 2:R0=1;
		Segment[4]=0xee;
		R1=2;
		break;
	case 3:R0=1;
		Segment[4]=0xee;
		R1=3;
		break;
	case 4:R0=1;
		Segment[4]=0xee;
		R1=4;
		break;
	case 5:R0=1;
		Segment[4]=0xee;
		R1=5;
		break;
	case 6:R0=1;
		Segment[4]=0xee;
		R1=6;
		break;
	case 7:R0=2;
		Segment[4]=0xee;
		R1=6;
		break;
	case 8:R0=1;
		Segment[4]=0xee;
		R1=7;
		break;
	case 9:R0=2;
		Segment[4]=0xee;
		R1=7;
		break;
	case 10:R0=1;
		Segment[4]=0xee;
		R1=8;
		break;
	case 11:R0=2;
		Segment[4]=0xee;
		R1=8;
		break;
	case 12:R0=1;
		Segment[4]=0x6e;
		R1=1;
		break;
	case 13:R0=2;
		Segment[4]=0x6e;
		R1=1;
		break;
	case 14:R0=3;
		Segment[4]=0x6e;
		R1=1;
		break;
	case 15:R0=1;
		Segment[4]=0x6e;
		R1=2;
		break;
	case 16:R0=2;
		Segment[4]=0x6e;
		R1=2;
		break;
	case 17:R0=3;
		Segment[4]=0x6e;
		R1=2;
		break;
	case 18:R0=1;
		Segment[4]=0x6e;
		R1=3;
		break;
	case 19:R0=2;
		Segment[4]=0x6e;
		R1=3;
		break;
	case 20:R0=1;
		Segment[4]=0x6e;
		R1=4;
		break;
	case 21:R0=2;
		Segment[4]=0x6e;
		R1=4;
		break;
	case 22:R0=3;
		Segment[4]=0x6e;
		R1=4;
		break;
	case 23:R0=1;
		Segment[4]=0x6e;
		R1=5;
		break;
	case 24:R0=1;
		Segment[4]=0x6e;
		R1=6;
		break;
	case 25:R0=1;
		Segment[4]=0x6e;
		R1=7;
		break;
	case 26:R0=1;
		Segment[4]=0x6e;
		R1=8;
		break;
	case 27:R0=1;
		Segment[4]=0x6e;
		R1=9;
		break;
	case 28:R0=3;
		Segment[4]=0xee;
		R1=10;
		break;
	case 29:R0=3;
		Segment[4]=0x6e;
		R1=10;
		break;
	case 30:R0=2;
		Segment[4]=0x6e;
		R1=11;
		break;

	default:break;
	}
	if(R0==1)
	{
		Segment[7]=0x8c;
		Segment[6]=0x60;
		Segment[5]=0;
	}
	else  if(R0==2)
	{
		Segment[7]=0x8c;
		Segment[6]=0xda;
		Segment[5]=0;
	}
	if(R0==3)
	{
		Segment[7]=0x9c;
		Segment[6]=0;
		Segment[5]=0;
	}

	//Indicator1(R0);
	Indicator33(R1);

}

void    ControlLed(void)
{
	uint8_t R0=0;
	
	if(RegS & 0x100)
	{
		Segment[0] |=1;//Ethernet
		PKDU_To_Eth.Indikator |= 1<<7;
		if((PKDU_From_Eth.Type&0x03)==1)
		{
			for(R0=0;R0<16;R0++)
			{
				if((PKDU_From_Eth.DG_Cmd>>R0)&1)
				{
					switch(R0)
						{
							case 15://AO
								break;
							case 14://Stop
								RegS |= 2;
								break;
							case 13://Pusk
								RegS |= 1;
								break;
							case 12://gash pole DG
								break;
							case 11://gash pole GOM
								RegS |= 1<<4;
								break;
							case 10: //rezerv
								RegS |= 1<<10;
								break;
							case 7://EPG
								RegS |= 1<<5;
								break;
							case 6://PJD
								RegS |= 1<<2;
								break;
							case 3://reset alarm
								RegS |= 1<<3;
								break;
							default: break;
						}
				}
				else
				{
					switch(R0)
						{
							case 15://AO
								break;
							case 14://Stop
								RegS &= ~2;
								break;
							case 13://Pusk
								RegS &= ~1;
								break;
							case 12://gash pole DG
								break;
							case 11://gash pole GOM
								RegS &= ~(1<<4);
								break;
							case 10: //rezerv
								RegS &= ~(1<<10);
								break;
							case 7://EPG
								RegS &= ~(1<<5);
								break;
							case 6://PJD
								RegS &= ~(1<<2);
								break;
							case 3://reset alarm
								RegS &= ~(1<<3);
							default: break;
						}
				}
			}
			if(PKDU_From_Eth.GOM_Cmd & 0xA000)
				RegS |= 0x200;
			else
				RegS &= ~0x200;
			
			if(PKDU_From_Eth.GOM_Cmd & 0x4000)
				RegS |= 0x08;
			else
				RegS &= ~0x08;
			
//			for(R0=0;R0<16;R0++)
//			{
//			if((PKDU_From_Eth.GOM_Cmd>>R0)&1)
//			{
//				if()
//				
//				switch(R0)
//				{
//					case 15://DG na lin
//						RegS |= 0x200;
//						break;
//					case 14://GOM na lin
//						RegS |= 0x08;
//						break;
//					case 13://Set na lin
//						RegS |= 0x200;
//						break;
//					default: break;
//				}
//			}
//				else
//				{
//					switch(R0)
//					{
//						case 15://DG na lin
//							RegS &= ~0x200;
//							break;
//						case 14://GOM na lin
//							RegS &= ~0x08;
//							break;
//						case 13://Set na lin
//							RegS &= ~0x200;
//							break;
//						default: break;
//					}
//				}
//		}
		PKDU_To_Eth.Indikator &= ~(3<<4);
		PKDU_To_Eth.Indikator |= ((PKDU_From_Eth.DG_Cmd>>8)&3)<<4;
		}
	}
	else
	{
	Segment[0] &=0xfe;//
	PKDU_To_Eth.Indikator &= ~(1<<7);
	}
	
	Segment[1] &=0xfe;//BP
	if(RomReceiveRS1[26][4] & 4)//RBP
	Segment[0] |=0x2;//RBP
	else
	Segment[0] &=0xfd;//RBP
	if(Error & 0x20)
	Segment[1] |=2;
	else
	Segment[1] &=0xfd;//BZA
	if(RomReceiveRS1[29][1] & 0x4)//RIzol DG
	Segment[0] |=0x4;//RIzol
	else if(RomReceiveRS1[13][3] & 0x4)//RIzol Gom
	Segment[0] |=0x4;//RIzol
	else
	Segment[0] &=0xfb;//RIzol
	if(RomReceiveRS1[36][3] & 1)
	{
	Segment[1] |= 0x4;//GOM
	PKDU_To_Eth.Indikator |= 1<<14;
	}
	else
	{
	Segment[1] &= 0xfb;
		PKDU_To_Eth.Indikator &= ~(1<<14);
	}
	if(RegS & 0x400)
	{
	Segment[0] |=8;//Rezesrv
	PKDU_To_Eth.Indikator |= 1<<6;
	}
	else
	{
	Segment[0] &=0xf7;//
		PKDU_To_Eth.Indikator &= ~(1<<6);
	}
	if(RegS & 0x20)
	{
	Segment[1] |=0x8;//EPG
		PKDU_To_Eth.Indikator |= 1<<12;
	}
	else
	{
	Segment[1] &=0xf7;//EPG
		PKDU_To_Eth.Indikator &= ~(1<<12);
	}
	if(RomReceiveRS1[36][4] & 1)
	{
	Segment[0] |= 0x10;//work from set
		PKDU_To_Eth.Indikator |= 1<<13;
	}
	else
	{
	Segment[0] &= 0xef;
		PKDU_To_Eth.Indikator &= ~(1<<13);
	}
	if(RegS & 0x4)
	{
	Segment[1] |=0x10;//PGD
		PKDU_To_Eth.Indikator |= 1<<11;
	}
	else
	{
	Segment[1] &=0xef;//PGD
		PKDU_To_Eth.Indikator &= ~(1<<11);
	}
	if((USet>=320)&&(USet<=440))
	{
	Segment[0] |=0x20;//Set=Norma
	PKDU_To_Eth.Indikator |= 1<<8;
	}
	else
	{
	Segment[0] &=0xdf;
	PKDU_To_Eth.Indikator &= ~(1<<8);
	}
	if(RomReceiveRS1[36][1] & 1)//DG
	{
	Segment[1] |= 0x20;
		PKDU_To_Eth.Indikator |= 1<<15;
	}
	else
	{
	Segment[1] &= 0xdf;
		PKDU_To_Eth.Indikator &= ~(1<<15);
	}

}

void    SetAvaria(void)
{
	unsigned int R0;
	unsigned int R1;
	unsigned int R2;
	unsigned int R3;
	R1=0;
	R0=RomReceiveRS1[30][1];//DG
	R2=RomReceiveRS1[26][4];//KRP
	R3=RomReceiveRS1[14][3];//Gom
	if(R0 & 1)
	{
		R1 |=1;//Raznos DG
		PKDU_To_Eth.DG_Cmd |= 1<<11;
	}
	if(R0 & 2)
	{
	R1 |=4;//PM DG
		PKDU_To_Eth.DG_Cmd |= 1<<6;
	}
	if(R0 & 4)
	{
	R1 |=8;//TOG DG
		PKDU_To_Eth.DG_Cmd |= 1<<5;
	}
	if(R0 & 8)
	{
	R1 |=0x40;//No Vozb DG
		PKDU_To_Eth.DG_Cmd |= 1<<13;
	}
	if(R0 & 0x10)
	{
	R1 |=0x100;//No oborotov DG
		PKDU_To_Eth.DG_Cmd |= 1<<12;
	}
	if(R0 & 0x20)
	{
	R1 |=0x400;//U No Norma DG
		PKDU_To_Eth.DG_Cmd |= 1<<1;
	}
	if(R0 & 0x80)
	{
	R1 |=0x1000;//K1 DG
		PKDU_To_Eth.Indikator |= 1<<15;
	}
	R0=RomReceiveRS1[29][1];//DG
	if(R0 & 1)
	{
	R1 |=0x8000;//QF1 DG
		PKDU_To_Eth.DG_Cmd |= 1<<10;
	}
	if(R0 & 2)
	{
		R1 |=0x40000;//No Sinxr DG
		PKDU_To_Eth.DG_Cmd |= 1<<7;
	}
	if(R0 & 0x4)
	{
	R1 |=0x100000;//RIzol DG
		PKDU_To_Eth.DG_Cmd |= 1<<8;
	}
	if(R0 & 0x10)
	{
	R1 |=0x2000000;//Urov OG
		PKDU_To_Eth.DG_Cmd |= 1<<4;
	}
	if(R0 & 0x20)
	{
	R1 |=0x8000000;//No Topliva DG
		PKDU_To_Eth.DG_Cmd |= 1<<2;
	}
	if(R2 & 0x2)
	{
	R1 |=0x10000000;//Pogar
		PKDU_To_Eth.SET_GOM_cmd |= 1<<7;
	}
	if(R2 & 0x4)
	{
	R1 |=0x400000;//RBP
		PKDU_To_Eth.SET_GOM_cmd |= 1<<8;
	}
	if(R2 & 0x8)
	{
	R1 |=0x20000;//QF1 Set
		PKDU_To_Eth.SET_GOM_cmd |= 1<<10;
	}
	if(R2 & 0x10)
	{
	R1 |=0x4000;//K1 Set
		PKDU_To_Eth.Indikator |= 1<<13;
	}
	if(R2 & 0x20)
	{
	R1 |=0x40000000;//Chered Faz
		
	}
	if(R3 & 0x1)//Raznos Gom
	{
	R1 |=2;
		PKDU_To_Eth.GOM_Cmd |= 1<<11;
	}
	if(R3 & 0x10)//No oborotov Gom
	{
	R1 |=0x200;
		PKDU_To_Eth.GOM_Cmd |= 1<<12;
	}
	if(R3 & 0x80)//K1 Gom
	{
	R1 |=0x2000;
		PKDU_To_Eth.Indikator |= 1<<14;
		
	}
	if(R3 & 0x20)//U No Norma Gom
	{
	R1 |=0x800;
		PKDU_To_Eth.GOM_Cmd |= 1<<13;
	}

	R3=RomReceiveRS1[13][3];//Gom
	if(R3 & 0x4)//RIzol Gom
	{
	R1 |=0x200000;
		PKDU_To_Eth.GOM_Cmd |= 1<<8;
	}
	if(R3 & 0x1)//QF1 Gom
	{
	R1 |=0x10000;
		PKDU_To_Eth.GOM_Cmd |= 1<<10;	
	}

	Avaria=R1;

}





//void main(void)
void vSmirnov_Task( void *pvParameters )
{
	uint8_t cnt = 0;
	uint32_t RMain=0, R0=0;
	//unsigned long RMain;
// 	#ifdef DEBUG
// 		debug();
// 	#endif
	
	
	vTaskDelay( 1000 );
	
	cnt = 0;
	while(cnt<10)
	{
		Segment[cnt] = 0;
		++cnt;
	}
	TestTimer2=98;	
//InitTimer3();
	ResetError();
	
	AvariaInd=0;
	for(R0=0;R0<=31;++R0)
		CtIndAvaria[R0]=CtIndAvaria0;
	
	while(1)
	{
		AccountUSet();
		AccountULine();
		AccountFLine();
		AccountILine();
		AccountPower();//
		AccountUBat();
		AccountIBat();
		AccountRIzol();
		AccountFSet();
		
		SetError();
		SetAvaria();
		
		R0=RomReceiveRS1[14][1];//TOG Power;//Avaria & 0xff;//USet;//AvariaInd;//CtError[6];//CtUsart[0];
		if(R0>=50)
		{
			R0 -=50;
			Minus=0;
		}
		else
		{
			R0=50-R0;
			Minus=1;
		}
		
		
		PKDU_To_Eth.TOG = ((Minus<<15)&0x8000)|R0;
		PKDU_To_Eth.DM = RomReceiveRS1[13][1];;
		PKDU_To_Eth.I_L = ILine;
		PKDU_To_Eth.Ris_DG=RIzol;
		PKDU_To_Eth.Ris_GOM=RIzol;
		
		PKDU_To_Eth.FLine =FLine; 
		PKDU_To_Eth.UL=ULine;
		
		PKDU_To_Eth.I_ab =IBat; 
		PKDU_To_Eth.U_ab=UBat;
		
		
		
		
		
		ControlRegimInd();
		ControlLed();
		
		if((!RegimIndMain)&&(Avaria))
			IndicatorAvaria();
		else
		{
			ControlParamInd();
			IndicatorParam();
		}
		
// 		if((!RegimIndMain) && (Avaria))
// 		//IndicatorAvaria();
// 			{
// 				Indicator1(123);
// 				Indicator2(456);
// 			}
// 		else
// 		{
// 			
// 		}
		
		
		
		if(TimeWork < 9999)
			++TimeWork;
		else
			TimeWork = 0;
		
		vTaskDelay( 10 );
	}
}


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




