 /*TWI Slave   
 WDTON CKOPT
BOODLEVEL BODEN SUT1 CKSEL2*/    
//����������� �������� �� Font
    #include <avr/boot.h>
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <avr/io.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>
 //===================================================
    unsigned    int             CtBegin;

    unsigned    int             TestInd;
volatile    unsigned    char             NumberIndicator;
    unsigned    int             CtChangeInd;
    unsigned    char             ClearBit;
    unsigned    char             CtErrorLink[2];
	const   unsigned    char     CtErrorLink0[2]={20,100};
    unsigned    char             Error;
/*
	7	No link
*/
	unsigned    char            RegS;
	unsigned    char            RegSTemp;
	unsigned    char            Data10[3];
	unsigned    char            TestTWI;
    static const unsigned  char  NumberLink=0;
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;
  	unsigned    char            RomReceive[12];
 	unsigned    char            RegTransmit[12];
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=5000;//500;


    unsigned int CtStart;
    unsigned    char            EnableLoad;

 	volatile	unsigned    char    CtTest;


 	volatile	unsigned    int    CtErrorReceive;

	const unsigned    int    CtErrorReceive0=10000; 
 	volatile unsigned    char    MaskaSetka;
	volatile unsigned    char    OverlowT1;
	volatile 	unsigned    int	            Alfa;
	volatile unsigned char TestSetka;

	volatile unsigned char CtByteAnod;
	volatile unsigned char CtSetka;
	volatile unsigned char RegSPI;
	volatile unsigned char RegSPIA;	
	volatile unsigned char RegSPI0;		    
	volatile  unsigned  char     RegInd[43];//data 0...41
	unsigned    char    Test;

  


 

       
    /*============================================*/
 
//1low-2-3-4-5

//Simvol 35bit B low stroke
const char Font[128][5] PROGMEM = {
								{0,0,0,0,0},//0x20
								{3,202,6,7,8},
								{3,0,0x54,7,0},
								{15,202,6,7,8},
								{31,200,3,4,5},
								{63,202,6,7,8},
								{127,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{0x6,0x3,0xa1,0x5a,0xf4},//0x30 "0"


								{0x4,0x6,0x44,0x20,0x53},
								{0x6,0xae,0x9,0x4a,0x50},
								{0x7,0x12,0xd8,0x3,0x74},
								{0x2,0x45,0x98,0xa5,0x98},
								{0x7,0x12,0xe0,0x57,0x74},
								{0x6,0x3,0xae,0,0xf6},
								{0x7,0x16,0x50,0x2,0x3},
								{0x6,3,0xab,0x4a,0x76},
								{0x6,0x42,0xab,0x5a,0x42},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{1,0,0,0x24,0xd8},//<
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{0x6,200,3,4,5},//0x40
								{0x4,0x8d,0xb5,0x90,0xac},//A

								{0x6,0x93,0xab,0x4a,0xf6},//B
								{0x6,0x3,0x20,0x4a,0xf4},//C
								{0x6,0xb2,0x87,0x1e,0x78},//D
								{0x7,0x9b,0x2a,0x42,0xd6},//E
								{0x7,0x91,0x2a,0x42,0x86},//F
								{0x6,0xb,0xa0,0xca,0xf4},//G
								{0x1,0x99,0xab,0x58,0xa6},//H
								{0x6,0x6,0x40,0x22,0x53},//I
								{0x1,0x2,0x81,0x18,0x74},//J
								{0x1,0xdd,0x72,0x40,0x84},//K
								{0,0x9b,0x20,0x40,0xd4},//L
								{0x1,0x99,0xf5,0x58,0xa4},//M
								{0x1,0x99,0xa1,0xdc,0xa6},//N
								{0x6,0x3,0xa1,0x5a,0xf4},//O
								{0x6,0x91,0x2b,0x4a,0x86},//0x50  P
								{0x6,0x21,0x21,0xda,0xa3},//Q
								{0x6,0xd9,0x2b,0xca,0x86},//R
								{0x6,0x2,0xaa,0x4a,0x76},//S
								{0x7,0x16,0x40,0x22,0x3},//T
								{0x1,0x13,0xa1,0x58,0xf4},//U
								{0x1,0x12,0x2b,0xc8,0x9},//V
								{0x1,0xfd,0xa1,0x58,0xa6},//W
								{0x1,0x88,0x10,0xcd,0x2e},//X
								{0x1,0x16,0,0x4d,3},//Y
								{0x7,0x9a,0,0xb,0x5e},//Z
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},//0x60
								{0x7,0x9b,0xaa,0x42,0xf6},//�
								{100,200,3,4,5},
								{0x7,0x91,0x20,0x4a,0x84},//�
								{0x7,0xe8,0x1e,0x87,0x29},//�
								{101,202,6,7,8},
								{100,200,3,4,5},
								{0x5,0x9e,0x40,0xed,0x2f},//�
								{100,200,3,4,5},
								{0x1,0xbd,0xa9,0x58,0xa4},//�
								{0x6,0xbb,0xa9,0x54,0xa4},//�
								{101,202,6,7,8},
								{0x7,0xa8,0x87,0x1c,0x28},//�
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{7,0x99,0xa1,0x5a,0xa4},//0x70 �
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{1,0x12,0xab,0x58,0x76},//�
								{4,6,0x75,0xd0,0x8b},//�
								{1,0x98,0x10,0xcd,0x2e},//X
								{2,0x79,0x38,0xc1,0x85},//�
								{1,0x18,0xab,0x58,0x22},//�
								{1,0x9f,0xe1,0x58,0xf7},//�
								{1,0x7d,0xe1,0x58,0xa7},//�
								{0,0x32,0x8e,6,0x7a},//�
								{1,0x9f,0xa3,0x58,0xe7},//�
								{0,0x93,0xaa,0x40,0xf6},//�
								{6,2,0x89,0x5a,0x76},//�
								{2,0x95,0xf3,0x78,0xb7},//�
								{7,0xac,0xab,0x5a,0x22},//0x80  �

												};

