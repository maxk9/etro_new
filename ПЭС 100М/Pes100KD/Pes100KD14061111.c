 /*œ«¿ A“Ã≈À*/   
    
//with Int comparator=off 
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

    unsigned    char            CtKn;
    unsigned    int            ArgL0;
    unsigned    int            ArgL1;
    unsigned    int            RegTG;
    unsigned    char            EndAd;
    unsigned    char            CtTG;
    unsigned    char            EnableLoad;
    unsigned    char            TestLink;
    unsigned    char            CtPusk;
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;
 	unsigned    char            RomReceive[12];
 	unsigned    char            RegTransmit[12];
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=5000;//500;
    unsigned    char            RegimWork;
    unsigned    int             NDiz;
    unsigned    char             TG;
 	unsigned    int         	ICR1Old;
    unsigned    int             CtTime[8];
    const unsigned    int       CtTime0[8]={152,305,61,459,5500,301,220,61};
    unsigned    char            RegimTime;
    unsigned int RegInt0;
   unsigned int RegInt1;

    unsigned    char             CtErrorLink;
	const   unsigned    char     CtErrorLink0=100;

    unsigned    long            TDiz;
    unsigned    int             TDizTemp[16];
    unsigned    char            CtTDiz;
    unsigned    char            RegIntCap1;
    unsigned    char            CtOverLow;
    unsigned char Regim;
    unsigned int CtStart;
    unsigned char CtEeprom;

 	unsigned    int	CtError[8];
 	const unsigned    int CtError0[8]={100,100,250,60,
										300,680,100,450};

    unsigned char RegimError;

 	unsigned    char    Avaria;
    unsigned int CtInitInd;
    unsigned int RegInt0;

	unsigned    int    Reg0;
	unsigned    char   Reg1;
    unsigned    char    SregTemp;   
	unsigned    char    IndData[6];
	unsigned    char	NumberAd;
    unsigned    char	CtAd;

	static const unsigned    char	    CtAd0=33;//3.2msec
	unsigned    long	AdTemp;
 	unsigned    int	AdResult;
	unsigned    int	RegS,RegSTemp;
    unsigned    int              CtUst;
    static const    unsigned int        CtUst0=10;
       
    /*============================================*/
     void    ReadKn(void)
    {

    unsigned    int R0;
    R0=8;
    if(bit_is_clear(PINB,PB0))
    R0 |=1;
    if(bit_is_clear(PINB,PB1))
    R0 |=2;
    if(bit_is_clear(PINB,PB4))
    R0 |=4;
    if(bit_is_clear(PIND,PD7))
    R0 &=0xfff7;
    if(bit_is_clear(PINC,PC2))
    R0 |=0x10;
    if(bit_is_clear(PINC,PC3))
    R0 |=0x20;
    if(bit_is_clear(PINC,PC5))
    R0 |=0x40;
    if(bit_is_clear(PINC,PC6))
    R0 |=0x80;
    if(bit_is_clear(PINB,PB5))
    R0 |=0x100;
    if(bit_is_clear(PINB,PB6))
    R0 |=0x200;
    if(R0==RegSTemp)
		{
	if(CtKn)
	--CtKn;
	else
    RegS=R0;
		}
	else
	CtKn=5;
    RegSTemp=R0;

 
    }

 
 
 

