 /*œ«¿ A“Ã≈À*/   
/* WDTON CKOPT
BOODLEVEL BODEN SUT1 CKSEL2*/    
    //Int comparator=on 
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/signal.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>
	unsigned    char    CtStabI;
	unsigned    char    RInt0;
	unsigned    char    RInt1;
    unsigned    int     RegInt0;
    unsigned    int     AdTest;
    unsigned    int     AdTestI;

	unsigned    char    Test;
	unsigned    char    CtIntens;/*30sec*/
	const unsigned    char	    CtIntens0=30;
    unsigned    int    RegWait;
    unsigned    char    SregTemp;   
    unsigned    int     Reg3;
	unsigned    char    CtInd;
	unsigned    char    IndData[6];
    unsigned    int     RegInd;
    const unsigned  char Table[0Xa]={0X7e,0X30,0X6d,0X79,0X33,
                                      0X5b,0X5f,0X70,0X7f,0X7b};

	unsigned    char	NumberAd;
    unsigned    char	CtAd;
	const unsigned    char	    CtAd0=40;
	unsigned    int	AdTemp;
 	unsigned    int	AdResult[31];

    unsigned    char	MyStatus;

	unsigned    char	RegS,RegSTemp,RegSOld,IndTest,CtRegimTest;
 	const  unsigned  char	    CtRegimTest0=20;
 
 	unsigned    int	            Alfa;
 
    const    unsigned char       AdrAlfaL=0x14;
    const    unsigned char       AdrAlfaH=0x15;
    const    unsigned char       AdrTL=6;
    const    unsigned char       AdrTH=7;
    const    unsigned int       AlfaMin=230;//270;//230;//210

    unsigned    char             KoefI;
    const    unsigned char       KoefIMax=250;
    const    unsigned char       KoefIMin=10;
    unsigned    char             DeltaI;
    const    unsigned char       DeltaIMax=100;
    const    unsigned char       DeltaIMin=1;
    unsigned int                 IAB;
    const    unsigned char       AdrKoefI=4;
    const    unsigned char       AdrDeltaI=1;
    const    unsigned char       AdrINomL=0xa;
    const    unsigned char       AdrINomH=0xb;
    unsigned int                 INom;
    const    unsigned int        INomMax=400;
    const    unsigned int        INomMin=50;
    unsigned int                 INomWork;
   const    unsigned int        U2_5=23000;/*26500;*/

    unsigned    char             KoefU;
    const    unsigned char       KoefUMax=240;
    const    unsigned char       KoefUMin=100;
    unsigned int                 UAB;
    const    unsigned char       AdrKoefU=2;
    const    unsigned char       AdrUNomL=8;
    const    unsigned char       AdrUNomH=9;
    unsigned int                 UNomWork;
    unsigned    int              UNom;
    const    unsigned int        UNomMax=285;
    const    unsigned int        UNomMin=240;
    const    unsigned int        U28_5=285;
    unsigned    char             DeltaU;
    const    unsigned char       DeltaUMax=130;
    const    unsigned char       DeltaUMin=1;
   const    unsigned char       AdrDeltaU=3;

 
    const    unsigned int        CtMig0=7812;
    unsigned    int              CtMig;

 
    unsigned    long             CtMoto;
    unsigned    int              CtMotoLow;
    static const    unsigned int        CtMotoLow0=359;/*360s; */
    static const    unsigned char       AdrMotoL=0x10;
    static const    unsigned char       AdrMotoH=0x11;
    unsigned    char             AdrMotoHH=0x12;

	unsigned    char	         Regim;
    unsigned    char             RegimWork;
    unsigned    char             RegimWorkOld;

    unsigned    char             CtChange;
    const    unsigned char       CtChange0=10;
 
    unsigned    int              AH;

    unsigned    char             CtSet;
    unsigned    int              CtSetOn;
    const    unsigned char       CtSet0=156;
    const  unsigned   char       CtSetOn0=50;
    unsigned    char             Set;
    unsigned    char             CtOff;
    const    unsigned char       CtOff0=40;/* 4 our*/


    unsigned    int              Temperatura;
    const    unsigned int        Temperatura85=85;//5630;//6500;
    const    unsigned int        Temperatura80=90;//6450;//7450;



    unsigned    int              CtUst;
    const    unsigned int        CtUst0=1000;
    unsigned    int              CtVT3;
    const    unsigned int        CtVT30=5000;

    unsigned    int             Avaria;
    unsigned    int             IAB0,IAB1,IAB2,IAB3;

    unsigned    char             CtReg;
    unsigned    int             RegInt2;

    unsigned    int             CtDif;
    unsigned    char             RegimInd;
    unsigned    char             RegimStab;
    unsigned    int             CtInom;
    const   unsigned    int     CtInom0=200;
 
 


