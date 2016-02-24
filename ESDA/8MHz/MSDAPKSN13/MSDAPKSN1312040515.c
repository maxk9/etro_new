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
    unsigned    char            TestRS;
    unsigned    char            TestRS0;
    unsigned    char            TestRS1;
    unsigned    char            TestRS2;
    unsigned    char            TestRS3;
    unsigned    char            RegimLuk;
    unsigned    char            ErrorLuk;
    unsigned    char            RegimError;
    unsigned    int            CtError[11];
    const unsigned    int      CtError0[11]={300,500,300,300,500,500,500,300,200,200,200};

    unsigned    char            CtTimeClose;
    unsigned    char            CtTimeOpen;
    const unsigned    char      CtTimeClose0=30;
    const unsigned    char      CtTimeOpen0=30;
    unsigned int Error;



    unsigned char CtErrorRS;
    const unsigned char CtErrorRS0=250;
    const unsigned char NumberBlok=0;
    unsigned char NumberReceiveRS;
    unsigned char ErrorLink;
    unsigned char CtUsart;


    unsigned char RegTransmitRS[9];
    unsigned char RamReceiveRS[9];
    unsigned char RomReceiveRS[9][8];

    unsigned char	ControlTransmitRS;
    unsigned char ControlReceiveRS;


	volatile unsigned char CtReceiveRS;

    const unsigned char CtReceiveRS0=8;//4;


    unsigned int URef;
	unsigned    char         	TM;
	unsigned    char         	TMRez;
    unsigned    char    		CtPlusTM;
    unsigned    char    		CtMinusTM;
    unsigned    int            AdTempMin;
    unsigned    int            AdTempMax;

	unsigned    int	AdResult;
    unsigned int CtStart;

    unsigned int RegInt0;
 	unsigned    char            Reg101;
 	unsigned    int             Reg100;



 
    unsigned    char RegimError;



	unsigned    int    Reg0;



 	unsigned    char            RomReceive[9][2];
 	unsigned    char            RegTransmit[12][2];
    unsigned    char            NumberLink;
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=10000;
    unsigned    char             CtErrorLink[2];
	const   unsigned    char     CtErrorLink0=20;    
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;

    unsigned    int	CtAd;

	static const unsigned    int	    CtAd0=1024;//3msec
 
//    unsigned    char	MyStatus;

	unsigned    char	RegS,RegSTemp,RegSOld;
       
    /*============================================*/
 

 
 
void    ReadKn(void)
    {
    unsigned    char R0;
    R0=0;

    if(bit_is_clear(PINA,PA3))//Luk3=open
    R0=R0 | 1;

    if(bit_is_clear(PINA,PA4))//Luk3=close
    R0=R0 | 2;

    if(bit_is_clear(PINA,PA5))//Luk2=close
    R0=R0 | 4;

    if(R0==RegSTemp)
		{

    RegS=R0;
		}
    RegSTemp=R0;
    }





void InitAd   (void)
    {
    ADMUX=0xc0;//2.56=Ref
    SFIOR=0;
 	CtAd=CtAd0;


	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
    ADCSRA |=(1<<ADPS1);

	ADCSRA |=(1<<ADPS0); /*32*/	 		
	ADCSRA |=(1<<ADIE);/* enable interrupt*/
    ADCSRA |=(1<<ADSC);/* Start*/
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
TT:    if(!WaitTWCR())
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

unsigned char    Receive(void)
    {
    if(!WaitTWCR())
    return 0;

    if((TWSR & 0xf8) == 0x8)//Well Start
            {
    TWDR=(NumberLink<<1)+5;//0x5;//SLA+R
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
    RegTransmit[0][0]=0;
    RegTransmit[0][1]=0;

    for(Reg0=1;Reg0<=10;++Reg0)
    {

    RegTransmit[0][0] +=RegTransmit[Reg0][0];

    RegTransmit[0][1] +=RegTransmit[Reg0][1];


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

    switch(NumberLink)
            {
    case 0:    for(Reg0=1;Reg0<=8;++Reg0)
    RomReceive[Reg0][0]=RamReceive[Reg0];
    CtErrorLink[0]=CtErrorLink0;
    CtError[9]=CtError0[9];
    break;

    case 1:    for(Reg0=1;Reg0<=10;++Reg0)
    RomReceive[Reg0][1]=RamReceive[Reg0];
    CtErrorLink[1]=CtErrorLink0;
    CtError[10]=CtError0[10];
    break;


    default:break;
			}
        }
    }
    else
    {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);


    }

} 

