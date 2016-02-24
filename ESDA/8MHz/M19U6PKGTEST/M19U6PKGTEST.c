/* TWI-Master*/
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>
    unsigned    char    TestPeriod;
    unsigned    char    RegimTest;
    unsigned    char    TestTWI;
   unsigned    int    KoefU;

	unsigned int CtEnableEeprom;
    unsigned    char    CtLinkTWI;
    unsigned    char    SregTemp;
	unsigned char	CtTransmitUsart;

	unsigned char	StatizmVozb;
/*
0	Gashenie=1/Vozb=0
1	Paralel=1/Avtonom=0
*/

	unsigned int	DeltaUITest;




	volatile   	unsigned int	FazaI;
	volatile   	unsigned int	FazaU;


	unsigned char	NumberLinkUsart;//CtNoReceive;
	unsigned char	CtNoReceiveUsart;
	unsigned char	RCHTransmit;
	unsigned char	RCLTransmit;
	unsigned char	RCHReceive;
	unsigned char	RCLReceive;
    unsigned char CtReceiveUsart;
 	unsigned    char            RegTransmitUsart[10];
 	unsigned    char            RamReceiveUsart[10];
 	unsigned    char            RomReceiveUsart[10][3];

	const unsigned char  high[256] PROGMEM= {0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,1,0xc0,0x80,0x41,0,0xc1,0x81,
						0x40,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,0,0xc1,0x81,0x40,1,0xc0,
						0x80,0x41,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,0,0xc1,0x81,0x40,1,
						0xc0,0x80,0x41,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,1,0xc0,0x80,0x41,
						0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,0,0xc1,0x81,
						0x40,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,1,0xc0,
						0x80,0x41,0,0xc1,0x81,0x40,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,1,
						0xc0,0x80,0x41,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,
						0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,1,0xc0,0x80,0x41,0,0xc1,0x81,
						0x40,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,1,0xc0,
						0x80,0x41,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,0,0xc1,0x81,0x40,1,
						0xc0,0x80,0x41,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,
						0,0xc1,0x81,0x40,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,0,0xc1,0x81,
						0x40,1,0xc0,0x80,0x41,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40,1,0xc0,
						0x80,0x41,0,0xc1,0x81,0x40,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,1,
						0xc0,0x80,0x41,0,0xc1,0x81,0x40,0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,
						0,0xc1,0x81,0x40,1,0xc0,0x80,0x41,1,0xc0,0x80,0x41,0,0xc1,0x81,0x40};

	const unsigned char low[256] PROGMEM={0,0xc0,0xc1,1,0xc3,0x3,0x2,0xc2,0xc6,0x6,0x7,0xc7,0x5,0xc5,0xc4,
						0x4,0xcc,0xc,0xd,0xcd,0xf,0xcf,0xce,0xe,0xa,0xca,0xcb,0xb,0xc9,
						0x9,0x8,0xc8,0xd8,0x18,0x19,0xd9,0x1b,0xdb,0xda,0x1a,0x1e,0xde,0xdf,0x1f,
						0xdd,0x1d,0x1c,0xdc,0x14,0xd4,0xd5,0x15,0xd7,0x17,0x16,0xd6,0xd2,0x12,0x13,
						0xd3,0x11,0xd1,0xd0,0x10,0xf0,0x30,0x31,0xf1,0x33,0xf3,0xf2,0x32,0x36,0xf6,
						0xf7,0x37,0xf5,0x35,0x34,0xf4,0x3c,0xfc,0xfd,0x3d,0xff,0x3f,0x3e,0xfe,0xfa,
						0x3a,0x3b,0xfb,0x39,0xf9,0xf8,0x38,0x28,0xe8,0xe9,0x29,0xeb,0x2b,0x2a,0xea,
						0xee,0x2e,0x2f,0xef,0x2d,0xed,0xec,0x2c,0xe4,0x24,0x25,0xe5,0x27,0xe7,0xe6,
						0x26,0x22,0xe2,0xe3,0x23,0xe1,0x21,0x20,0xe0,0xa0,0x60,0x61,0xa1,0x63,0xa3,
						0xa2,0x62,0x66,0xa6,0xa7,0x67,0xa5,0x65,0x64,0xa4,0x6c,0xac,0xad,0x6d,0xaf,
						0x6f,0x6e,0xae,0xaa,0x6a,0x6b,0xab,0x69,0xa9,0xa8,0x68,0x78,0xb8,0xb9,0x79,
						0xbb,0x7b,0x7a,0xba,0xbe,0x7e,0x7f,0xbf,0x7d,0xbd,0xbc,0x7c,0xb4,0x74,0x75,
						0xb5,0x77,0xb7,0xb6,0x76,0x72,0xb2,0xb3,0x73,0xb1,0x71,0x70,0xb0,0x50,0x90,
						0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9c,


						0x5c,0x5d,0x9d,0x5f,0x9f,0x9e,0x5e,0x5a,0x9a,0x9b,0x5b,0x99,0x59,0x58,0x98,
						0x88,0x48,0x49,0x89,0x4b,0x8b,0x8a,0x4a,0x4e,0x8e,0x8f,0x4f,0x8d,0x4d,0x4c,
						0x8c,0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,
						0x80,0x40};	


    unsigned char RegimTime;
    unsigned char CtTime;
    const unsigned char CtTime0=100;

    unsigned char AdEnd;
    unsigned char NumberAdSlow;
    unsigned char ChangeNumberAd;
    unsigned int AdTempMin;
	volatile	unsigned long AdTemp[7];
 	unsigned    long	AdResult[7];
	unsigned    char	NumberAd;
    unsigned    int	CtAd;
	static const unsigned    int	    CtAd0=1024;


    unsigned char ChangeFNom;
    unsigned char CtChangeUNom;
    const unsigned int DeltaUDop=10;
    const unsigned int DeltaFDop=200;
    const unsigned int DeltaFDopSet=200;
    const unsigned int DeltaULineDop=40;


	unsigned char NumberRegim;


    unsigned int UNom;
    unsigned int UNomConst;
	unsigned int UGen;
	unsigned int USet;
	unsigned int ULine;
	unsigned int ULineGenOld;
	unsigned int ULineSetOld;
	unsigned int ULineGen;
	unsigned int ULineSet;
	unsigned int ULineLine;

    unsigned char CtSetOff;
	unsigned char StatusKontaktorSet;
	unsigned char StatusKontaktorGen;
    unsigned int KStatizm;


    unsigned char CtKn;
	unsigned char EnableReadKn;
