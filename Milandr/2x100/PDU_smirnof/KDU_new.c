
//viev memory layout 0x8006010 URef


#include <stdio.h>
#include <1986be91.h>
//#include "MDR32Fx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>


#if defined ( __ICCARM__ )
#define __RAMFUNC      __ramfunc
#else
#define __RAMFUNC
#endif

#ifdef __CMCARM__
#pragma ramfunc write_EEPROM
#pragma ramfunc erase_EEPROM

#endif


unsigned int LM(unsigned int R1);

void init_SSP(void);

__RAMFUNC         void    erase_EEPROM(unsigned int ADR);
__RAMFUNC         void    write_EEPROM(unsigned int ADR,unsigned int DATA);


const unsigned  char Stroka1[50]={0x3e,0x4,0x42,0x41,0xf,0x4f,0x3e,0x41,0x36,0x6,
	0x7e,0x7f,0x7f,0x7f,0x60,0x7f,0x77,0x41,0x36,0x6,
	0x3e,0x7f,0x7f,0x3e,0x7f,0x7f,0x3e,0x1,0x36,0x6,
	0x3e,0x4,0x42,0x22,0xf,0x4f,0x3e,0x41,0x36,0x6,
	0x0,0x0,0x8,0x0,0x0,0x3f,0x7f,0x7f,0x36,0x6};



const unsigned  char Stroka2[50]={0x41,0x2,0x61,0x49,0x8,0x49,0x49,0x21,0x49,0x49,
	0x9,0x49,0x49,0x1,0x30,0x49,0x8,0x41,0x36,0x6,
	0x3e,0x2,0x8,0x41,0x9,0x9,0x41,0x1,0x36,0x6,
	0x3e,0x4,0x42,0x22,0xf,0x4f,0x3e,0x41,0x36,0x6,
	0x0,0x0,0x14,0x41,0x41,0x40,0x9,0x4,0x36,0x6};


const unsigned  char Stroka3[50]={0x41,0x7f,0x51,0x49,0x8,0x49,0x49,0x11,0x49,0x49,
	0x9,0x49,0x49,0x1,0x2f,0x49,0x7f,0x41,0x36,0x6,
	0x3e,0x4,0x8,0x41,0x9,0x9,0x41,0x7f,0x36,0x6,
	0x3e,0x4,0x42,0x22,0xf,0x4f,0x3e,0x41,0x36,0x6,
	0x0,0x50,0x22,0x22,0x7f,0x40,0x9,0x8,0x36,0x6};

const unsigned  char Stroka4[50]={0x41,0x0,0x49,0x49,0x8,0x49,0x49,0x9,0x49,0x49,
	0x9,0x49,0x49,0x1,0x21,0x49,0x8,0x41,0x36,0x6,
	0x3e,0x2,0x8,0x41,0x9,0x9,0x41,0x1,0x36,0x6,
	0x3e,0x30,0x42,0x22,0xf,0x4f,0x3e,0x41,0x36,0x6,
	0x0,0x30,0x41,0x14,0x41,0x40,0x9,0x10,0x36,0x6};

const unsigned  char Stroka5[50]={0x3e,0x0,0x46,0x36,0x7f,0x31,0x31,0x7,0x36,0x36,
	0x7e,0x71,0x3e,0x1,0x7f,0x49,0x77,0x41,0x36,0x6,
	0x3e,0x7f,0x7f,0x3e,0xf,0x6,0x41,0x1,0x36,0x6,
	0x3e,0x4,0x42,0x22,0xf,0x4f,0x3e,0x41,0x36,0x6,
	0x0,0x0,0x0,0x8,0x0,0x3f,0x1,0x7f,0x36,0x6};
unsigned int TestPort;
unsigned char Indicator[16];
unsigned int RegSWork;
unsigned char CtKn;
unsigned int RegSSP;
unsigned int TestSSP[8];
unsigned int CtAnod;
unsigned int TOG1;
unsigned int TGG1;
unsigned int UT1;
unsigned int PM1;
unsigned int TOG2;
unsigned int TGG2;
unsigned int UT2;
unsigned int PM2;
unsigned int UGen1;
unsigned int FGen1;
unsigned int IGen1;
unsigned int PowerGen1;



unsigned char CtTestOut;
int abs(int x);
unsigned int RIzol;
unsigned int Minus;
unsigned int UBat;
unsigned int IBat;
unsigned int FLine;
unsigned int ILine;
unsigned int ULine;
unsigned int ISet;
unsigned int Power;
unsigned int PowerSet;
unsigned int FSet;
unsigned int RomReceiveEthernet[10];
unsigned int TestAd[8];


const unsigned  char TabDecSeg[10]={0Xfc,0X60,0Xda,0Xf2,0X66,
									0Xb6,0Xbe,0Xe0,0Xfe,0Xf6};


unsigned int RegSSP;
unsigned int TestSSP[8];
unsigned int CtAnod;
unsigned int MaskaSetka;
unsigned int CtSetka;
unsigned int CtByteAnod;
unsigned int Segment[10];