//Simvol 35bit B high stroke


//6-7-8-9
const char FontA[128][5] PROGMEM = {
								{0,0,0,0,0},//0x20
								{3,202,6,7,8},
								{0x23,0,0x40,0x84,0},
								{15,202,6,7,8},
								{31,200,3,4,5},
								{63,202,6,7,8},
								{127,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},

								{0xc8,0x84,0x19,0xbb,0},//0x30 "0"
								{0xa2,0x20,0x24,0x4a,0},
								{0x8c,0x33,0x11,0x8a,0},
								{0x93,0x90,0x8,0x8b,0},
								{0x41,0xb8,0xc6,0x2c,0},
								{0x92,0x84,0x58,0x9f,0},
								{0xe0,0xd4,8,0x6b,0},
								{0x93,0x20,0x20,0xc0,0},//7


								{0xc8,0xd4,0x19,0xcb,0},//8
								{0x88,0xdc,0x11,0xd2,0},//9
								{0x69,0xa7,0x8a,0x31,0},
								{0xbc,0xd0,0xb,0xce,0},
								{0x0,0x0,0x26,0x2c,0},//<
								{0xbc,0xc1,0xb,0x9e,0},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},//0x40
								{0x69,0xa7,0x8a,0x31,0},//A
								{0xd8,0xd5,0x19,0xeb,0},//B
								{0xc0,0x4,0x19,0xab,0},//C
								{0xbc,0xc1,0xb,0x9e,0},//D
								{0xd0,0x57,0x10,0xeb,0},//E
								{0x50,0x55,0x10,0xe1,0},//F
								{0xc0,0x86,0x99,0xab,0},//G
								{0x58,0xd7,0x19,0x71,0},//H
								{0x82,0x20,0x24,0xca,0},//I
								{0x88,0x80,0x9,0x1b,0},//J
								{0x53,0x6f,0x10,0x21,0},//K
								{0xd0,0x7,0x10,0x2b,0},//L
								{0x7b,0x87,0x19,0x31,0},//M
								{0x58,0x87,0x99,0x75,0},//N
								{0xc8,0x84,0x19,0xbb,0},//O
								{0x58,0x55,0x11,0xe1,0},//0x50  P
								{0xc8,0xae,0x11,0xb3,0},//Q
								{0x58,0x7f,0x11,0xe1,0},//R
								{0x80,0xd4,0x19,0xcb,0},//S
								{0x92,0x20,0x24,0xc0,0},//T
								{0xd8,0x84,0x19,0x3b,0},//U
								{0x98,0x54,0xb3,0,0},//V
								{0x5c,0xaf,0x19,0x71,0},//W
								{0x10,0x3,0xdb,0x45,0},//X
								{0x90,0x20,0x71,0x44,0},//Y
								{0x90,0x3,0x43,0xcb,0},//Z
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{100,200,3,4,5},//0x60
								{0xd0,0xd7,0x18,0xeb,0},//�
								{100,200,3,4,5},
								{0x50,5,0x11,0xa1,0},//�
								{0x25,0x5b,0xea,0x85,0},//�
								{101,202,6,7,8},
								{100,200,3,4,5},
								{0x92,0x23,0xff,0x45,0},//�
								{100,200,3,4,5},
								{0x5c,0xb7,0x19,0x31,0},//�
								{0x4c,0xb7,0x19,0xb1,0},//�
								{101,202,6,7,8},
								{0x2c,0xc3,0xb,0x14,0},//�
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{0x58,0x87,0x19,0xb1,0},//0x70 �
								{101,202,6,7,8},
								{100,200,3,4,5},
								{101,202,6,7,8},
								{0x98,0xd4,0x19,0x5b,0},//�
								{0xab,0x24,0xa6,0x70,0},//�
								{0x10,3,0xdb,0x45,0},//X
								{0x55,0x1e,0xf0,0x21,0},//�
								{0x18,0xd6,0x19,0x50,0},//�
								{0xda,0xa7,0x39,0x7b,0},//�
								{0x5e,0xae,0x39,0x71,0},//�
								{0xb4,0xd0,0xa,0xde,0},//�
								{0xd8,0xe7,0x39,0x73,0},//�
								{0xd0,0xd5,0x18,0x6b,0},//�
								{0x88,0x90,0x19,0xdb,0},//�
								{0x5a,0xe5,0x3d,0x79,0},//�
								{0xc,0xf7,0x19,0xd0,0},//�

												};


//1 & 0xf8 high stroke

