/*
правлено для кварца 8Мгц
*/
 #include <compat/ina90.h>
 #include <avr/iom8535.h>
 #include <avr/signal.h>
 #include <math.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <string.h>
 #include <avr/pgmspace.h>
 
 
 volatile unsigned char MaskaError1;
 volatile unsigned char Error1;
 volatile unsigned int MaskaError;
 volatile unsigned int Error;
 volatile unsigned int TInd;
	const unsigned int TInd0=500;
 unsigned char MaskaZv;
 volatile unsigned int CtErrorLink;
 unsigned char ZnakTM;
 unsigned char ZnakTBap;
 volatile unsigned int CtMig;
 volatile unsigned char TestPr;
 volatile unsigned char TestLink[3];
 unsigned char SregTemp;
	unsigned int Par[17];
 volatile unsigned long CtKoefMoto;
 const unsigned long CtKoefMoto0=219720;//1757760;//29296;//60s 7324;//15s;
 volatile unsigned int CtMoto;
	const unsigned char CtTransmit0=2;
 volatile unsigned char TestA[6];
 volatile unsigned char ControlReceive;
 volatile unsigned char CtReceive0=16;
 volatile unsigned char ControlTransmit;
	const unsigned int CtLink0=10;
 volatile unsigned int CtLink;
	const unsigned char Table[10]={0X3f,0X6,0X5b,0X4f,0X66,
						0X6d,0X7d,0X7,0X7f,0X6f};
	const unsigned char TableZap[10]={0Xbf,0X86,0Xdb,0Xcf,0Xe6,
						0Xed,0Xfd,0X87,0Xff,0Xef};

 volatile unsigned char Segment[7];
 unsigned char CtAnod;
 unsigned char CtKn;

	unsigned char CtTimeInd;
	unsigned char CtNumberInd;
	unsigned int	IG;
	unsigned int	UG;
	unsigned int	FGen;

	unsigned int Reg0;

 volatile 	unsigned char RomReceive[17];
 volatile	unsigned char RegTransmit[3];

 volatile 	unsigned char RamReceive[17];
 
	unsigned char CtTransmit;
	unsigned char CtReceive;

	unsigned char CtInd;
	 const unsigned char	 CtInd0=8;

	unsigned char IndData[5];




	unsigned int	RegS,RegSTemp,RegSOld;
  
 /*============================================*/
 

 
 
void ReadKn(void)
 {

 unsigned int Reg2;
 Reg2=0;
 if(bit_is_clear(PINB,PB3))
 Reg2=Reg2 | 1;

 if(bit_is_clear(PINB,PB4))
 Reg2=Reg2 | 2;

 if(bit_is_clear(PINC,PC0))
 Reg2=Reg2 | 4;

 if(bit_is_clear(PINC,PC1))
 Reg2=Reg2 | 8;

 if(bit_is_clear(PINC,PC2))
 Reg2=Reg2 | 0x10;

 if(bit_is_clear(PINC,PC3))
 Reg2=Reg2 | 0x100;

 if(bit_is_clear(PINC,PC4))
 Reg2=Reg2 | 0x40;

 if(bit_is_clear(PINC,PC5))
 Reg2=Reg2 | 0x80;
 if(bit_is_clear(PINC,PC6))
 Reg2=Reg2 | 0x20;

 if(Reg2==RegSTemp)
		{
	if(CtKn)
	--CtKn;
	else
 RegS=Reg2;
		}
	else
	CtKn=5;


 RegSTemp=Reg2;

 
 }





void		DecSegm( unsigned int R0)
 {
 unsigned int	R1;
 R1=R0/10000;
	IndData[0]=R1;
 R0=R0-(R1*10000);
 R1=R0/1000;
	IndData[1]=R1;
 R0=R0-(R1*1000);
 R1=R0/100;
	IndData[2]=R1;
 R0=R0-(R1*100);
 R1=R0/10;
	IndData[3]=R1;
 R0=R0-(R1*10);
	IndData[4]=R0;
	
 }
 void WriteEeprom(unsigned char R0,unsigned char R1)
 {
 _WDR();
	++TestA[0];
 while(bit_is_set(EECR,EEWE))
 ;


	EEARL=R0;
	EEARH=0;
	EEDR=R1;

 _CLI();
	EECR |=(1<<EEMWE);
 		 
	EECR |=(1<<EEWE);
				
 while(bit_is_set(EECR,EEWE))
 ;


}
 void WriteEepromWord(unsigned char R0,unsigned int R1)
	