volatile	unsigned    int	RegS;
/*RegS
0	Pusk
1	U+
2	F+
3	DG
4	Vibor
5	Stop
6	U-
7	F-
8	Set
9	Sdvig
*/
volatile	unsigned int RegSTemp;
volatile	unsigned int RegSOld;
volatile	unsigned int RegSWork;
volatile	unsigned char CtEnableKn;


    unsigned int TNT1Old[4];
    unsigned int Period[5];
    unsigned char CtGate[4];
    unsigned char CtOverLow[4];


 	unsigned    int             Error;
    unsigned    char RegimError;
 	unsigned    char	CtError[13];
    static const unsigned  char CtError0[13]=
    {250,250,250,250,100,100,100,250,30,30,30,100,100};
    unsigned    char RegimClearError;
 	unsigned    char	CtClearError[4];
    static const unsigned  char CtClearError0=20;

/*	Error
0	No on set
1	No off set
2	No on Gen
3	No off Gen
4	No NR Set
5	No NR Gener
6	Set !=Norma
7	Gen !=Norma
8	No link Ind
9	No link KD
10  USART
11  Obratnaya P
12	Peregruz
*/

 	unsigned    char            RomReceive[9][2];
 	unsigned    char            RegTransmit[12][2];
    unsigned    char            NumberLink;
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=10000;
    unsigned    char            CtErrorLink[2];
	const   unsigned    char    CtErrorLink0=250;//19;
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;



       
//============================================ 
    void    WriteEeprom(unsigned char Arg4,unsigned char Arg5)
    {
    _WDR();
    while(bit_is_set(EECR,EEWE))
    ;
    while(bit_is_set(SPMCR,SPMEN))
    ;

	EEARL=Arg4;
	EEARH=0;
	EEDR=Arg5;
    SregTemp=SREG;
    _CLI();
	EECR |=(1<<EEMWE);
                       		 
	EECR |=(1<<EEWE);
    SREG=SregTemp;				
    while(bit_is_set(EECR,EEWE))
    ;

    while(bit_is_set(SPMCR,SPMEN))
    ;
}
    void    WriteEepromWord(unsigned char  Arg4,unsigned int Arg6)
	
{
    WriteEeprom(Arg4,Arg6);
    Arg6=Arg6>>8;
    WriteEeprom((Arg4+1),Arg6);
}
    unsigned char    ReadEeprom(unsigned char Arg6)
	
    {		 	
	
	EEARH=0;
    EEARL=Arg6;
	EECR=EECR | (1<<EERE);

	while	(bit_is_set(EECR,EERE))
    ;

	return EEDR;
    }

    unsigned int    ReadEepromWord(unsigned char Arg6)
	
    {
    unsigned int Arg7;
    Arg7=ReadEeprom(Arg6+1);		 	
    Arg7=Arg7<<8;
    Arg7=Arg7 | ReadEeprom(Arg6);   	
   	return Arg7;
    }


//	TWI
	unsigned char WaitTWCR(void)
    {
   while(!(TWCR & 0x80))
        {
    if(CtTWCR)
		{
	_WDR();
    --CtTWCR;
		}
    else
            {
    CtTWCR=CtTWCR0;
    return 0;//error
            }
        }
    CtTWCR=CtTWCR0;
    return 1;
    }

unsigned char    Receive(void)
    {
    if(!WaitTWCR())
    return 0;

    if((TWSR & 0xf8) == 0x8)//Well Start
            {
    TWDR=(NumberLink<<1)+5;//0x5;//SLA+R
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);


            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }

    if(!WaitTWCR())
    return 0;

   if((TWSR & 0xf8)==0x40)//Adres+ACK
            {
    CtReceive=11;

   RamReceive[CtReceive]=TWDR;
   TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);

            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
    return 0;

            }
RR:  if(!WaitTWCR())
    return 0;
 
   if((TWSR & 0xf8)==0x50)//Data+ACK
            {
    if(CtReceive)
                {
   
    --CtReceive;
   RamReceive[CtReceive]=TWDR;


   TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
   goto RR;
                }
    else
                {
    TWCR=(1<<TWINT)|(1<<TWEN);
 
    goto RR;

                }

            }
    else
            {
 
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 1;

            }

    }

unsigned char    Transmit(void)
    {


    if(!WaitTWCR())
    return 0;

    if(((TWSR & 0xf8) == 0x8)||((TWSR & 0xf8) == 0x10))//start
            {
    TWDR=(NumberLink<<1)+4;//0x4;//SLA+W
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);


            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }
    if(!WaitTWCR())
    return 0;


   if((TWSR & 0xf8)==0x18)//Adres+ACK
            {
    CtTransmit=10;
   TWDR=RegTransmit[CtTransmit][NumberLink];
   TWCR=(1<<TWINT)|(1<<TWEN);

            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }
TT:


    if(!WaitTWCR())
    return 0;

   if((TWSR & 0xf8)==0x28)//Data+ACK
            {
    if(CtTransmit)
                {
    --CtTransmit;
   TWDR=RegTransmit[CtTransmit][NumberLink];
   TWCR=(1<<TWINT)|(1<<TWEN);
   goto TT;
                }
    else
                {

    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 1;

                }

            }
    else
            {


    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }
    }
    void    ReceiveTransmitMaster(void)
{
	unsigned char R0;
	unsigned int R1;
	unsigned char R2;
	unsigned int R3;
	unsigned int R4;

//ControlSum for Transmit

	R3=0;
	R4=0;
	if(Error & 0x41)
	RegTransmit[4][1] &=0xfd;

    for(R0=1;R0<=10;++R0)
    {
	R3 +=RegTransmit[R0][0];
	R4 +=RegTransmit[R0][1];
    }
    RegTransmit[0][0]=R3;
    RegTransmit[0][1]=R4;


//Transmit
    TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//Start
    CtTWCR=CtTWCR0;
	R1=5000;
    if(!Transmit())
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    while(R1)
	{_WDR();
    --R1;
	}


//Receive
   TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//Start
   CtTWCR=CtTWCR0;
    if(Receive())
    {

//ControlSum
    R2=0;
    for(R0=1;R0<=10;++R0)
    R2=R2+RamReceive[R0];
    if(R2==RamReceive[0])
        {
    for(R0=1;R0<=8;++R0)
			{
    RomReceive[R0][NumberLink]=RamReceive[R0];
	RamReceive[R0]=R0;
			}
    CtErrorLink[NumberLink]=CtErrorLink0;
    CtError[NumberLink+8]=CtError0[NumberLink+8];

        }
    }
    else
    {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);


    }

}
//===============================================================


	
	


	    void    ReadKnPB5(void)
    {
    unsigned    int R0;


    R0=RegSTemp & 0x3e0;

    if(bit_is_clear(PINB,PB7))
    R0 |=0x1;

    if(bit_is_clear(PINB,PB0))
    R0 |=0x2;

    if(bit_is_clear(PINB,PB1))
    R0 |=0x4;

    if(bit_is_clear(PINB,PB4))
    R0 |=0x8;

    if(bit_is_clear(PINA,PA7))
    R0 |=0x10;

	RegSTemp=R0;
	}



	    void    ReadKnPB6(void)
    {
    unsigned    int R0;
    unsigned    char R1;

    R0=RegSTemp & 0x1f;

    if(bit_is_clear(PINB,PB7))
    R0 |=0x20;

    if(bit_is_clear(PINB,PB0))
    R0 |=0x40;

    if(bit_is_clear(PINB,PB1))
    R0 |=0x80;

    if(bit_is_clear(PINB,PB4))
    R0 |=0x100;

    if(bit_is_clear(PINA,PA7))
    R0 |=0x200;
	 
    if(R0==RegSTemp)
			{
	RegSOld=RegS;
    RegS=R0;
	for(R1=0;R1<=9;++R1)
				{
	if((RegS & (1<<R1))&&(!(RegSOld & (1<<R1))))
					{
	RegSWork |=(1<<R1);
	CtKn=30;
					}
	
				}
			}

	if(!CtKn)
	RegSWork=0;

    RegSTemp=R0;

 
    }

void InitAd   (void)
    {
//    ADMUX=0xc0;//2.56=Ref
//    ADMUX |=0x12;//18 chanel
    ADMUX=0xd2;//2.56=Ref
    SFIOR=0;//SFIOR & 0x1f;//start from end AD
 	CtAd=CtAd0;
	NumberAd=6;
	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
    ADCSRA |=(1<<ADPS1);
	ADCSRA |=(1<<ADPS0); /*32*/	 		
	ADCSRA |=(1<<ADIE);/* enable interrupt*/
    ADCSRA |=(1<<ADSC);/* Start*/
    }

	void	LoadRegInd(void)
	{

	unsigned char R2;

	RegTransmit[1][0]=0;
	RegTransmit[2][0]=RegimTest;




	switch(RegimTest)
		{
	case 0:	if(Error & 0x300)
	RegTransmit[3][0]=Error>>8;
	else
	RegTransmit[3][0]=123;
	RegTransmit[4][0]=UGen;
	RegTransmit[5][0]=USet;
	RegTransmit[6][0]=ULine;
	RegTransmit[7][0]=ULineGen>>1;;
	RegTransmit[8][0]=ULineSet>>1;
	RegTransmit[9][0]=ULineLine>>1;
	RegTransmit[10][0]=AdResult[6]>>2;
	PORTC |=0xfc;
	break;
	case 1:
	
	RegTransmit[3][0]=Period[0]>>8;
	RegTransmit[4][0]=Period[1]>>8;
	RegTransmit[5][0]=Period[2]>>8;
	RegTransmit[6][0]=PIND & 1;
	RegTransmit[7][0]=KoefU;
	RegTransmit[8][0]=KoefU>>8;
	PORTD &=0xfd;
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];
	PORTC |=0xf8;
	PORTC &=0xfb;
	break;
	case 2:
	RegTransmit[3][0]=Period[3]>>8;//0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;

	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[8][0]=PIND & 1;
	PORTD |=2;
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];
	PORTC |=0xf4;
	PORTC &=0xf7;

	break;
	case 3:
	RegTransmit[3][0]=0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;
	RegTransmit[8][0]=0;
	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];
	PORTC |=0xec;
	PORTC &=0xef;
	break;
	case 4:
	RegTransmit[3][0]=0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;
	RegTransmit[8][0]=0;	
	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];	
		PORTC |=0xdc;
	PORTC &=0xdf;
	break;
	case 5:
	RegTransmit[3][0]=0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;
	RegTransmit[8][0]=0;
	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];	
		PORTC |=0xbc;
	PORTC &=0xbf;
	break;
	case 6:
	RegTransmit[3][0]=0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;
	RegTransmit[8][0]=0;
	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];	
		PORTC |=0x7c;
	PORTC &=0x7f;
	break;
	case 7:
	RegTransmit[3][0]=0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;
	RegTransmit[8][0]=0;
	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];	
	break;
	case 8:
	RegTransmit[3][0]=0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;
	RegTransmit[8][0]=0;
	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];	
	break;
	case 9:
	RegTransmit[3][0]=0;
	RegTransmit[4][0]=0;
	RegTransmit[5][0]=0;
	RegTransmit[6][0]=0;
	RegTransmit[8][0]=0;
	RegTransmit[7][0]=RomReceive[3][1];
	RegTransmit[8][0]=0;
	RegTransmit[9][0]=RomReceive[1][1];
	RegTransmit[10][0]=RomReceive[2][1];	
	break;

	default:break;
		}
	R2=RegimTest;

	if(RegS & 1)
	R2=0;
	else if(RegS & 2)
	R2=1;
	else if(RegS & 4)
	R2=2;
	else if(RegS & 8)
	R2=3;
	else if(RegS & 0x10)
	R2=4;
	else if(RegS & 0x20)
	R2=5;
	else if(RegS & 0x40)
	R2=6;
	else if(RegS & 0x80)
	R2=7;
	else if(RegS & 0x100)
	R2=8;
	else if(RegS & 0x200)
	R2=9;


	RegimTest=R2;
	if(RegimTest>9)
	RegimTest=9;


//	RegTransmit[2][0]=TestTWI;//CtError[8];//R2;
	}