const char Font0[128] PROGMEM = {0,1,0x50,1,1,1,1,1,1,1,1,1,1,1,1,1,
0x50,0x10,0x50,0x58,0x40,0x58,0x50,0x58,0x50,0x50,1,1,0x40,1,1,1,//0x30
1,0x10,0x50,0x50,0x50,0x58,0x58,0x50,0x8,0x50,0x8,0x8,0,8,8,0x50,//0x40
0x50,0x50,0x50,0x50,0x58,0x8,0x8,0x8,0x8,0x8,0x58,1,1,1,1,1,//0x50
1,0x58,1,0x58,0x50,1,1,0x18,1,8,0x50,1,0x58,1,1,1,//0x60
0x58,1,1,1,0x8,0x10,8,0x40,8,8,8,0,8,0,0x50,0x40,//0x70
0x58,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,//0x80
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//0x90
unsigned char Tab3[5];
char Reg0;
char NumberInd;
char Simvol;
unsigned int RRR0;

	void LoadRegInd(void)//prov
	{
	unsigned char R0;


	RegInd[0]=0x20;
//	RegInd[1]=0x31;
//	RegInd[2]=0x32;
//	RegInd[3]=0x33;
	RegInd[4]=0x20;
	R0=CtErrorLink[0] & 7;
//	RegInd[5]=R0+0x30;
//	RegInd[6]=0x30;
//	RegInd[7]=0x30;
	R0=RamReceive[5];
	R0 &=7;

	RegInd[8]=0x20;//0x7c;


	RegInd[12]=0x20;

	RegInd[16]=0x20;

	RegInd[20]=0x20;
	RegInd[21]=0x20;

	RegInd[25]=0x20;

	
	RegInd[29]=0x20;//0x7c;

	RegInd[33]=0x20;

	RegInd[37]=0x20;

	RegInd[41]=0x20;
	RegInd[42]=0x20;
	}
	void ClearDisplay(void)//prov
	{
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	RegInd[R0]=0x20;
	}
/*	void ClearDisplay2(void)//prov
	{
	unsigned char R0;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}*/


	void LoadRegSPI(unsigned char R0,unsigned char R1)
	{
	unsigned char R2;
	unsigned char R3;
	R2=RegInd[R0+21]-0x20;//Simvol from TWI
	R3=pgm_read_byte_near(&Font[R2][R1]);
	RegSPI=R3;

	}
	void LoadRegSPIA(unsigned char R0,unsigned char R1)
	{
	unsigned char R2;
	unsigned char R3;
	R2=RegInd[R0]-0x20;//Simvol from TWI
	R3=pgm_read_byte_near(&FontA[R2][R1]);
	RegSPIA=R3;
//	RegSPI=0xff;

	}
	void LoadRegSPI0(unsigned char R0)
	{
	unsigned char R2;
	unsigned char R3;
	R2=RegInd[R0]-0x20;//Simvol from TWI
	R3=pgm_read_byte_near(&Font0[R2]);
	RegSPI0=R3;
//	RegSPI=0xff;

	}


	
	void	LoadAlfaStart(void)
	{
	unsigned int R0;

	R0=ICR1-Alfa;	 
    OCR1B=Alfa;
    OCR1A=R0;

	}

     void LoadRomReceive(void)
    {
    unsigned char R0;
    unsigned char R1;           
    R0=0;
    for(R1=1;R1<=10;++R1)
    R0 +=RamReceive[R1];
    if(R0==RamReceive[0])
        {
//	++TestTWI;
    for(R0=1;R0<=10;++R0)
   RomReceive[R0]=RamReceive[R0];




	CtErrorLink[0]=CtErrorLink0[0];
	CtErrorLink[1]=CtErrorLink0[1];
        }
   
    }  
  
    void ReceiveTransmitSlave(void)
    {

    if((TWSR & 0xf8)==0x60)//Adr Slave Receive
        {

    CtReceive=10;


		}
	else    if((TWSR & 0xf8)==0x80)//Data Slave Receive
        {

    RamReceive[CtReceive]=TWDR;
    if(CtReceive)
	{

    --CtReceive;

		}
	else
	{
	LoadRomReceive();

	}
		}
    else if((TWSR & 0xf8)==0xA8)//Adr Slave Transmit
        {

    CtTransmit=10;
	EnableLoad=0;
    TWDR=RegTransmit[CtTransmit];


		}
    else if((TWSR & 0xf8)==0xb8)//Data Slave Transmit
        {
    if(CtTransmit)
			{
    --CtTransmit;
    TWDR=RegTransmit[CtTransmit];
			}
	else
			{
    TWDR=RegTransmit[CtTransmit];
	EnableLoad=1;
			}

        }
    TWCR |=(1<<TWINT);


    }  
     void    LoadControlSum(void)
    {
    unsigned char R0;
    RegTransmit[0]=0;
    for(R0=1;R0<=10;++R0)
    RegTransmit[0] +=RegTransmit[R0];
    }  
     void    LoadRegTransmit(void)
    {
 //   unsigned char R0;
//	++TestTwi;    
//    for(R0=1;R0<=10;++R0)
    RegTransmit[7]=TestTWI;
    RegTransmit[6]=3;
    RegTransmit[5]=4;
    RegTransmit[4]=15;
    RegTransmit[3]=16;
    RegTransmit[2]=17;
    RegTransmit[1]=RegS;//TestTWI;
/*    RegTransmit[7]=RegTransmitUsart[7];
    RegTransmit[6]=RegTransmitUsart[8];
    RegTransmit[5]=RegTransmitUsart[9];
    RegTransmit[4]=RegTransmitUsart[10];
    RegTransmit[3]=RegTransmitUsart[11];
    RegTransmit[2]=RegTransmitUsart[12];*/
    }
	void HexDec(unsigned int R2)
	{
	unsigned int R0;
	unsigned char R1;
		
	R0=R2;

	R1=0;
	while(R0>=100)
		{
	R0 -=100;
	++R1;
		}	
	Data10[0]=0x30+R1;//0x7c;
	R1=0;
	while(R0>=10)
		{
	R0 -=10;
	++R1;
		}	
	Data10[1]=0x30+R1;//0x7c;
	Data10[2]=0x30+R0;//0x7c;	
	}	
	   
   
void	Load0_999(unsigned char R0,unsigned int R1)
	{
	unsigned int R2;
	R2=R1;//Data   
 	HexDec(R2);  
 	RegInd[R0]=Data10[0];
 	RegInd[R0+1]=Data10[1];
 	RegInd[R0+2]=Data10[2];
	}
	    void    ReadKn(void)
    {


    unsigned    char R0;

    R0=RegSTemp;//Temp;
    if(bit_is_clear(PINB,PB2))
    R0 |=0x1;
	else
    R0 &=0xfe;
    if(bit_is_clear(PINB,PB3))
    R0 |=0x2;
	else
    R0 &=0xfd;
    if(bit_is_clear(PINB,PB4))
    R0 |=0x4;
	else
    R0 &=0xfb;

    if(R0==RegSTemp)
    RegS=R0;
    RegSTemp=R0;	
	}
	void IndicatorTest(void)
	{	
	Load0_999(1,RomReceive[1]); 
	Load0_999(5,RomReceive[2]);
	Load0_999(9,RomReceive[3]); 
	Load0_999(13,RomReceive[4]);
	Load0_999(17,RomReceive[5]); 
	Load0_999(22,RomReceive[6]);
	Load0_999(26,RomReceive[7]); 
	Load0_999(30,RomReceive[8]);
	Load0_999(34,RomReceive[9]); 
	Load0_999(38,RomReceive[10]);	
	}



	void IndNoMasla(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;				
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;		
	RegInd[6]=0x48;
	RegInd[7]=0x45;
	RegInd[8]=0x54;
	RegInd[9]=0x20;
	RegInd[10]=0x4d;
	RegInd[11]=0x41;
	RegInd[12]=0x43;
	RegInd[13]=0x6c;
	RegInd[14]=0x41;		
	RegInd[15]=0x20;
	RegInd[16]=0x20;	
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}
	
	void IndNoTopliva(void)
	{
	unsigned char R0;	
	RegInd[0]=0x20;
	RegInd[1]=0x20;				
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;			
	RegInd[6]=0x48;
	RegInd[7]=0x45;
	RegInd[8]=0x54;
	RegInd[9]=0x20;
	RegInd[10]=0x54;
	RegInd[11]=0x4f;
	RegInd[12]=0x70;
	RegInd[13]=0x6c;
	RegInd[14]=0x69;
	RegInd[15]=0x42;
	RegInd[16]=0x41;	
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;			
	}	
	void IndT8Gr(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;				
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;
	RegInd[6]=0x20;
	RegInd[7]=0x20;
	RegInd[8]=0x54;
	RegInd[9]=0x3c;
	RegInd[10]=0x38;
	RegInd[11]=0x22;
	RegInd[12]=0x43;
	RegInd[13]=0x20;
	RegInd[14]=0x20;	
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}	
	void IndT37Gr(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;				
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;
	RegInd[6]=0x20;
	RegInd[7]=0x20;
	RegInd[8]=0x54;
	RegInd[9]=0x3c;
	RegInd[10]=0x33;
	RegInd[11]=0x37;
	RegInd[12]=0x22;
	RegInd[13]=0x43;
	RegInd[14]=0x20;	
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}
	
	void IndZashitaOff(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;
	RegInd[6]=0x33;
	RegInd[7]=0x41;
	RegInd[8]=0x7a;
	RegInd[9]=0x69;
	RegInd[10]=0x54;
	RegInd[11]=0x41;
	RegInd[12]=0x20;
	RegInd[13]=0x4f;
	RegInd[14]=0x54;
	RegInd[15]=0x4b;	
	RegInd[16]=0x6c;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}
	
	
	void IndLowIzol(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	RegInd[4]=0x48;
	RegInd[5]=0x69;
	RegInd[6]=0x33;
	RegInd[7]=0x4b;
	RegInd[8]=0x41;
	RegInd[9]=0x80;
	RegInd[10]=0x20;
	RegInd[11]=0x69;
	RegInd[12]=0x33;
	RegInd[13]=0x4f;	
	RegInd[14]=0x6c;
	RegInd[15]=0x80;
	RegInd[16]=0x77;
	RegInd[17]=0x69;
	RegInd[18]=0x80;
	for(R0=19;R0<=41;++R0)
	RegInd[R0]=0x20;

	}
	
	void IndNoPusk(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;
	RegInd[6]=0x48;
	RegInd[7]=0x45;
	RegInd[8]=0x54;
	RegInd[9]=0x20;
	RegInd[10]=0x70;
	RegInd[11]=0x74;
	RegInd[12]=0x43;
	RegInd[13]=0x4b;	
	RegInd[14]=0x41;
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;

	}

	void IndNoOborotov(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x48;
	RegInd[5]=0x45;
	RegInd[6]=0x54;
	RegInd[7]=0x20;
	RegInd[8]=0x4f;
	RegInd[9]=0x61;
	RegInd[10]=0x4f;
	RegInd[11]=0x50;
	RegInd[12]=0x4f;
	RegInd[13]=0x54;	
	RegInd[14]=0x4f;
	RegInd[15]=0x42;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}	
	void IndNoVozb(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x48;
	RegInd[5]=0x45;
	RegInd[6]=0x54;
	RegInd[7]=0x20;
	RegInd[8]=0x42;
	RegInd[9]=0x4f;
	RegInd[10]=0x33;
	RegInd[11]=0x61;
	RegInd[12]=0x74;
	RegInd[13]=0x67;	
	RegInd[14]=0x64;
	RegInd[15]=0x45;
	RegInd[16]=0x48;
	RegInd[17]=0x69;
	RegInd[18]=0x80;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}
	void IndErrorU(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x55;
	RegInd[6]=0x20;
	RegInd[7]=0x48;
	RegInd[8]=0x45;
	RegInd[9]=0x20;
	RegInd[10]=0x48;
	RegInd[11]=0x4f;
	RegInd[12]=0x50;
	RegInd[13]=0x4d;	
	RegInd[14]=0x41;
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}	

	void IndErrorKG(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;
	RegInd[6]=0x4f;
	RegInd[7]=0x54;
	RegInd[8]=0x4b;
	RegInd[9]=0x41;
	RegInd[10]=0x33;
	RegInd[11]=0x20;
	RegInd[12]=0x42;	
	RegInd[13]=0x63;
	RegInd[14]=0x20;
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}
	void IndErrorKS(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;
	RegInd[6]=0x4f;
	RegInd[7]=0x54;
	RegInd[8]=0x4b;
	RegInd[9]=0x41;
	RegInd[10]=0x33;
	RegInd[11]=0x20;
	RegInd[12]=0x42;	
	RegInd[13]=0x43;
	RegInd[14]=0x20;
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}

	void IndErrorLuk(void)
	{
	unsigned char R0;			
	RegInd[0]=0x20;
	RegInd[1]=0x48;
	RegInd[2]=0x45;
	RegInd[3]=0x69;
	RegInd[4]=0x43;
	RegInd[5]=0x70;
	RegInd[6]=0x50;
	RegInd[7]=0x41;
	RegInd[8]=0x42;
	RegInd[9]=0x48;
	RegInd[10]=0x4f;	
	RegInd[11]=0x43;
	RegInd[12]=0x54;
	RegInd[13]=0x7d;
	RegInd[14]=0x20;
	RegInd[15]=0x6c;
	RegInd[16]=0x7f;
	RegInd[17]=0x4b;
	RegInd[18]=0x4f;
	RegInd[19]=0x42;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}



	void IndErrorSet(void)
	{
	unsigned char R0;			
	RegInd[0]=0x20;
	RegInd[1]=0x48;
	RegInd[2]=0x45;
	RegInd[3]=0x69;
	RegInd[4]=0x43;
	RegInd[5]=0x70;
	RegInd[6]=0x50;
	RegInd[7]=0x41;
	RegInd[8]=0x42;
	RegInd[9]=0x48;
	RegInd[10]=0x4f;	
	RegInd[11]=0x43;
	RegInd[12]=0x54;
	RegInd[13]=0x7d;
	RegInd[14]=0x20;
	RegInd[15]=0x43;
	RegInd[16]=0x45;
	RegInd[17]=0x54;
	RegInd[18]=0x69;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}


	void IndObratnajaP(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x4f;			
	RegInd[2]=0x61;
	RegInd[3]=0x50;
	RegInd[4]=0x41;
	RegInd[5]=0x54;
	RegInd[6]=0x48;
	RegInd[7]=0x41;
	RegInd[8]=0x80;
	RegInd[9]=0x20;
	RegInd[10]=0x4d;
	RegInd[11]=0x4f;
	RegInd[12]=0x7a;	
	RegInd[13]=0x48;
	RegInd[14]=0x4f;
	RegInd[15]=0x43;
	RegInd[16]=0x54;
	RegInd[17]=0x7d;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}	
	

	
	void IndPeregruzKG(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x70;
	RegInd[5]=0x45;
	RegInd[6]=0x50;
	RegInd[7]=0x45;
	RegInd[8]=0x63;
	RegInd[9]=0x50;
	RegInd[10]=0x74;
	RegInd[11]=0x33;
	RegInd[12]=0x4b;	
	RegInd[13]=0x41;
	RegInd[14]=0x20;
	RegInd[15]=0x42;
	RegInd[16]=0x63;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;

	}	



	void IndNoLink(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x48;
	RegInd[5]=0x45;
	RegInd[6]=0x54;
	RegInd[7]=0x20;
	RegInd[8]=0x43;
	RegInd[9]=0x42;
	RegInd[10]=0x80;
	RegInd[11]=0x33;
	RegInd[12]=0x69;
	RegInd[13]=0x20;	
	RegInd[14]=0x20;
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;

	}	
	
		

	void IndPogar(void)
	{
	unsigned char R0;

	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x20;
	RegInd[5]=0x20;
	RegInd[6]=0x20;
	RegInd[7]=0x20;
	RegInd[8]=0x70;
	RegInd[9]=0x4f;
	RegInd[10]=0x67;
	RegInd[11]=0x41;
	RegInd[12]=0x50;
	RegInd[13]=0x20;	
	RegInd[14]=0x20;
	RegInd[15]=0x20;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}	

	void IndUKorpus(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x20;			
	RegInd[2]=0x20;
	RegInd[3]=0x20;
	RegInd[4]=0x55;
	RegInd[5]=0x20;
	RegInd[6]=0x48;
	RegInd[7]=0x41;
	RegInd[8]=0x20;
	RegInd[9]=0x4b;
	RegInd[10]=0x4f;
	RegInd[11]=0x50;
	RegInd[12]=0x70;
	RegInd[13]=0x74;	
	RegInd[14]=0x43;
	RegInd[15]=0x45;
	RegInd[16]=0x20;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;

	}	
	void IndDozapravkaTopliva(void)
	{
	unsigned char R0;

	RegInd[0]=0x20;
	RegInd[1]=0x64;			
	RegInd[2]=0x4f;
	RegInd[3]=0x33;
	RegInd[4]=0x41;
	RegInd[5]=0x70;
	RegInd[6]=0x50;
	RegInd[7]=0x41;
	RegInd[8]=0x42;
	RegInd[9]=0x4b;
	RegInd[10]=0x41;
	RegInd[11]=0x20;
	RegInd[12]=0x54;
	RegInd[13]=0x4f;	
	RegInd[14]=0x70;
	RegInd[15]=0x6c;
	RegInd[16]=0x69;
	RegInd[17]=0x42;
	RegInd[18]=0x41;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;

	}	
	void IndDozapravkaMasla(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x64;			
	RegInd[2]=0x4f;
	RegInd[3]=0x33;
	RegInd[4]=0x41;
	RegInd[5]=0x70;
	RegInd[6]=0x50;
	RegInd[7]=0x41;
	RegInd[8]=0x42;
	RegInd[9]=0x4b;
	RegInd[10]=0x41;
	RegInd[11]=0x20;
	RegInd[12]=0x4d;
	RegInd[13]=0x41;	
	RegInd[14]=0x43;
	RegInd[15]=0x6c;
	RegInd[16]=0x41;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;

	}
	
	void IndOtkazFVU(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[5]=0x4f;
	R1[6]=0x54;
	R1[7]=0x4b;
	R1[8]=0x41;
	R1[9]=0x33;

	R1[11]=0x75;
	R1[12]=0x42;
	R1[13]=0x74;	

	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];

	}	
	void IndAvarDvig(void)
	{
	unsigned char R0;
	RegInd[0]=0x20;
	RegInd[1]=0x41;			
	RegInd[2]=0x42;
	RegInd[3]=0x41;
	RegInd[4]=0x50;
	RegInd[5]=0x69;
	RegInd[6]=0x80;
	RegInd[7]=0x20;
	RegInd[8]=0x64;
	RegInd[9]=0x42;
	RegInd[10]=0x69;
	RegInd[11]=0x63;
	RegInd[12]=0x41;
	RegInd[13]=0x54;	
	RegInd[14]=0x45;
	RegInd[15]=0x6c;
	RegInd[16]=0x80;
	RegInd[17]=0x20;
	RegInd[18]=0x20;
	RegInd[19]=0x20;
	RegInd[20]=0x20;
	for(R0=21;R0<=41;++R0)
	RegInd[R0]=0x20;
	}
	
	
	

	
	
	void IndRabota(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;
	R1[6]=0x50;
	R1[7]=0x41;
	R1[8]=0x61;
	R1[9]=0x4f;
	R1[10]=0x54;
	R1[11]=0x41;
	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];

	}
	
	

	
	void IndNoLinkSHU(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[3]=0x4f;
	R1[4]=0x61;
	R1[5]=0x50;
	R1[6]=0x7c;
	R1[7]=0x42;

	R1[9]=0x43;
	R1[10]=0x42;
	R1[11]=0x80;
	R1[12]=0x33;	
	R1[13]=0x69;

	R1[15]=0x79;
	R1[16]=0x74;

	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];

	}	
		
	void IndNoLinkMSHU(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[3]=0x4f;
	R1[4]=0x61;
	R1[5]=0x50;
	R1[6]=0x7c;
	R1[7]=0x42;

	R1[9]=0x43;
	R1[10]=0x42;
	R1[11]=0x80;
	R1[12]=0x33;	
	R1[13]=0x69;

	R1[15]=0x4d;
	R1[16]=0x7a;
	R1[17]=0x74;

	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];

	}
		
	void IndOtkazLuk(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;


	R1[3]=0x4f;
	R1[4]=0x54;
	R1[5]=0x4b;
	R1[6]=0x41;
	R1[7]=0x33;

	R1[9]=0x6c;
	R1[10]=0x7f;
	R1[11]=0x4b;
	R1[12]=0x41;
		
	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];




	}	

	void IndNoLinkRU(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[3]=0x48;//4f;
	R1[4]=0x45;//61;
	R1[5]=0x54;//50;
	R1[6]=0x20;//7c;
	R1[7]=0x43;

	R1[8]=0x42;
	R1[9]=0x80;
	R1[10]=0x33;
	R1[11]=0x69;	
	R1[12]=0x20;
	R1[13]=0x43;
	R1[14]=0x20;
	R1[15]=0x50;
	R1[16]=0x48;


	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];

	}
	void IndNoLinkSN(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[3]=0x4f;
	R1[4]=0x61;
	R1[5]=0x50;
	R1[6]=0x7c;
	R1[7]=0x42;

	R1[9]=0x43;
	R1[10]=0x42;
	R1[11]=0x80;
	R1[12]=0x33;	
	R1[13]=0x69;

	R1[15]=0x79;
	R1[16]=0x43;
	R1[17]=0x48;

	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];

	}	
	void IndSDA(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[5]=0x4b;
	R1[6]=0x79;
	R1[7]=0x74;

	R1[9]=0x7e;
	R1[10]=0x43;
	R1[11]=0x64;
	R1[12]=0x41;


	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];
	}
	void IndNiobij(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[5]=0x4b;
	R1[6]=0x79;
	R1[7]=0x74;
	R1[9]=0x48;
	R1[10]=0x69;

	R1[11]=0x4f;
	R1[12]=0x61;
	R1[13]=0x69;
	R1[14]=0x6a;

	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];
	}	
	void Ind19U6(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;

	R1[5]=0x4b;
	R1[6]=0x79;
	R1[7]=0x74;
	R1[9]=0x31;
	R1[10]=0x39;

	R1[11]=0x74;
	R1[12]=0x36;


	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];
	}
	void IndGotovSystem(void)
	{
	unsigned char R1[42];
	unsigned char R0;
	for(R0=0;R0<=41;++R0)
	R1[R0]=0x20;
	R1[2]=0x63;
	R1[3]=0x4f;
	R1[4]=0x54;
	R1[5]=0x4f;
	R1[6]=0x42;
	R1[7]=0x48;
	R1[8]=0x4f;
	R1[9]=0x43;
	R1[10]=0x54;
	R1[11]=0x7d;
	R1[12]=0x20;
	R1[13]=0x43;	
	R1[14]=0x69;
	R1[15]=0x43;
	R1[16]=0x54;
	R1[17]=0x45;
	R1[18]=0x4d;
	R1[19]=0x7c;


	for(R0=0;R0<=41;++R0)
	RegInd[R0]=R1[R0];


	}		
	
																								   