void		DecSegm( unsigned int Arg0)
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



 

    unsigned char    ReadEeprom(unsigned char Arg6)
	
    {		 	
	
	EEARH=0;
    EEARL=Arg6;
	EECR=EECR | (1<<EERE);

	while	(bit_is_set(EECR,EERE))
    ;

	return EEDR;
    }
 
    void InitAd   (void)
    {
    ADMUX=0xc0;//AVCC=Ref
    SFIOR=SFIOR & 0x1b;//start from end AD
 	CtAd=CtAd0;
	NumberAd=9;
	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
    ADCSRA |=(1<<ADPS1);
    ADCSRA |=(1<<ADATE);//Auto trigger enable
//	ADCSRA |=(1<<ADPS0); /*32*/	 		
	ADCSRA |=(1<<ADIE);/* enable interrupt*/
    ADCSRA |=(1<<ADSC);/* Start*/
    }

    unsigned   int    ChangeUst(int unsigned R0)   
    {
    if(!CtUst)
        {
        if(RegS & 0x400)
        ++R0;

        else   if(RegS & 0x800)
        --R0;

    CtUst=CtUst0;
        }
    return R0;    
    }

    void IndPar(void)
    {
    unsigned int R0;
	const unsigned int R1=300;  
    R0=R1;
    while(R0--)         _WDR(); 
    SPDR=IndData[3]+0x30;
    R0=R1;
    while(R0--)         _WDR(); 
    SPDR=IndData[2]+0x30;
    R0=R1;
    while(R0--)         _WDR(); 
    SPDR=IndData[1]+0x30;

    R0=R1;
    while(R0--)         _WDR(); 
    SPDR=IndData[0]+0x30;


    }

   void	SetCursor(unsigned char R2,unsigned char R1)
    {
    unsigned int R0;
	const unsigned int R3=300; 	  
    R0=R3;
    while(R0--)         _WDR();
    SPDR=0x1f;
    R0=R3;
    while(R0--)         _WDR();
    SPDR=0x24;
    R0=R3;
    while(R0--)         _WDR();
    SPDR=R2;
    R0=R3;
    while(R0--)         _WDR();
    SPDR=0;
    R0=R3;
    while(R0--)         _WDR();
    SPDR=R1;
    R0=R3;
    while(R0--)         _WDR();
    SPDR=0;

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
    for(R0=1;R0<=10;++R0)
    RomReceive[R0]=RamReceive[R0];
	CtErrorLink=CtErrorLink0;
	Avaria &=0xbf;
        }
   
    }
 unsigned char WaitTWCR(void)
    {
   while(!(TWCR & 0x80))
        {
   _WDR();

    if(CtTWCR)
    --CtTWCR;
    else
            {
    CtTWCR=CtTWCR0;
    return 0;
            }
        }
    CtTWCR=CtTWCR0;
    return 1;
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
    unsigned char R0;    
    for(R0=7;R0<=10;++R0)
    RegTransmit[R0]=10;


    RegTransmit[1]=Avaria;
	if(CtTime[6]<150)
	{
    RegTransmit[2]=NDiz;
    RegTransmit[3]=NDiz>>8;
	}
	else if(!CtTime[7])
	{
    RegTransmit[2]=0;
    RegTransmit[3]=0;
	}
	else if(NDiz<1400)
	{
    RegTransmit[2]=NDiz;
    RegTransmit[3]=NDiz>>8;
	}
	else 
	{
    RegTransmit[2]=78;
    RegTransmit[3]=5;//1400
	}
	RegTransmit[4]=TG;
	RegTransmit[5]=RegimWork;
	RegTransmit[6]=RegS;
    } 



/*
Avaria
0	TM>117
1   TG>112
2   PM<1.0
3   F>1800
4	no 1400
5   no stop
6   no link
7   No Pusk
8
9
10
11
12
13
14
15	*/
	void SetRegimError(void)
	{
    if(RegS & 1)//TM>117
    RegimError |=1;
    else
    RegimError &=0xfffe;
    if((RegS & 2))// || (TG>112))//TG>112
    RegimError |=2;
    else
    RegimError &=0xfffd;
    if((RegS & 4)&&(NDiz>400))//PM<1.0
    RegimError |=4;
    else
    RegimError &=0xfffb;
	if(NDiz>1800)//N>1800
    RegimError |=8;
    else
    RegimError &=0xfff7;
//	if((NDiz<1400)&&(NDiz>400))//No 1400
//    RegimError |=0x10;
//    else
    RegimError &=0xffef;
	if((NDiz>400)&&(!(PORTD & 0x2)))//stop 
    RegimError |=0x20;
    else
    RegimError &=0xffdf;
	if((NDiz<1400) &&(NDiz>400)&& (RegimWork & 2))
    RegimError |=0x80;
    else
    RegimError &=0xff7f;

	}