void    LoadRegTransmit(void)
    {
	unsigned char R0;
	unsigned char R1;
	unsigned int R2;


	R1=RegTransmit[5][1];	
	R0=RegS;
	if((NumberRegim !=2))
		{
	R0 &=0x7b;
	R0 |=ChangeFNom;
		}
    RegTransmit[1][1]=RegimTest;
    RegTransmit[2][1]=RegS>>8;
	R1=RomReceive[1][0];
	R1 &=0x7;
	if(StatusKontaktorGen & 0x11)
	R1 |=0x40;
	if(StatusKontaktorSet & 0x11)
	R1 |=0x80;
    RegTransmit[3][1]=R1;
	R1=0;
	if((ULine>190)&&(ULine<250))
	R1|=1;
	if(Error & 0x8c)
	R1 |=0x80;
	if((!(Error & 0x40))&&(USet>180))
	R1 |=0x20;

	if(Error & 0x43)
	R1 |=0x40;



	RegTransmit[5][1]=R1;
	R2=AdResult[5];
	R2=R2>>2;
	RegTransmit[6][1]=R2;

    }    

void    SetError(void)
    {
    unsigned char R0;   
     for(R0=0;R0<=11;++R0)
        {
    if(!(CtError[R0]))
    Error |=(1<<R0);

        }


    }

void    ResetError(void)
    {
    unsigned char R0;
     Error=0;
    RegimError=0;      
     for(R0=0;R0<=11;++R0)
    CtError[R0]=CtError0[R0];



    }
/*void	InitUSART(void)
	{
    UCSRA=0;//������� ��������
    UCSRB=0x1c;//enable transmit,receive,8bit
    UCSRC=0x86;//0x24 7 bit control parity
    UBRRH=0;
    UBRRL=103;//9600

     while(!(UCSRA & 0x20))
   _WDR();

	UCSRB  |=0x80;//Interrupt Receive enable

	}*/
	void	TransmitUsart(unsigned char R0)
	{
	unsigned char R1;
	for(R1=0;R1<=R0;++R1)
		{
     while(!(UCSRA & 0x20))
   _WDR();
    UDR=RegTransmitUsart[R1];
		}
			
	}

	unsigned char ProgramPageHigh(unsigned int R0)
	{
 	unsigned char R3;
	R3=pgm_read_byte_near(high+R0);//flash
	return R3;	
	}


	unsigned char ProgramPageLow(unsigned int R0)
	{
 	unsigned char R3;
	R3=pgm_read_byte_near(low+R0);//flash
	return R3;	
	}


	void	 CRCReceive(unsigned char R0)
	{

	unsigned char R2;
	R2=RCHReceive^ R0;//TransmitIrbi[R4][R5][(R0++)];
	RCHReceive=RCLReceive^ ProgramPageHigh(R2);
	RCLReceive=ProgramPageLow(R2);

	}

	void	ResetErrorLinkUsart(void)
	{
	unsigned char R0;
	R0=UDR;//UDR1;
	UCSRA &=0x22;//reset error
	CtReceiveUsart=0;
	}
		
	void	ReceiveUsart(void)
	{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	unsigned char R3;
	unsigned char R4;

	R3=CtReceiveUsart;
	if(UCSRA &(1<<RXC))
		{
	R1=UCSRA;
	R2=UCSRB;
	R4=UCSRC;
	R0=UDR;
	if(R1 & 0x1c)//error
			{

	ResetErrorLinkUsart();
	return;
			}
	if(R3>9)
			{

	ResetErrorLinkUsart();
	return;
			}

	RamReceiveUsart[R3]=R0;
	if(R3<=5)
			{
	++R3;
	CRCReceive(R0);

	CtReceiveUsart=R3;




			}
	else
			{
			
				
	if(R3<=6)
				{
	++R3;
	CtReceiveUsart=R3;
				}

	else
				{	
	
	 if((RamReceiveUsart[6]==RCHReceive)&&(RamReceiveUsart[7]==RCLReceive))
					{


	for(R2=0;R2<=5;++R2)
						{
	RomReceiveUsart[R2][RamReceiveUsart[3]]=RamReceiveUsart[R2];
	RamReceiveUsart[R2]=R2;
						}
	CtError[10]=CtError0[10];

	CtNoReceiveUsart=19;
					}

	CtReceiveUsart=0;
	RCHReceive=0xff;
	RCLReceive=0xff;

				}
			}


		}

	}    
	unsigned int CalcU(unsigned char R1)
	{
	unsigned long R0;
	R0=AdResult[R1];
	R0 *=KoefU;
	R0=R0/5320;	   
	return R0;
	}


	unsigned int CalcDeltaU(unsigned char R1)
	{
	unsigned long R0;
	R0=AdResult[R1];
	R0=R0/6;   
	return R0;
	}



	

	
		
	    



	
										
	void	ChangeStatusKontaktorGenHand(void)
	{
	unsigned char R0;
//0	on/off
//1	NR was on
//2	enable on/off
//3	off from out



	R0=StatusKontaktorGen;



	if(!(RomReceive[1][1] & 1))//KG denable from KD
			{

	R0 &=0xee;
	R0 |=0x40;//first on
	RegSWork &=0xfff7;
			}
	else if(Error & 0x88c)//UGen no norma,no on/off,obratnaya P
	R0 &=0xee;	

	else if((R0 & 0x40)&&((NumberRegim>=3)||(!NumberRegim)))
			{

	R0 |=1;
	R0 &=0xbf;
			}

	else if((RegSWork & 0x8)&&(NumberRegim==2))//Gen
			{
	if(R0 & 1)
	R0 &=0xee;

	else
	R0 |=1;

	RegSWork &=0xfff7;
			}
	if((R0 & 0x20)&&(RomReceive[1][1] & 2))//KG=off
	R0 &=0xce;
	StatusKontaktorGen=R0;
		}





	void	ChangeStatusKontaktorSet(void)
	{
	unsigned char R0;
	R0=StatusKontaktorSet;
	if((Error & 0x43)||(RomReceive[6][1] & 0x20))// set no norma, error on/off

			{

	R0 &=0xee;
	RegTransmit[4][1] |=4;//bit Off
	RegTransmit[4][1] &=0xfd;//bit On=off
			}
	else if(RomReceive[2][1] & 0x20)//&&(NumberRegim))
		{
	R0 &=0xee;
	R0 |=0x80;
		}

	else if((RomReceive[2][1] & 0x10)&&(NumberRegim))//bit On from SHU2
	R0 |=0x1;
	else if(!NumberRegim)//DU
		{
	RegTransmit[4][1] &=0xfe;			
	if((RomReceive[5][1] & 0x10)&&(!(StatusKontaktorSet & 0xa0)))
			{
	R0 |=1;
//	RegTransmit[4][1] |=2;//bit On 
	RegTransmit[4][1] &=0xfb;//bit Off=off
			}		
	else if(!CtSetOff)
			{
	R0 &=0xee;
	RegTransmit[4][1] |=4;//bit Off
	RegTransmit[4][1] &=0xfd;//bit On=off
			}	
			
		}
	else if((NumberRegim !=1)&&(NumberRegim !=4))			 
		{

	RegTransmit[4][1] &=0xfe;
	if(RegSWork & 0x100)//Set
			{
	if(R0 & 1)
				{

	R0 &=0xee;
	RegTransmit[4][1] |=4;//bit Off
	RegTransmit[4][1] &=0xfd;//bit On=off
				}

	else 
				{
	R0 |=1;
	RegTransmit[4][1] |=2;//bit On
	RegTransmit[4][1] &=0xfb;//bit Off=off
				}


	RegSWork &=0xfeff;
			}

		}


	if(NumberRegim>=4)//Rezerv
		{
	if((USet>207)&&(USet<248))
			{
	RegimTime &=0xfe;
	if(NumberRegim==4)
	R0 |=1;
	RegTransmit[4][1] |=2;//bit On
		}

	if(Error & 0x40)//Set no norma
			{


//	RegimTime |=1;
	if(Error & 0x88c)
	RegTransmit[4][1] &=0xfe;

	else if(!CtTime)
	RegTransmit[4][1] |=1;//Pusk for KD
			}
	else
			{
	if(RomReceive[1][1] & 4)//Kset=on
	RegTransmit[4][1] &=0xfe;




	if(NumberRegim==5)//Rezerv DU
				{
	if(RomReceive[5][1] & 0x10)//Set ON
					{
	R0 |=1;

	RegTransmit[4][1] |=2;//bit On 
	RegTransmit[4][1] &=0xfb;//bit Off=off
					}		
	else if(!CtSetOff)
					{
	R0 &=0xee;

	RegTransmit[4][1] |=4;//bit Off
	RegTransmit[4][1] &=0xfd;//bit On=off
	if(RomReceive[5][1] & 1)
	RegTransmit[4][1] |=1;//Pusk for KD
					}
				}
	CtTime=CtTime0;
//	RegimTime &=0xfe;
			}
		}
	if(!(RomReceive[5][1] & 0x10))
	R0 &=0x7f;

	StatusKontaktorSet=R0;

	}




	void	ChangeRegimClearError(void)
	{
	if((USet>=207) && (USet<=248))
	RegimClearError |=1;
	else
	RegimClearError &=0xfe;
		
	}
	
