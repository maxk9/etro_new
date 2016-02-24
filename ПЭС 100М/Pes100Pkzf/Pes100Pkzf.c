 /*œ«¿ A“Ã≈À*/   
//	Control PB6 for Kalibrovka    
//with Int comparator=off 
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/signal.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>


    unsigned    char            CtKn;
	unsigned int	AdTest;
	const	unsigned char	KoefMax=150;
	const	unsigned char	KoefMin=5;
	const	unsigned char	Koef0=50;

	unsigned char	Koef[6];
	unsigned char	CtInd;	
	unsigned long	AdResultTemp;
	unsigned char	CtAdEnd;
	unsigned int	AdResultEnd[32];
	unsigned int	Current[6];

    unsigned char Regim;
    unsigned int CtStart;

    unsigned char ii;
 	unsigned    int	CtError[8];
 	const unsigned    int CtError0[8]={55000,60,55000,60,
										60,60,60,999};
    const    unsigned int INom100_1_1=367;
    const	unsigned int INom100_1_2=400;
    const	unsigned int INom50_1_1=183;
    const	unsigned int INom50_1_2=200;
    unsigned char RegimError;
    unsigned int IMax100;
    unsigned int IMax50;
	unsigned    long    AdTempLong;
	unsigned    int	AdTempMax;
	unsigned    int	AdTempMin;
 	unsigned    char    Avaria;
    unsigned int CtInitInd;
    unsigned int RegInt0;

	unsigned    int    Reg0;
	unsigned    char   Reg1;
    unsigned    char    SregTemp;   
	unsigned    char    IndData[6];
	unsigned    char	NumberAd;
    unsigned    char	CtAd;

	static const unsigned    char	    CtAd0=129;//3.2msec

 	unsigned    int	AdResult[6];
	unsigned    int	RegS,RegSTemp,RegSOld;

    static const    unsigned int        CtUst0=10;
       
    /*============================================*/
 void ReadKn(void)
{
	unsigned int Reg2 = 0;

	if(bit_is_clear(PIND,PD4))
		Reg2 = Reg2 | 1;
	if(bit_is_clear(PIND,PD5))
		Reg2 = Reg2 | 2;
	if(bit_is_clear(PIND,PD7))
		Reg2 = Reg2 | 4;
	if(bit_is_clear(PIND,PD0))
		Reg2 = Reg2 | 8;
	if(bit_is_clear(PIND,PD1))
		Reg2 = Reg2 | 0x10;
	if(bit_is_clear(PIND,PD6))
		Reg2 = Reg2 | 0x20;
	if(bit_is_clear(PINB,PB0))
		Reg2 = Reg2 | 0x40;
	if(bit_is_clear(PINB,PB1))
		Reg2 = Reg2 | 0x80;
	if(bit_is_clear(PINB,PB2))
		Reg2 = Reg2 | 0x100;
	if(bit_is_clear(PINB,PB3))
		Reg2 = Reg2 | 0x200;
	if(bit_is_clear(PINB,PB6))
		Reg2 = Reg2 | 0x400;

	if(Reg2 == RegSTemp)
	{
		if(CtKn)
			--CtKn;
		else
			RegS = Reg2;
	}
	else
		CtKn = 5;

	RegSTemp = Reg2;
}

 
 
 