/*
RegimTime
0	5s Starter=on
1	10s Starter=off
2	2s after N>400
3	15s after N<400
4	180s Rashol
5	N>400 for PM
6	N>400 On
7	N>400 Off
8
*/


    void    SetRegimWork(void)
    {
    unsigned char R0;
    if(NDiz>400)
    RegimTime |=(1<<5);//N>400 for PM
	else
    RegimTime &= 0xdf;//N<400 for PM

    if(bit_is_set(RegimWork,0))//Stop
    RegimWork=RegimWork & 0xf7;//Rashol=off
    else if((!(CtTime[4]))||(TG<70))
    RegimWork=RegimWork & 0xf7;
    else    if((NDiz>1400)&&(TG>72))
    RegimWork |=(1<<3);//Rashol

    if(bit_is_set(RegimWork,2))
    RegimTime |=(1<<4);//180 sec Rashol
    else
    RegimTime=RegimTime & 0xef;



    if(bit_is_set(RegimWork,0))//Stop
        {
    RegimWork=RegimWork & 0xfd;//Pusk=off
    if(NDiz<400)
            {
    RegimTime |=(1<<3);//15 sec after N<400
    if(!(CtTime[3]))
                {
    RegimWork=RegimWork & 0xfe;
    RegimTime=RegimTime & 0xf7;
                }
            }
    else
    RegimTime=RegimTime & 0xf7;
        }

    else
//Stop=off
        {
    RegimTime=RegimTime & 0xf7;
    R0=0xbc;
    if(RegS & 0x8)//Protect=on
    R0=0xbf;

    if(Avaria & R0)
            {
    RegimWork=RegimWork & 0xfd;//Pusk=off
    if((NDiz>400) && (Avaria & 0xC))
    RegimWork |=(1<<0);//stop
            }

    if(bit_is_set(RegimWork,1))//Pusk
            {

    if(NDiz>1400)
                {
    RegimTime |=(1<<2);//2 sec after N>1400
    if(!(CtTime[2]))
                   {
    RegimWork=RegimWork & 0xfd;
    RegimTime=RegimTime & 0xfb;
                   }
                }
    else
    RegimTime=RegimTime & 0xfb;
            }

    else
            {
//no pusk
    RegimTime=RegimTime & 0xfb;
    if(bit_is_set(RegS,4))//Pusk
                {
    RegimWork=RegimWork & 0xfb;//Temp Stop=off
    RegimWork=RegimWork & 0xf7;//Rashol=off
    if(NDiz<400)
    RegimWork |=(1<<1);//Pusk
                }
    RegimTime=RegimTime & 0xf7;
            }




    if(bit_is_set(RegS,5))//Stop
            {
    RegimWork=RegimWork & 0xfd;//Pusk=off
    if(NDiz>400)
    RegimWork |=(1<<2);//Temp Stop
            }
    else if(Avaria & R0)
            {
    RegimWork=RegimWork & 0xfd;//Pusk=off
    if(NDiz>400)
    RegimWork |=(1<<2);//Temp Stop

            }


    if((bit_is_set(RegimWork,2)) && (NDiz>400))
            {    
    if(bit_is_clear(RegimWork,3))
                {
    RegimWork |=(1<<0);// Stop
    RegimWork=RegimWork & 0xfb;//Temp Stop=off
                }
            }

        }

    }
   void ControlStarter(void)
   {
    if(RegimWork & 0x2)//Pusk


        {
    if(!(CtTime[1]))//30 sec
    RegimTime |=(1<<0);//10 sec
    if(bit_is_set(RegimTime,0))
        {
    if((CtTime[0]) && (NDiz<400))
            {
    PORTD=PORTD & 0xfb;//CT=on
    RegimTime=RegimTime & 0xfd;
    CtTime[1]=CtTime0[1];
            }
    else if(!CtTime[0])
            {
    RegimTime=RegimTime & 0xfe;
    PORTD |=(1<<2);//CT=off

    if(!CtPusk)
                {
    Avaria |=(1<<7);//no Pusk
    RegimWork=RegimWork & 0xfd;
                }
    else
                {
    --CtPusk;
    RegimTime |=(1<<1);//30 sec
    if(!(CtTime[1]))
                    {
    RegimTime=RegimTime & 0xfd;
    RegimTime |=(1<<0);//10 sec 
    CtTime[0]=CtTime0[0];    
      
                    }
    else
    CtTime[0]=0;
                }
            }
            }
        }

    else
        {
    CtPusk=2;
    RegimTime |=(1<<1);//30 sec
    CtTime[1]=0;
    PORTD |=(1<<2);//CT=off
        }



    }


    void    AccountNDiz(void)
    {
    unsigned char R0;
    unsigned long R1;
    unsigned long R2;
	if(!CtOverLow)
	NDiz=0;
	else
	{
    TDiz=0;
    for(R0=0;R0<=15;++R0)
	{
    TDiz +=TDizTemp[R0];
	_WDR();
	}
	R2=TDiz<<3;
	R1=RomReceive[4];
	R1=R2/R1;

	R1=950000/R1;

	NDiz=R1;

                                   
    }
	}
    void	IndAvaria (void)
    {
    if(Avaria & 0xbf)
    PORTD &=0xfe;
    else if(RomReceive[2] & 0xdf)
    PORTD &=0xfe;
    else
    PORTD |=1;
    }
    void ControlStop(void)
    {

    if(RegimWork & 0x1)
    PORTD &=0xfd;//STOP
	else
    PORTD |=0x2;//STOP=off	   
    }
    void    ControlN400(void)
    {
    if(NDiz>400)
        {
    RegimTime |=0x40;
    RegimTime &=0x7f;
    if(!(CtTime[6]))
    PORTD &=0xf7;//N>400
        }
    else
        {
    RegimTime |=0x80;
    RegimTime &=0xbf;
    if(!(CtTime[7]))
    PORTD |=0x8;//N>400=off
        }

    }
    void    ResetAvaria(void)
    {
	unsigned char R0;
    Avaria=0;
    RegimError=0;
    for(R0=0;R0<=7;++R0)
	CtError[R0]=CtError0[R0];
    }
    void    SetAvaria(void)
    {
    unsigned char R0;
    for(R0=0;R0<=7;++R0)
        {
    if(!(CtError[R0]))
	Avaria |=(1<<R0);
        }
	

    }

    void    AccountTG(void)
    {
	unsigned long R0;
	unsigned long R1;
	RegTG=RegTG+AdResult;
	--CtTG;
	if(!CtTG)
		{
	R0=RegTG>>8;;
	CtTG=63;
	RegTG=0;
	R1=RomReceive[5];

	R0=R0*R1;
	R0=R0>>7;
	if(R0>=86)
	R0=R0-86;
	else
	R0=0;
	if(R0>125)
	R0=125;
	TG=R0;
		}
    }