void	ChangeStatusNRSet(void)	
	{
	if(RomReceive[1][1] & 4)
	StatusKontaktorSet &=0xfd;
	else if(!(PORTC & 0x80))//NRSet=on 		
	StatusKontaktorSet |=0x2;		
	}

void	ChangeStatusNRGen(void)	
	{
	if(RomReceive[1][1] & 2)
	StatusKontaktorGen &=0xfD;
	else if(!(PORTC & 0x40))//NRGen=on 		
	StatusKontaktorGen |=0x2;		
	}	
					

	void	ChangeRegimError(void)
	{
	unsigned char R0;
	unsigned char R1;
	R0=0;
	R1=RomReceive[1][1];
	if(StatusKontaktorSet & 0x10)//Set
		{
	if(!(R1 & 4))
	R0 |=1;
		
		}
	else
		{
	if(R1 & 4)
	R0 |=0x2;
			
		}				
	if(StatusKontaktorGen & 0x10)//Generator
		{
	if(!(R1 & 2))
	R0 |=4;
			
		}
	else
		{
	if(R1 & 2)
	R0 |=0x8;
			
		}
	if((R1 & 4) && (!(PORTC & 0x80)))//NR Set
	R0 |=0x10;
		
	if((R1 & 2) && (!(PORTC & 0x40)))//NR Gen
	R0 |=0x20;

	if((StatusKontaktorSet & 0x11)||(R1 & 4))
			{		
	if((USet<=208)||(USet>=247))
	R0 |=0x40;
			}

	
	if(RomReceive[1][1] & 1)//enable KD
		{
	StatizmVozb |=4;//vozb=on
	StatizmVozb &=0xf7;//vozb=on


	if((UGen<=206)||(UGen>=248))
	R0 |=0x80;
		
		}
	else
		{
	StatizmVozb &=0xfb;//vozb=off
	StatizmVozb |=8;//vozb=off
		}
	if(NumberRegim==1)
		{
	StatizmVozb |=0x10;
	R0=0;
		}
	else
	StatizmVozb &=0xef;
	RegimError=R0;		
			
	}	
