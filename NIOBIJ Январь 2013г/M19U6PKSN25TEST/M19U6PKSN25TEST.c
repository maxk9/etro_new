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


    static const unsigned  char  NumberLink=2;

    unsigned    char            CtTimeClose[9];
    unsigned    char            CtTimeOpen[9];
    const unsigned    char      CtTimeClose0=30;
    const unsigned    char      CtTimeOpen0=30;

    unsigned    int             Error;
    unsigned    char             Error1;
 	unsigned    int	CtError[16];
 	const unsigned    int CtError0=1200;
	unsigned    int	CtError1[8];
    unsigned int RegimError;
    unsigned char RegimError1;
	volatile    unsigned char CtErrorGlobal;
	const    unsigned char CtErrorGlobal0=30;

	volatile    unsigned char CtEnableKn;

    unsigned    char            EnableLoad;

	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;
 	unsigned    char            RomReceive[12];
 	unsigned    char            RegTransmit[12];
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=5000;//500;

    unsigned    char             CtErrorLink;
	const   unsigned    char     CtErrorLink0=10;

    unsigned int CtStart;
 


  

	unsigned    int	RegS;
	unsigned    int	RegSTemp;
	unsigned    int	RegSOld;
	unsigned    char	RegS1;
	unsigned    char	RegSTemp1;
	unsigned    char	RegSOld1;

       
    /*============================================*/


 	    void    ReadKn(void)
    {

    unsigned    int R0;


    R0=RegSTemp;//Temp;

    if(bit_is_clear(PINA,PA7))
    R0 |=0x1;
	else
    R0 &=0xfffe;
    if(bit_is_clear(PINA,PA6))
    R0 |=0x2;
	else
    R0 &=0xfffd;
    if(bit_is_clear(PINA,PA5))
    R0 |=0x4;
	else
    R0 &=0xfffb;
    if(bit_is_clear(PINA,PA4))
    R0 |=0x8;
	else
    R0 &=0xfff7; 
	if(bit_is_clear(PINA,PA3))
    R0 |=0x10;
	else
    R0 &=0xffef;
	if(bit_is_clear(PINA,PA2))
    R0 |=0x20;
	else
    R0 &=0xffdf;
	if(bit_is_clear(PINA,PA1))
    R0 |=0x40;
	else
    R0 &=0xffbf;
	if(bit_is_clear(PINA,PA0))
    R0 |=0x80;
	else
    R0 &=0xff7f;
    if(R0==RegSTemp)
		{
	RegSOld=RegS;
    RegS=R0;
		}
    RegSTemp=R0;

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
	CtErrorGlobal=CtErrorGlobal0;
    for(R1=1;R1<=10;++R1)
			{
    RomReceive[R1]=RamReceive[R1];
	RamReceive[R1]=1;
			}
	CtErrorLink=CtErrorLink0;
	RamReceive[0]=0xff;
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
    RegTransmit[7]=	RegS1;//RomReceive[6];
    RegTransmit[6]=Error1;
    RegTransmit[5]=Error>>8;
    RegTransmit[4]=Error;
	R0=RegS1;
	if(RegTransmit[1] !=0xaa)
	R0 &=0x7f;
	else if((RegTransmit[2] & 0xcf) !=0x8a)
	R0 &=0x7f;
	else if((RegTransmit[3] & 0x4f) !=0x4a)
	R0 &=0x7f;
	else
	R0 |=0x80;
    RegTransmit[3]=R0;

    RegTransmit[1]=RegS;//RegimError;//S;//RomReceive[1];//44;//RegS;
	R0=RomReceive[1];
	if(!R0)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;

		}
	else if(R0==1)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTB &=0x7;
	PORTD &=0xfe;
		}
	else if(R0==2)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTD &=0xf9;
		}
	else if(R0==3)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTB &=0x7;
	PORTD &=0xe7;
		}
	else if(R0==4)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTD &=0x9f;
		}
	else if(R0==5)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTB &=0x6;
	PORTD &=0x7f;
		}
	else if(R0==6)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTB &=0xf9;
		}
	else if(R0==7)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTB &=0x7;
	PORTC &=0x3f;
		}
	else if(R0==8)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTC &=0xcf;
		}
	else if(R0==9)
		{
	PORTB |=0xff;
	PORTD |=0xff;
	PORTC |=0xfc;
	PORTB &=0x7;
	PORTC &=0xf3;
		}
    RegTransmit[2]=RegS;

    } 

 
	void	ResetError(void)
	{
	unsigned char R0;
	CtErrorGlobal=CtErrorGlobal0;
	Error=0;
	Error1=0;
	for(R0=1;R0<=15;++R0)
	CtError[R0]=CtError0;
	for(R0=0;R0<=7;++R0)
	CtError1[R0]=CtError0;
	}
	void	SetError(void)
	{
	unsigned char R0;
	if(!CtErrorGlobal)
	Error |=1;
	for(R0=1;R0<=15;++R0)
		{
	if(!CtError[R0])
	Error |=(1<<R0);
		}
	for(R0=0;R0<=7;++R0)
		{
	if(!CtError1[R0])
	Error1 |=(1<<R0);
		}

	}

	void	SetRegimError(void)
	{
	if(!(PORTC & 8))//Luk1 on=on
	RegimError |=2;
	else
	RegimError &=0xfffd;
	if(!(PORTC & 4))//Luk1 off=on
	RegimError |=4;
	else
	RegimError &=0xfffb;

	if(!(PORTB & 0x80))//Luk2 on=on
	RegimError |=8;
	else
	RegimError &=0xfff7;
	if(!(PORTD & 1))//Luk2 off=on
	RegimError |=0x10;
	else
	RegimError &=0xffef;

	if(!(PORTD & 0x2))//LPCH1 on=on
	RegimError |=0x20;
	else
	RegimError &=0xffdf;
	if(!(PORTD & 4))//LPCH1 off=on
	RegimError |=0x40;
	else
	RegimError &=0xffbf;

	if(!(PORTD & 0x8))//LPCH2 on=on
	RegimError |=0x80;
	else
	RegimError &=0xff7f;
	if(!(PORTD & 0x10))//LPCH2 off=on
	RegimError |=0x100;
	else
	RegimError &=0xfeff;

	if(!(PORTD & 0x20))//LR1 on=on
	RegimError |=0x200;
	else
	RegimError &=0xfdff;
	if(!(PORTD & 0x40))//LR1 off=on
	RegimError |=0x400;
	else
	RegimError &=0xfbff;

	if(!(PORTD & 0x80))//LG1 on=on
	RegimError |=0x800;
	else
	RegimError &=0xf7ff;
	if(!(PORTB & 0x1))//LG1 off=on
	RegimError |=0x1000;
	else
	RegimError &=0xefff;

	if(!(PORTB & 0x2))//LR2 on=on
	RegimError |=0x2000;
	else
	RegimError &=0xdfff;
	if(!(PORTB & 0x4))//LR2 off=on
	RegimError |=0x4000;
	else
	RegimError &=0xbfff;

	if(!(PORTC & 0x80))//LG2 on=on
	RegimError |=0x8000;
	else
	RegimError &=0x7fff;
	if(!(PORTC & 0x40))//LG2 off=on
	RegimError1 |=0x1;
	else
	RegimError1 &=0xfffe;

	if((!(PORTC & 0x20)) &&(!(RegS1 & 4)))//Luk4 on=on
	RegimError1 |=0x2;
	else
	RegimError1 &=0xfffd;
	if(!(PORTC & 0x10))//Luk4 off=on
	RegimError1 |=0x4;
	else
	RegimError1 &=0xfffb;

	}
	




	
	
																						        
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	unsigned char Arg0;
 