void	LoadRegimLukRevunPech(void)
	{
/*	if(RomReceiveRS[6][3] & 0x80)//Error from SHU,
	RegimLuk &=0xf8;//LUki=off
	else*/
		{

	if(RomReceiveRS[3][2] <=2)//SHU1 N<400
	RegimLuk &=0xf8;//Luki=off
	else
			{
	RegimLuk |=2;//LPrit1on L2
	if(TM>=85)//35gr
	RegimLuk |=4;//LPrit2=on L3
	else if(TM<=60)
	RegimLuk &=0xfb;//LPrit2=off
	if(RomReceiveRS[6][2] >=125)//T>=85
	RegimLuk |=1;//LR=on L1
	else if(RomReceiveRS[6][2] <=120)//T<=80
	RegimLuk &=0xfe;//LR=off

			}
		}

	if(RomReceiveRS[5][3] & 0x80)//Error from SHU,
	RegimLuk |=0x8;//Revun=on
	else if(Error & 0x76)//ErrorLuk,RS,Link
	RegimLuk |=0x8;//Revun=on
	else if(RomReceive[4][1] & 0x1e)//ErrorLuk D3
	RegimLuk |=0x8;//Revun=on
	else
	RegimLuk &=0xf7;//Revun=off

	if(TM<=65)//15gr
	RegimLuk |=0x10;//Pech=on
	else if(TM>=75)//25gr
	RegimLuk &=0xef;//Pech=off


	}

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



	CtError[NumberReceiveRS]=250;
	if(NumberReceiveRS != NumberBlok)
	CtErrorRS=39;

	R5=0;
	R5 |=1<<(NumberReceiveRS);
	R5=~R5;
	Error &=R5;

	for(R4=1;R4<=7;++R4)
					{
	RomReceiveRS[R4][NumberReceiveRS]=RamReceiveRS[R4];
	RamReceiveRS[R4]=2;
					}

				}

		}

	}
}

 

		  



	void	ResetError(void)
	{
	unsigned char R0;
	Error=0;
	for(R0=0;R0<=10;++R0)
	CtError[R0]=CtError0[R0];


	}			
	void	SetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=10;++R0)
		{
	if(!CtError[R0])
	Error |=(1<<R0);
	Error &=0x6ec;
		}


	}
    
	void CalcTM(void)
	{
	unsigned long R0;
	unsigned int R1;
	R0=AdResult;
	R0 *=URef;
	R0 >>=8;
	R1=R0;
	if(R1<400)
	RegimError |=0x80;
	else if(R1>950)
	RegimError |=0x80;
	else
	RegimError &=0x7f;
	if(R1>=535)
	R1=R1-535;
	else
	R1=0;
	R0=R1;
	R0 *=108;
	R0 >>=8;
	R1=R0;
	if(R1>TM)
		{
	CtMinusTM=255;

	if(CtPlusTM)
	--CtPlusTM;
	else
			{
	++TM;
	CtPlusTM=2;
			}
		}
	else if(R1<TM)
		{
	CtPlusTM=255;

	if(CtMinusTM)
	--CtMinusTM;
	else
			{
	CtMinusTM=2;
	--TM;
			}
		}
	if(TM>=50)
	TMRez=TM-50;
	else
		{
	TMRez=50-TM;
	TMRez +=128;
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
	void OpenLUK3(void)
	{
	PORTB |=0x10;
	if(RegS & 0x1)
	PORTB |=0x8;	 	
	else if(CtTimeClose)
	PORTB |=0x8;
	else if(Error & 0x20)
	PORTB |=0x8;			
	else
	PORTB &=0xf7;
	}	
	void CloseLUK3(void)
	{
	PORTB |=0x8;
	if(RegS & 0x2)
	PORTB |=0x10;	 	
	else if(CtTimeOpen)
	PORTB |=0x10;
	else if(Error & 0x40)
	PORTB |=0x8;			
	else
	PORTB &=0xef;
	}

		
	void	ControlLuk(void)
	{
	if(RegimLuk & 4)
	OpenLUK3();

	else
	CloseLUK3();
						
	}	

	
	void ChangeRegTransmitRS(void)
	{

	RegTransmitRS[1]=Error;
	RegTransmitRS[2]=RomReceive[4][1] & 0x1f;
	RegTransmitRS[3]=RomReceive[5][1] & 0x1f;				
	}
	void LoadIndicator(void)
	{
	unsigned char R0;
	unsigned char R1;
    RegTransmit[1][0]=2;
	R0=0;
	R1=0;
	if(Error & 4)
		{		
	R0=7;
	R1=1;
		}
	RegTransmit[2][0]=R0;//No link MSHU	    
	R0=0;
	if(Error & 8)
		{
	R0=6;
	R1=1;
		}
	RegTransmit[3][0]=R0;//No link SHU
	R0=0;
	if(Error & 0x60)
		{
	R0=5;
	R1=1;
		}
	RegTransmit[4][0]=R0;//Luk3
	R0=0;
	if(RomReceive[5][1] & 6)
		{
	R0=1;
	R1=1;
		}
    RegTransmit[5][0]=R0;//Luk1
	R0=0;
	if(RomReceive[5][1] & 0x18)
		{
	R0=2;
	R1=1;
		}
    RegTransmit[6][0]=R0;//Luk2
	R0=0;
	if(Error & 0x80)
		{
	R0=21;
	R1=1;
		}
    RegTransmit[7][0]=R0;//DT
	R0=0;
	if(Error & 0x600)
		{
	R0=8;
	R1=1;
		}
    RegTransmit[8][0]=R0;//Otkaz SN


	if(R1)
	R0=0;
	else if((RomReceiveRS[3][2]>2)&&(RomReceiveRS[1][3] & 2))//KG=on
	R0=4;//Rabota
	else
	R0=17;//Gotov
	RegTransmit[9][0]=R0;

    RegTransmit[10][0]=0;	
	}	

	void SetRegimErrorLuk(void)
	{
	if(!(PORTB & 8))
	RegimError |=0x20;
	else
	RegimError &=0xdf;
	if(!(PORTB & 0x10))
	RegimError |=0x40;
	else
	RegimError &=0xbf;
	}
	void LoadRegTransmit(void)
	{
	RegTransmit[1][1]=RegimLuk;	
	RegTransmit[2][1]=~RegimLuk;
	
	}
	void InitTWI(void)
	{
	unsigned int R0;
    TWAR=0;
    TWCR =0;
	TWSR=0xf8;
    TWBR=0x7f;//F TWI
    R0=65000;
	while(R0--)	_WDR();
	CtErrorLink[0]=CtErrorLink0;
	CtErrorLink[1]=CtErrorLink0;	
	}	
															   
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	DDRA=0;
	PORTA=0;
 
    DDRB=0xbc;
	PORTB |= 0xff;

    DDRC=0x1c;
 //   PORTC |=0xff;

    DDRD=0xba;
    PORTD=0xff;

	InitTWI();

//Init capture1
    TCCR1B=0x82;//0.5mkc,falling edge
    TIMSK=TIMSK | 0x4;//enable Int overlowT1

	InitUsart();



//  INIT SPI
    SPCR=0x72;//f/64
    SPSR=0;//f/64



    InitAd();


    MCUCR=0;


	TCCR0=5;// /1024

	ASSR=0;

	TCCR2=2;// T=0,5 mkc

	ASSR=0;





	NumberLink=1;

	CtUsart=NumberBlok;
	ResetError();
	URef=ReadEepromWord(8);

    _SEI();

					             
     while(1)
    {

     _WDR();
	CalcTM();
	LoadIndicator();
	LoadRegimLukRevunPech();
	SetRegimErrorLuk();
	ControlLuk();


	if(RomReceiveRS[5][3] & 4)
		{
	ResetError();
	RegTransmit[3][1] |=0x1;// set ResetError for D3
		}
	else
	RegTransmit[3][1] &=0xfe;// clear ResetError for D3

	SetError();

	ChangeRegTransmitRS();

	if((!CtErrorLink[0])||(!CtErrorLink[1]))
	InitTWI();	

    ReadKn();

    if(NumberLink)
    --NumberLink;
    else
    NumberLink=1;

    LoadRegTransmit();
    ReceiveTransmitMaster();






    }
  
}
SIGNAL(SIG_ADC)
{
    unsigned int RegInt0;
	unsigned char R1;

    RegInt0=ADC;


	if(CtAd)
	{
	--CtAd;
	if(CtAd<=CtAd0)
		{
	if(RegInt0<AdTempMin)
	AdTempMin=RegInt0;
	if(RegInt0>AdTempMax)
	AdTempMax=RegInt0;		

		}
	}
	else
	 {
	 AdResult=(AdTempMax+AdTempMin)>>1;
	 AdTempMin=0xffff;
	 AdTempMax=0;
	 CtAd=CtAd0+2;


	 }

	R1=ADMUX;
	R1 &=0xe0;
	 ADMUX = R1;

	 ADCSRA |=(1<<ADSC);
  	
}



 SIGNAL(SIG_OVERFLOW1)/*26.2 Mc*/
{

	unsigned char R0;
	++TestRS3;

	if(CtError[2])
	--CtError[2];//MSHU
	if(CtError[3])
	--CtError[3];//SHU


	for(R0=5;R0<=7;++R0)
		{
	if(RegimError &(1<<R0))
			{
	if(CtError[R0])
	--CtError[R0];
			}
	else
	CtError[R0]=CtError0[R0];
		}



	if(!(PORTB & 0x10))//Luk3=(off=on)
	CtTimeClose=CtTimeClose0;
	else if(CtTimeClose)
	--CtTimeClose;
	if(!(PORTB & 0x8))//Luk3=(on=on)
	CtTimeOpen=CtTimeOpen0;
	else if(CtTimeOpen)
	--CtTimeOpen;






	if(CtErrorLink[0])
	--CtErrorLink[0];
	if(CtErrorLink[1])
	--CtErrorLink[1];

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


}



	SIGNAL(SIG_UART_RECV)
{
												
	LinkRS();

}