/*============================================*/

void Indicator_1( unsigned char Arg0, unsigned char Arg1)
{
	PORTB &= 0xef;/*CS*/
	SPDR=Arg0 | 0xf0;

	while(!(SPSR & (1<<SPIF)));

	SPDR=Arg1;

	while(!(SPSR & (1<<SPIF)));

	PORTB |= 0x10;/*CS*/
}


void ReadKn(void)
{

unsigned    char Reg2;
Reg2=0;
if(bit_is_clear(PINB,PB3))
Reg2=Reg2 | 1;

if(bit_is_clear(PINC,PC1))
Reg2=Reg2 | 4;

if(bit_is_clear(PINC,PC2))
Reg2=Reg2 | 2;

if(bit_is_clear(PINC,PC4))
Reg2=Reg2 | 8;

if(bit_is_clear(PINC,PC6))
Reg2=Reg2 | 0x10;

if(bit_is_clear(PIND,PD2))
Reg2=Reg2 | 0x20;

if(bit_is_clear(PIND,PD3))
Reg2=Reg2 | 0x40;

if(bit_is_clear(PIND,PD1))
Reg2=Reg2 | 0x80;

if(Reg2==RegSTemp)
RegS=Reg2;

RegSTemp=Reg2;

if((RegS & 6)||(Avaria))
CtIntens=CtIntens0;

if(CtIntens)
Indicator_1(0xa,0x6);/*Intensity Max*/
else
Indicator_1(0xa,0x2);/*Intensity Min*/
}



void InitInd(void)
{   

Indicator_1(0xf,0);/*Intensity Max*/
Indicator_1(9,0);/*Normal work*/
Indicator_1(0xc,1);/*Normal work*/
Indicator_1(0xb,7);/*Scan 8*/
Indicator_1(0xa,0x2);/*Intensity Max*/

}

void		DecSegm( unsigned int Arg0)
{
unsigned int	Arg1;

if(Arg0>9999)
Arg0=9999;

Arg1=0;

while(Arg0>=1000)
{
Arg0=Arg0-1000;
++Arg1;
}

IndData[3]=Table[Arg1];
Arg1=0;

while(Arg0>=100)
{
Arg0=Arg0-100;
++Arg1;
}

IndData[2]=Table[Arg1];
Arg1=0;

while(Arg0>=10)
{
Arg0=Arg0-10;
++Arg1;
}

IndData[1]=Table[Arg1];
IndData[0]=Table[Arg0];	
}
void		DecSegmLong( unsigned long Arg0)
{
unsigned int	Arg1,Arg2;
if(Arg0>999999)
Arg0=999999;

Arg1=0;

while(Arg0>=100000)
{
Arg0=Arg0-100000;
++Arg1;
}

IndData[5]=Table[Arg1];
Arg1=0;

while(Arg0>=10000)
{
Arg0=Arg0-10000;
++Arg1;
}

IndData[4]=Table[Arg1];
Arg2=Arg0;
DecSegm(Arg2);
}

void    Indicator2( unsigned char ArgAdr, unsigned char Arg00,
unsigned char Arg01, unsigned char Arg02, unsigned char Arg03)
{

Indicator_1(ArgAdr,Arg00);
++ArgAdr;
Indicator_1(ArgAdr,Arg01);
++ArgAdr;
Indicator_1(ArgAdr,Arg02);
++ArgAdr;
Indicator_1(ArgAdr,Arg03);
}       


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
void    WriteEepromLong(unsigned char  Arg4,unsigned long Arg8)