void    ClearError(void)
    {

	if(!CtClearError[0])
	Error &=0xffbf;

    }
	







	void	ChangeUNomHand(void)
	{
	unsigned int R1;
	if(EnableReadKn)
		{	
	if(RegS & 2)
			{
	++UNomConst;

			}

	else if(RegS & 0x40) 
			{
	--UNomConst;

			}
	EnableReadKn=0;
	if(!CtEnableEeprom)
			{
	CtEnableEeprom=10000;
	R1=ReadEepromWord(4);
	if(UNomConst !=R1)
	WriteEepromWord(4,UNomConst);
			}
		}
				
	}	


	void	InitTWI(void)
	{
	unsigned int R0;		

    TWBR=0;//F TWI
    TWAR=0;
    TWCR =0;
    TWSR =0xf8;
    R0=50;
	while(R0--)	_WDR();
    TWBR=0x7f;//F TWI
    R0=200;
	while(R0--)	_WDR();
	CtErrorLink[0]=CtErrorLink0;
	CtErrorLink[1]=CtErrorLink0;

		
		
	}		
	void	ChangeNumberRegim(void)
	{
/*
0	DU
1	hand MU
2	hand SHU
3	Avt SHU	Planov perevod
4	Rezervirovanie

*/
	unsigned char R0;
	
	R0=RomReceive[6][1];
//	if(!(RomReceive[1][0] & 6))
	NumberRegim=R0 & 0x7;
//	else
//	NumberRegim=1;
	}


	void	OnParalelGen(void)
	{
	unsigned int R0;
	unsigned int R1;
	unsigned int R2;
	if(RomReceive[2][1] & 4)//KSet2=on
	RegTransmit[4][1] |=4;//off set2
	if(!CtChangeUNom)
		{

	CtChangeUNom=20;
		}
	if(RomReceive[1][1] & 4)//KSet=on
			{
	R2=DeltaFDop;

			}
	else
			{

	R2=DeltaFDop+150;
			}


	R0=abs(UGen-ULine);

	R1=abs(Period[0]>=Period[2]);		



	if(R0<=DeltaUDop)
		{
	if(R1<=R2)
			{
	if((ULineGenOld<=DeltaULineDop+20)&&(ULineGen<=DeltaULineDop))
				{

	StatusKontaktorGen |=0x10;
	if(RomReceive[1][1] & 4)//KSet=on
	StatusKontaktorSet |=0xa0;

	ChangeFNom=0;
				}
			}
		}
	}														
		
	void	OnParalelSet(void)
	{
	unsigned int R0;
	unsigned int R1;
	if(!CtChangeUNom)
		{

	CtChangeUNom=20;
		}

	R0=abs(USet>=ULine);
	
	R1=abs(Period[1]>=Period[2]);


	if(R0<=DeltaUDop)
		{

	if(R1<=DeltaFDopSet)
			{
	if((ULineSetOld<=(DeltaULineDop+20))&&(ULineSet<=DeltaULineDop))
				{

	StatusKontaktorSet |=0x10;
	StatusKontaktorGen |=0x20;
	ChangeFNom=0;
				}
			}
		}												
	}	
 /*  void InitComparator   (void)
    {
//    ACSR=ACSR & 0xf7;
//
//  ACSR &= 0x30;
//    ACSR &=0xfc;
//	ACSR=0;

    ACSR |=0Xa;//1 > 0
 //   ACSR |=8;
    }*/


	void	AvtoRegim(void)