{
 WriteEeprom(R0,R1);
 R1=R1>>8;
 WriteEeprom((R0+1),R1);
}



 

 unsigned char ReadEeprom(unsigned char R0)
	
 {		 	
	_WDR();	

 EEARL=R0;
	EEARH=0;
 _CLI();
	EECR=EECR | (1<<EERE);

	while	(bit_is_set(EECR,EERE))
 ;
 _SEI();
	return EEDR;
 }

 unsigned int ReadEepromWord(unsigned char R0)
	
 {
 unsigned int R1;
 R1=ReadEeprom(R0+1);		 	
 R1=R1<<8;
 R1=R1 | ReadEeprom(R0); 	
 	return R1;
 }






void LoadRegTransmit(void)
 {

 RegTransmit[1]=CtInd;//RegS>>8; 
 RegTransmit[2]=RegS;




 } 

 void ChangeCtInd(void)
{
 if((RegS & 0x40) && (!(RegSOld & 0x40))) //нажата кнопка опроса
 {
	TInd=0;
	if(CtInd<7)
		{
 ++CtInd;
	if (CtInd==2) CtInd++; //пропуск показа давления топлива.
		}
	else
	CtInd=0;

 }
 RegSOld=RegS;
}





	void	Indicator(void)
	{
	unsigned char R0;
	unsigned int R1;
	unsigned int R2;
	
	PORTB |=2;

	if(CtAnod)
		--CtAnod;
	else
		CtAnod=6;

	R1=Segment[CtAnod];

	for(R0=0;R0<=7;++R0)
		{
		PORTB &=0xfb;
		if(R1 & 1)
		PORTB |=4;
		R1 >>=1;
		PORTB &=0xfe;
		PORTB &=0xfe;
		PORTB &=0xfe;
		PORTB |=1;
		}	

	R2=1;
	R2 <<=CtAnod;

	for(R0=0;R0<=7;++R0)
		{
		PORTB |=4;
		if(R2 & 1)
		PORTB &=0xfb;
		R2 >>=1;
		PORTB &=0xfe;
		PORTB &=0xfe;
		PORTB &=0xfe;
		PORTB |=1;
		}
	PORTB &=0xfd;	
	}	
	
	void	LinkStart(void)
	{
		unsigned char R0;
		R0=UDR;//UDR1;
		UCSRA &=0x22;//reset error
	 PORTD |=4;//Tran=enable
	
	 while(!(UCSRA & 0x20))
	 _WDR();
		UCSRB |=1;//9bit=1
	//	UCSRA |=1;//9bit=1
		CtLink=CtLink0;
		R0=RegTransmit[CtTransmit0];
		ControlTransmit=R0;
		CtTransmit=CtTransmit0;
	 UDR=R0;
	}	
	
	void InitUSART(void)
{
	UCSRA=2;//двойная скорость
	UCSRB=0x1c;//enable transmit,receive,9bit
	UCSRC=0x6;//8 bit 
	UBRRH=0;
	UBRRL=51;//207;//9600 51 - для кварца 8мГц
	PORTD |=0x4;//Tran=enable
	while(!(UCSRA & 0x20))
		_WDR();
	UCSRB |=0x80;//Interrupt Receive enable
	UDR=55;
}	
	
	void Link(void)
{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	unsigned char R3;

	if(UCSRA &(1<<RXC))
	{

	R1=UCSRA;
	R2=UCSRB;
	R3=UCSRC;
	R0=UDR;
	TestLink[CtTransmit]=R0;
	if(R1 & 0x1c)//error
		{
		LinkStart();
		return;
		}
	
	if(CtTransmit)//Transmit enable
		{
		--CtTransmit;
	
	 while(!(UCSRA & 0x20))
	 _WDR();
		UCSRB &=0xfe;//9bit=0
		UCSRA &=0xfe;//receive all data 
		if(!CtTransmit)
			{
			UDR=ControlTransmit;
			CtReceive=CtReceive0;
			}
		else
			{
			UDR=RegTransmit[CtTransmit];
			ControlTransmit +=RegTransmit[CtTransmit];
			}
		return;
		}
	else
		{
//Receive
//	TestLink[CtTransmit]=R0;

			PORTD &= 0xfb;//Transmit denable
			RamReceive[CtReceive]=R0;
			if(CtReceive)
			{
			if(CtReceive==CtReceive0)
				{
				ControlReceive =0;
				--CtReceive;
				}
			else if(CtReceive==1)
				{
				if(R0==ControlReceive)
					{
					if(CtErrorLink)
					CtErrorLink=500;
					for(R1=2;R1<=CtReceive0;++R1)
						RomReceive[R1]=RamReceive[R1];
					}
				--CtReceive;
				}
			else if(CtReceive<=(CtReceive0-1))
				{
				//	CtLink=CtLink0;
				ControlReceive +=R0;
				--CtReceive;
				++TestPr;
				}
			}


/*	else
			{
	PORTD |= 0x4;//Transmit enable
	CtTransmit=CtTransmit0;
	ControlTransmit=0;
	ControlReceive=0;
 while(!(UCSRA & 0x20))
 _WDR();
	UCSRB |=1;//9bit=1
 UDR=RegTransmit[CtTransmit0];
	ControlTransmit =RegTransmit[CtTransmit0];
	
			}*/
		}
	}
}		

	void SaveCtMoto(void)
	{
	unsigned int R0;
	R0=ReadEepromWord(2);
	if(R0 != CtMoto)
		WriteEepromWord(2,CtMoto);
	}

	void LoadPar(void)
	{
	unsigned char R0;
	R0=RomReceive[10];//TM;
	if(R0>=30)
		{
		R0=R0-30;
		ZnakTM=0;//+
		}
	else
		{
		R0=30-R0;
		ZnakTM=1;//-
		}

	Par[0]=R0;//TM;
	Par[1]=RomReceive[9];//DM;pr0v
	Par[2]=RomReceive[8];//DT;
	R0=RomReceive[13];//TBap;
	if(R0 !=0)
		{
		ZnakTBap=0;//+
		}
	else
		{
		ZnakTBap=1;//-
		R0=15;
		}
	Par[3]=R0;//TBap;	
	Par[4]=(RomReceive[5]<<8) | RomReceive[4];//UGen
	Par[5]=(RomReceive[7]<<8) | RomReceive[6] ;//IGen;
	Par[6]=(RomReceive[12]<<8) | RomReceive[11];//NDiz
	Par[7]=CtMoto>>3;
	
	}	
	
		
	void IndPar(unsigned char R0)
	{
	volatile unsigned char R1;
	volatile unsigned char R2;
	const unsigned char TabSimvol0[8]={0x3f,0x3f,0x71,0x39,0x3e,0x30,0x71,0x76};
	const unsigned char TabSimvol1[6]={0x63,0x73,0x73,0x63,0,0};

	Segment[0]=TabSimvol0[R0];
	if(R0<=5)
		Segment[1]=TabSimvol1[R0];
	if((R0==1)||(R0==2))
		Segment[2]=0;
	if((!R0) && (ZnakTM))
		Segment[2]=0x40;
	if((R0==3) && (ZnakTBap))
		Segment[2]=0x39;
		
	switch(CtInd)
		{
		case 0:if(ZnakTM)
				R2=3;
				else
				R2=2;
		break;
		case 1:R2=3;
		break;
		case 2:R2=3;
		break;
		case 3:if(ZnakTBap)
				R2=3;
				else
				R2=2;
		break;
		case 4:R2=2;
		break;
		case 5:R2=2;
		break;
		case 6:R2=1;
		break;
		case 7:R2=1;
		break;
		default: R2=2;
		break;
		}

	DecSegm(Par[CtInd]);	
	for(R1=R2;R1<=4;++R1)
		{
		if(R1==3)
			{
			if((CtInd==1)||(CtInd==2)||(CtInd==4))
				Segment[R1]=TableZap[IndData[R1]];
			else
				Segment[R1]=Table[IndData[R1]];
			}
		else
			Segment[R1]=Table[IndData[R1]];
		}
	}	
	
	void IndLed(void)
	{
	unsigned char R0;
	unsigned char R1;
	R0=RomReceive[14];
	R1=RomReceive[15];
	if(Error1 & 1)
		R0 |=0x40;
	else
		R0 &=0xbf;
		
	R1 &=0xbf;
	if(RegS & 0x80)
		R1 |=0x40;
	if((Error & 0x200) && (!(Error & 0x900)))
		{
		if(CtMig>250)
			R0 |=1;
		else
			R0 &=0xfe;
		}
	if(Error & 0x80)
		{
		if(CtMig>250)
			R1 |=1;
		else
			R1 &=0xfe;
		}
	if(Error & 0x10)
		{
		if(CtMig>250)
			R1 |=2;
		else
			R1 &=0xfd;
		}
	if(Error & 0x20)
		{
		if(CtMig>250)
			R1 |=0x10;
		else
			R1 &=0xef;
		}
	if((RegS & 0x140)==0x140)
		{
		Segment[5]=0xff;
		Segment[6]=0xff;
		}
	else
		{
		Segment[5]=R0;
		Segment[6]=R1;
		}
	}

	void ResetError(void)
	{
	if(RegS & 0x10)	
		CtErrorLink=1000;
	}
	
	void ControlZvonok(void)
	{
	Error=RomReceive[3];
	Error=Error<<8;
	Error |=RomReceive[2];

	if(!CtErrorLink)
		Error1 |=1;
	else
		Error1 &=0xfe;

	if(RegS & 0x100)
		{
		MaskaError=Error;
		MaskaError1=Error1;
		}
	if(!Error)
		MaskaError=0;
	if(!Error1)
		MaskaError1=0;
	if((Error>MaskaError) || (Error1>MaskaError1))
		PORTC |=0x80;
	else
		PORTC &=0x7f;
	}
	
												 
