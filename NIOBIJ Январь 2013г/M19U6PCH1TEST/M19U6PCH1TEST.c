
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>

   unsigned    int    URef;
     unsigned char ChangeNumberAd;
    unsigned    char    CtLuk;
    unsigned    char    RegStop1;
    unsigned    int    CtK5;
     unsigned    char    RegStop;
    unsigned    int    AvariaI;
    unsigned    int    CtIMax;
    unsigned    int    CtUError;
    unsigned    char    CtPlusI50;
    unsigned    char    CtMinusI50;
    unsigned    char   I500;
    unsigned char EnablePusk;
    unsigned char CtBegin;
	
    unsigned char ErrorFar;//Error other bloks
    unsigned char CtErrorFar[7];
    unsigned char TestTimeAd;
    unsigned char CtErrorRS;
    unsigned char TestRS;
    unsigned char NumberBlok;
    unsigned char NumberReceiveRS;

	volatile    unsigned char CtUsart;
    unsigned char NumberLinkRS;

    unsigned char RegTransmitRS[9];
    unsigned char RamReceiveRS[9];

    unsigned char RomReceiveRS[9][7];
    unsigned char	ControlTransmitRS;
    unsigned char ControlReceiveRS;


	volatile unsigned char CtReceiveRS;

    const unsigned char CtReceiveRS0=8;







    const unsigned char CtK5_0=200;
    unsigned char RegimWork;
    unsigned int IA;
    unsigned int IB;
    unsigned int IC;
    unsigned int U400Out;
    unsigned int U400In;
    const unsigned int I50_0=100;
    const unsigned int I50_Nom=200;
    const unsigned int I50PuskMin=50;
   const unsigned int I50PuskMax=300;

    unsigned int I50;
    unsigned int U50;
    const unsigned int U50_0=100;
    const unsigned int U50_Nom=500;

	volatile unsigned int TestAd;
	volatile unsigned int AdTemp;
     unsigned int CtStart;
    unsigned    char            CtKn;
    unsigned char CtEnableInd;
    const unsigned char CtEnableInd0=20;
    unsigned int RegSPCH2;
    unsigned char RegSPCH2Old;

    unsigned char Regim;
    unsigned int RegIntInd0;
    unsigned char RegIntInd1;
    unsigned int CtInitInd;
	unsigned char CtTimeInd;
	unsigned char CtNumberInd;
	unsigned long IndicatorBit;
	unsigned char EnableInd;

    const    unsigned int INom100_1_15=384;
    const	unsigned int INom100_1_2=400;
	unsigned    int             NDiz;
	unsigned    char            TG;


 	unsigned    char            Reg101;
 	unsigned    int             Reg100;


	unsigned    int	TGenTemp[16];
    unsigned    char CtTGen;
    unsigned    char CtOverLow;
    unsigned    char IndexInt;
	unsigned    int	UU;
	unsigned    int	UV;
	unsigned    int	UW;
	unsigned    int	UF;

	unsigned    int	IU;
	unsigned    int	IV;
	unsigned    int	IW;
	unsigned    int	IMax;
	unsigned    int	UMax;
	unsigned    int	UMin;
	unsigned    int	FGen;



	unsigned    int	AdTempResult;

	unsigned    int	AdTempMax;
	unsigned    int	AdTempMin;
 	unsigned    int	AdResult[8];
    unsigned    char RegimError;
 	unsigned    int	CtError[8];
    static const unsigned  int CtError0[8]=
    {50,1000,100,100,100,100,100,100};
    static const unsigned  char KoefMax[9]=
    {240,240,240,240,240,240,100,240,240};
    static const unsigned  char KoefMin[9]=
    {40,40,40,40,40,40,2,40,40};
    static const unsigned  char Koef0[9]=
    {0x84,0x7c,0x7f,0x8d,0x7a,0x7a,5,0x89,0x7d};
 	unsigned    char	Koef[9];




	unsigned    int    ICR1U;
	unsigned    int    TGen;


	unsigned    int    Reg0;


 	unsigned    char             Avaria;
 	unsigned    char            RomReceive[9];
 	unsigned    char            RegTransmit[12];
    unsigned    char            NumberLink;
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=10000;//500;
    unsigned    char             CtErrorLink;
	const   unsigned    char     CtErrorLink0=30;    
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;

	unsigned    char    CtInd;
	 const unsigned    char	    CtInd0=8;


    unsigned    char    SregTemp;   


	unsigned    char    IndData[6];


	unsigned    char	NumberAd;
    unsigned    int	CtAd;

	static const unsigned    int	    CtAd0=1000;//3msec
 