/*++++++++++++++++++++++++++++++++++++++++++*/



    int main(void)
    {
    DDRA=0xff;


    DDRB=0xb1;
	PORTB |= 0xff;// Control UAB

    DDRC=0xfc;
    PORTC |=0xff;/*VT3=off*/

    DDRD=0xbf;
    PORTD=0xff;/*imp=off,Reset=off*/
//    PORTD=0xef;/*Reset=on*/
	ICR1=200;//20KH
    TCCR1A=0xb2;
    TCCR1B=0x11;
	Alfa=(ICR1>>1)-80;//-24;

	LoadAlfaStart();

 	/*InitCpi*/

	TCCR0=5;// /1024
 	TIMSK |=0x1;//enable int overlow T0
	ASSR=0;
	SPCR=0x70;//0x73;
//SPDR=0x55;
    Test=1;
	SPCR |=0x80;


 	SPDR=0;//RegSPI;

        //INIT TWI;
        TWBR=0x7f;//F TWI
    TWAR=(NumberLink<<1)+4;   
   TWCR =(1<<TWEA)|(1<<TWEN);

    CtStart=200;
	while(CtStart--)	_WDR();

	LoadRegInd(); 
	CtSetka=0;
	CtByteAnod=0;
	LoadRegSPI(CtSetka,CtByteAnod);
	CtErrorLink[0]=CtErrorLink0[0];
	CtErrorLink[1]=CtErrorLink0[1];
	Error=0;
	NumberIndicator=2;
    _SEI();
 	ClearDisplay();
	CtBegin=250;    
  /*Work program*/ 
  RomReceive[1]=2;    	 
    while(1)
	    {
    _WDR();
	ReadKn();
	if(CtBegin)
	IndSDA();
//	else if(RegS !=2)
//	IndNiobij();
	else if(!RomReceive[1])
			{
	IndicatorTest();

			}
	else
			{



//	if(RomReceive[NumberIndicator] !=19)
//	ClearDisplay2();
	switch(RomReceive[NumberIndicator])
				{

	case 1:
	IndNoTopliva();
	break;
	case 2:
	IndT8Gr();
	break;
	case 3:
	IndT37Gr();
	break;

	case 4:
	IndZashitaOff();
	break;
	case 5:
	IndOtkazLuk();
	break;

	case 6:
	IndLowIzol();
	break;
	case 7:
	IndNoPusk();
	break;
	case 8:
	IndNoLinkMSHU();
	break;

	case 9:
	IndNoOborotov();
	break;
	case 10:
	IndNoVozb();
	break;
	case 11:
	IndErrorU();
	break;
	case 12:
	IndPeregruzKG();
	break;
	case 13:
	IndErrorKG();
	break;

	case 15:
	IndNoLinkRU();
	break;
	case 16:
	IndObratnajaP();
	break;

	case 17:
	IndPeregruzKG();
	break;
	case 18:


	case 19:
	IndPogar();
	break;

	case 20:
	IndErrorKS();
	break;

	case 21:
	IndUKorpus();
	break;
	case 22:
	IndErrorLuk();
	break;
	case 23:
	IndErrorSet();
	break;
	case 24:
	IndNoMasla();
	break;
	case 25:
	IndDozapravkaTopliva();
	break;
	case 26:
	IndDozapravkaMasla();
	break;
	case 27:
	IndOtkazFVU();
	break;
	case 28:
	IndAvarDvig();
	break;
	case 29:
	IndGotovSystem();
	break;


	case 31:
	IndRabota();
	break;


	case 32:
	IndNoLinkSN();
	break;



				}
			}
	if(EnableLoad)
		{
    LoadRegTransmit();
    LoadControlSum();
		}
	if(TWCR & 0x80)
			{

  ReceiveTransmitSlave();

		}

	if(!CtErrorLink[0])//ErrorLink;
			{

//INIT TWI;
    TWBR=0;//F TWI
   TWAR=0;
   TWCR =0;
	TWSR=0xf8;
    CtStart=50;
	while(CtStart--)	_WDR();
    TWBR=0x7f;//F TWI
    TWAR=(NumberLink<<1)+4;
   TWCR =(1<<TWEA)|(1<<TWEN);
 //   TWCR |=(1<<TWINT);
	CtErrorLink[0]=CtErrorLink0[0];

    CtStart=200;
	while(CtStart--)	_WDR();
			}


        }

 
 }

 SIGNAL(SIG_OVERFLOW2)/*128mks*/
{
	if(CtChangeInd)
	--CtChangeInd;
	else
	{
	if(TestInd)
	--TestInd;
	else
	TestInd=999;
	CtChangeInd=5000;
M1:	if(NumberIndicator<=9)
	++NumberIndicator;
	else
	NumberIndicator=2;
	if((!RomReceive[NumberIndicator])&&(NumberIndicator !=2))
	goto M1;
	}


	++TestTWI;
	if(!MaskaSetka)
	{
	switch(CtSetka)
		{
	case 0:PORTA &=1;
	PORTB &=0xfc;
	PORTD &=0x30;
	PORTC &=0x83;
	PORTA |=1;
	break;
	case 1:PORTA &=2;
	PORTA |=2;
	break;	
	case 2:PORTA &=4;
	PORTA |=4;
	break;	
	case 3:PORTA &=8;
	PORTA |=8;
	break;
	case 4:PORTA &=0x10;
	PORTA |=0x10;
	break;	
	case 5:PORTA &=0x20;
	PORTA |=0x20;
	break;	
	case 6:PORTA &=0x40;
	PORTA |=0x40;
	break;	
	case 7:PORTA &=0x80;
	PORTA |=0x80;
	break;	
	case 8:PORTA=0;
	PORTD |=1;
	break;	
	case 9:	PORTD &=0x32;
	PORTD |=2;
	break;
	case 10:	PORTD &=0x38;
	PORTD |=8;
	break;	
	case 11:	PORTD &=0x34;
	PORTD |=4;
	break;
	case 12:	PORTD &=0x30;
	PORTB |=2;
	break;		
	case 13:	PORTB &=0xfd;
	PORTB |=1;
	break;
	case 14:	PORTB &=0xfc;
	PORTD |=0x40;
	break;	
	case 15:	PORTD &=0x30;//0x38;
	PORTD |=0x80;
	break;	
	case 16:	PORTD &=0x30;
	PORTC |=8;
	break;
	case 17:	PORTC &=0x83;
	PORTC |=4;
	break;	
	case 18:	PORTC &=0x83;
	PORTC |=0x10;
	break;	
	case 19:	PORTC &=0x83;
	PORTC |=0x20;
	break;
	case 20:	PORTC &=0x83;
	PORTC |=0x40;
	break;	
									

	default:
	break;
		}
	MaskaSetka=1;
	TCCR2=2;
	TCNT2=0;
	}
	else
	{
	PORTA =0;
	PORTB &=0xfc;
	PORTD &=0x30;
	PORTC &=0x83;

 	TIMSK &=0xbf;
	TCCR2=0;
	if(CtSetka<=18)
	++CtSetka;

	else
	CtSetka=1;

	CtByteAnod=0;
	LoadRegSPI(CtSetka,0);
	LoadRegSPI0(CtSetka);
	RegSPI |=RegSPI0;
	SPCR |=0x80;//enable Interrupt
	PORTC |=0x80;

 	SPDR=RegSPI;

	}
}
SIGNAL(SIG_SPI)
{
	unsigned char R1;



	if(CtByteAnod<8)
	{
	++CtByteAnod;
	if(CtByteAnod<=4)
		{
	LoadRegSPI(CtSetka,CtByteAnod);

 	SPDR=RegSPI;

		}
	else
		{
	R1=CtByteAnod-5;
	LoadRegSPIA(CtSetka,R1);

	SPDR=RegSPIA;

		}

	}
	else
	{
	PORTC &=0x7f;
	SPCR &=0x7f;//denable interrupt
	TestSetka=1;

	TCNT2=0x8;

	TIFR |=0x40;//reset flag
	TIMSK |=0x40;
	TCCR2=0x2;
	MaskaSetka=0;
	}



}

 SIGNAL(SIG_OVERFLOW0)//16ms
	{
	if(CtBegin)
	--CtBegin;
	if(CtErrorLink[0])
	--CtErrorLink[0];

	if(CtErrorLink[1])
	--CtErrorLink[1];
	else
	Error |=0x80;

	}