//if DDRX,X=1 PORTX,X=OUT
    DDRA=0;
	PORTA = 0xff;//0xc0 prov

    DDRB=0xff;
	PORTB = 0xff;

    DDRC=0xfc;
    PORTC =0xff;

    DDRD=0xff;
    PORTD=0xff;

//INIT TWI;
    TWBR=0x7f;//F TWI
    TCCR1B=0xc2;//0.5mkc
    TIMSK=TIMSK | 0x4;//enable Int overlowT1
    TWAR=(NumberLink<<1)+4;   
    TWCR =(1<<TWEA)|(1<<TWEN);

   _SEI();



		
	for(Arg0=1;Arg0<=10;++Arg0)
	RomReceive[Arg0]=0;




	CtErrorLink=CtErrorLink0;

	CtEnableKn=7;
 	ResetError(); 
	RegS=0xffff;			
	RegSTemp=0xffff;
	RegS1=0xff;			
	RegSTemp1=0xff;	
	Error=0;
	Error1=0;	
		
//WORK	
	           
      while(1)
    {

     _WDR();

	if(RomReceive[3] & 1)
	ResetError();
	 SetRegimError();
	 SetError();



	if(EnableLoad)
		{
    LoadRegTransmit();
    LoadControlSum();
		}
	if(TWCR & 0x80)
			{
    ReceiveTransmitSlave();

			}
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
    TWAR=(NumberLink<<1)+4;
   TWCR =(1<<TWEA)|(1<<TWEN);
 //   TWCR |=(1<<TWINT);
	CtErrorLink=CtErrorLink0;
    CtStart=200;
	while(CtStart--)	_WDR();
		}

    }
  
}

 SIGNAL(SIG_OVERFLOW1)/*32.7 Mc*/
    {
	unsigned char R0;
	for(R0=1;R0<=15;++R0)
		{

	if(CtError[R0])
	--CtError[R0];
		}

	for(R0=0;R0<=2;++R0)
		{

	if(CtError1[R0])
	--CtError1[R0];

		}


	if(!(PORTC & 4))//Luk1=(off=on)
	CtTimeClose[0]=CtTimeClose0;
	else if(CtTimeClose[0])
	--CtTimeClose[0];
	if(!(PORTC & 8))//Luk1=(on=on)
	CtTimeOpen[0]=CtTimeOpen0;
	else if(CtTimeOpen[0])
	--CtTimeOpen[0];

	if(!(PORTD & 1))//Luk2=(off=on)
	CtTimeClose[1]=CtTimeClose0;
	else if(CtTimeClose[1])
	--CtTimeClose[1];
	if(!(PORTB & 0x80))//Luk2=(on=on)
	CtTimeOpen[1]=CtTimeOpen0;
	else if(CtTimeOpen[1])
	--CtTimeOpen[1];

	if(!(PORTD & 4))//LPCH1=(off=on)
	CtTimeClose[2]=CtTimeClose0;
	else if(CtTimeClose[2])
	--CtTimeClose[2];
	if(!(PORTD & 0x2))//LPCH1=(on=on)
	CtTimeOpen[2]=CtTimeOpen0;
	else if(CtTimeOpen[2])
	--CtTimeOpen[2];

	if(!(PORTD & 0x10))//LPCH2=(off=on)
	CtTimeClose[3]=CtTimeClose0;
	else if(CtTimeClose[3])
	--CtTimeClose[3];
	if(!(PORTD & 0x8))//LPCH2=(on=on)
	CtTimeOpen[3]=CtTimeOpen0;
	else if(CtTimeOpen[3])
	--CtTimeOpen[3];

	if(!(PORTD & 0x40))//LR1=(off=on)
	CtTimeClose[4]=CtTimeClose0;
	else if(CtTimeClose[4])
	--CtTimeClose[4];
	if(!(PORTD & 0x20))//LR1=(on=on)
	CtTimeOpen[4]=CtTimeOpen0;
	else if(CtTimeOpen[4])
	--CtTimeOpen[4];

	if(!(PORTB & 0x1))//LG1=(off=on)
	CtTimeClose[5]=CtTimeClose0;
	else if(CtTimeClose[5])
	--CtTimeClose[5];
	if(!(PORTD & 0x80))//LG1=(on=on)
	CtTimeOpen[5]=CtTimeOpen0;
	else if(CtTimeOpen[5])
	--CtTimeOpen[5];

	if(!(PORTB & 0x4))//LR2=(off=on)
	CtTimeClose[6]=CtTimeClose0;
	else if(CtTimeClose[6])
	--CtTimeClose[6];
	if(!(PORTD & 0x2))//LR2=(on=on)
	CtTimeOpen[6]=CtTimeOpen0;
	else if(CtTimeOpen[6])
	--CtTimeOpen[6];

	if(!(PORTC & 0x40))//LG2=(off=on)
	CtTimeClose[7]=CtTimeClose0;
	else if(CtTimeClose[7])
	--CtTimeClose[7];
	if(!(PORTC & 0x80))//LG2=(on=on)
	CtTimeOpen[7]=CtTimeOpen0;
	else if(CtTimeOpen[7])
	--CtTimeOpen[7];

	if(!(PORTC & 0x10))//Luk4=(off=on)
	CtTimeClose[8]=CtTimeClose0;
	else if(CtTimeClose[8])
	--CtTimeClose[8];
	if(!(PORTC & 0x20))//Luk4=(on=on)
	CtTimeOpen[8]=CtTimeOpen0;
	else if(CtTimeOpen[8])
	--CtTimeOpen[8];



	if(CtErrorGlobal)
	--CtErrorGlobal;
    if(CtErrorLink)
    --CtErrorLink;

	if(CtEnableKn)
	--CtEnableKn;
	else
		{
	CtEnableKn=7;

		}


	ReadKn();


    }