void    TransmitInd(unsigned char R1)
{


    if(R1>=192)
    R1=R1-64;
    while(!(UCSRA & 0x20))
    _WDR();    
    UDR=R1;           
 }



   

	
	                 

	



	
	
	
						        
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	unsigned char Arg0;
//if DDRX,X=1 PORTX,X=OUT
    DDRA=0;
	PORTA = 0xff;//0xc0 prov

    DDRB=0;
	PORTB = 0xff;

    DDRC=0x90;
    PORTC =0xff;

    DDRD=0xf;
    PORTD=0xff;

//INIT TWI;
    TWBR=0x7f;//F TWI

    InitAd();
//Init Comparator
    SFIOR &=0xf7;//PB2,PB3
    ACSR=0;
    ACSR |=0x4;//enable capture
    TIMSK=TIMSK | 0x20;//enable Int capture1
    TCCR1B=0xc2;//0.5mkc
    TIMSK=TIMSK | 0x4;//enable Int overlowT1
   TWAR=4;
   TWCR =(1<<TWEA)|(1<<TWEN);
    ReadKn();
    CtStart=200;
	while(CtStart--)	_WDR();
    ReadKn();
	Regim=1;

   _SEI();
    RegimTime &=0xc0;
	ResetAvaria();
    CtUst=CtUst0;
		
	for(Arg0=1;Arg0<=10;++Arg0)
	RomReceive[Arg0]=0;


	CtEeprom=1;
	CtTDiz=15;	
	RegimWork=0;
	CtErrorLink=CtErrorLink0;
	CtKn=5;		
	