unsigned int RegTransmitCAN[16];
unsigned int CtTransmitCAN;
unsigned int CtErrorCAN=1000;
unsigned int CANPauza;
unsigned int TestI2C;
unsigned int CtUEnd;
unsigned int CtUEndL;
unsigned char TestNew[10];
unsigned int Test141223[5];
unsigned int TimeWork;
unsigned int CtTimeKSKG;
unsigned int NNomTemp;
unsigned int Ct_N_U;
unsigned int TB;
unsigned int UOut;
unsigned int ErrorPKDU2=0;
unsigned int U500;
const unsigned int UNom500=490;//520;
unsigned int T[5];
unsigned int RegimTr;
unsigned int TestError[5];
unsigned int CtStarterOn;
unsigned int CtStarterOff;
unsigned int CtStarterN;
unsigned int USet;
unsigned int CtUNew;
unsigned int RegimError1;
unsigned int Error1;
unsigned int CtError1[16];

//1==8mc

const unsigned int CtError1_0[16]={500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500};
unsigned int RegimError2;
unsigned int Error2;
unsigned int CtError2[16];

//1==8mc
const unsigned int CtError2_0[16]={200,1000,500,500,500,500,500,500,500,500,500,500,500,500,500,500};

unsigned int TM;
unsigned int EnableStarter;
unsigned int CtDPNom;
unsigned int DPNom;
unsigned int TestTimer2;
unsigned int PORTFF;
unsigned int CtReikaOn;
unsigned int CtReikaOff;
unsigned int CtReikaOn0;
unsigned int CtReikaOff0;
unsigned int WellTimer3;
unsigned int CtTime[16];
const unsigned long CtTime0[16]={1000,10,200,1000,22500,10,10,10,75000,1000,1000,37500,7500,1000,1015,1016};
unsigned int RegimTime;
unsigned int Timer3OverLow;
unsigned int TestTimer3[6];
unsigned int TestComp[6];
unsigned int RegKn1Old;
unsigned int RegKnOld;

unsigned int RegKn1;
unsigned int RegKn;
unsigned int CtReadKn;
unsigned int TestUsart[10];
unsigned char CtTestCAN[10];
unsigned int CtCAN[10];
unsigned int CtUAB;
unsigned int RRR;
#pragma locate RRR 0x8006000 noinit
unsigned int RRR1;
#pragma locate RRR1 0x8007000 noinit

unsigned    int CtEepromT2;
unsigned    int SysIsTemp;
unsigned    int UNomHand;
unsigned    int NDiz1;
unsigned int NDiz2;
unsigned    int NNom;


unsigned    int  RegimWorkTransmit;
unsigned    int  RegimWork;
unsigned    int  DP;


unsigned int PeriodCalc;
unsigned int TestSicle;
unsigned int BitSinh;
unsigned int BitSinh1;
unsigned int CtUsart[2];
const unsigned int CtReceiveRS0=80;

unsigned int NumberBlock1=2;
unsigned int ControlReceive1;
long TestRS[9];
unsigned int RomReceiveRS1[81][3];
unsigned int RamReceiveRS1[81];
unsigned int RegTransmitRS1[81];
//        unsigned int RegimBlock1;
unsigned int CtReceiveRS1;
unsigned int CtTransmitRS1;
unsigned int ControlTransmit1;

int RP;
int RI;
int RD;


unsigned int OverLowTimer3;
unsigned int Well;
unsigned int t[3];
unsigned long AdSum[8];
//                        unsigned int RBig2;
//                       unsigned int RBig1;



unsigned int CtMaxMin;
unsigned int TMin;
unsigned int TMax;
unsigned int CtSicle;
unsigned int RBig2;
unsigned int RBig1;

//               unsigned int KStatizm; //

unsigned int   Error;


unsigned int   CtError[32];
const unsigned int CtError0[32]={100,100,100,100,100,100,100,100,100,40,40,40,200,200,200,120,
	100,100,100,100,100,100,30,30,30,30,30,10,120,300,120,120};
unsigned int Timer3Old[3];
unsigned int NumberParInd;

unsigned int CtEeprom;

unsigned int CtInitInd;

//       unsigned int IndData[4];


unsigned int RegS=0;
unsigned int RegSOld=0;
unsigned int RegS1;
unsigned int CtStart;

unsigned int CtU;
unsigned int AdResult[8];

unsigned int PeriodTemp[6];
unsigned int Period[16];
unsigned int PeriodUITemp[6];
unsigned int PeriodUI[6];
unsigned int TimerUIOld[6];
unsigned int AD[8][500];
unsigned int RegTim3[2];



unsigned int                 Avaria;
unsigned    int                 AvariaInd;
unsigned    int                 AvariaOld;
unsigned int                 AvariaHigh;
unsigned    int                 AvariaIndHigh;
unsigned    int                 AvariaOldHigh;
unsigned int                 AvariaHighHigh;
unsigned    int                 AvariaIndHighHigh;
unsigned    int                 AvariaOldHighHigh;
unsigned    int                 CtIndAvaria[96];
unsigned    int                 CtIndRegim[16];
unsigned    int                 ParamInd;
unsigned    int                 RegimIndMainOld;
unsigned    int                 RegimIndMain;
unsigned    int                 RegimIndParametr;
unsigned    int                 RegimIndRegim;
unsigned    char                 RegimIndAvaria;
const unsigned    int           CtIndAvaria0=60;
const unsigned    int           CtIndRegim0=200;