void DecSegm( unsigned int Arg0)
    {
     unsigned int	Arg1;

	if(Arg0>9999)
	Arg0=9999;
    Arg1=Arg0/1000;
	IndData[3]=Arg1;
    Arg0=Arg0%1000;
    Arg1=Arg0/100;
	IndData[2]=Arg1;
    Arg0=Arg0%100;
    Arg1=Arg0/10;
	IndData[1]=Arg1;
    Arg0=Arg0%10;
	IndData[0]=Arg0;
    }
	
    void    WriteEeprom(unsigned char Arg4,unsigned char Arg5)
    {
    _WDR();
    while(bit_is_set(EECR,EEWE));
    while(bit_is_set(SPMCR,SPMEN));

	EEARL=Arg4;
	EEARH=0;
	EEDR=Arg5;
    SregTemp=SREG;
    _CLI();
	EECR |=(1<<EEMWE);
                       		 
	EECR |=(1<<EEWE);
    SREG=SregTemp;				
    while(bit_is_set(EECR,EEWE));

    while(bit_is_set(SPMCR,SPMEN));
}



 

    unsigned char    ReadEeprom(unsigned char Arg6)
	
    {		 	
	
	EEARH=0;
    EEARL=Arg6;
	EECR=EECR | (1<<EERE);

	while	(bit_is_set(EECR,EERE));

	return EEDR;
    }
 
    void InitAd (void)
    {
    ADMUX=0xc0;//2.56=Refprov
    SFIOR=SFIOR & 0x1b;//start from end AD
    SFIOR=SFIOR | 0x10;//high speed
 	CtAd=CtAd0;
	NumberAd=0;
	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
    ADCSRA |=(1<<ADPS1);
    ADCSRA |=(1<<ADATE);//Auto trigger enable
//	ADCSRA |=(1<<ADPS0); /*32*/	 		
	ADCSRA |=(1<<ADIE);/* enable interrupt*/
    ADCSRA |=(1<<ADSC);/* Start*/
    }

 

    void IndPar(void)
    {
    unsigned int R0; 
    R0=400;
    while(R0--)         _WDR(); 
    SPDR=IndData[3]+0x30;
    R0=400;
    while(R0--)         _WDR(); 
    SPDR=IndData[2]+0x30;
    R0=400;
    while(R0--)         _WDR(); 
    SPDR=IndData[1]+0x30;

    R0=400;
    while(R0--)         _WDR(); 
    SPDR=IndData[0]+0x30;


    }

   void	SetCursor(unsigned char R2,unsigned char R1)
    {
    unsigned int R0;  
    R0=300;
    while(R0--)         _WDR();
    SPDR=0x1f;
    R0=300;
    while(R0--)         _WDR();
    SPDR=0x24;
    R0=300;
    while(R0--)         _WDR();
    SPDR=R2;
    R0=300;
    while(R0--)         _WDR();
    SPDR=0;
    R0=300;
    while(R0--)         _WDR();
    SPDR=R1;
    R0=300;
    while(R0--)         _WDR();
    SPDR=0;

    }



void TransmitInd(unsigned char R1)
{
    if(R1>=192)
    R1=R1-64;
    while(!(UCSRA & 0x20))
    _WDR();    
    UDR=R1;           
 }

void InitInd(void)
{
	unsigned int R0;
	PORTD &=0xfb;
	R0=3500;
	while(R0--)         _WDR();
	PORTD |=0x4;
	R0=5000;
	while(R0--)         _WDR();
	SPDR=0x1b;
	R0=100;
	while(R0--)         _WDR();
	SPDR=0x74;
	R0=100;
	while(R0--)         _WDR();
	SPDR=17;//Kirilica 
	while(R0--)         _WDR();
	SPDR=0x1f;
	R0=100;
	while(R0--)         _WDR();
	SPDR=0x58;
	R0=100;
	while(R0--)         _WDR();
	SPDR=1;
	R0=100;
	while(R0--)         _WDR();     
	SPDR=0xc;//clear ind
	 
}
void InitIndStart(void)
{
	unsigned int R0;
	PORTD &=0xfb;
	R0=65000;
	while(R0--)         _WDR();
	PORTD |=0x4;
	R0=5000;
	while(R0--)         _WDR();
	SPDR=0x1b;
	R0=100;
	while(R0--)         _WDR();
	SPDR=0x74;
	R0=100;
	while(R0--)         _WDR();
	SPDR=17;//Kirilica 

	R0=100;
	while(R0--)         _WDR();     
	SPDR=0xc;//clear ind
}
   