//    unsigned    char	MyStatus;

	unsigned    char	RegS,RegSTemp,RegSOld;
       
    /*============================================*/
 

 	void	ResetErrorLinkRS(void)
{
	unsigned char R0;
	R0=UDR;//UDR1;
	UCSRA &=0x22;//reset error


	CtReceiveRS=CtReceiveRS0;
}
void	InitUsart(void)
	{	

   UCSRA=0;
   UCSRA |=1;//only 9bit=1
   UCSRB=0x1c;//enable transmit 9bit
   UBRRH=0;
   UBRRL=103;

	UCSRC=(1<<7)|(1<<2)|(1<<1);

   UCSRB |=0x80;//enable int receive

   PORTD &=0xf7;//Out Transmit=off


	}	
void	TransmitUsart(void)
	{
	volatile unsigned char R0;

   PORTD |=0x8;//Out Transmit=on 

   R0=100;
   while(R0)
   --R0;
	UCSRA =1;//Receive only 9 bit=1
	R0=NumberBlok;

     while(!(UCSRA & 0x20))
   _WDR();
    UCSRB |=1;//9 bit=1
    UDR=R0;//LinkRS;
	ControlTransmitRS =NumberBlok;
	
	}		  

	void	LinkRS(void)
{

	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	unsigned char R3;
	volatile unsigned char R4;
	unsigned int R5;
	PORTD &= 0xf7;//Transmit denable
	if(UCSRA &(1<<RXC))
	{

	R1=UCSRA;
	R2=UCSRB;
	R3=UCSRC;
	R0=UDR;
	RamReceiveRS[CtReceiveRS]=R0;
	if(R1 & 0x1c)//error
		{

	ResetErrorLinkRS();

	return;
		}

	if(R2 & 2)
		{
	NumberReceiveRS=R0;
	if(R0>6)
		{
	CtUsart=(NumberBlok<<1)+14;
	return;
		}
	if(NumberBlok>NumberReceiveRS)
			{
	R4=NumberBlok-NumberReceiveRS;
	CtUsart=R4<<1;
			}
	else
			{
	R4=NumberReceiveRS-NumberBlok;

	R4 <<=1;
	R4=~R4;
	CtUsart=R4+15;
			}

	ControlReceiveRS =R0;
	CtReceiveRS=CtReceiveRS0-1;
	UCSRA &=0xfe;
	UCSRB &=0xfe;
	if(NumberReceiveRS==NumberBlok)
			{
	PORTD |= 0x8;//Transmit enable

	R0=100;
	while(R0)
	--R0;
	R0=RegTransmitRS[CtReceiveRS];
     while(!(UCSRA & 0x20))
   _WDR();
	UCSRB &=0xfe;
    UDR=R0;

	ControlTransmitRS +=RegTransmitRS[CtReceiveRS];
			}
	return;
		}

	if(CtReceiveRS)
		{	
	ControlReceiveRS +=R0;
	--CtReceiveRS;
	if(NumberReceiveRS==NumberBlok)
			{
	PORTD |= 0x8;//Transmit enable
     while(!(UCSRA & 0x20))
   _WDR();
   if(!CtReceiveRS)
   				{
	UCSRB &=0xfe;
    UDR=ControlTransmitRS;
				}
	else
				{
	R0=RegTransmitRS[CtReceiveRS];
	UCSRB &=0xfe;
    UDR=R0;
	ControlTransmitRS +=RegTransmitRS[CtReceiveRS];
				}
			}
		}
	else
		{

	UCSRA |=1;
	PORTD &= 0xf7;//Transmit denable
	if(R0==ControlReceiveRS)
					{
	CtErrorFar[NumberReceiveRS]=60;
	if(NumberReceiveRS != NumberBlok)
			{

	CtErrorRS=39;

			}
	R5=0;
	R5 |=1<<NumberReceiveRS;
	R5=~R5;
	ErrorFar &=R5;

	for(R4=1;R4<=7;++R4)
					{
	RomReceiveRS[R4][NumberReceiveRS]=RamReceiveRS[R4];
	RamReceiveRS[R4]=1;
					}


					}
		}

	}
}

 
void    ReadKn(void)
    {

    unsigned    char Reg2;
    Reg2=0;
    if(bit_is_clear(PINB,PB0))
    Reg2=Reg2 | 1;

    if(bit_is_clear(PINB,PB1))
    Reg2=Reg2 | 2;

    if(bit_is_clear(PINB,PB2))
    Reg2=Reg2 | 4;

    if(bit_is_clear(PINB,PB3))
    Reg2=Reg2 | 8;

    if(bit_is_clear(PIND,PD6))
    Reg2=Reg2 | 0x10;

    if(bit_is_clear(PIND,PD2))
    Reg2=Reg2 | 0x20;

    if(bit_is_clear(PIND,PD4))
    Reg2=Reg2 | 0x40;



    if(Reg2==RegSTemp)
		{

    RegS=Reg2;
		}
    RegSTemp=Reg2;

 
    }