void IndicatorBukva(unsigned int R0,unsigned char R5)
{

	Indicator[R5]=R0;

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
	Indicator[R5+3]=R4;
	Indicator[R5+2]=R3;
	Indicator[R5+1]=R2;
	Indicator[R5]=R1;
}
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

	Indicator[R5+2]=R3;
	Indicator[R5+1]=R2;
	Indicator[R5]=R1;
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

	Indicator[R5+3]=R3;
	Indicator[R5+2]=R2;
	IndicatorBukva(41,R5+1);
	Indicator[R5]=R1;
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


	Indicator[R5+2]=R2;
	IndicatorBukva(41,R5+1);
	Indicator[R5]=R1;
}





void InitUsart2(void)
{
	UART2->CR =0;
	RST_CLK->PER_CLOCK |=0x80;
	RST_CLK->UART_CLOCK |=0x2000000;

	UART2->IFLS=0;
	UART2->IBRD=104;
	UART2->FBRD=11;
	UART2->LCR_H=0x60;
	UART2->CR=0x201;//301;//301;
	UART2->IMSC =0x10;//0x390; //enable Receive Int
	SYS->ISER00 |=0x80;
	UART2->LCR_H |=0x6;//control chetn
}

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



void InitSsp(void)
{
	RST_CLK->PER_CLOCK |=0x100; //SSP1
	RST_CLK->SSP_CLOCK |=0x1000003;

	SPI1->SSPx_CR0=0x86;//0x86;//f;
	SPI1->SSPx_CR1=0x2;//enable work
	SPI1->SSPx_CPSR=10;//clock

}


void   ReadKn(void)
{
	unsigned char R0;
	RegSOld=RegS;
	RegKnOld=RegKn;


	if(!(PORTC->RXTX & 0x10))
	RegKn |=1;
	else
	RegKn &=0xffffe;
	if(!(PORTC->RXTX & 0x20))
	RegKn |=2;
	else
	RegKn &=0xffffd;
	if(!(PORTD->RXTX & 0x8))
	RegKn |=4;
	else
	RegKn &=0xffffb;
	if(!(PORTD->RXTX & 0x20))
	RegKn |=0x8;
	else
	RegKn &=0xffff7;

	if(!(PORTC->RXTX & 0x100))
	RegKn |=0x10;
	else
	RegKn &=0xfffef;
	if(!(PORTC->RXTX & 0x200))
	RegKn |=0x20;
	else
	RegKn &=0xfffdf;
	if(!(PORTC->RXTX & 0x400))
	RegKn |=0x40;
	else
	RegKn &=0xffbf;
	if(!(PORTC->RXTX & 0x800))
	RegKn |=0x80;
	else
	RegKn &=0xfff7f;

	if(!(PORTC->RXTX & 0x8))
	RegKn |=0x100;
	else
	RegKn &=0xffeff;

	if(!(PORTC->RXTX & 0x1000))
	RegKn |=0x200;
	else
	RegKn &=0xfdff;
	if(!(PORTC->RXTX & 0x2000))
	RegKn |=0x400;
	else
	RegKn &=0xffbff;
	if(!(PORTC->RXTX & 0x4000))
	RegKn |=0x800;
	else
	RegKn &=0xff7ff;
	if(!(PORTC->RXTX & 0x8000))
	RegKn |=0x1000;
	else
	RegKn &=0xfefff;
	if(!(PORTD->RXTX & 0x40))
	RegKn |=0x2000;
	else
	RegKn &=0xfdfff;
	if(!(PORTD->RXTX & 0x80))
	RegKn |=0x4000;
	else
	RegKn &=0xfbfff;
	if(!(PORTD->RXTX & 0x100))
	RegKn |=0x8000;
	else
	RegKn &=0xf7fff;
	if(!(PORTD->RXTX & 0x200))
	RegKn |=0x10000;
	else
	RegKn &=0xeffff;
	if(!(PORTD->RXTX & 0x400))
	RegKn |=0x20000;
	else
	RegKn &=0xdffff;
	if(!(PORTD->RXTX & 0x800))
	RegKn |=0x40000;
	else
	RegKn &=0xbffff;
	if(!(PORTE->RXTX & 0x8000))
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
/* void    SetAvaria(void)
	{
static unsigned int R0;
static unsigned int R1;
static unsigned int R2;
static unsigned int R3;
	R1=0;
	R0=RomReceiveRS1[30][1];//DG
	R2=RomReceiveRS1[26][4];//KRP
	R3=RomReceiveRS1[14][3];//Gom
	if(R0 & 1)
	R1 |=1;//Raznos DG
	if(R0 & 2)
	R1 |=4;//PM DG
	if(R0 & 4)
	R1 |=8;//TOG DG
	if(R0 & 8)
	R1 |=0x40;//No Vozb DG
	if(R0 & 0x10)
	R1 |=0x100;//No oborotov DG
	if(R0 & 0x20)
	R1 |=0x400;//U No Norma DG
	if(R0 & 0x80)
	R1 |=0x1000;//K1 DG
	R0=RomReceiveRS1[29][1];//DG
	if(R0 & 1)
	R1 |=0x8000;//QF1 DG
	if(R0 & 2)
	R1 |=0x40000;//No Sinxr DG
	if(R0 & 0x4)
	R1 |=0x100000;//RIzol DG
	if(R0 & 0x10)
	R1 |=0x2000000;//Urov OG
	if(R0 & 0x20)
	R1 |=0x8000000;//No Topliva DG
	if(R0 & 0x40)
	R1 |=0x40000000;//DVF Topliva DG
	if(R2 & 0x2)
	R1 |=0x10000000;//Pogar
	if(R2 & 0x4)
	R1 |=0x400000;//RBP
	if(R2 & 0x8)
	R1 |=0x20000;//QF1 Set
	if(R2 & 0x10)
	R1 |=0x4000;//K1 Set
	if(R2 & 0x20)
	R1 |=0x20000000;//Chered Faz
	if(R3 & 0x1)//Raznos Gom
	R1 |=2;
	if(R3 & 0x10)//No oborotov Gom
	R1 |=0x200;
	if(R3 & 0x80)//K1 Gom
	R1 |=0x2000;
	if(R3 & 0x20)//U No Norma Gom
	R1 |=0x800;

	R3=RomReceiveRS1[13][3];//Gom
	if(R3 & 0x4)//RIzol Gom
	R1 |=0x200000;
	if(R3 & 0x1)//QF1 Gom
	R1 |=0x10000;


	R0=RomReceiveRS1[28][1];//DG
	if(R0 & 2)//no masla
	R1 |=0x4000000;
	Avaria=R1;

	} */


/*void    ResetError(void)
	{
	unsigned char R0;
	Error=0;
	for(R0=0;R0<=31;++R0)
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

	} */
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
	R1=RomReceiveEthernet[5];
	RegTransmitRS1[5]=R1 & 0xff;
	R1=RomReceiveEthernet[4];
	RegTransmitRS1[4]=R1 & 0xff;
	R1=RomReceiveEthernet[3];
	RegTransmitRS1[3]=R1 & 0xff;
	R1=RomReceiveEthernet[2];
	RegTransmitRS1[2]=R1 & 0xff;
	R1=RomReceiveEthernet[1];
	RegTransmitRS1[1]=0x55;//R1 & 0xff;

}