void CalcIMax50(void)
{
	IMax50=Current[3];
	if(IMax50<Current[4])
		IMax50=Current[4];
	if(IMax50<Current[5])
		IMax50=Current[5];
}   

void CalcIMax100(void)
{
	IMax100=Current[0];
	if(IMax100<Current[1])
		IMax100=Current[1];
		
	if(IMax100<Current[2])
		IMax100=Current[2];
}
	
	                 
void SetRegimError(void)
{
	if(IMax100>INom100_1_2)
		RegimError |= 1;
	else if(IMax100>INom100_1_1)
		{
		RegimError |= 2;
		RegimError &= 0xfe;
		}
		else
			RegimError &= 0xfc;
			
	if(IMax50>INom50_1_2)
		RegimError |= 4;
	else if(IMax50>INom50_1_1)
			{
			RegimError |= 8;
			RegimError &= 0xfb;
			}
		else
			RegimError &= 0xf3;

	if((!(PORTC & 1)) && (!(RegS & 0x4)))//K1
		RegimError |= 0x10;
	else if((PORTC & 1) && (RegS & 4))
			RegimError |= 0x10;
		else 
			RegimError &= 0xef;

	if((!(PORTC & 2)) && (!(RegS & 0x20)))//K2
		RegimError |= 0x20;
	else if((PORTC & 2) && (RegS & 0x20))
			RegimError |= 0x20;
		else 
			RegimError &= 0xdf;

	if(!(RegS & 0x200))//Vdd1
		RegimError |= 0x40;
	else 
		RegimError &= 0xbf;

}	
	
void ResetError(void)
{	
	Avaria=0;
	RegimError=0;
	for(ii=0;ii<=6;++ii)
		CtError[ii]=CtError0[ii];	
}
	
void SetError(void)
{
	unsigned char R0;
	for(R0=0;R0<=6;++R0)
		{
		if(!CtError[R0])
			Avaria |=(1<<R0);
		}
	for(R0=0;R0<=3;++R0)
		{
		if(CtError[R0]==CtError0[R0])
			{
			Avaria = ~Avaria;
			Avaria |=(1<<R0);
			Avaria = ~Avaria;
			}				
		}		
}

void ControlK1(void)//50%
{
	if(Avaria & 0x54)
		PORTC |=1;
	else if(RegS & 2) 		  
		PORTC |=1;
		else if(RegS & 1) 		  
			PORTC &=0xfe;
}

void ControlK2(void)//100%
{
	if(Avaria & 0x61)
		PORTC |=2;
	else if(RegS & 0x10) 		  
		PORTC |=2;
		else if(RegS & 8) 		  
			PORTC &=0xfd;
}	
	