//WORK	
	           
      while(1)
    {
	++TestLink;
     _WDR();
	if(EndAd)
		{
    AccountTG();
	EndAd=0;
		}
	if(EnableLoad)
		{
    LoadRegTransmit();
    LoadControlSum();
		}
	if(TWCR & 0x80)
    ReceiveTransmitSlave();
	if(!CtErrorLink)//ErrorLink;
		{

//INIT TWI;
    TWBR=0;//F TWI
   TWAR=0;
   TWCR =0;
	TWSR=0xf8;
    CtStart=50;
	while(CtStart--)	_WDR();
    TWBR=0x7f;//F TWI
   TWAR=4;
   TWCR =(1<<TWEA)|(1<<TWEN);
 //   TWCR |=(1<<TWINT);
	CtErrorLink=CtErrorLink0;
    ReadKn();
    CtStart=200;
	while(CtStart--)	_WDR();
		}

    ReadKn();

    AccountNDiz();



    if(RomReceive[1] & 4)//Reset Avaria
    ResetAvaria();
    ControlN400();
    if(!(RomReceive[1] & 2))// RegSKG DU prov
	    {
    SetRegimError();
    SetRegimWork();
    SetAvaria();
    ControlStarter();
    ControlStop();


    IndAvaria();
	    }
	else
		{
	PORTD |=0x7;
    RegimTime &=0xc0;
    ResetAvaria();
		}				

    }
  
}
SIGNAL(SIG_ADC)
{
    TIMSK=TIMSK | 0x20;//enable Int capture1
    RegInt0=ADC;
	if(RegInt0 & 0x200)
		{
	RegInt0=0x3ff-RegInt0;
	RegInt0=511+RegInt0;
		}
	else
	RegInt0=512-RegInt0;

    if(CtAd>=CtAd0)
    RegInt0=0;


    if(CtAd)
	{
    AdTemp +=RegInt0;
    --CtAd;
	ADCSRA &=0x7f;
	ADCSRA |=0x80;
    ADCSRA |=(1<<ADSC);

	}
    else
    {
	EndAd=1;
	ADCSRA &=0x7f;
	ADCSRA |=0x80;
    AdTemp=AdTemp>>5;
    AdResult=AdTemp;
    AdTemp=0;
    ADCSRA |=(1<<ADSC);
	 ADMUX = ADMUX & 0xe0;
	 ADMUX =ADMUX | NumberAd;
	 CtAd=CtAd0+2;

    }

   	
}
 SIGNAL(SIG_OVERFLOW1)/*32.7 Mc*/
    {
    if(CtOverLow)
    --CtOverLow;
    else
	NDiz=0;

    if(CtErrorLink)
    --CtErrorLink;
    else
    Avaria |=0x40;


    for(RegInt1=0;RegInt1<=7;++RegInt1)
    {
    if(RegimError & (1<<RegInt1))
        {
    if(CtError[RegInt1])
    --CtError[RegInt1];
        }
    else
    CtError[RegInt1]=CtError0[RegInt1];

    if(RegimTime & (1<<RegInt1))
        {
    if(CtTime[RegInt1])
    --CtTime[RegInt1];
        }
    else
    CtTime[RegInt1]=CtTime0[RegInt1];
    }







    }

 SIGNAL(SIG_INPUT_CAPTURE1)
{
    TIMSK=TIMSK & 0xdf;//denable Int capture1
	if((ICR1>=ICR1Old) && (CtOverLow==1))
    TDizTemp[CtTDiz]=0xffff;
    else if(!CtOverLow)
    TDizTemp[CtTDiz]=0xffff;
    else
    {

    TDizTemp[CtTDiz]=ICR1-ICR1Old;
	if(TDizTemp[CtTDiz]<300)
    TDizTemp[CtTDiz]=0xffff;

    ICR1Old=ICR1;
    }
    if(CtTDiz)
    --CtTDiz;
    else
    CtTDiz=15;
    
    CtOverLow=2;






}   