/*++++++++++++++++++++++++++++++++++++++++++*/

 int main(void)
{
//if DDRX,X=1 PORTX,X=OUT
 DDRA=0;
	PORTA = 0xff;
 DDRB=7;
	PORTB = 0xff;

 DDRC=0x80;
 PORTC =0x7f;

 DDRD=0xe;
 PORTD=0xff;


 TCCR1B=0x1;//0.5mkc,falling edge
 TIMSK=TIMSK | 0x4;//enable Int overlowT1
// TIMSK=TIMSK | 0x20;//enable Int capture1 prov

	InitUSART();
	CtKoefMoto=CtKoefMoto0;
	CtMoto=ReadEepromWord(2);

	ReadKn();
 RegSOld=RegS;
 _SEI();
	RomReceive[10]=10;
	RomReceive[9]=9;
	RomReceive[8]=8;
	CtMig=500;
	CtErrorLink=500;
	MaskaZv=0;
	TInd=TInd0;
		 
 while(1)
 {
	ResetError();
	SaveCtMoto();
	ChangeCtInd();
	ReadKn();
	LoadRegTransmit();
	LoadPar();

	if(!CtErrorLink)
		{
		Segment[0]=0x40;
		Segment[1]=0x7d;
		Segment[2]=0x77;
		Segment[3]=0x37;
		Segment[4]=0x40;
		Segment[5]=0x40;
		Segment[6]=0;
		}
	else
		{
		if(Error & 0x800)
			CtInd=0;
		
		if((RegS & 0x140)==0x140)
			{
			Segment[0]=0xff;
			Segment[1]=0xff;
			Segment[2]=0xff;
			Segment[3]=0xff;
			Segment[4]=0xff;
			}
		else if(!TInd)
			{
			IndPar(CtInd);
			TInd=TInd0;
			}
		IndLed();
		}
	ControlZvonok();
 _WDR();
 }
}




 SIGNAL(SIG_OVERFLOW1)//4.096 Mc
{
	if(TInd)
		--TInd;

	if(CtErrorLink)
		--CtErrorLink;

	if(CtMig)
		--CtMig;
	else
		CtMig=500;
	
	TCNT1=0x8000;
	if(CtKoefMoto)
		--CtKoefMoto;
	else
	{
	CtKoefMoto=CtKoefMoto0;
	if(CtMoto<8192)
		++CtMoto;
	else
		CtMoto=0;
	}

	Indicator();
	if(CtLink)
		--CtLink;
	else
	{
	PORTD &=0xf7;
	LinkStart();
	CtLink=CtLink0;
	}
}

	SIGNAL(SIG_UART_RECV)
{
	Link();
	PORTD |=8;

}
 