void	SetOut(void)
{
	if(!(Avaria & 0x40))//5 B dd
		PORTC &=0xbf;//NORMA
	else
		PORTC |=0x40;

	if(Avaria & 3)
		PORTC &=0xfb;//Peregruz 100%
	else
		PORTC |=0x4;
		
	if(Avaria & 0xc)
		PORTC &=0xf7;//Peregruz 50%
	else
		PORTC |=8;

	if(Avaria & 0x61)
		PORTC &=0xef;//Avaria 100%
	else
		PORTC |=0x10;
		
	if(Avaria & 0x54)
		PORTC &=0xdf;//Peregruz 50%
	else
		PORTC |=0x20;
		
}
	unsigned int AccountCurrent(unsigned char R0)
	{
	unsigned long R1;
	unsigned long R2;
	unsigned int R3;
	R1=Koef[R0];
	R2=AdResultEnd[R0];
	R1=R1*R2;
	R1=R1>>8;
	R3=AdResultEnd[R0]+R1;
	return R3;
	}
    void LimitKoef(void)
	{
	unsigned char R0;
	for(R0=0;R0<=5;++R0)
		{
	if(Koef[R0]>KoefMax)
	Koef[R0]=KoefMax;
	if(Koef[R0]<KoefMin)
	Koef[R0]=KoefMin;		
		}
	}
    void LimitKoefStart(void)
	{
	unsigned char R0;
	for(R0=0;R0<=5;++R0)
		{
	if(Koef[R0]>KoefMax)
	Koef[R0]=Koef0;
	if(Koef[R0]<KoefMin)
	Koef[R0]=Koef0;		
		}
	}



    void Kalibrovka(void)
{
	unsigned char R0;
	
	if((RegS & 0x89) !=(RegSOld & 0x89))
	{
		if((RegS & 0x80) && (!(RegSOld & 0x80)))
			++CtInd;
			
		if(CtInd>5)
			CtInd=0;

		if((RegS & 8) && (!(RegSOld & 8)))
			++Koef[CtInd];
		else if((RegS & 0x1) && (!(RegSOld & 0x1)))
			--Koef[CtInd];
			
		LimitKoef();
		
		if(Koef[CtInd] != ReadEeprom(CtInd+1))
		{
			R0 = Koef[CtInd];
			WriteEeprom((CtInd+1),R0);
		}		
		RegSOld = RegS;
	}
}							        
/*++++++++++++++++++++++++++++++++++++++++++*/


int main(void)
{

//if DDRX,X=1 PORTX,X=OUT
    DDRA=0xc0;;
	PORTA = 0xc0;

	DDRB=0xb0;

	PORTB = 0xff;

    DDRC=0x7f;
    PORTC =0xff;
    DDRD=0x4;//PD2=OUT

    PORTD=0xff;



    InitAd();


    TCCR1B=0xc2;//0.5mkc
    TIMSK=TIMSK | 0x4;//enable Int overlowT1

    ReadKn();
    CtStart=65000;
	while(CtStart--)	_WDR();
    ReadKn();
	Regim=1;
//	if(!(RegS & 0x2000))
	{
	Regim=0;


//  INIT SPI
    SPCR=0x72;//f/64
    SPSR=0;//f/64

    InitIndStart();
    CtInitInd=100;
	} 
   _SEI();

	ResetError();


	Koef[0]=ReadEeprom(1);
	Koef[1]=ReadEeprom(2);
	Koef[2]=ReadEeprom(3);
	Koef[3]=ReadEeprom(4);
	Koef[4]=ReadEeprom(5);
	Koef[5]=ReadEeprom(6);
	LimitKoefStart();	
	CtKn=5;		
	
	
	
		           
     while(1)
		{
		 _WDR();
		 
		if((RegS & 0x400)&&(RegS & 0x40))//RU	 
			Kalibrovka();
			
		--CtInitInd;

		if(!CtInitInd)
			{
			InitInd();//Reset error Ind
			CtInitInd=500;
			}

		 ReadKn();
		 
		 if(RegS & 0xc0)//RU,Reset Error
			ResetError();
			
		if(!(RegS & 0x40))//RU=off
		{

			ControlK1();
			ControlK2();
			SetOut();
		}
		else
			PORTC |=0x7f;
			
		CalcIMax50();
		CalcIMax100();
		SetRegimError();
		SetError();


		Current[0]=AccountCurrent(0);
		Current[1]=AccountCurrent(1);
		Current[2]=AccountCurrent(2);
		Current[3]=AccountCurrent(3);
		Current[4]=AccountCurrent(4);
		Current[5]=AccountCurrent(5);
		{
		DecSegm(CtInd);
		SetCursor(0,0);
		IndPar();
	/*     DecSegm(Koef[CtInd]);//1.22V
		SetCursor(32,0);
		IndPar();*/

		DecSegm(Current[CtInd]);//0V
		SetCursor(64,0);
		IndPar();
	/*    DecSegm(AdResultEnd[CtInd]);//0V
		SetCursor(0,1);
		IndPar();

		DecSegm(Avaria);//0V
		SetCursor(32,1);
		IndPar();


		DecSegm(RegS);//0V
		SetCursor(64,1);
		IndPar();*/

		}
    }
}