{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	R0=RomReceive[1][1];
	R2=RomReceive[2][1];
	R1=RegTransmit[4][1];

	if(!(R0 & 4))//KSet=off
	StatusKontaktorSet &=0xdf;
	
	if(!(RomReceive[2][1] & 4))//KSet2=off
	R1 &=0xfb;//bit off for Set2=Off

	if(!(R0 & 1))//KG denable
		{
	R1 &=0xef;//stop DG=off
	ChangeFNom=0;
		}
	if(!(R0 & 4))//KSet=off
				{

	if(R0 & 0x2)//Gen=on
					{

	if(!(StatusKontaktorGen & 1))
						{
	StatusKontaktorGen &=0xef;

						}
	else	if(RomReceive[2][1] & 2)//KG_Far=on
						{
	if(StatusKontaktorSet & 1)//
							{

	if(RomReceive[4][1] & 2)//SHU2
								{	
	StatusKontaktorGen &=0xee;
	R1 |=0x10;//Stop DG
								}
	else
	R1 &=0xef;
							}
	else
	R1 &=0xef;


						}
	else
						{
	R1 &=0xef;
	if(StatusKontaktorSet & 1)
									
	OnParalelSet();

						}
					}
	else	if(!(RomReceive[2][1] & 2))//KG1=off,KG2=off,KSet2=off
					{


	if(StatusKontaktorSet & 1)
						{							
	StatusKontaktorSet |=0x10;

						}
	else if((StatusKontaktorGen & 1)&&(!(RomReceive[2][1] & 4)))
	StatusKontaktorGen |=0x10;

					}
	else if(RomReceive[2][1] & 2)//KG2=on
					{
	if(StatusKontaktorGen & 1)
						{
	RegTransmit[4][1]=R1;
	OnParalelGen();
	R1=RegTransmit[4][1];
						}
					}


				}



	else
//KSet=on
				{
	if(!(StatusKontaktorSet & 1))
	StatusKontaktorSet &=0xee;
	else
					{
	if(!(R0 & 2))//KG=off
						{

	StatusKontaktorSet &=0xdf;

	if(StatusKontaktorGen & 1)
							{
	RegTransmit[4][1]=R1;
	OnParalelGen();
	R1=RegTransmit[4][1];
							}
						}
	else
						{
//Gen=on
	if(StatusKontaktorGen & 0x20)//after parall
							{

	StatusKontaktorGen &=0xce;
	R1 &=0xfe;//stop
	R1 |=0x10;//stop
							}
	if(StatusKontaktorSet & 0x20)//after parall
	StatusKontaktorSet &=0xee;



						}
					}

				}
	RegTransmit[4][1]=R1;

}
	void	 CRCTransmit(unsigned char R0)
	{

	unsigned char R2;

	R2=RCHTransmit^ R0;//TransmitIrbi[R4][R5][(R0++)];
	RCHTransmit=RCLTransmit^ ProgramPageHigh(R2);
	RCLTransmit=ProgramPageLow(R2);

	}
	
	void	 CRCTransmitGlobal(unsigned char R0)
	{

	unsigned char R2;
	RCHTransmit=0xff;
	RCLTransmit=0xff;

	for(R2=0;R2<=R0;++R2)
	CRCTransmit(RegTransmitUsart[R2]);
	}	
	
		
	
	void	LoadRegTransmitUsart(unsigned char NumberFunction)
	{
	
	RegTransmitUsart[0]=0x1;//Adres	
	switch(NumberFunction)
		{
	case 0:
	RegTransmitUsart[1]=0x6;//NumberFunction	
	RegTransmitUsart[2]=0;//Adres H
	RegTransmitUsart[3]=0;//Adres L
	RegTransmitUsart[4]=UNom>>8;
	RegTransmitUsart[5]=UNom;
	CRCTransmitGlobal(5);
	RegTransmitUsart[6]=RCHTransmit;
	RegTransmitUsart[7]=RCLTransmit;
	TransmitUsart(7);

	break;
	case 1:
	RegTransmitUsart[1]=0x6;//NumberFunction	
	RegTransmitUsart[2]=0;//Adres H
	RegTransmitUsart[3]=1;//Adres L
	RegTransmitUsart[4]=KStatizm>>8;
	RegTransmitUsart[5]=KStatizm;
	CRCTransmitGlobal(5);
	RegTransmitUsart[6]=RCHTransmit;
	RegTransmitUsart[7]=RCLTransmit;
	TransmitUsart(7);
	break;

	case 2:
	RegTransmitUsart[1]=0xf;//NumberFunction	
	RegTransmitUsart[2]=0;//Adres H
	RegTransmitUsart[3]=0x2;//Adres L
	RegTransmitUsart[4]=0;//Number bits H
	RegTransmitUsart[5]=4;//2;//Number bits L
	RegTransmitUsart[6]=1;//Number byts 
	RegTransmitUsart[7]=StatizmVozb;//Vozb=on,Statizm=on 
	CRCTransmitGlobal(7);
	RegTransmitUsart[8]=RCHTransmit;
	RegTransmitUsart[9]=RCLTransmit;
	TransmitUsart(9);


	break;


	default:break;
		}
	


	}
	void AccountCurrentLC(void)
	{
	unsigned int R0;
	unsigned int R1;
	R0=AdResult[5];
	R1=DeltaUITest>>8;
	if((R1<110)||(R0<100)||(R1>170))
	CtError[11]=CtError0[11];
//	RegTransmit[7][1]=R1;

	}



	void LoadFromEeprom(void)
	{

	UNomConst=ReadEepromWord(4);
	if(UNomConst>250)
	UNomConst=230;
	UNom=UNomConst;
	KoefU=ReadEepromWord(6);
	if((KoefU<235)||(KoefU>270))	
	KoefU=256;
	}	
																																		 												   
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {


	DDRA=0;
	PORTA=0;
 
    DDRB=0x60;
	PORTB = 0xf3;

    DDRC=0xfc;
   PORTC =0xff;

    DDRD=0xb2;
    PORTD=0xff;

//INIT TWI;
    TWBR=0x7f;//F TWI

//Init capture1
    TCCR1B=0x82;//0.5mkc,falling edge
//    TIMSK=TIMSK | 0x24;//enable Int overlowT1
 //   TIMSK=TIMSK | 0x20;//enable Int capture1 

    InitAd();

 //   InitComparator();
    ACSR =0Xa;//1 > 0
    MCUCR=0xcb;
//    MCUCR=MCUCR | 0xc3;/* Log1>0 Int1*/
 //   MCUCR=MCUCR | 0x3;/* Log1>0 Int0*/
 
    GICR |=(1<<INT1);/*Enable INT1*/
    GICR |=(1<<INT0);/*Enable INT0*/

	TCCR0=5;// /1024
 //	TIMSK |=0x1;//enable int overlow T0
//	ASSR=0;

	TCCR2=2;// T=0,5 mkc
 	TIMSK |=0x64;//enable int overlow T2,T1
	ASSR=0;

    ResetError();
	LoadFromEeprom();






	NumberLink=1;
//	UNom=220;
	RegSWork=0;

//	StatizmVozb=0xa;//avton,vozb=off



	ChangeFNom=0;

	RegTransmit[4][1]=0;

	InitTWI();
    _SEI();
//	RegimTime=0;
	StatusKontaktorGen=0;
	StatusKontaktorSet=0;

	RegS=0;					             
     while(1)
    {

     _WDR();

	if((!(StatusKontaktorSet & 0x11))&&(!(RomReceive[1][1]& 0x20)))
	UNom=UNomConst;
	if(!CtTransmitUsart)
		{
	if(NumberLinkUsart)
	--NumberLinkUsart;
	else
	NumberLinkUsart=2;
	CtReceiveUsart=0;
	LoadRegTransmitUsart(NumberLinkUsart);
	CtTransmitUsart=15;
	RCHReceive=0xff;
	RCLReceive=0xff;

		}
	ChangeNumberRegim();
	LoadRegInd();
	 if(RomReceive[4][1] & 1)
	 ResetError();
	if(UNom>260)
	UNom=260;
	if(UNom<204)
	UNom=204;
	if(UNomConst>260)
	UNomConst=260;
	if(UNomConst<204)
	UNomConst=204;	
		 
	KStatizm=200;
	if((StatusKontaktorGen & 0x11) && (RomReceive[1][1] & 0x20))//Status KG1,2=on
		{
	StatizmVozb |=1;
	StatizmVozb &=0xfd;
		}
	else
		{
	StatizmVozb |=2;
	StatizmVozb &=0xfe;
		}



	if(CtLinkTWI)
	--CtLinkTWI;
	else
	{
	CtLinkTWI=100;
    if(NumberLink)
    NumberLink=0;
    else
    NumberLink=1;
	_WDR();
	++TestTWI;
    LoadRegTransmit();//++RegTransmit
    ReceiveTransmitMaster();
	}

	if(AdEnd)
		{


	AccountCurrentLC();



	UGen=CalcU(3);//UGen
	USet=CalcU(4);//USet
	ULine=CalcU(5);//ULine
	ULineGenOld=ULineGen;
	ULineGen=CalcDeltaU(0);
	ULineLine=CalcDeltaU(1);
	ULineSet=CalcDeltaU(2);

	ChangeRegimError();
	ChangeRegimClearError();
	SetError();
	ClearError();

	if(!(RomReceive[2][1] & 4))//KSetShu2=off
	RegTransmit[4][1] &=0xfb;
	if(RomReceive[2][1] & 4)//KSetShu2=on
	RegTransmit[4][1] &=0xfd;


	if(NumberRegim !=1)
		{
	ChangeStatusNRSet();//hand SHU
	ChangeStatusNRGen();
	ChangeStatusKontaktorSet();

	ChangeStatusKontaktorGenHand();
	AvtoRegim();
		}

	switch (NumberRegim)
		{
	case 3:
	ChangeUNomHand();
	break;

	case 1:
	RegSWork=0;
	break;

	case 2:ChangeFNom=0;
	ChangeUNomHand();
	break;

	default:break;
		}

	AdEnd=0;
		}
	
    }
  
}



	void MeagerPeriod(unsigned char R2)

	{
	unsigned int R0;
	unsigned int R1;
	unsigned int R3;
	TestPeriod +=(R2+1);
	if(R2==2)
		{
	R1=ICR1;
	FazaU=R1;
		}
	else
		{

    TCCR1B &=0xf8;//0.5mkc
	R1=TCNT1;
    TCCR1B |=0x2;//0.5mkc
	if(R2==3)
			{
	FazaI=R1;

	R3=FazaI-FazaU;
	DeltaUITest=R3;


			}
		}



	R0=R1-TNT1Old[R2];
	if(!CtOverLow[R2])
	Period[R2]=0xffff;
	else if((CtOverLow[R2]==1) && (R1>=TNT1Old[R2]))
	Period[R2]=0xffff;
	else
	Period[R2]=R0;
	TNT1Old[R2]=R1;
	CtGate[R2]=60;
	CtOverLow[R2]=2;
	}

 SIGNAL(SIG_ADC)
	{
    unsigned int RegInt0;
    unsigned char R0;
	
	
		
    RegInt0=ADC;
	R0=NumberAd;


	if(CtAd)
		{
	--CtAd;
	if(CtAd<=CtAd0)
			{ 
			
	if(R0==6)
				{
	if(RegInt0>AdTemp[6])
	AdTemp[6]=RegInt0;
	if(RegInt0<AdTempMin)
	AdTempMin=RegInt0;	
									 	
				}
	else
	AdTemp[R0] +=RegInt0;
			}
		}
	else
		 {
	 ChangeNumberAd=1;
	 if(R0==6)
	 AdResult[6]=AdTemp[6]-AdTempMin;

	else
	 AdResult[R0]=AdTemp[R0]>>7;
	 AdTemp[R0]=0;
	 AdTempMin=0xffff;
	 CtAd=CtAd0+2;

	if(NumberAd)
	--NumberAd;
	else
	NumberAd=6;
	AdEnd=1;
		 }

	if(ChangeNumberAd)
		 {
	 R0=ADMUX;
	 R0 &= 0xe0;
	 R0 |=NumberAd;
	 ADMUX=R0;
	 ChangeNumberAd=0;
		 }
	 ADCSRA |=(1<<ADSC);
  	
	}

	 SIGNAL(SIG_INPUT_CAPTURE1)
	{
	MeagerPeriod(2);
    TIFR=TIFR | 0x20; 
    TIMSK=TIMSK & 0xdf;//denable Int capture1 
    ACSR |=8;//Enable Int comp
	}


	 SIGNAL(SIG_OVERFLOW1)// 32 Ms
	{
	unsigned char R0;

	if(CtTransmitUsart)
	--CtTransmitUsart;
	if(CtNoReceiveUsart)
	--CtNoReceiveUsart;
	else
		{

	RCHReceive=0xff;
	RCLReceive=0xff;
	CtNoReceiveUsart=19;
		}

	if(CtKn)
	--CtKn;

	if((!NumberRegim)||(NumberRegim==5))
		{
	if((!(RomReceive[5][1] & 0x10))||((StatusKontaktorSet & 0x20))||(Error & 0x40))

			{
	if(CtSetOff)
	--CtSetOff;
			}
	else
	CtSetOff=2;
		}
	else 
		{	
	if(!(StatusKontaktorSet & 0x10))
			{
	if(CtSetOff)
	--CtSetOff;
			}

	else
	CtSetOff=2;
		}
	if(CtChangeUNom)
	--CtChangeUNom;

	if(CtEnableKn)
	--CtEnableKn;
	else
		{
	CtEnableKn=3;
	EnableReadKn=1;
		}
	switch(CtEnableKn)
		{

	case 0:ReadKnPB5();

	break;
	case 1:	PORTB &=0xdf;
	PORTB |=0x40;
	break;
	case 2:ReadKnPB6();


	break;
	case 3:	PORTB &=0xbf;
	PORTB |=0x20;

	break;
	default :CtEnableKn=3;
	break;
		}

	for(R0=0;R0<=3;++R0)
	{
	if(CtOverLow[R0])
	--CtOverLow[R0];
	else
	Period[R0]=0xffff;
   if(RegimClearError & (1<<R0))
        {
    if(CtClearError[R0])
    --CtClearError[R0];
        }
    else
    CtClearError[R0]=CtClearError0;

	}

    for(R0=0;R0<=7;++R0)
    {
    if(RegimError & (1<<R0))
        {
    if(CtError[R0])
    --CtError[R0];
        }
    else
    CtError[R0]=CtError0[R0];

    }


    for(R0=8;R0<=11;++R0)
    {
    if(CtError[R0])
    --CtError[R0];
    }


    if(CtTime)
    --CtTime;



}

	SIGNAL(SIG_INTERRUPT1)
	{
	MeagerPeriod(0);
	GIFR |=0x80;
    GICR = GICR & 0x7f;//Denable INT1
	GIFR |=0x80;
	} 
	  
	SIGNAL(SIG_INTERRUPT0)
	{
	MeagerPeriod(1);
	GIFR |=0x40;
    GICR = GICR & 0xbf;//Denable INT0

	}



	 SIGNAL(SIG_OVERFLOW2)//128mks
	{
	if(CtEnableEeprom)
	--CtEnableEeprom;


//	if(CtErrorLink[0])
//	--CtErrorLink[0];
//	else
//	InitTWI();
	if(CtErrorLink[1])
	--CtErrorLink[1];
	else
	InitTWI();


	if(CtGate[0])
		{
	--CtGate[0];

	if(CtGate[0]==1)
			{
	GIFR |=0x80;
    GICR = GICR | 0x80;//enable INT1

			}
		}


	if(CtGate[1])
		{
	--CtGate[1];

	if(CtGate[1]==1)
			{
	GIFR |=0x40;
    GICR = GICR | 0x40;//enable INT0
			}
		}


	if(CtGate[2])
		{
	--CtGate[2];
	if(CtGate[2]==1)
			{

    TIMSK=TIMSK | 0x20;//enable Int capture1 
			}
		}
	if(CtGate[3])
		{
	--CtGate[3];
	if(CtGate[3]==1)
			{
    ACSR |=8;//Enable Int comp

			}

		}

	}

	SIGNAL(SIG_UART_RECV)
	{
	ReceiveUsart();
	}

    SIGNAL(SIG_COMPARATOR)
    {
	MeagerPeriod(3);

    ACSR &=0xf7;//denable Int comp

    } 