{
unsigned char    Arg7;
Arg7=Arg8;
WriteEeprom(Arg4,Arg7);
Arg8=Arg8>>8;
Arg7=Arg8;
WriteEeprom((Arg4+1),Arg7);
Arg8=Arg8>>8;
Arg7=Arg8;
WriteEeprom((Arg4+2),Arg7);
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

unsigned long    ReadEepromLong(unsigned char Arg6)

{
unsigned long Arg8;
unsigned int   Arg9;
Arg8=ReadEeprom(Arg6+2);		 	
Arg8=Arg8<<16;
Arg9=ReadEeprom(Arg6+1);		 	
Arg9=Arg9<<8;
Arg8=Arg8 | Arg9;
Arg8=Arg8 | ReadEeprom(Arg6);      	
return Arg8;
}

void    ChangeRegim(void)
{

if(RegS != RegSOld)
{
if(RegS & 4)
++Regim;

RegSOld=RegS;
}

if(Regim>11)
Regim=0;

}

void    CalcUAB(void)
{
unsigned  int Reg0,Reg1;
Reg0=AdResult[2]>>7;

Reg1=Reg0;
Reg1=Reg1*KoefU;
Reg1=Reg1>>8;

Reg0=Reg0+Reg1;
if(Reg0>999)
Reg0=999; 
UAB=Reg0;

}

void    CalcIAB(void)
{
unsigned    int Reg0,Reg1;
Reg0=AdResult[0]>>8;

Reg1=Reg0;
Reg0=Reg0+(Reg0>>1);
Reg1=Reg1*KoefI;
Reg1=Reg1>>8;

Reg0=Reg0+Reg1;
if(Reg0>999)
Reg0=999;
IAB=Reg0;

}







unsigned int MaxMin(unsigned int A00,unsigned int A01,
	unsigned int A02)
{
if(A00>A01)
A00=A01;

else if(A00<A02)
A00=A02;

return A00;
}
void    StabI(void)
{
int unsigned Reg1000;
if(IAB>(INomWork+3))
{
Reg1000=IAB-INomWork;

//    if(Reg1000>32)
//    Reg1000=Reg1000>>4;
//    else
Reg1000=1;             

Alfa=Alfa+Reg1000;
}
else if(IAB<(INomWork-2))
{
Reg1000=INomWork-IAB;
//    if(Reg1000>32)
//    Reg1000=Reg1000>>4;
//    else
Reg1000=1;
if(Alfa>Reg1000)
Alfa=Alfa-Reg1000;
else
Alfa=AlfaMin;
}
Alfa=MaxMin(Alfa,ICR1,AlfaMin);
}


void    StabU(void)
{
int unsigned Reg1000;
if(UAB>(UNomWork+2))
{
Reg1000=UAB-UNomWork;
/*   if(IAB<20)
Reg1000=1;
else */if(Reg1000>=16)
Reg1000=Reg1000>>3;
else
Reg1000=1;            
Alfa=Alfa+Reg1000;
}
else if(UAB<(UNomWork-2))
{
Reg1000=UNomWork-UAB;
/*   if(IAB<20)
Reg1000=1;
else*/ if(Reg1000>=16)
Reg1000=Reg1000>>3;
else
Reg1000=1;
if(Alfa>=Reg1000)
Alfa=Alfa-Reg1000;
else
Alfa=AlfaMin;

}
if(IAB<30)
Alfa=MaxMin(Alfa,ICR1,280);//280
else
Alfa=MaxMin(Alfa,ICR1,AlfaMin);

}
void    AccountAlfa0(void)
{
if(RegimStab==1)/*Stab I*/
{
if(UAB>(UNomWork+3))
RegimStab=0;/* Stab U*/
}
else if(IAB>(INomWork+4))
RegimStab=1;

if(RegimStab==1)

StabI();

else
StabU();

if(RegimStab==1)
CtChange=CtChange0;

}

void    AccountAlfa1(void)
{
if(RegimStab==1)/*Stab I*/
{
if(UAB>(UNomWork+3))
RegimStab=0;
}
else if(IAB>(INomWork+4))
RegimStab=1;
if(RegimStab==1)

StabI();




else
StabU();

if(IAB>INomWork/4)
CtChange=CtChange0;

}

void InitComparator   (void)
{
ACSR=ACSR & 0xf7;/*Denable Int Comp*/
SFIOR &= 0xf7;
ACSR &= 0x30;
ACSR &=0xfc;/*1 > 0*/
ACSR |=3;
ACSR |=8;/*Enable Int*/
}

void InitAd   (void)
{
ADMUX=0xc0;
CtAd=CtAd0;
NumberAd=0;
MyStatus=0;
ADCSRA=0;
ADCSRA |=(1<<ADEN);/*enable AD*/
ADCSRA |=(1<<ADPS2);
ADCSRA |=(1<<ADPS1);
ADCSRA |=(1<<ADATE);//Auto trigger enable
ADCSRA |=(1<<ADPS0); /*128*/	 		
ADCSRA |=(1<<ADIE);/* enable interrupt*/
ADCSRA |=(1<<ADSC);/* Start*/
}





void LoadFromEeprom(void) 
{


CtMoto=ReadEepromLong(AdrMotoL);

KoefI=ReadEeprom(AdrKoefI);
KoefI=MaxMin(KoefI,KoefIMax,KoefIMin);

DeltaI=ReadEeprom(AdrDeltaI);
DeltaI=MaxMin(DeltaI,DeltaIMax,DeltaIMin);

KoefU=ReadEeprom(AdrKoefU);
KoefU=MaxMin(KoefU,KoefUMax,KoefUMin);

DeltaU=ReadEeprom(AdrDeltaU);
DeltaU=MaxMin(DeltaU,DeltaUMax,DeltaUMin);

ICR1=400;

Alfa=ReadEepromWord(AdrAlfaL);

Alfa=MaxMin(Alfa,ICR1,AlfaMin);

INom=ReadEepromWord(AdrINomL);
INom=MaxMin(INom,INomMax,INomMin);


UNom=ReadEepromWord(AdrUNomL);
UNom=MaxMin(UNom,UNomMax,UNomMin);
UNomWork=UNom;

}
unsigned   int    ChangeUst(int unsigned Par)   
{
if(!CtUst)
{
if(RegS & 2)
++Par;

else   if(RegS & 4)
--Par;

CtUst=CtUst0;
}
return Par;    
}

void    ChangeUNom(void)   
{
unsigned    int Reg1;
UNom=ChangeUst(UNom);      
UNom=MaxMin(UNom,UNomMax,UNomMin); 
Reg1=ReadEepromWord(AdrUNomL);

if(Reg1!=UNom)
WriteEepromWord(AdrUNomL,UNom);
}

void    ChangeINom(void)
{
unsigned     int    Reg1;
INom=ChangeUst(INom);
INom=MaxMin(INom,INomMax,INomMin);                
Reg1=ReadEepromWord(AdrINomL);

if(INom != Reg1)
WriteEepromWord(AdrINomL,INom);
}

void    ChangeKoefUWork(void)
{
unsigned     char   Reg2;
KoefU=ChangeUst(KoefU);
KoefU=MaxMin(KoefU,KoefUMax,KoefUMin);
Reg2=ReadEeprom(AdrKoefU);

if(Reg2!=KoefU)
WriteEeprom(AdrKoefU,KoefU);

}

void    ChangeKoefIWork(void)

{
unsigned    char Reg2;
KoefI=ChangeUst(KoefI);
KoefI=MaxMin(KoefI,KoefIMax,KoefIMin);
Reg2=ReadEeprom(AdrKoefI);

if(Reg2!=KoefI)
WriteEeprom(AdrKoefI,KoefI);

}   


void    IndicatorAvaria(void)
{
if(Avaria & 5)/*IMax*/
{
DecSegm(UAB);
Indicator2(1,0x3e,IndData[2],(IndData[1] | 0x80),IndData[0]);
Indicator2(5,0x77,0,0,0);
}



else    if(Avaria & 0x42)/* UMax*/
{
DecSegm(IAB);
Indicator2(5,0x77,IndData[2],(IndData[1] | 0x80),IndData[0]);
Indicator2(1,0x3e,0,0,0);
}

else    if(Avaria & 0xa0)/* U<UNom*/
{
DecSegm(UAB);
Indicator2(1,0x3e,IndData[2],(IndData[1] | 0x80),IndData[0]);
DecSegm(IAB);

if(RegimInd)
Indicator2(5,0x77,IndData[2],(IndData[1] | 0x80),IndData[0]);
else
Indicator2(5,0,IndData[2],(IndData[1] | 0x80),IndData[0]); 
}
else    if(Avaria & 0x100)/* U=0,I=0*/
{

DecSegm(IAB);
Indicator2(5,0x77,IndData[2],(IndData[1] | 0x80),IndData[0]);
DecSegm(UAB);
if(RegimInd)
Indicator2(1,0x3e,IndData[2],(IndData[1] | 0x80),IndData[0]);
else
Indicator2(1,0,IndData[2],(IndData[1] | 0x80),IndData[0]); 
}



else    if(Avaria & 0x10)/* T1*/
{
Indicator2(5,0x77,0,0,0);
Indicator2(1,0x3e,0,0,0);
}


}
void	AccountTemper(void)
{
unsigned long R0;
unsigned long R1;
R0=AdResult[3]>>6;
R1=AdResult[30]>>6;
R0=R0*R1;
R0=R0/156;
Temperatura=R0;
} 
void   TestProgram(void)
{
unsigned  int       Reg00;
unsigned    char    Reg2;    
_WDR();

if(MyStatus & 1)
{
CalcUAB();
CalcIAB();
AccountTemper();
MyStatus &=0xfe;
}

if(UAB<(UNomWork+5))
Avaria &=0x1bd;/*Avaria UAB=off*/


if(Avaria & 0x47)
{
Alfa=ICR1;

}


OCR1A=Alfa;

if(!RegInd)
{
InitInd();
RegInd=20000;
}

ReadKn();
ChangeRegim();
RegWait=5;

while(RegWait--)
_WDR();

/*Change F*/
switch(Regim)
{
case 0:Indicator2(1,0x77,0x3e,0x77,0);
break;

case 1:
Indicator2(1,0x77,0xe,8,0);
break;

case 2:
Indicator2(1,0x77,0xe,0x40,0);
break;



case 3:
Indicator2(1,0x3e,0x40,0,0);
break;

case 4:
Indicator2(1,0x3e,8,0,0);
break;

case 5:
Indicator2(1,0x77,0x40,0,0);
break;

case 6:
Indicator2(1,0x77,8,0,0);
break;



case 7:
Indicator2(1,0x77,0x7e,8,0);
break;

case 8:
Indicator2(1,0x77,0x7e,0x40,0);
break;
case 9:
Indicator2(1,0x3e,0x7e,8,0);
break;

case 10:
Indicator2(1,0x3e,0x7e,0x40,0);
break;
case 11:
Indicator2(1,0xf,0,0,0);
break;       
default: break;
}
if(RegS & 2)
{
if(!CtUst)
{
switch(Regim)
{

case 1:++Alfa;
break;

case 2:--Alfa;
break;



case 3:++KoefU;
break;

case 4:--KoefU;
break;

case 5:++KoefI;
break;

case 6:--KoefI;
break;



case 7:++DeltaI;
break;

case 8:--DeltaI;
break;

case 9:++DeltaU;
break;

case 10:--DeltaU;
break;    
default: break;
}
CtUst=CtUst0;
}
}
ICR1=400;

if(!RegInd)
{
InitInd();
RegInd=20000;
}

if(!Regim)

Reg00=Avaria;

else    if(Regim<=2)
{

Reg00= Alfa;
Reg00=ICR1-Reg00;
Reg00=Reg00*10;
Reg00=Reg00>>3;
}



else   if(Regim<=4)
Reg00=UAB;

else   if(Regim<=6)
Reg00=IAB;
else  if(Regim<=8)
Reg00=IAB;
else  if(Regim<=10)
Reg00=UAB;
else  if(Regim==11)
Reg00=Temperatura;//>>8;
else
Reg00=0;
DecSegm(Reg00);
Indicator2(5,IndData[3],IndData[2],IndData[1],IndData[0]);         
Reg2=ReadEeprom(AdrKoefU);

if(Reg2!=KoefU)
WriteEeprom(AdrKoefU,KoefU);

Reg2=ReadEeprom(AdrKoefI);

if(Reg2!=KoefI)
WriteEeprom(AdrKoefI,KoefI);

Reg2=ReadEeprom(AdrDeltaI);
if(Reg2!=DeltaI)
WriteEeprom(AdrDeltaI,DeltaI);

Reg2=ReadEeprom(AdrDeltaU);
if(Reg2!=DeltaU)
WriteEeprom(AdrDeltaU,DeltaU);         

Reg00= ReadEepromWord(AdrAlfaL);

if(Alfa != Reg00)
WriteEepromWord(AdrAlfaL,Alfa); 


}    

void    IndicatorWork(void)
{
if(!CtInd)
{
if(RegimInd)
RegimInd=0;
else
RegimInd=1;                   
CtInd=50;
if(Avaria & 8)
{
if(RegimInd)
PORTD=PORTD | 1;
else
PORTD=PORTD & 0xfe;
}

if((RegS & 2)&&(RegS & 4))
{
if(RegimWork<=2) 
{
DecSegm(AH/10);
Indicator2(1,0x77,1,0x33,1);
}
else
{
DecSegmLong(Temperatura);
Indicator2(1,0x37,1,IndData[5],IndData[4]);
}
Indicator2(5,IndData[3],IndData[2],(IndData[1]),IndData[0]); 
}


else    if((RegS & 4)&& ((Regim<20)||(Regim>23)))
{
DecSegm(UNomWork);

if(RegimInd)
IndData[1] |= 0x80;

Indicator2(1,0x3e,IndData[2],IndData[1],IndData[0]);
DecSegm(IAB);
Indicator2(5,0x77,IndData[2],(IndData[1] | 0x80),IndData[0]);
}


else if((RegS & 2)&& ((Regim<20)||(Regim>23)))
{
DecSegm(INom);
if(RegimInd)
IndData[1] |= 0x80;

Indicator2(5,0x77,IndData[2],IndData[1],IndData[0]);
DecSegm(UAB);
Indicator2(1,0x3e,IndData[2],(IndData[1] | 0x80),IndData[0]);
}

else    if(Avaria & 0x1f7) 

IndicatorAvaria();
else    if(RegimWork==2)
{     
DecSegm(UAB);
Indicator2(1,0x3e,IndData[2],(IndData[1] | 0x80),IndData[0]);
Indicator2(5,0x77,0x40, 0xc0,0x40);
}
else 
{
if(Regim==21)
DecSegm(UNomWork);

else
DecSegm(UAB);

if((Regim != 20) &&(Regim !=22))
Indicator2(1,0x3e,IndData[2],(IndData[1] | 0x80),IndData[0]);

else


Indicator2(1,0,0,0,0);

if(Regim==22)
DecSegm(INom);

else
DecSegm(IAB);

if((Regim!=21)&&(Regim !=23))
Indicator2(5,0x77,IndData[2],(IndData[1] | 0x80),IndData[0]);

else
Indicator2(5,0,0,0,0);


}
CtInd=50;
}

}   




void WriteCtMoto(void)
{
unsigned long Reg0Long;
if(CtMoto>999999)
CtMoto=0;
Reg0Long= ReadEepromLong(AdrMotoL);

if(CtMoto != Reg0Long)
WriteEepromLong(AdrMotoL,CtMoto);
}

/*++++++++++++++++++++++++++++++++++++++++++*/


int main(void)
{

DDRB=0xb0;
PORTB |= 0xfb;// Control UAB

DDRC=0x20;
PORTC |=0xff;/*VT3=off*/

DDRD=0x91;
PORTD=0xff;/*imp=off,Reset=off*/
PORTD=0xef;/*Reset=on*/

/*Interrupt1*/
MCUCR=0;//Urov 0
//   MCUCR=MCUCR | 8;/* Log1>0 Int1*/
//   MCUCR=MCUCR | 2;/* Log1>0 Int0*/

GICR |=(1<<INT1);/*Enable INT1*/
GICR |=(1<<INT0);/*Enable INT0*/

/*Timer2*/
TCCR2=0x2;
TIMSK |=(1<<TOIE2);/*Enable Int Overlow Timer2*/

/*InitCpi*/
SPCR=0x52;
Test=1;
/*   _SEI();*/

InitInd();

InitAd();

InitComparator();


/*ReadEeprom*/
LoadFromEeprom();
IAB0=0;
IAB1=0;
IAB2=0;
CtMig=CtMig0;
CtMotoLow=CtMotoLow0;

TCCR1A=0x82;
TCCR1B=0x019;
OCR1A=Alfa;

AH=0;/* A*H*/
CtSet=CtSet0;
CtSetOn=CtSetOn0;

ReadKn();
RegWait=300;
while(RegWait--)

_WDR();

ReadKn();
RegSOld=RegS;
DDRD=0xb1;
_SEI();

if(RegS & 0x80)/*KT8=0*/
Regim=6;

else    if((RegS & 6)==6)
Regim=20;

else
Regim=0;

RegWait=0;
while(RegS & 6)
/*ChangeNumberRegim*/
{
if(!RegWait)
{
_WDR();
RegWait=1;
_WDR();
switch(Regim)
{

case 20:
Indicator2(1,0x3e,0,0,0);
Indicator2(5,0,0,0,0);
++Regim;
break;


case 21:
Indicator2(5,0x77,0,0,0);
Indicator2(1,0,0,0,0);
++Regim;
break;

case 22:
Indicator2(1,0x3e,0,0x80,0);
Indicator2(5,0,0,0,0);
++Regim;
break;

case 23:
Indicator2(5,0x77,0,0x80,0);
Indicator2(1,0,0,0,0);
Regim=20;
break;

default:Indicator2(1,Table[0],Table[1],Table[2],Table[3]);
Indicator2(5,Table[4],Table[5],Table[6],Table[7]);
break;
}
}

_WDR();
ReadKn();
}


_SEI();
PORTD=PORTD | 0x10;/*Reset=off*/ 
CtStabI=5;          
if(Regim==6)
{
/*Test*/

while(1)
TestProgram();

} 	

else
{
Test=0;
RegimWork=0x40;/* Podzar*/
RegimStab=0;/* Stab U */
RegimWorkOld=0x40;
IAB=0;
UAB=0;
Alfa=ICR1;
OCR1A=Alfa;

CtVT3=CtVT30;
Set=0;
Avaria=0;/*Set=on*/
CtRegimTest=CtRegimTest0;
CtDif=400;
CtChange=CtChange0;          
/*Work program*/     	 
while(1)
{
_WDR();
if(bit_is_clear(RegS,4))
{
RegimWork=0x40;
RegimWorkOld=0x40;
}
else    if(RegimWorkOld==0x40)
{
RegimStab=1;
RegimWork=0;
RegimWorkOld=0;
}

WriteCtMoto();

if(!RegInd)
{
/*Reset Indicator*/
InitInd();
RegInd=20000;
}
ICR1=400;

Alfa=MaxMin(Alfa,ICR1,AlfaMin);



if((Avaria & 0x15f))
{
Alfa=ICR1; 
}


OCR1A=Alfa;

ReadKn();
if((RegS & 6) !=(IndTest & 6))
CtRegimTest=CtRegimTest0;
if(!CtRegimTest)
{
if((Regim>=20)&&(Regim<=23))
Regim=0;
}   
IndTest=RegS;
/*    UNomWork=UNom;*/

if((MyStatus & 1))
{
/*AD=end*/
if(CtStabI)
--CtStabI;
CalcUAB();
CalcIAB();


if(UAB<(UNomWork+5))
Avaria &=0x1bd;//Avaria UMax=off

AccountTemper();
if(Temperatura<Temperatura85)
Avaria=Avaria | 0x10;
else   if(Temperatura>Temperatura80)
Avaria=Avaria & 0x1ef;
if(RegimWork<=2)
{
Avaria=Avaria & 0x17f;
UNomWork=U28_5;
INomWork=INom;
}
if(RegimWork<=1)
{
if((IAB<=10) && (UAB>280))
Avaria=Avaria | 0x100;
}
switch(RegimWork)
{
/*I=const*/
case 0:


Avaria=Avaria & 0x1df;
AccountAlfa0();
if(Avaria & 0x58)
CtChange=CtChange0;
if((!CtChange) &&(!(Avaria & 0x15f)))
{
RegimWork=1;
CtChange=CtChange0;
CtOff=CtOff0;
}
break;

/*U=const*/
case 1:

AccountAlfa1();
if(Avaria & 0x58)
CtChange=CtChange0;
if(!CtChange)
{
RegimWork=2;

}
if(!CtOff)
RegimWork=2;
break;

/*off*/
case 2:Alfa=ICR1;
if(UAB<=20)
Avaria=Avaria | 0x100;
else
Avaria=Avaria & 0xeff;

break;




/*U=Podzar*/
case 0x40:UNomWork=UNom;
AH=0;
if(!(Avaria & 0x20))
{

if(RegimStab==1)
{
Avaria=Avaria | 0x80;
if(UAB<(UNomWork>>1))
{
if(CtInom)
--CtInom;
}
else
CtInom=CtInom0;
if(!CtInom)
{
Avaria=Avaria | 0x20;
CtInom=CtInom0;
}
}
else
Avaria=Avaria & 0x17f;
}
else
{

Reg3=UNomWork*50;
Reg3=Reg3/INom;
if((UAB>Reg3)&&(IAB<INomMin+10))
{
if(CtInom)
--CtInom;
}
else
CtInom=CtInom0;
if(!CtInom)
{
Avaria=Avaria & 0x1df;
CtInom=CtInom0;
}
}

if(Avaria & 0x20)
INomWork=INomMin;

else
INomWork=INom;


AccountAlfa1();
break;
default:break;
}
if(Regim==21)
{
ChangeUNom();
//    Alfa=ICR1;
//  OCR1A=Alfa;
}

else    if(Regim==22)
{
ChangeINom();
//   Alfa=ICR1;
//  OCR1A=Alfa;
}

else    if(Regim==23)
ChangeKoefUWork();

else    if(Regim==20)
ChangeKoefIWork();

/*  else*/
IndicatorWork();


MyStatus=MyStatus & 0xfe;
}




}
}

}





SIGNAL(SIG_OVERFLOW2)/*128mks*/
{


GICR |=(1<<INT1);/*Enable INT1*/
GICR |=(1<<INT0);/*Enable INT0*/
ACSR |=8;/*Enable Int Comparator*/
if(bit_is_set(PINC,3))
{
Avaria=Avaria | 2;/*UMax*/
Alfa=ICR1;

OCR1A=Alfa;
}


if(CtUst)
--CtUst;

if(Set==1)/*Set=norma*/
{
PORTD=PORTD | 1;
if(CtVT3)
--CtVT3;
else
{
PORTC=PORTC & 0xdf;/*VT3=on*/
Avaria=Avaria & 0x1f7;
}
}

else
{
PORTC=PORTC | 0x20;/*VT3=off*/
CtVT3=CtVT30;
if(!Test) 
Avaria=Avaria | 8;
}

/*ControlSet*/
if(CtSet)
{
--CtSet;
if(bit_is_set(RegS,3))
{
if(CtSetOn)
--CtSetOn;
else
Set=1;
}
else
{
if(CtSetOn<(CtSetOn0*2))
++CtSetOn;
else
Set=0;
}
}
else
{
CtSet=CtSet0;
CtSetOn=CtSetOn0;
}

/*    Set=1;prov*/
if(CtMig)
--CtMig;

else
{
CtMig=CtMig0;
if(CtRegimTest)
--CtRegimTest;
if(CtIntens)
--CtIntens;

if(RegWait)
--RegWait;

if(CtChange)
--CtChange;

if(CtMotoLow)
--CtMotoLow;
else
{
CtMotoLow=CtMotoLow0;
++CtMoto;
if(CtOff)
--CtOff;
if(RegimWork<=1)
AH=AH+IAB;
}
}

}
INTERRUPT(SIG_COMPARATOR)
{
Avaria |=0x40;
ACSR &= 0xf7;


Alfa=ICR1;
OCR1A=Alfa;
} 
SIGNAL(SIG_ADC)
{

if(RegInd)
--RegInd;
RegInt0=ADC;


if(!NumberAd)
{

if(RegInt0>=DeltaI)
RegInt0=RegInt0-DeltaI;
else
RegInt0=0;
}
else if(NumberAd==2)
{

if(RegInt0>=DeltaU)
RegInt0=RegInt0-DeltaU;
else
RegInt0=0;
}
if(CtAd)
{
--CtAd;
if(CtAd<=CtAd0)
AdTemp +=RegInt0;
} 	
else

{
AdResult[NumberAd]=AdTemp;

AdTemp=0;




ADCSRA |=(1<<ADSC);
switch(NumberAd)
{
case 0:NumberAd=30;
if(CtInd)
--CtInd;
ADMUX=0x40;
CtAd=255;
MyStatus=MyStatus | 1;
break;
CtAd=CtAd0+2;
case 2:NumberAd=0;
CtAd=CtAd0+2;
break;
case 3:NumberAd=2;
CtAd=CtAd0+2;
break;
case 30:NumberAd=3;
ADMUX=0xc0;
CtAd=255;
break;

default:NumberAd=0;
break;
}
ADMUX = ADMUX & 0xe0;
ADMUX =ADMUX | NumberAd;
} 	
}
SIGNAL(SIG_INTERRUPT1)
{
if((Alfa !=ICR1) &&(!(PIND & 8)))
{
RInt1=100;
if(RInt1)
--RInt1;
else
{

GICR = GICR & 0x7f;
Avaria=Avaria | 1;
Alfa=ICR1;
OCR1A=Alfa;
}

}
}
SIGNAL(SIG_INTERRUPT0)
{
if((Alfa !=ICR1) &&(!(PIND & 4)))
{


GICR = GICR & 0xbf;//Denable INT0
Avaria=Avaria | 4;//IIn>IMax
Alfa=ICR1;

OCR1A=Alfa;

}
}