SIGNAL(SIG_ADC)
{
	ADCSRA |=0x10;
    RegInt0=ADC;

    if(CtAd)
	{
	--CtAd;
	if(CtAd<(CtAd0-2))
		{
				
    if(RegInt0>AdTempMax)
    AdTempMax=RegInt0;
    if(RegInt0<AdTempMin)
    AdTempMin=RegInt0;

	    }

    }

    else
	{

	AdResult[NumberAd]=AdTempMax-AdTempMin;


/*	if(AdResult[NumberAd]>=40)
	AdResult[NumberAd] -=40;
	else
	AdResult[NumberAd] =0;*/
//	AdResult[NumberAd]=0;
	 CtAd=CtAd0;
     AdTempMax=0;
     AdTempMin=0xffff;
	if(CtAdEnd)
		{
	--CtAdEnd;
	AdResultTemp +=AdResult[NumberAd];
		}
	else
		{
	AdResultTemp=AdResultTemp>>6;
	AdResultEnd[NumberAd]=AdResultTemp;
	AdResultTemp=0;
	CtAdEnd=40;
		

     switch(NumberAd)
        {
    case 0:NumberAd=6;//6;

    break;
    case 1:NumberAd=0;
    break;
    case 2:NumberAd=1;
    break;
    case 3:NumberAd=2;
    break;
    case 4:NumberAd=3;
    break;
    case 5:NumberAd=4;
    break;
    case 6:NumberAd=5;
    break;
    case 31:NumberAd=6;
    break;
    default:NumberAd=0;
    break;
        }
	 ADCSRA &=0xf7;
	 ADMUX = ADMUX & 0xe0;
	 ADMUX =ADMUX | NumberAd;
	 ADCSRA |=(1<<ADSC);
	 ADCSRA |=0x8;
		}
    }

   	
} 

 SIGNAL(SIG_OVERFLOW1)/*32.7 Mc*/
    {



    if(RegimError & 1)//1.2 I100
	    {
    if(CtError[0]>=360)
    CtError[0] -=360;
	else
    CtError[0] =0;
    if(CtError[1])
    --CtError[1];
 		}
    else if(RegimError & 2)//1.0 I100
	    {
    if(CtError[1])
    --CtError[1];
	    }
    else if(!(RegimError & 0x3))
	    {
    if(CtError[0]<CtError0[0])
    ++CtError[0];
    if(CtError[1]<CtError0[1])
    ++CtError[1];
	    }
    if(RegimError & 4)//1.2 I50
	    {
    if(CtError[2]>=360)
    CtError[2] -=360;
	else
    CtError[2] =0;
    if(CtError[3])
    --CtError[3];
 		}
    else if(RegimError & 8)//1.0 I50
	    {
    if(CtError[3])
    --CtError[3];
	    }
    else if(!(RegimError & 0xc))
	    {
    if(CtError[2]<CtError0[2])
    ++CtError[2];
    if(CtError[3]<CtError0[3])
    ++CtError[3];
	    }

    if(RegimError & 0x10)//K1
	    {
    if(CtError[4])
    --CtError[4];
		}
    else
	    {
    if(CtError[4]<CtError0[4])
    ++CtError[4];
	    }
    if(RegimError & 0x20)//K2
	    {
    if(CtError[5])
    --CtError[5];
		}
    else
	    {
    if(CtError[5]<CtError0[5])
    ++CtError[5];
	    }
    if(RegimError & 0x40)//Vdd1
	    {
    if(CtError[6])
    --CtError[6];
		}
    else
	    {
    if(CtError[6]<CtError0[6])
    ++CtError[6];
	    }

    }