void UART2_IRQHandler(void)
{
	unsigned int R0;
	unsigned int R1;
	unsigned int R2;

	if(TestUsart[5])
	--TestUsart[5];
	else
	TestUsart[5]=110;


	R1=UART2->MIS & 0xfff;


	R0= UART2->DR & 0xfff;
	RamReceiveRS1[CtTransmitRS1]=R0;
	if(R1 & 0x780)//Int Error
	{

		UART2->ICR |=0xfff;//reset int
		UART2->RSR_ECR=0xff;//Reset Error
		CtReceiveRS1=CtReceiveRS0;
		CtTransmitRS1=CtReceiveRS0;
		PORTD->RXTX &=0xfffb;
		return;
	}
	else if(R1 & 0x10)
	{
		R0 &=0xff;
		UART2->ICR |=0xfff;//reset int
		UART2->RSR_ECR=0xff;//Reset Error

		if(RegimTr)
		{

			if(CtTransmitRS1>1)
			{

				--CtTransmitRS1;
				UART2->DR=RegTransmitRS1[CtTransmitRS1];
				if(CtTransmitRS1>=(CtReceiveRS0-1))
				ControlTransmit1=0;
				else
				ControlTransmit1 +=RegTransmitRS1[CtTransmitRS1];

			}
			else if(CtTransmitRS1==1)
			{

				--CtTransmitRS1;
				UART2->DR=ControlTransmit1;

			}
			else
			{
				++TestUsart[0];
				if(TestUsart[0]>999)
				TestUsart[0]=0;
				RegimTr=0;

				PORTD->RXTX &=0xfffb;
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







void    TransmitUsart(void)
{
	unsigned int R3;
	LoadRegTransmit();
	++Test141223[1];
	++Test141223[2];
	++Test141223[0];
	RegimTr=1;
	PORTD->RXTX |=0x4;
	R3=50;
	while(R3)
	--R3;
	UART2->CR |=0x100;
	UART2->DR=0xff;//NumberBlock1;
	UART2->ICR |=0x20;//Reset Int Transmit
	CtTransmitRS1=CtReceiveRS0;

	ControlTransmit1=0;//NumberBlock1;
	ControlReceive1=0;

	if(TestUsart[4])
	--TestUsart[4];
	else
	TestUsart[4]=100;



}
void ControlLed(void)
{
	static unsigned int R0;
	R0=RegimWork;

	if(R0 & 1)//DG1
	PORTE->RXTX &=0xfeff;
	else
	PORTE->RXTX |=0x100;

	if(R0 & 2)//DG2
	PORTE->RXTX &=0xfbff;
	else
	PORTE->RXTX |=0x400;

	if(R0 & 4)//Set
	PORTE->RXTX &=0xffbf;
	else
	PORTE->RXTX |=0x40;

	if(R0 & 8)//Rezerv
	PORTE->RXTX &=0xff7f;
	else
	PORTE->RXTX |=0x80;

	if(R0 & 0x10)//SES
	PORTE->RXTX &=0xdfff;
	else
	PORTE->RXTX |=0x2000;

	if(R0 & 0x20)//Eternet
	{
		PORTE->RXTX &=0xf7ff;
		PORTE->RXTX |=0x200;
	}
	else
	{
		PORTE->RXTX |=0x800;
		PORTE->RXTX &=0xfdff;
	}
	if(R0 & 0x40)//FVU
	PORTE->RXTX &=0xbfff;
	else
	PORTE->RXTX |=0x4000;
}




void Timer1_IRQHandler(void)
{
	static unsigned char R0;
	static unsigned char R1;
	static unsigned char R2;
	if(TIMER1->STATUS & 0x2)//CNT=ARR
	{

		TIMER1->STATUS=0;

		SYS->ICPR00 |=0x4000;//Timer1 Reset irq

		ControlLed();
		TIMER1->CNT=0;
		if(CtAnod<4)
		++CtAnod;
		else
		CtAnod=0;
		switch(CtAnod)
		{
		case 0:
			PORTD->RXTX &=0xffef;
			for(R0=0;R0<=15;++R0)
			{
				R2=Indicator[R0];
				R1=Stroka1[R2];
				while(!(SPI1->SSPx_SR & 2))
				;
				SPI1->SSPx_DR=R1;

			}
			while((SPI1->SSPx_SR & 0x10))
			;

			PORTE->RXTX &=0xffe0;
			PORTE->RXTX |=0x1;
			PORTD->RXTX |=0x10;



			break;
		case 1:
			PORTD->RXTX &=0xffef;
			for(R0=0;R0<=15;++R0)
			{
				R2=Indicator[R0];
				R1=Stroka2[R2];

				while(!(SPI1->SSPx_SR & 2))
				;
				SPI1->SSPx_DR=R1;

			}

			while((SPI1->SSPx_SR & 0x10))
			;
			PORTE->RXTX &=0xffe0;
			PORTE->RXTX |=0x2;
			PORTD->RXTX |=0x10;
			break;
		case 2:
			PORTD->RXTX &=0xffef;
			for(R0=0;R0<=15;++R0)
			{
				R2=Indicator[R0];
				R1=Stroka3[R2];
				while(!(SPI1->SSPx_SR & 2))
				;
				SPI1->SSPx_DR=R1;

			}
			while((SPI1->SSPx_SR & 0x10))
			;

			PORTE->RXTX &=0xffe0;
			PORTE->RXTX |=0x4;
			PORTD->RXTX |=0x10;
			break;
		case 3:
			PORTD->RXTX &=0xffef;
			for(R0=0;R0<=15;++R0)
			{

				R2=Indicator[R0];
				R1=Stroka4[R2];
				while(!(SPI1->SSPx_SR & 2))
				;
				SPI1->SSPx_DR=R1;

			}
			while((SPI1->SSPx_SR & 0x10))
			;
			PORTE->RXTX &=0xffe0;
			PORTE->RXTX |=0x8;
			PORTD->RXTX |=0x10;
			break;
		case 4:
			PORTD->RXTX &=0xffef;
			for(R0=0;R0<=15;++R0)
			{
				R2=Indicator[R0];
				R1=Stroka5[R2];
				while(!(SPI1->SSPx_SR & 2))
				;
				SPI1->SSPx_DR=R1;

			}
			while((SPI1->SSPx_SR & 0x10))
			;

			PORTE->RXTX &=0xffe0;

			PORTE->RXTX |=0x10;
			PORTD->RXTX |=0x10;
			break;
		default:break;
		}
		TIMER1->STATUS=0;
	}
	SYS->ICPR00 |=0x4000;//Timer1 Reset irq
}


void Timer2_IRQHandler(void)
{
	static unsigned int R0;
	static unsigned int R1;
	static unsigned int R2;


	if(TIMER2->STATUS & 0x2)//CNT=ARR
	{

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



		if(!(PORTD->RXTX & 4))//Transmit =off
		CtError[10]=CtError0[10];

		if(Error & 0xe00)
		{
			PORTD->RXTX &=0xfffb;
			InitUsart2();

			CtReceiveRS1=CtReceiveRS0;
			CtTransmitRS1=CtReceiveRS0;
			CtUsart[0]=8;
			Error &=0xfffff1ff;

			CtError[9]=CtError0[9];
			CtError[10]=CtError0[10];
			CtError[11]=CtError0[11];

		}





		for(R0=1;R0<=31;++R0)//prov
		{
			if(CtError[R0])
			--CtError[R0];
		}


		if(CtUsart[0])
		--CtUsart[0];
		else
		{
			if(PORTD->RXTX & 0x1)
			TransmitUsart();
			CtUsart[0]=8;//64;//32;
		}
		TIMER2->STATUS &=0xfffd;
	}
	TIMER2->STATUS=0;


}

void    InitTimer1(void)
{
	RST_CLK->PER_CLOCK |=0x4000; //Timer1
	TIMER1->CNTRL = 0;
	TIMER1->CNT = 0x00000007; //CT
	TIMER1->ARR =0x7fff;//CT from 0 to ARR
	TIMER1->PSG =0x1;//0x1;//FTim2=clk/2
	TIMER1->CNTRL = 0x1;//Enable Timer1
	TIMER1->IE =0x2;//CNT=ARR

	SYS->ISER00 |=0x4000;//Timer1
}
void    InitTimer2(void)
{
	RST_CLK->PER_CLOCK |=0x8000; //Timer2

	TIMER2->CNTRL = 0;
	TIMER2->CNT = 0x00000007; //CT
	TIMER2->ARR =0xffff;//CT from 0 to ARR
	TIMER2->PSG =0xf;//FTim2=clk/2
	TIMER2->CNTRL = 0x1;//Enable Timer2



	TIMER2->IE = 0x2;//CNT=ARR,CH3
	SYS->ISER00 |=0x8000;//Timer2
}

void InitPORTA(void)
{
	PORTA->OE=0xbd;

	PORTA->FUNC =0;

	PORTA->ANALOG =0xffff; //PA-digital
	PORTA->PULL=0x420000; //PA-R+=on
	PORTA->PWR=0x5555;
	PORTA->PD=0;////Trigger Shmitta=on

}
void InitPORTB(void)
{
	RST_CLK->PER_CLOCK |=0x400000; //PORTB
	PORTB->ANALOG =0xffff;
	PORTB->OE =0xa000;
	PORTB->PULL=0x40000000;//R +=on
	PORTB->FUNC =0x88000000;
	PORTB->PWR=0x55555555;
	PORTB->PD=0;//Trigger Shmitta=on
}
void InitPORTC(void)
{
	RST_CLK->PER_CLOCK |=0x800000;
	PORTC->RXTX=0;
	PORTC->OE =0x3;
	PORTC->FUNC =0;
	PORTC->ANALOG =0xffff; //PC0-digital
	PORTC->PULL =0xfff80000; //PC0-R+=on
	PORTC->PWR=0x55555555;
	PORTC->PD=0;//Trigger Shmitta=on

}

void InitPORTD(void)
{
	RST_CLK->PER_CLOCK |=0x1000000; //PORTD
	PORTD->ANALOG=0xffff; //analog
	PORTD->PULL=0x50000;//R +=on
	PORTD->PWR=0x55555555; //
	PORTD->OE =0x16;
	PORTD->FUNC =0xa;//ssp1,UART2 TXD,UART2 RXD
	PORTD->PD=0;//Trigger Shmitta=on
}

void InitPORTE(void)
{

	RST_CLK->PER_CLOCK |=0x2000000; //PORTE
	PORTE->OE=0x6fdf; //
	PORTE->FUNC=0;
	PORTE->ANALOG |=0xffff; //PF-digital
	PORTE->PULL=0; //PF2,PF3-R-=on
	PORTE->PWR=0x55555555; //Slow front
	PORTE->PD=0; //driver
}
void InitPORTF(void)
{


	PORTF->OE=0xff; //PF0,PF1=out
	PORTF->FUNC=0;
	PORTF->ANALOG |=0xffff; //PF-digital
	PORTF->PULL=0; //PF2,PF3-R-=on
	PORTF->PWR=0x5555; //Slow front
	PORTF->PD=0; //driver
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
	R0=RomReceiveRS1[76][1];
	R0 <<=8;
	R0 |=RomReceiveRS1[77][1];
	USet=R0;
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


/*void AccountUBat(void)
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
}*/

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

/*void AccountILine(void)
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
} */
/*void AccountPower(void)
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
}*/
void    IndicatorAvaria(void)
{
	static unsigned char R0;

	if((!AvariaInd)&&(!AvariaIndHigh)&&(!AvariaIndHighHigh))
	{
		AvariaInd=Avaria;
		AvariaIndHigh=AvariaHigh;
		AvariaIndHighHigh=AvariaHighHigh;
	}
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
		if(AvariaIndHigh & (1<<R0))
		{
			RegimIndAvaria=R0+32;
			if(CtIndAvaria[R0+32])
			break;
			else

			{
				AvariaIndHigh &=~(1<<R0);
				CtIndAvaria[R0+32]=CtIndAvaria0;
			}
		}

	}
	for(R0=0;R0<=31;++R0)
	{
		if(AvariaIndHighHigh & (1<<R0))
		{
			RegimIndAvaria=R0+64;
			if(CtIndAvaria[R0+64])
			break;
			else

			{
				AvariaIndHighHigh &=~(1<<R0);
				CtIndAvaria[R0+64]=CtIndAvaria0;
			}
		}

	}

	for(R0=0;R0<=31;++R0)
	{
		if(!(AvariaInd & (1<<R0)))
		CtIndAvaria[R0]=CtIndAvaria0;
		if(!(AvariaIndHigh & (1<<R0)))
		CtIndAvaria[R0+32]=CtIndAvaria0;
		if(!(AvariaIndHighHigh & (1<<R0)))
		CtIndAvaria[R0+64]=CtIndAvaria0;
	}

	switch(RegimIndAvaria)
	{
	case 0: IndicatorBukva(40,15);
		IndicatorBukva(25,14);//       P
		IndicatorBukva(10,13);//       A
		IndicatorBukva(3,12);//        3
		IndicatorBukva(22,11);//       H
		IndicatorBukva(23,10);//       O
		IndicatorBukva(26,9);//        C
		IndicatorBukva(40,8);//

		IndicatorBukva(40,7);
		IndicatorBukva(40,6);//
		IndicatorBukva(14,5);//       D
		IndicatorBukva(13,4);//       Ã
		IndicatorBukva(1,3);//        1
		IndicatorBukva(40,2);//
		IndicatorBukva(40,1);//
		IndicatorBukva(40,0);//
		break;
	case 1:IndicatorBukva(40,15);
		IndicatorBukva(25,14);//       P
		IndicatorBukva(21,13);//       M
		IndicatorBukva(42,12);//
		IndicatorBukva(4,11);//        4
		IndicatorBukva(41,10);//       ,
		IndicatorBukva(0,9);//         0
		IndicatorBukva(40,8);//

		IndicatorBukva(40,7);
		IndicatorBukva(40,6);//
		IndicatorBukva(14,5);//       D
		IndicatorBukva(13,4);//       Ã
		IndicatorBukva(1,3);//        1
		IndicatorBukva(40,2);//
		IndicatorBukva(40,1);//
		IndicatorBukva(40,0);//
		break;
	case 31:IndicatorBukva(27,15);//T
		IndicatorBukva(0,14);//        O
		IndicatorBukva(16,13);//       Æ
		IndicatorBukva(43,12);//
		IndicatorBukva(1,11);//        1
		IndicatorBukva(0,10);//        0
		IndicatorBukva(5,9);//         5
		IndicatorBukva(40,8);//

		IndicatorBukva(40,7);
		IndicatorBukva(40,6);//
		IndicatorBukva(14,5);//       D
		IndicatorBukva(13,4);//       Ã
		IndicatorBukva(1,3);//        1
		IndicatorBukva(40,2);//
		IndicatorBukva(40,1);//
		IndicatorBukva(40,0);//
		break;
	case 32:IndicatorBukva(22,15);//H
		IndicatorBukva(15,14);//       E
		IndicatorBukva(27,13);//       T
		IndicatorBukva(40,12);//
		IndicatorBukva(12,11);//       B
		IndicatorBukva(0,10);//        0
		IndicatorBukva(3,9);//         3
		IndicatorBukva(11,8);//        Á

		IndicatorBukva(40,7);
		IndicatorBukva(40,6);//
		IndicatorBukva(14,5);//       D
		IndicatorBukva(13,4);//       Ã
		IndicatorBukva(1,3);//        1
		IndicatorBukva(40,2);//
		IndicatorBukva(40,1);//
		IndicatorBukva(40,0);//
		break;
	case 64:IndicatorBukva(22,15);//H
		IndicatorBukva(15,14);//       E
		IndicatorBukva(27,13);//       T
		IndicatorBukva(40,12);//
		IndicatorBukva(0,11);//        O
		IndicatorBukva(11,10);//       Á
		IndicatorBukva(0,9);//         O
		IndicatorBukva(25,8);//        P

		IndicatorBukva(40,7);
		IndicatorBukva(40,6);//
		IndicatorBukva(14,5);//       D
		IndicatorBukva(13,4);//       Ã
		IndicatorBukva(1,3);//        1
		IndicatorBukva(40,2);//
		IndicatorBukva(40,1);//
		IndicatorBukva(40,0);//
		break;
	case 66: Indicator1(100,0);
		break;

	default:break;
	}
}
void IndicatorParam(void)
{
	static unsigned int R0;
	static unsigned int R1;

	switch(ParamInd)
	{
	case 0:
		IndicatorBukva(45,15);// U
		IndicatorBukva(40,14);//
		IndicatorBukva(13,13);//       Ã
		IndicatorBukva(1,12);//        1
		IndicatorBukva(40,11);//
		Indicator3(UGen1,8);

		IndicatorBukva(44,7);//       I
		IndicatorBukva(40,6);//
		IndicatorBukva(13,5);//       Ã
		IndicatorBukva(1,4);//        1
		IndicatorBukva(40,3);//
		Indicator3(IGen1,0);



		break;
	case 1:IndicatorBukva(46,15);// F
		IndicatorBukva(40,14);//
		IndicatorBukva(13,13);//       Ã
		IndicatorBukva(1,12);//        1
		Indicator3_(FGen1,8);

		IndicatorBukva(25,7);//       P
		IndicatorBukva(40,6);//
		IndicatorBukva(13,5);//       Ã
		IndicatorBukva(1,4);//        1
		IndicatorBukva(40,3);//
		Indicator3(PowerGen1,0);

		break;
	case 2:IndicatorBukva(47,15);// N
		IndicatorBukva(40,14);//
		IndicatorBukva(1,13);//        1
		IndicatorBukva(40,12);//
		Indicator1(NDiz1, 8);

		IndicatorBukva(25,7);//        P
		IndicatorBukva(21,6);//        M
		IndicatorBukva(40,5);//
		IndicatorBukva(1,4);//         1
		IndicatorBukva(40,3);//
		Indicator2_(PM1, 0);

		break;
	case 3:IndicatorBukva(27,15);// T
		IndicatorBukva(0,14);//         O
		IndicatorBukva(16,13);//        Æ
		IndicatorBukva(1,12);//         1
		Indicator1(TOG1, 8);


		IndicatorBukva(27,7);//        T
		IndicatorBukva(13,6);//        Ã
		IndicatorBukva(16,5);//       Æ
		IndicatorBukva(1,4);//         1
		Indicator3(TGG1, 0);

		break;
	case 4:IndicatorBukva(28,15);//Ó
		IndicatorBukva(27,14);//        Ò
		IndicatorBukva(40,13);//
		IndicatorBukva(1,12);//         1
		Indicator3(UT1, 8);


		IndicatorBukva(48,7);//        R
		IndicatorBukva(18,6);//        È
		IndicatorBukva(3,5);//        3
		IndicatorBukva(1,4);//         1
		Indicator2_(CtError[2], 0);
		break;




	case 10:IndicatorBukva(45,15);// U
		IndicatorBukva(40,14);//
		IndicatorBukva(13,13);//       Ã
		IndicatorBukva(2,12);//        2
		IndicatorBukva(40,11);//
		Indicator3(RegS, 8);

		IndicatorBukva(44,7);//       I
		IndicatorBukva(40,6);//
		IndicatorBukva(13,5);//       Ã
		IndicatorBukva(2,4);//        2
		IndicatorBukva(40,3);//
		Indicator3(RegS, 0);



		break;
	case 11:IndicatorBukva(46,15);// F
		IndicatorBukva(40,14);//
		IndicatorBukva(13,13);//       Ã
		IndicatorBukva(2,12);//        2
		Indicator3_(RegS, 8);

		IndicatorBukva(25,7);//       P
		IndicatorBukva(40,6);//
		IndicatorBukva(13,5);//       Ã
		IndicatorBukva(2,4);//        2
		IndicatorBukva(40,3);//
		Indicator3(RegS, 0);

		break;
	case 12:IndicatorBukva(47,15);// N
		IndicatorBukva(40,14);//
		IndicatorBukva(1,13);//         2
		IndicatorBukva(40,12);//

		Indicator1(NDiz2, 8);

		IndicatorBukva(25,7);//        P
		IndicatorBukva(21,6);//        M
		IndicatorBukva(40,5);//
		IndicatorBukva(2,4);//         1
		IndicatorBukva(40,3);//
		Indicator2_(RegS, 0);

		break;
	case 20:IndicatorBukva(45,15);// U
		IndicatorBukva(26,14);//        C
		IndicatorBukva(40,13);//
		IndicatorBukva(40,12);//
		IndicatorBukva(40,11);//
		Indicator3(USet, 8);

		IndicatorBukva(44,7);//       I
		IndicatorBukva(26,6);//       C
		IndicatorBukva(40,5);//
		IndicatorBukva(40,4);//
		IndicatorBukva(40,3);//
		Indicator3(ISet, 0);



		break;
	case 21:IndicatorBukva(46,15);// F
		IndicatorBukva(26,14);//       C
		IndicatorBukva(40,13);//
		IndicatorBukva(40,12);//
		Indicator3_(FSet, 8);

		IndicatorBukva(25,7);//       P
		IndicatorBukva(26,6);//       C
		IndicatorBukva(40,5);//
		IndicatorBukva(40,4);//
		IndicatorBukva(40,3);//
		Indicator3(PowerSet, 0);

		break;
	case 22:IndicatorBukva(46,15);// F
		IndicatorBukva(22,14);//       H
		IndicatorBukva(40,13);//
		IndicatorBukva(40,12);//
		Indicator3_(FLine, 8);

		IndicatorBukva(45,7);//       U
		IndicatorBukva(22,6);//       H
		IndicatorBukva(40,5);//
		IndicatorBukva(40,4);//
		IndicatorBukva(40,3);//
		Indicator3(ULine, 0);

		break;



	default:break;
	}

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

void main(void)
{
	unsigned long RMain;
	unsigned int R0;
#ifdef DEBUG
	debug();
#endif

	RST_CLK->PER_CLOCK |=0x10; //RST_CLK
	RST_CLK->PER_CLOCK |=0x8;//EEPROM
	RST_CLK->PER_CLOCK |=0x200000; //PORTA

	InitPORTB();
	InitPORTC();
	InitPORTD();
	RMain=10000;
	while(RMain)
	--RMain;
	while(!(PORTB->RXTX & 0x4000))
	//if(!(PORTC->RXTX & 4))
	{
		//while(1)
		{

			PORTC->RXTX &= 0xfffc;
			PORTC->RXTX &= 0xfffc;
			PORTC->RXTX &= 0xfffc;
			PORTC->RXTX &= 0xfffc;
			PORTC->RXTX |= 0x3;
			PORTC->RXTX |= 0x3;
			PORTC->RXTX |= 0x3;
			PORTC->RXTX |= 0x3;
			PORTD->RXTX &=0xfffb;
		}
	}

	//else
	{

		InitPORTC();


		RST_CLK->PER_CLOCK |=0x20000000; //PORTF
		InitPORTF();
		InitPORTA();
		InitPORTE();

		RST_CLK->PER_CLOCK |=0x1000000; //PORTD
		InitPORTD();

		RST_CLK->HS_CONTROL=0x1;//osc_E=on
		while(!(RST_CLK->CLOCK_STATUS & 4))
		RST_CLK->CPU_CLOCK=0x102;//CLOCK_CPU=osc_E




		RST_CLK->TIM_CLOCK=0x7000000;
		InitTimer1();
		InitTimer2();
		InitUsart2();
		InitSsp();




		CtReceiveRS1=CtReceiveRS0;
		CtTransmitRS1=CtReceiveRS0;




		//ResetError();

		RegimWork=0;
		/*RomReceiveRS1[7][4]=0;
RomReceiveRS1[5][4]=0;
RomReceiveRS1[7][2]=0;*/
		DPNom=3350;
		CtDPNom=100;
		CtReikaOff0=50;
		CtReikaOn0=5;
		NNom=2600;
		CtUsart[0]=NumberBlock1*2;
		ErrorPKDU2=0;
		PORTB->RXTX &=0xfbf;
		CtTimeKSKG=100;

		CtReadKn=40;
		AvariaInd=0;
		for(R0=0;R0<=31;++R0)
		CtIndAvaria[R0]=CtIndAvaria0;
		PORTD->RXTX &=0xfffb;
		PORTE->RXTX |=0x6fc0;
		while(1)


		{

			ReadKn();
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
				IndicatorParam();
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

			//AccountPowerSet;//
			AccountULine();
			//AccountUBat();
			//AccountIBat();
			AccountRIzol();

			AccountUGen1();
			AccountFGen1();
			AccountIGen1();
			AccountPowerGen1();


			SetError();
			//SetAvaria();






			if(TimeWork<9999)
			++TimeWork;
			else
			TimeWork=0;


		}
	}

}