void		DecSegm( unsigned int Arg0)
    {
     unsigned int	Arg1;

	if(Arg0>9999)
	Arg0=9999;
    Arg1=Arg0/1000;
	IndData[3]=Arg1;
    Arg0=Arg0-(Arg1*1000);
    Arg1=Arg0/100;
	IndData[2]=Arg1;
    Arg0=Arg0-(Arg1*100);
    Arg1=Arg0/10;
	IndData[1]=Arg1;
    Arg0=Arg0-(Arg1*10);
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





void InitAd   (void)
    {
    ADMUX=0xc0;//2.56=Ref
    SFIOR=SFIOR & 0x1f;//start from end AD
 	CtAd=CtAd0;
	NumberAd=7;

	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
//    ADCSRA |=(1<<ADPS1);
    ADCSRA |=(1<<ADATE);//Auto trigger enable
	ADCSRA |=(1<<ADPS0); /*32*/	 		
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

void IndPar3(void)
    {



    while(!(UCSRA & 0x20))
   _WDR();
    UDR=IndData[2]+0x30;
    while(!(UCSRA & 0x20))
   _WDR();
    UDR=IndData[1]+0x30;

    while(!(UCSRA & 0x20))
    _WDR();
    UDR=IndData[0]+0x30;
 
    }
void	ClearInd(void)
    {
     
     while(!(UCSRA & 0x20))//!RegInd)
     _WDR();
    UDR=0xc;
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


void    TransmitInd(unsigned char R1)
{


    if(R1>=192)
    R1=R1-64;
    while(!(UCSRA & 0x20))
    _WDR();    
    UDR=R1;           
 }
unsigned char WaitTWCR(void)
    {
   while(!(TWCR & 0x80))
        {

    if(CtTWCR)
		{
    --CtTWCR;
	_WDR();
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

unsigned char    Transmit(void)
    {

    if(!WaitTWCR())
    return 0;

    if(((TWSR & 0xf8) == 0x8)||((TWSR & 0xf8) == 0x10))//start
            {
    TWDR=4;//0x4;//SLA+W
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
   TWDR=RegTransmit[CtTransmit];
   TWCR=(1<<TWINT)|(1<<TWEN);

            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }
TT:    if(!WaitTWCR())
    return 0;

   if((TWSR & 0xf8)==0x28)//Data+ACK
            {
    if(CtTransmit)
                {
    --CtTransmit;
   TWDR=RegTransmit[CtTransmit];
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

unsigned char    Receive(void)
    {
    if(!WaitTWCR())
    return 0;

    if((TWSR & 0xf8) == 0x8)//Well Start
            {
    TWDR=5;//0x5;//SLA+R
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
//	++TestLink;

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



    void    ReceiveTransmitMaster(void)
{
//ControlSum for Transmit
    RegTransmit[0]=0;


    for(Reg0=1;Reg0<=10;++Reg0)
    {
//    RegTransmit[Reg0][0]=3;
    RegTransmit[0] +=RegTransmit[Reg0];
//    RegTransmit[Reg0][1]=4;


    }
//Transmit
    TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//Start
    CtTWCR=CtTWCR0;
    if(!Transmit())
    {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    Reg0=5000;

    }
    else
    {

    Reg0=5000;//
    }
    while(Reg0)
	{_WDR();
    --Reg0;
	}
//for Ind ControlSum
    Reg100=0;
    for(Reg0=1;Reg0<=10;++Reg0)
    Reg100=Reg100+RamReceive[Reg0];
    Reg100=RamReceive[0];

//Receive
   TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//Start
   CtTWCR=CtTWCR0;
    if(Receive())
    {

//ControlSum
    Reg101=0;
    for(Reg0=1;Reg0<=10;++Reg0)
    Reg101=Reg101+RamReceive[Reg0];
    if(Reg101==RamReceive[0])
        {
    for(Reg0=1;Reg0<=8;++Reg0)
    RomReceive[Reg0]=RamReceive[Reg0];
    CtErrorLink=CtErrorLink0;


        }
    }
    else
    {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);


    }

} 
void    LoadRegTransmit(void)
    {
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;

	if(NumberBlok==1)
		{
	R0=4;
	R1=0x10;
	R2=0x40;
		}
	else
		{
	R0=8;
	R1=0x20;
	R2=0x80;
		}
	if(RomReceive[1]<=7)
		{					
    RegTransmit[1]=AdResult[1]>>1;//RegS;
    RegTransmit[2]=U400In>>1;

    RegTransmit[3]=U400Out>>1;
    RegTransmit[4]=AdResult[4];
    RegTransmit[5]=AdResult[5];
    RegTransmit[6]=AdResult[6];

    RegTransmit[7]= AdResult[7];
		}
	else
		{					
    RegTransmit[1]=RegS;
	PORTC |=0xfc;
	PORTD |=0x20;

	if(RomReceive[1]==9)
			{

	PORTC &=0x53;
	PORTD &=0xdf;
			}
	else if(RomReceive[1]==10)
			{

	PORTC &=0xbb;

			}


		}

    }    

int unsigned AccountI(unsigned char Arg0)
    {
    unsigned int R0;
    unsigned int R1;
    unsigned char R2;
    unsigned int  R3;

    R0=AdResult[Arg0]>>1;
    R2=Arg0;
    R1=Koef[Arg0];
    R1=R0*R1;
    R1=R1>>8;
    R0 +=R1;
    R3=R0;
    return R3; 
    }


    



    void ChangeCtInd(void)
{
    if((RegS & 0x30) != (RegSOld & 0x30))
    {
    if((RegS & 0x10) && (!(RegSOld & 0x10)))
    ++CtInd;
    else if((RegS & 0x20) && (!(RegSOld & 0x20)))
    --CtInd;
    RegSOld=RegS;
    }

}





void AccountIMax(void)
    {
    IMax=IA;
    if(IB>IMax)
    IMax=IB;
    if(IC>IMax)
    IMax=IC;
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
void    ControlRegimError(void)
    {

	if(RegimWork==2) 
	RegimError |=2;
	else
	RegimError &=0xfd;
	if(RegimWork==3) 
	RegimError |=4;
	else
	RegimError &=0xfb;
	if(RegimWork==5) 
	RegimError |=8;
	else
	RegimError &=0xf7;
/*	if((RegimWork==7)&&(!(RegS & 0x10)))
	RegimError |=0x10;
	else*/
	RegimError &=0xef;
//	if((!(RegS & 0x20))&&(!(RegS & 1))) 
//	RegimError |=0x20;
//	else
	RegimError &=0xdf;
//	if((!(RegS & 0x40))&&(!(RegS & 1))) 
//	RegimError |=0x40;
//	else
	RegimError &=0xbf;
/*	if(RomReceiveRS[3][0] & 8)
	RegimError |=0x80;
	else
	RegimError &=0x7f;*/

    }


void    ControlRegimWork(void)
    {

	unsigned char R1;
	unsigned char R2;
	R1=8;
	R2=1;
	if(NumberBlok==1)
		{
	R1=4;
	R2=4;
		}

	if(Avaria)
	++RegStop;
	else if((RomReceiveRS[1][R2] & 0x10)&&(RegS & 0x10))//Zamena
	++RegStop;
	else if(RomReceiveRS[5][6] & 8)
	RegStop=0;
	else if(RomReceiveRS[5][3] & 8)
	RegStop=0;
	else
	++RegStop;

	if(RegStop>10)
	RegStop=10;
	switch(RegimWork)
		{
	case 0:
	if((RegSPCH2 & 1)&&(!RegStop)&&(!(AvariaI)))
	RegimWork=1;
	I50=I50PuskMin+50;
	U50=U50_Nom-50;
	I500=250;
	break;
	case 1:
	if((RegSPCH2 & 2)||(RegStop>=9))
	RegimWork=0;
	if(U50>U50_Nom)
		{
	RegimWork=2;
	I50=I50PuskMin+50;
	I500=250;

		}
	CtK5=200;
	break;
	case 2:
	if((RegSPCH2 & 2)||(RegStop>=9))
	RegimWork=0;
//	if((I50<I50PuskMin)&&(RegS & 4))//K5=on
	if((I500<50)&&(RegS & 4))//K5=on
			{
		if(!CtK5)
	RegimWork=3;
			}
	else
	CtK5=200;
	break;
	case 3:
	if((RegSPCH2 & 2)||(RegStop>=9))
	RegimWork=0;
	if(!(RegS & 4))//K5=off
	RegimWork=4;

	break;
	case 4:
	if((RegSPCH2 & 2)||(RegStop>=9))
	RegimWork=0;
	else
	RegimWork=5;
	break;
	case 5:
	if((RegSPCH2 & 2)||(RegStop>=9))
	RegimWork=0;
	if((RegS & 2) && (RegS & 8))//K4,K6=on
	RegimWork=6;
	break;
	case 6:
	if((RegSPCH2 & 2)||(RegStop>=9))
	RegimWork=0;
	if(RegS & 0x10)//K7=on
	RegimWork=7;
	break;
	case 7:
	if((RegSPCH2 & 2)||(RegStop>=9)||(AvariaI))
	RegimWork=0;
	break;
	default:break;
		}
    }

void    ControlRegimWorkDU(void)
    {

	unsigned char R1;

	if(NumberBlok==1)
		{
	R1=4;
	if(ErrorFar & 0x10)
	EnablePusk=1;
	else if(RomReceiveRS[1][0] & 8)
	EnablePusk=(RomReceiveRS[3][4] & 1);
	else
	EnablePusk=1;


		}
	else if(NumberBlok==4)
		{
	R1=8;
	if(ErrorFar & 0x2)
	EnablePusk=1;
	else if(RomReceiveRS[1][0] & 4)
			{
	EnablePusk=(RomReceiveRS[3][1] & 1);
	if(!RegimWork)
	RegTransmitRS[3] |=1;//enable pusk PCH1
			}
	else
	EnablePusk=1;
		}
	else
	R1=4;
	if(Avaria)
		{
	++RegStop;
	RegTransmitRS[3] |=1;//enable pusk PCH2
		}


	else if(RomReceiveRS[1][0] & R1)
		{
	if(RomReceiveRS[5][6] & 8)
	RegStop=0;
	else if(RomReceiveRS[5][3] & 8)
	RegStop=0;
	else
			{
	++RegStop;
	RegTransmitRS[3] &=0xfe;//denable pusk PCH2
			}
		}
	else if(RomReceiveRS[5][0] & R1)
		{
	++RegStop;
	RegTransmitRS[3] &=0xfe;
		}
	if(RegStop>10)
	RegStop=10;				
	switch(RegimWork)
		{
	case 0:

	if((!RegStop)&&(EnablePusk)&&(!AvariaI))
	RegimWork=1;

	I50=I50PuskMin+50;
	U50=U50_Nom-50;
	I500=250;
	break;
	case 1:
	if(RegStop1>30)
	RegimWork=0;
	RegTransmitRS[3] &=0xfe;
	if(U50>U50_Nom)
		{
	RegimWork=2;
	I50=I50PuskMin+50;
	I500=250;
		}
	CtK5=200;
	RegTransmitRS[3] &=0xfe;
	break;
	case 2:
	if(RegStop1>=30)

	RegimWork=0;
//	if((I50<I50PuskMin)&&(RegS & 4))//K5=on
//	if((I500<100)&&(RegS & 4))//K5=on
	if((I500<100)&&(RegS & 4))//K5=on
			{
		if(!CtK5)
	RegimWork=3;
			}
	else
	CtK5=200;



	break;
	case 3:

	if(RegStop1>=30)

	RegimWork=0;
	if(!(RegS & 4))//K5=off
	RegimWork=4;

	break;
	case 4:
	if(RegStop1>=30)
	RegimWork=0;
	else
	RegimWork=5;
	break;
	case 5:
	if(RegStop1>=30)

	RegimWork=0;
	if((RegS & 2) && (RegS & 8))//K4,K6=on
	RegimWork=6;
	break;
	case 6:

	if(RegStop1>=30)

	RegimWork=0;
	if(U400In >(UMin-50))
	RegimWork=7;
	break;
	case 7:
	RegTransmitRS[3] |=1;
	if((RegStop1>=30)||(AvariaI))
	RegimWork=0;
	break;
	default:break;
		}
    }





void    ResetAvaria(void)
    {
    unsigned char R0;
     Avaria=0;
    RegimError=0;      
     for(R0=0;R0<=7;++R0)
    CtError[R0]=CtError0[R0];
	AvariaI=0;
    }

   
    
    
         



void	ClearPoz(unsigned char R0)
{
static unsigned const char  Tab[]="         ";


    unsigned char R2;
    unsigned char R3;

	for(R2=0;R2<=R0;++R2)
	{
    R3=Tab[R2];
    TransmitInd(R3);
    }

}





	
 

 	


	
	
		


/*	void	ControlRegim(void)
	{
	if(!Regim)//all=off
		{
	if(RegSPCH2 & 1)//Pusk

 	
	}*/	

	void	AccountU50(void)
	{
	unsigned int R0;
	R0=AdResult[1];

	if(R0>U50)
	++U50;
	else if(R0<U50)
	--U50;

	}
	void	AccountI50(void)
	{
	unsigned int R0;
	R0=AdResult[7];
	if(R0>I50)
	++I50;
	else if(R0<I50)
	--I50;
	}
	void CalcI50(void)
	{
	unsigned int R0;
	R0=AdResult[7];
	if(R0>I500)
		{
	CtMinusI50=200;

	if(CtPlusI50)
	--CtPlusI50;
	else
			{
	++I500;
	CtPlusI50=2;
			}
		}
	else if(R0<I500)
		{
	CtPlusI50=200;

	if(CtMinusI50)
	--CtMinusI50;
	else
			{
	CtMinusI50=2;
	--I500;
			}
		}
							
	}		
	


	void	AccountU400In(void)
	{
	unsigned long R0;
	unsigned long R1;
	R0=AdResult[2];
	R1=URef;
	R0 *=R1;
	R0 /=256;
	if(R0>U400In)
		{
	if(R0>(U400In+50))
	U400In=R0;
	else
	++U400In;
		}
	else if(R0<U400In)
		{
	if(R0<(U400In-50))
	U400In=R0;
	else
	--U400In;
		}
	}
	void	AccountU400Out(void)
	{
	unsigned int R0;
	R0=AdResult[3];
	if(R0>U400Out)
		{
	if(R0>(U400Out+50))
	U400Out=R0;
	else
	++U400Out;
		}
	else if(R0<U400Out)
		{
	if(R0<(U400Out-50))
	U400Out=R0;
	else
	--U400Out;
		}
	}


	void	AccountIA(void)
	{
	unsigned int R0;
	R0=AdResult[4];
	if(R0>IA)
	++IA;
	else if(R0<IA)
	--IA;
	IA=R0;
	}
	void	AccountIB(void)
	{
	unsigned int R0;
	R0=AdResult[5];
	if(R0>IB)
	++IB;
	else if(R0<IB)
	--IB;
	IB=R0;
	}
	void	AccountIC(void)
	{
	unsigned int R0;
	R0=AdResult[6];
	if(R0>IC)
	++IC;
	else if(R0<IC)
	--IC;
	IC=R0;
	}
 void	ClearErrorFar(void)
 {
	unsigned char R0;
	for(R0=0;R0<=6;++R0)
 	{
	CtErrorFar[R0]=50;
	ErrorFar=0;
	}

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
	void LoadFromEeprom(void)
	{


	URef=ReadEepromWord(6);
	if((URef<235)||(URef>270))	
	URef=256;
	}

							   
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	DDRA=0;
	PORTA=0;
 
    DDRB=0xa0;
	PORTB |= 0xff;

    DDRC=0xfc;
    PORTC |=0xff;

    DDRD=0x2a;
    PORTD=0xf7;

//INIT TWI;
    TWBR=0x7f;//F TWI
    CtStart=65000;
	while(CtStart--)	_WDR();
//Init capture1
    TCCR1B=0x82;//0.5mkc,falling edge
    TIMSK=TIMSK | 0x4;//enable Int overlowT1
//    TIMSK=TIMSK | 0x20;//enable Int capture1 prov

//INIT USART
	InitUsart();

//  INIT SPI
    SPCR=0x72;//f/64
    SPSR=0;//f/64



    InitAd();
    TCCR1B=0xc2;//0.5mkc
    TIMSK=TIMSK | 0x4;//enable Int overlowT1




 
    ResetAvaria();
	LoadFromEeprom();
	RomReceive[3]=1;
	RomReceive[4]=0;
	RomReceive[6]=0;

	RegSPCH2=0;
	RegSPCH2Old=0;
	CtKn=5;
	RegimWork=0;
	U50=0;
	I50=200;
	U400In=0;
	U400Out=0;
	IA=0;
	IB=0;
	IC=0;
	CtUsart=NumberBlok;
	RegTransmitRS[1]=0;
	CtLuk=200;
    _SEI();
	CtBegin=50;
	RomReceiveRS[3][1]=0;//denablePusk
	RomReceiveRS[3][4]=0;//denablePusk
	RegTransmitRS[3]=0;	
	RegTransmitRS[1]=0;
	ClearErrorFar();
	CtIMax=80;
	CtUError=160;
	AvariaI=0;
	RegStop=0;								             
     while(1)
    {

     _WDR();
	 if(CtBegin)
	 	{
	if(RomReceive[3] & 2)
	NumberBlok=4;
	else
	NumberBlok=1;
		}
//	if(RomReceiveRS[4][0] & 2)
	UMin=385;
//	else
//	UMin=810;
	if(RegimWork>=6)
		{
	if((U400In>=UMin) && (U400In<=1110))
	CtUError=160;
		}
	else
	CtUError=80;
	if(RomReceiveRS[5][3] & 4)
		{
	ResetAvaria();
	ClearErrorFar();
		}
	if((RomReceiveRS[5][6] & 4)||(RomReceive[4] & 0x80))
		{
	ResetAvaria();
	ClearErrorFar();
		}
	if((Avaria)||(AvariaI))
	RegTransmitRS[1] |=1;
	else
	RegTransmitRS[1] &=0xfe;
	if(RegimWork==7)
	RegTransmitRS[1] |=4;
	else
	RegTransmitRS[1] &=0xfb;
	if((RegimWork)&&(!CtLuk))
	RegTransmitRS[1] |=8;//Open Luk
	else
	RegTransmitRS[1] &=0xf7;//Close Luk

	if((RomReceive[5] & 1)&&(RegS & 0x10))
	RegTransmitRS[1] |=0x10;
	else
	RegTransmitRS[1] &=0xef;
	if(RegS & 0x10)//nagr=on
	RegTransmitRS[1] |=0x20;
	else
	RegTransmitRS[1] &=0xdf;

	RegTransmitRS[2]=RomReceiveRS[3][0];//Avaria;//RegS;//RegimError;//NumberBlok;//45;//U50;

	RegTransmitRS[4]=U400In>>2;//RomReceive[2];//UNom
	RegTransmitRS[5]=Avaria;//46;//AdResult[1];
	RegTransmitRS[6]=TestTimeAd;//16;

	RegSPCH2=RomReceive[3]<<8;
	RegSPCH2 |=RomReceive[4];
	AccountU50();
	AccountI50();
	CalcI50();
	AccountU400In();
	AccountU400Out();
	AccountIA();
	AccountIB();
	AccountIC();
	AccountIMax();
	if(IMax<732)
	CtIMax=80;


	ControlRegimError();
	if(RomReceive[3] & 1)
		{
	ControlRegimWork();

		}
	else
		{
	ControlRegimWorkDU();

		}
	SetAvaria();		




	 if(!CtErrorLink)
		{
//INIT TWI;
    TWBR=0;//F TWI
   TWAR=0;
   TWCR =0;
	TWSR=0xf8;

    TWBR=0x7f;//F TWI

	CtErrorLink=CtErrorLink0;
		}
    ReadKn();


    LoadRegTransmit();//++RegTransmit
   ReceiveTransmitMaster();




     DecSegm(PORTC);
    SetCursor(0,0);
    IndPar();
     DecSegm(RomReceive[4]);
    SetCursor(40,0);
    IndPar();
    DecSegm(RegimWork);
    SetCursor(80,0);
    IndPar();
     DecSegm(RegSPCH2);
    SetCursor(0,1);
    IndPar();
     DecSegm(RegStop);
    SetCursor(40,1);
    IndPar();
    DecSegm(Avaria);
    SetCursor(80,1);
    IndPar();
    Reg0=600;
    while(Reg0)
    --Reg0;

    }
  
}
SIGNAL(SIG_ADC)
{
    unsigned int RegInt0;
    unsigned char R0;
    RegInt0=ADC;
//	SPCR |=0x10;


	if(CtAd)
	{
	--CtAd;
	if(CtAd<=CtAd0)
		{  	
	if(RegInt0>AdTemp)
	AdTemp=RegInt0;
		}
	}
	else
	 {

	 ChangeNumberAd=1;
	 ++TestTimeAd;
	 AdResult[NumberAd]=AdTemp;
	 AdTemp=0;
	 CtAd=CtAd0+2;
	 if(NumberAd==1)
	 	{

	NumberAd=7;
		}
	else
	--NumberAd;

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

/* SIGNAL(SIG_INPUT_CAPTURE1)
{
 //   ACSR |=0x8;//enable interrupt comparator
    if(!CtOverLow)
    TGen=0xffff;
    else
    {
    TGenTemp[CtTGen]=ICR1-ICR1U;
    ICR1U=ICR1;
    }
    if(CtTGen)
    --CtTGen;
    else
    CtTGen=15;
    CtOverLow=2;

}*/

 SIGNAL(SIG_OVERFLOW1)/*26.2 Mc*/
{
 	unsigned char R0;
	if(CtLuk)
	--CtLuk;
	if(RegStop)
	++RegStop1;
	else
	RegStop1=0;

	if(CtK5)
	--CtK5;
	if(CtIMax)
	--CtIMax;
	else
	AvariaI |=1;
	if(CtUError)
	--CtUError;
	else
	AvariaI |=2;

	if(CtBegin)
	--CtBegin;
	for(R0=0;R0<=6;++R0)
		{
	if(CtErrorFar[R0])
	--CtErrorFar[R0];
	else
	ErrorFar |=(1<<R0);
		}


	if(CtErrorRS)
	--CtErrorRS;
	else
		{
	InitUsart();
	CtErrorRS=NumberBlok+39;
		}
	if(CtUsart)
	--CtUsart;
	else
		{


	TransmitUsart();
	CtUsart=(NumberBlok<<1)+15;
		}



    for(IndexInt=0;IndexInt<=7;++IndexInt)
    {
    if(RegimError & (1<<IndexInt))
        {
    if(CtError[IndexInt])
    --CtError[IndexInt];
        }
    else
    CtError[IndexInt]=CtError0[IndexInt];
    }
 



	if(CtErrorLink)
	--CtErrorLink;





}

	SIGNAL(SIG_UART_RECV)
{
	LinkRS();

}
   

