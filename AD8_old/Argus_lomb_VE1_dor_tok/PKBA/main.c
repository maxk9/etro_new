/*
 * pkba.c
 *
 * Created: 19.12.2013
 *  Author: kopusov

ArgusBA 8квт
с двиг Lombardgini
переделка на ВЕ1 проц 8 мгц
число зубьев 108 шт
 WDTON CKOPT
BOODLEVEL BODEN SUT1 CKSEL2
переделка алг подсчета частоты

*/
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <math.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <ctype.h>
	#include <string.h>
	#include <avr/pgmspace.h>
	#include <avr/wdt.h>


//**********************************
#define trans_toka
//#define PLATA950214 //с делителем по 4х2к
//#define PLATA1312131	//kapsul
//#define ST23613145//kompensac temper
//#define RUBCOVCK12
//**********************************
//**********************************
// #define PLATA131213	//делитель тока 3.2
//**********************************

 #ifdef PLATA131213
 	#warning "Delit toka 3.2"	//делитель тока 3.2
 #endif
  #ifdef PLATA0930214
 	#warning "Delit toka 3.0"	//делитель тока 3.2
 #endif

	volatile unsigned int RabReg0;
	//unsigned int DeltaI;
	volatile unsigned int CtErrorBap;
	const unsigned int CtErrorBap0=99;
	unsigned long IGenSumTemp;
	unsigned int CtKalib;
	const unsigned int CtKalib0=300;
	unsigned char EnableKoef;
	// 	unsigned char	Led1Mig;
	unsigned char	RegLed1;
	unsigned char	Led1;
	unsigned char	Led2;
	unsigned char	CtPusk;
	unsigned char	RegimWork;
	//volatile unsigned char	CtIntT1;

	volatile unsigned int	Error;
	volatile unsigned int	CtError[16];

	/*volatile const unsigned int	CtError0[16]={20,80,30,30,100,	//для 16мГц
											50,20,600,20,20,
											20,20,2500,20,20,50};//1=0.1s
											*/
volatile const unsigned int	CtError0[16]={20,80,25,25,100,	//8мГц
											45,20,500,20,20,
											100,20,2000,20,20,20};//1=0.1s

volatile unsigned int CtTime[16];
	/*volatile const unsigned int	CtTime0[16]={30,100,20,100,100, //16МГц
											40,20,6000,3000,150,
											50,6000,100,100,100,20};
		*/
volatile const unsigned int	CtTime0[16]={20,95,15,95,95,	//8Мгц
											50,15,5681,1500,75,
											50,3000,95,95,95,30};


	volatile unsigned int	RegimTime; //
	volatile unsigned int	RegimError;
	unsigned char	SregTemp;
	unsigned char	KoefEeprom[8];
/*	const unsigned char		KoefMax[8]={80,10,100,20,70,200,200,145};
	//const unsigned char		KoefMax[8]={80,99,100,200,70,200,200,200};
	const unsigned char		KoefMin[8]={1,  1,20, 0,  30,50,  2, 105};
	const unsigned char		Koef0[8]={ 30,  1,44,10, 51,64,2,145};
*/

	const unsigned char		KoefMax[8]={80,	10,	100,20,	70,25,254,	145};
	//const unsigned char		KoefMax[8]={80,99,100,200,70,200,200,200};
	const unsigned char		KoefMin[8]={1,	1,	20,	0,	30,	0, 	0,	105};
	const unsigned char		Koef0[8]={	30,	1,	44,	10,	47,	16,	0,	145};


	//unsigned char IKoefEeprom[6];

	//const unsigned char IKoef0[6]={120,140,120,150,110,127};
	// Формула для расчета поправки - нулевая поправка при значении ячейки 127.
	// Начальные значения поправок. 1. при 30гр, 2 при 40, и так далее до 80 с шагом 10.
	// при температуре 20гр. поправка считается нулевой, так как в этой точке калибруется 0 ток при комнатной температуре.
 // нулевая поправка при значении в массиве=127.

	unsigned char	Kalib;
	unsigned int	UGen;
	unsigned int	IGen;
	unsigned char	DT;
	unsigned char	DM;
	unsigned char	DMT;

	unsigned char	TM;
	unsigned char	TBap;
	const unsigned char	TBap85=85;

	unsigned char	CtTest[3];
	/*
	последовательность
	дифф ток ADC1 ADC0 x10
	ADC2 - напряж питания +5В
	ADC3 - ДТМ
	ADC4 - ДДМ
	ADC5 - темп БАП
	ADC6 - ДДТ
	ADC7 - Uг
	*/
	#define MAX_AD_CH 6 //посл индекс канала ADC
	const unsigned char NumberAdNew[7]={0,2,3,4,5,6,7}; //последовательность каналов АЦП
	unsigned char	MyStatus;
	unsigned char	NumberAd;
	unsigned char	CtAd;
	#define 	 	CtAd0 64
	unsigned int	AdTemp;
	unsigned int 	I_Arr[64];
	volatile unsigned char read=0;
 	unsigned int	AdResult[MAX_AD_CH+1];//значения АЦП по каналам

	unsigned char CtTDiz;
	unsigned char CtTmr1;
	unsigned long TDiz;
	unsigned int TDizTemp[16];

	unsigned int NDiz;
	unsigned int ICR1Old;
	unsigned char ControlTransmit;
	unsigned char ControlReceive;

	unsigned char CtErrorLink;

	//const unsigned char CtErrorLink0=20;
	const unsigned char CtErrorLink0=40;
	volatile unsigned char RegTransmit[17];
	volatile unsigned char RamReceive[6];
	volatile unsigned char RomReceive[6];
	volatile unsigned char CtReceive;
	const unsigned char CtReceive0=2;
	volatile unsigned char CtTransmit;
	const unsigned char CtTransmit0=16;
	unsigned int RegWait;

	volatile unsigned char	RegS,RegSOld;

 /*============================================*/


void ReadKn(void)
	{
		static unsigned char RegSTemp=0;
		unsigned char R0=0;
		
		
		//R0=~PINB;
		//R0 &=0b10001110;
		if(bit_is_set(PINB,PB0))//сраб ДВФ
			R0 |=1;
		if(bit_is_clear(PIND,PD6))//S3 stop
			R0 |=2;
		if(bit_is_clear(PINB,PB2))//TM
			R0 |=4;
    	if(bit_is_clear(PINB,PB3))//ND
    		R0 |=8;
		if(bit_is_clear(PINB,PB5))//МУ
			R0 |=0x10;
		if(bit_is_clear(PINB,PB6))//S1
			R0 |=0x20;
		if(bit_is_clear(PINB,PB7))//S2
			R0 |=0x40;

		if(R0==RegSTemp)
			{
			//RegS&=(R0|0b11111110);
			RegS=R0;
			}
		RegSTemp=R0;
	}

void InitUsart(void)
	{

		UCSRA=2;//двойная скорость
		UCSRA |=1;//only 9bit=1 multiproc mode
		UCSRB=0x1c;//11100 enable transmit 9bit
		UCSRC=6;//110   9 bit
		UBRRH=0;
		UBRRL=103;//207;//9600

		PORTD &=0b11111011;//11111011 Out Transmit=off
		CtErrorLink=CtErrorLink0;
	}

void LinkPult(void)
	{
	volatile unsigned char R0;
	volatile unsigned char R1;
	volatile unsigned char R2;
	volatile unsigned char R3;

	if(UCSRA &(1<<RXC))//RX complet
		{
		R1=UCSRA;
		R2=UCSRB;
		R3=UCSRC;
		R0=UDR;

		if(R1 & 0x1c)//error
			{
			return;
			}
		if(R2 & 2)//adres
			{
			CtReceive=CtReceive0;
			RamReceive[CtReceive]=R0;
			CtTransmit=CtTransmit0;
			ControlReceive=R0;
			ControlTransmit=0;
			--CtReceive;
			UCSRA &=0xfe;//11111110 сбрасываем MPCM
			}
		else
		//Receive Data
			{
			if(CtTransmit==CtTransmit0)
				{
				RamReceive[CtReceive]=R0;
				if(!CtReceive)
					{
					if(R0==ControlReceive)
						{
						CtErrorLink=CtErrorLink0;
						for(R1=1;R1<=5;++R1)
							RomReceive[R1]=RamReceive[R1];
						}
					--CtTransmit;

					PORTD |=0x4;//enable transmit
					while(!(UCSRA & 0x20))
						wdt_reset();
					UCSRB  &=0xfe;//9bit=0
					UDR=RegTransmit[CtTransmit];
					ControlTransmit=RegTransmit[CtTransmit];
					--CtTransmit;
					}
				else
					{
				//CtReceive !=0
					ControlReceive +=R0;
					--CtReceive;
					}
				}
			else
				if(CtTransmit)
				{
				 while(!(UCSRA & 0x20))
					wdt_reset();
				UCSRB &=0xfe;//9bit=0

				if(CtTransmit==1)
					UDR=ControlTransmit;
				else
					{
					UDR=RegTransmit[CtTransmit];
					ControlTransmit +=RegTransmit[CtTransmit];
					}
				--CtTransmit;
				}
			else
				{
				UCSRA |=1;
				PORTD &=0b11111011;//denable transmit
				CtTransmit=CtTransmit0;
				}
			}
		}
	}

void InitAd(void)
	{

#if defined(PLATAxxxxxx)
#warning "Vref int"
	ADMUX=0b11001011;//11001011 2.56 V diff input ADC1 ADC0 x200
#else
	ADMUX=0b00000000;//11001011 2.56 V diff input ADC1 ADC0 x200
	#warning "external Vref"
#endif
//#if defined(PLATA131213)||defined(PLATA0930214)
//#warning "external Vref"
//	ADMUX=0b00000000;//11001011 2.56 V diff input ADC1 ADC0 x200
//#else
//	#error "Vref undefined"
//#endif

	CtAd=CtAd0;//CtAd0=64

	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
	ADCSRA |=(1<<ADPS1);
	ADCSRA |=(1<<ADPS0); /*128*/
	ADCSRA |=(1<<ADSC);/* Start*/
	}

unsigned char ReadEeprom(unsigned char Arg6)
	{
	EEARH=0;
	EEARL=Arg6;
	EECR=EECR | (1<<EERE);

	while(bit_is_set(EECR,EERE));

	return EEDR;
	}

void AccountUGen(void)
	{
	unsigned int R0;
	unsigned int R1;

	UGen=AdResult[6];
	UGen >>=8;
	//UGen >>=2;
	R0=KoefEeprom[4];
	R1=UGen * R0; //
	R1 >>=7;
	UGen +=R1;
	}

//На основе трансформатора тока

#ifdef trans_toka //на плате делитель по 4х2к
 void AccountIGen(void)
	{
	static unsigned long sumI=0;//,sqI=0;
	static unsigned int tmp_i=0;
	//unsigned int lastSampleI=0,sampleI=0;
	//unsigned long 	filteredI=0,lastFilteredI=0;
	unsigned int i=0;
	//KoefEeprom[6] уст 0

	if(read)
		{
			while(i<(64))
			{
				/*
				lastSampleI = sampleI;
				sampleI = I_Arr[i];
				lastFilteredI = filteredI;
				filteredI = 99*(lastFilteredI+sampleI-lastSampleI);
				filteredI/=100;
		*/
			// Root-mean-square method current
			// 1) square current values
				//sqI = filteredI * filteredI;
			// 2) sum
				//sumI += sqI;
				//sumI += filteredI;
				sumI += I_Arr[i];
				i++;
			}
			read=0;

			++tmp_i;
			if(tmp_i>4)
				{
					sumI/=(320);
					sumI*=233;
					sumI/=100;
					sumI*=KoefEeprom[5];
					sumI>>=4;
					IGen=sumI;
					tmp_i=0;
				}
			//double I_RATIO = ICAL *((SUPPLYVOLTAGE/1000.0) / (ADC_COUNTS));
			//Irms = I_RATIO * sqrt(sumI / CtAd0);
			//sumI = sqrt(sumI);
			//sumI>>=3;


		}
	}
#endif
 //Без компенсации температурного дрейфа
//#define TBAP20C	63	//значение АЦП при 50%нагрузке 156А при 20С
//50-86 значение АЦП при 0 нагрузки при -25С (с учетом формул)
#ifdef PLATA950214 //на плате делитель по 4х2к
 void AccountIGen(void)
	{
	unsigned long R0,R1;
	//unsigned int tmp_I=0;
	static unsigned int prev_R0=0;
	static unsigned int prev_V=0;
	static unsigned char i=0;
	//KoefEeprom[6] уст 0

	R0=AdResult[0]>>6;//AdResult int 255 max

	if(i>7)
		{
		i=0;
		prev_R0>>=3;
		R0=prev_R0;//937-960
		prev_V>>=3;
		prev_V=0;
		/*
		937 1110101001
		960 1111000000
		*/
		prev_R0=0;


		//зависм от ++25С
		if((TBap>25)&&(R0>KoefEeprom[6]))
			{
				R1=17*TBap-375;
				R1/=100;
//				if(R0>KoefEeprom[6])//зависим от тока
//				{
//					R1*=R0;
//					tmp_I=270/KoefEeprom[5];
//					tmp_I>>=3;
//					R1/=(tmp_I);
//					R0+=R1;
//				}
				R0+=R1;
			}

		if(R0>KoefEeprom[6])
			R0-=KoefEeprom[6];//уст 0
		else
			R0=0;

		R0*=KoefEeprom[5];//уст макс
		R0>>=3;

		if((R0>70)&&(R0<220))//учитываем провал
			{
				R1=4000-14*R0;
				R1/=100;
				R0-=R1;
				//R0-=30;
			}

		//зависим от -
		R1=AdResult[4]>>8;//TBAp
		if(R1>203)
			{
				R1=R1*13;
				R1-=2645;
				R1=R1/10;
			}
		else
			R1=0;

		if(R0>R1)
			R0-=R1;

		IGen=R0;
		}
		else
			{
				prev_R0+=R0;
				prev_V+=UGen;
				++i;
			}

//	IGen=R0;



//		IGen=5*R0+95*prev_R0;
	//	IGen/=100;
//		prev_R0=IGen;

	}
#endif
#ifdef PLATA1312131
 void AccountIGen(void)
	{
	unsigned long R0,R1;
	//unsigned int tmp_I=0;
	static unsigned int prev_R0=0;
	static unsigned int prev_V=0;
	static unsigned char i=0;
	//KoefEeprom[6] уст 0

	R0=AdResult[0]>>6;//AdResult int 255 max

	if(i>7)
		{
		i=0;
		prev_R0>>=3;
		R0=prev_R0;//937-960
		prev_V>>=3;
		prev_V=0;
		/*
		937 1110101001
		960 1111000000
		*/
		prev_R0=0;

/*
		//зависм от ++25С
		if((TBap>25)&&(R0>KoefEeprom[6]))
			{
				R1=15*TBap-375;
				R1/=100;
//				if(R0>KoefEeprom[6])//зависим от тока
//				{
//					R1*=R0;
//					tmp_I=270/KoefEeprom[5];
//					tmp_I>>=3;
//					R1/=(tmp_I);
//					R0+=R1;
//				}
				R0+=R1;
			}
*/
		if(R0>KoefEeprom[6])
			R0-=KoefEeprom[6];//уст 0
		else
			R0=0;

		R0*=KoefEeprom[5];//уст макс
		R0>>=3;

		if((R0>70)&&(R0<220))//учитываем провал
			{
				R1=4000-14*R0;
				R1/=100;
				R0-=R1;
				//R0-=30;
			}

		//зависим от -
		R1=AdResult[4]>>8;//TBAp
		if(R1>203)
			{
				R1=R1*13;
				R1-=2645;
				R1=R1/10;
			}
		else
			R1=0;

		if(R0>R1)
			R0-=R1;

		IGen=R0;
		}
		else
			{
				prev_R0+=R0;
				prev_V+=UGen;
				++i;
			}

//	IGen=R0;



//		IGen=5*R0+95*prev_R0;
	//	IGen/=100;
//		prev_R0=IGen;

	}
#endif

#ifdef ST23613145//темпер компенсац с старым делителем
 void AccountIGen(void)
	{
	unsigned long R0,R1;
	//unsigned int tmp_I=0;
	static unsigned int prev_R0=0;
	static unsigned int prev_V=0;
	static unsigned char i=0;
	//KoefEeprom[6] уст 0

	R0=AdResult[0]>>6;//AdResult int 255 max

	if(i>7)
		{
		i=0;
		prev_R0>>=3;
		R0=prev_R0;//937-960
		prev_V>>=3;
		prev_V=0;
		/*
		937 1110101001
		960 1111000000
		*/
		prev_R0=0;


		//зависм от ++25С
		if((TBap>25)&&(R0>KoefEeprom[6]))
			{
				R1=21*TBap-375;
				R1/=100;
//				if(R0>KoefEeprom[6])//зависим от тока
//				{
//					R1*=R0;
//					tmp_I=270/KoefEeprom[5];
//					tmp_I>>=3;
//					R1/=(tmp_I);
//					R0+=R1;
//				}
				R0+=R1;
			}

		if(R0>KoefEeprom[6])
			R0-=KoefEeprom[6];//уст 0
		else
			R0=0;

		R0*=KoefEeprom[5];//уст макс
		R0>>=3;

		if((R0>70)&&(R0<220))//учитываем провал
			{
				R1=4000-14*R0;
				R1/=100;
				R0-=R1;
				//R0-=30;
			}

		//зависим от -
		R1=AdResult[4]>>8;//TBAp
		if(R1>203)
			{
				R1=R1*13;
				R1-=2645;
				R1=R1/10;
			}
		else
			R1=0;

		if(R0>R1)
			R0-=R1;

		IGen=R0;
		}
		else
			{
				prev_R0+=R0;
				prev_V+=UGen;
				++i;
			}

//	IGen=R0;



//		IGen=5*R0+95*prev_R0;
	//	IGen/=100;
//		prev_R0=IGen;

	}
#endif


#ifdef RUBCOVCK12
 void AccountIGen(void)
	{
	unsigned long R0,R1;
	unsigned int tmp_I=0;
	static unsigned int prev_R0=0;
	static unsigned int prev_V=0;
	static unsigned char i=0;
	//KoefEeprom[6] уст 0
		R0=AdResult[0]>>6;//AdResult int 1024 max
		//R1=AdResult[6]>>6;//Uг

	if(i>7)
		{
		i=0;
		prev_R0>>=3;
		prev_V>>=3;
		R0=prev_R0;
		prev_R0=0;
	//	IGen=prev_R0;


		//зависм от ++25С
		if(R0>KoefEeprom[6])
			R0-=KoefEeprom[6];//уст 0
		else
			R0=0;

		if((TBap>25)&&(R0>KoefEeprom[6]))
			{
				R1=20*TBap-375;
				R1/=100;
				if(R0>KoefEeprom[6])
					{
						R1*=R0;
						tmp_I=270/KoefEeprom[5];
						tmp_I>>=3;
						R1/=(tmp_I);
						R0+=R1;
					}
			}

		if(prev_V>286)//28.5V
			{
				R1=prev_V-285;
				R1*=6;
				if(R0>R1)
					R0-=R1;
				else
					R0=0;
			}

		R0*=KoefEeprom[5];//уст макс
		R0>>=3;

		if((R0>70)&&(R0<220))
			{
				R1=4000-14*R0;
				R1/=100;
				R0-=R1;
				//R0-=30;
			}

		//зависим от -
		R1=AdResult[4]>>8;//TBAp
		if(R1>203)
			{
				R1=R1*13;
				R1-=2645;
				R1=R1/10;
			}
		else
			R1=0;

		if(R0>R1)
			R0-=R1;


		prev_V=0;
		IGen=R0;
		}
		else
			{
				prev_R0+=R0;
				prev_V+=UGen;
				++i;
			}

//	IGen=R0;



//		IGen=5*R0+95*prev_R0;
	//	IGen/=100;
//		prev_R0=IGen;

	}
#endif

#ifdef PLATA131213
 void AccountIGen(void)
	{
	unsigned int R0,R1;
	static unsigned char cnt_i=0;
	static int tmp_I=0;

	//KoefEeprom[6] уст 0

		R0=AdResult[0]>>6;//AdResult int
		R1=AdResult[6]>>6;//Uг

		if(R0>870)//зависимость от Uг
			{
			R0-=870;

				if(R1<817)//28.5V
				{
					R1=817-R1;
					R0+=R1;
				}
				else
				{
					R1=R1-817;
					R0-=R1;
				}
			}
		else
			R0=0;

		//переводим в ток
		if(R0>31)
			{
			R0=65*R0-2027;
			R0=R0/10;
			R0=R0*KoefEeprom[5];
			R0=R0>>6;
			}
			else R0=0;

	//зависим от -
	R1=AdResult[4]>>8;//TBAp
	if(R1>203)
		{
			R1=R1*13;
			R1-=2645;
			R1=R1/10;
		}
	else
		R1=0;

	if(R0>R1)
		R0-=R1;

	if(tmp_I==R0)
		{
		++cnt_i;
		if(cnt_i==1)
			{
				IGen=R0;
				cnt_i=0;
			}
		}
		else
			{
				cnt_i=0;
				tmp_I=R0;
			}
	}
#endif

#ifdef PLATA0930214
 void AccountIGen(void)
	{
	unsigned int R0,R1;
	static unsigned char cnt_i=0;
	static int tmp_I=0;

		R0=AdResult[0]>>6;//AdResult int
		R1=AdResult[6]>>6;//Uг

		if(R0>870)//зависимость от Uг
			{
			R0-=870;

				if(R1<817)//28.5V
				{
					R1=817-R1;
					R0+=R1;
				}
				else
				{
					R1=R1-817;
					R0-=R1;
				}
			}
		else
			R0=0;

		//переводим в ток
		if(R0>74)
			{
			R0=60*R0-4468;
			R0=R0/10;
			R0=R0*KoefEeprom[5];//64
			R0=R0>>6;
			}
			else R0=0;

	//зависим от -
	R1=AdResult[4]>>8;//TBAp
	if(R1>203)
		{
			R1=R1*13;
			R1-=2645;
			R1=R1/10;
		}
	else
		R1=0;

	if(R0>R1)
		R0-=R1;

		if(tmp_I==R0)
		{
		++cnt_i;
		if(cnt_i==1)
			{
				IGen=R0;
				cnt_i=0;
			}
		}
		else
			{
				cnt_i=0;
				tmp_I=R0;
			}
	}
#endif


unsigned char AccountP(void)
	{
	unsigned int R0;
	unsigned int R1;
	unsigned char R4;

	R0=AdResult[3]>>8;
	//R0=AdResult[3]>>2;
	R1=KoefEeprom[1];
	//if(R0>39)
	if(R0>42)
		{
			R1=(2*R0-77)/3;
			R4=R1;
			R1=KoefEeprom[1];
			if(R4>R1)
				R4-=R1;
		}
		else
			R4=0;


	//R1 *=R0;
	//R1 >>=9;

//	if(R0<40)
//		R0=0;
//		else
//			R0=R0-39;
//	R4=R0>>1;
//	return 45;
	return R4;
	}

void AccountTM(void)
	{
	unsigned int R0;
	//unsigned int R1;

	R0=AdResult[2]>>8;
	//R0=AdResult[2]>>2;
	//if(R0<68)
	if(R0<158)
		{
			R0*=163;
			R0=25700-R0;
			R0=R0/100;
			R0 +=KoefEeprom[0];
			//R0=30-R0;
		}
		else
			{
				R0*=163;
				R0=R0-25700;
				R0=R0/100;
				R0 +=KoefEeprom[0];
				//R0+=30;
			}
#if defined(PLATA91013)
	TM=R0-30;
#else
	TM=R0+10;
#endif
	}

void CalcTBap(void)
	{

	unsigned long R0;
	unsigned long R1;
//	unsigned int R2;
/*
 static const unsigned char Tab1[39]={99,98,97,96,95,94,93,92,91,90,//0
									88,87,86,85,83,82,81,80,79,78,//10
									77,76,76,75,74,73,73,72,71,70,//20
									70,69,68,68,67,66,66,65,65};//30

 static const unsigned char Tab2[39]={ 64,63,62,61,60,59,58,57,56,55,//0
									55,54,54,53,53,52,52,51,51,50,//10
									50,49,48,47,46,46,46,45,45,45,//20
									44,43,42,41,41,40,40,39,39};//30

 static const unsigned char Tab3[30]={	39,38,37,36,35,34,33,32,31,30,//0
										29,28,27,26,25,24,23,22,21,21,//10
										20,20,19,19,18,18,17,17,16,16};//20

*/
	R0=AdResult[4];//T=251 при -20С
	R1=AdResult[1]>>8;//5.0 B
	R0=R0/R1;//40-200 bit

	R0*=3;
	if(R0<793)
		{
			R0=793-R0;
			R0/=10;
		}
	else
		R0=0;

	R1=KoefEeprom[3];

	if(R1<10)
		{
			if(R0>(10-R1))
				R0=R0-(10-R1);
			else
				R0=0;
		}
	else
		R0+=(R1-10);

		TBap=R0;

	/*R0=AdResult[4];//T
	R1=AdResult[1]>>8;//5.0 B
	//R1=AdResult[1]>>2;//5.0 B
	R0=R0/R1;//40-200 bit
	R1=156+KoefEeprom[3];
	R0=R0*R1;
	R0=R0>>8;

	if(R0>=16)
		R0-=16;
	else
		R0=0;
	if(R0>300)
		R0=300;

	R2=R0;
	if(R2<39)
		{
		TBap=Tab1[R2];
		}
	else
		{
		R2-=39;
		R2>>=1;
		if(R2<39)
			{
			TBap=Tab2[R2];
			}
		else
			{
			R2-=39;
			R2>>=1;
			if(R2<=29)
				{
				TBap=Tab3[R2];
				}
			else
				{
				TBap=0;
				}
			}
		}
	*/
	}

void WriteEeprom(unsigned char Arg4,unsigned char Arg5)
	{
	wdt_reset();

	while(bit_is_set(EECR,EEWE));
	while(bit_is_set(SPMCR,SPMEN));

	EEARL=Arg4;
	EEARH=0;
	EEDR=Arg5;
	SregTemp=SREG;
	cli();
	EECR |=(1<<EEMWE);
	EECR |=(1<<EEWE);
	SREG=SregTemp;
	while(bit_is_set(EECR,EEWE));

	while(bit_is_set(SPMCR,SPMEN));
	}

void ChangeKoef(void)
	{
	unsigned char R0;
	unsigned char R1;

	R0=RomReceive[1];
	if(R0<=5)
		{
		if(EnableKoef==55)
			{
			if((RomReceive[2] & 0x5f)==4)
				{
				if((R0==1)||(R0==2)||(R0==3))
					{
					if(KoefEeprom[R0]<KoefMax[R0])
						++KoefEeprom[R0];
					}
				else
					{
					if((R0==5) && (NDiz<150))
						R0=7;
					else if((R0==5) && (RomReceive[2] & 0x20))
							R0=6;

					if(KoefEeprom[R0]>KoefMin[R0])
						--KoefEeprom[R0];
					}
				}
			else if((RomReceive[2] & 0x5f) ==8)
				{
				if((R0==1)||(R0==2)||(R0==3))
					{
					if(KoefEeprom[R0]>KoefMin[R0])
						--KoefEeprom[R0];
					}
				else
					{
					if((R0==5) && (NDiz<150))
						R0=7;
					else
						if((R0==5) && (RomReceive[2] & 0x20))
							R0=6;//значение DeltaI
					if(KoefEeprom[R0]<KoefMax[R0])
						++KoefEeprom[R0];
					}
				}

			R1=ReadEeprom(R0+1);
			if(R1 != KoefEeprom[R0])
				WriteEeprom((R0+1),KoefEeprom[R0]);

			EnableKoef=0;
			}
		}
	}

void LoadKoef(void)
	{
	unsigned char R0;
	unsigned char R1;

	for(R0=0;R0<=7;++R0)
		{
		R1=ReadEeprom(R0+1);
		if((R1>KoefMax[R0])||(R1<KoefMin[R0]))
			R1=Koef0[R0];

		KoefEeprom[R0]=R1;
		}
	}
/*
void LoadIKoef(void) //Загрузка таблицы поправок тока от температуры
	{
	unsigned char R0;
	unsigned char R1;
	for(R0=8;R0<=13;++R0) //Начинается с адреса EEPROM 9 (Адрес начальной ячейки 0х2011
	// 0 ячейка не используется
		{
		R1=ReadEeprom(R0+1); //первые 8 ячеек уже заняты. чтение с 9 по 14 ячейку, 6 значений
		if(R1==0xFF)  //Запись коэффициента из EEPROM если оно не 0xFF и не 0.
			R1=IKoef0[R0-8]; 	//иначе - коэффициенты по умолчанию
		IKoefEeprom[R0-8]=R1;
		}
	}
	*/

void ChangeCtError(void)
	{

	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{
		if(RegimError & (1<<R0))
			{
			if(CtError[R0])
				--CtError[R0];
			}
		else
			if(CtError[R0]<CtError0[R0])
				++CtError[R0];
		}
	}

void ChangeCtTime(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{
		if(RegimTime & (1<<R0))
			{
			if(CtTime[R0])
				--CtTime[R0];
			}
		else
			CtTime[R0]=CtTime0[R0];
		}
	}


void ResetCtTime(void)
	{
	unsigned char R0;

	for(R0=0;R0<=15;++R0)
		CtTime[R0]=CtTime0[R0];
	}

void SetRegimError(void)
	{
	unsigned int R0;

	if(TM<=10)//-20gr
		RegimError |=0x800;
	else
		if(TM>=12)
		{
			RegimError &=0xf7ff;
			Error &=0xf7ff;
		}
	if((TBap>=TBap85) || (Error & 0x1000))
		RegimError |=0x1000;
	else
		RegimError &=0xefff;

	R0=KoefEeprom[7]<<1;
	if(IGen>=R0)//280
		{
		RegimError |=(unsigned int)1<<15;
		RegimError &=0xffdf;
		Error  &=0xffdf;
		}
	else if(IGen>=(R0-10))
		{
		RegimError |=0x20;
		RegimError &=0x7fff;
		}
	else
		{
		RegimError &=0x7fdf;
		Error &=0xffdf;
		}

	if(NDiz>3300)
		{
		RegimError |=1;//NDiz>3300
		RegimError &=0xfffd;
		}
	else
		{
		RegimError &=0xfffe;

		if((NDiz<2850) && (NDiz>400) && (!(RegimWork & 1)))
			RegimError |=2;//NDiz<2850
//		else
//			if((NDiz<2850) && (UGen>100) &&(!(RegimWork & 1)))
//				RegimError |=2;
		else
			RegimError &=0b1111111111111101;
		}

	RegimError &=0xffbf;

	if((NDiz>=2850) && (UGen<250))
		RegimError |=0x4;
	else
		RegimError &=0xfffb;

	if((NDiz>=2850) && (NDiz<=3300))
		{
		if((UGen<250) || (UGen>300))
			RegimError |=8;
		else
			RegimError &=0xfff7;
		}
	else
		RegimError &=0xfff7;

	if(RegS & 4)//TM 120gr
		{
		RegimError |=0x100;
		RegimError &=0xfdff;
		}
	else
		{
		RegimError &=0xfeff;

		if(TM>=142)//TM 112gr
			{
			//RegimError |=0x200;
			RegimError |=0x100;
			}
		else
			{
			RegimError &=0xfdff;
			Error &=0xfdff;
			}
		}
		
	if(RegS & 8)//DM
		Error |=0x400;
	else //if((!CtTime[5]) &&(DM<=4))//DM
		if((DM<=4)&&(NDiz>2800))//DM
			RegimError |=0x400;
		else 
			RegimError &=0xfbff;

/*	if((NDiz>2800)&&(DT<=5))//DT
		RegimError |=0x2000;
	else
		RegimError &=0xdfff;
*/
	if(RegS & 1)//Vozd Filtr
		RegimError |=0x4000;
	else
		RegimError &=0xbfff;
	}

void SetError(void)
	{
	unsigned char R0;
	unsigned int R1;
	unsigned int R2;

	for(R0=0;R0<=15;++R0)
		{
		if(!CtError[R0])
			Error |=(1<<R0);
		}
	R1=0x1d9f;
	R2=0x1d9f;
//	if(!CtErrorBap)
//	R1 |=0x1000;
 	if(RomReceive[2] & 0x80)//Protect=on
		{
		 R1=0xc93;
		 R2=0xc9f;
		}

	if(Error & R1)
		PORTB &=0b11101111;
	else
		PORTB |=0b10000;

	if((!(Error & R2)) && (UGen>=250))
		PORTD &= 0b11101111;
	else
		PORTD |= 0b10000;

	}

void ResetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{
		CtError[R0]=CtError0[R0];
		Error &= 0x400;//reset all err кроме ND
		RegimError=0;
		}
	}

void SetRegimWork(void)
	{
	unsigned int R0;
	unsigned int R1;
	R0=0b1111111110111111;
	R1=0b110110011111;
	if(RomReceive[2] & 0x80)//Protect=on
		{
		R0=0xafbf;
		//R1=0xc93; //1100 1001 0011
		R1=0xD93; //1101 1001 0011 	TM 8 - bit
		}

//	if(NDiz>2500)
//		RegimTime |=(1<<5);// for DM
//	else
//		RegimTime &= 0b1111111111011111;

	if(bit_is_set(RegimWork,0))//Stop
		RegimWork = RegimWork & 0b11110111;//Rashol=off
	else if(((!(CtTime[4]))) && (TM<=125))
			RegimWork = RegimWork & 0b11110111;
	else if(!(CtTime[7])) //10min
			//RegimWork = RegimWork & 0b11110111;
			RegimWork |=(1<<0);// Stop
	else if(NDiz>2000)
			RegimWork |= (1<<3);//Rashol

	if(bit_is_set(RegimWork,2))
		{
		RegimTime |= (1<<4);//10s Rashol
		RegimTime |= (1<<7);//10min Rashol
		}
	else
		RegimTime = RegimTime & 0b1111111101101111;

	if(bit_is_set(RegimWork,0))//Stop
	{
	RegimWork=RegimWork & 0b11101111;//Pusk=off
		if(NDiz<150)
		{
			RegimTime |=(1<<3);//10 sec after N<400
			if(!(CtTime[3]))
			{
				//RegimWork=RegimWork & 0b11101111;
				RegimWork=RegimWork & 0b11101110;
				RegimTime &=0b1111111111110111;
			}
		}
		else
			RegimTime &=0b1111111111110111;
	}
	else
	//Stop=off
	{
	RegimTime &=0b1111111111110111;
	if(bit_is_set(RegimWork,1))//Pusk
		{
		if(NDiz>400)
			{
			RegimTime |=(1<<2);//2 sec after N>400
			}
		else
			RegimTime &=0b1111111111111011;

		if(NDiz>2850)
			RegimWork &=0b111101;//Pusk=off
		}
		else
		{
		//no pusk
		RegimTime &=0b1111111111111011;
		if((RomReceive[2] & 1) && (!(Error & 0x100))) //Pusk
			{
			RegimWork=RegimWork & 0b11111011;//Temp Stop=off
			RegimWork=RegimWork & 0b11110111;//Rashol=off

			if(!(Error & R0))//TM>-20
				{
				if(NDiz<150)
					{
					RegimTime |=0b1000000000;//25sec
					RegimWork |=(1<<5);//TM Pusk
					if(TM>=35)//>5
						{
						RegimTime |=0b10000000000;
						RegimWork |=(1<<4);//TPN Pusk
						}
					}
				}
			}
		if(!(Error & R0))//TM>-20
			{
			if(!CtTime[9])
				{
					RegimTime |=0x400;
					RegimWork |=(1<<4);//TPN Pusk
				}
			if(!(CtTime[10]))
				{
					RegimWork |=(1<<1);//Pusk
					RegimWork &=0b11001111;//TM Pusk=off
				}
			}
		RegimTime &=0b1111111111110111;
		}

	if((RomReceive[2] & 2) || (RegS & 2))//Stop
		{
		RegimWork=RegimWork & 0b11001101;//Pusk=off
		RegimTime &=0xf0ff;
		if(NDiz>150)
			RegimWork |=(1<<2);//Temp Stop
		}
	else
		if(Error & R1)
			{
				RegimWork=RegimWork & 0xed;//Pusk=off
			if(NDiz>150)
				RegimWork |=(1<<2);//Temp Stop
			}

	if((bit_is_set(RegimWork,2)) && (NDiz>150))
		{
		if(bit_is_clear(RegimWork,3))
			{
				RegimWork |=(1<<0);// Stop
				RegimWork=RegimWork & 0xfb;//Temp Stop=off
			}
		}
	}

	if(RegimWork & 0b10000)//TempPusk
		{
		if(!(RegimWork & 2))//Pusk=off
			{
			if(!CtTime[10])
				{
					RegimWork &=0b11101111;
					RegimWork |=0b10;
				}
			}
		}
	if((RegimWork & 7) || (NDiz>400))
		{
		RegimWork &=0xef;
		RegimTime &=0xf9ff;
		CtTime[9]=CtTime0[9];
		CtTime[10]=CtTime0[10];
		}
	}

void ControlStarter(void)
	{
	if(RegimWork & 0x2)//Pusk
		{
		if(!(CtTime[1]))//10 sec
			RegimTime |=(1<<0);//3 sec
			if(bit_is_set(RegimTime,0))
			{
			if(CtTime[0])
				{
					PORTC &=0xf7;//CT=on
					RegimTime &=0xfffd;//Ct10s=off
					CtTime[1]=CtTime0[1];
				}
			else
				 {
					RegimTime &=0xfffe;
					PORTC |=8;//CT=off
					if(!CtPusk)
						{
						Error |=(1<<7);//no Pusk
						RegimWork &=0xfd;//Pusk=off
						}
					else
						{
						--CtPusk;
						RegimTime |=2;//10 sec
						if(!(CtTime[1]))
							{
							RegimTime &=0xfffd;
							RegimTime |=1;//3 sec
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
		RegimTime |=2;//10 sec
		CtTime[1]=0;
		PORTC |=8;//CT=off
		}
	}

void ControlVozb(void)
	{
	if(NDiz>=400)
		{
		RegimTime |=0x40;
		if(!(CtTime[6]))
			PORTC |=0x10;
		else if(UGen>100)
				PORTC |=0x10;
			else
				if(CtTime[6])
					PORTC &=0xef;//vozb=on
		}
	else
		{
		RegimTime &=0xffbf;
		PORTC |=0x10;
		}
	}

void ControlStop(void)
	{
	if((Error & 0x401)&&(NDiz>150))
		{
		PORTC &=0b11111001;
		RegimWork |=1;//Stop
		return;
		}

	if(!(RegimWork & 5))
		{
		CtTime[4]=CtTime0[4];
		CtTime[7]=CtTime0[7];
		}

	if(!(CtTime[3]))
			PORTC |=6;//AO=off,PO=off
	else
		if(Error & 0x10)
			PORTC &=0b11111011;
/*
	if(!(PORTC & 2))//Stop
		{
		if(NDiz>150)
			{
			RegimError |=0x10;
			}
		else
			{
			RegimError &=0b1111111111101111;
			}

		if(!(CtTime[3]))
			PORTC |=6;//AO=off,PO=off
		else
			if(Error & 0x10)
				PORTC &=0xfb;
		}
	else
		{
		*/
		RegimError &=0b1111111111101111;
		if(RegimWork & 0x1)
			PORTC &=0xfd;//STOP
//		}
	}

void ControlKG(void)
	{
	unsigned int R0;

		R0=0b1001110110011111;

	if(RomReceive[2] & 0x80)
		R0=0xc93;
	if(!(PORTC & 1))//KG=on
		{
		Led2 |=0x20;
		Led1 &=0x7f;
		}
	else
		{
		Led2 &=0xdf;
		if(RegLed1 & 0x80)
			Led1 |=0x80;
		}
	if(!(PORTC & 1))//KG=on
		RegimTime |=0x8000;

	if((Error & R0) || ((UGen<240) && (NDiz<2600)) ||(RegS & 0x10))
		{
		Led2 &=0xdf;
		RegLed1 &=0x7f;
		Led1 &=0x7f;
		PORTC |=1;//kg=off
		}
	else
		if(RegimWork & 0x5)
		{
		RegLed1 &=0b01111111;
		}
		else if((NDiz>=2850) && ((UGen>=250)||(RomReceive[2] & 0x80)))
			{
			if((TM>=70) || (RomReceive[2] & 0x80))//40gr
				{
				if((PORTC & 2) && (PORTC & 4) && (!(RegS & 0x10)))
					{
					if((!(RomReceive[2] & 0x20)) && (CtTime[15]))//Avt Upr
						{
						PORTC &=0xfe;//KG=on
						RegLed1 |=0b10000000;
						}
					else
						RegLed1 |=0b10000000;
					}
				}
			}

	if(RegimWork & 0x5)
		PORTC |=1;//KG=off
	else
		if(Error & R0)
			PORTC |=1;//KG=off
	else
		if((RomReceive[2] & 8) || (RegS & 0x40))
			PORTC |=1;//KG=off
	else
		if((Led1 & 0x80)&&((RomReceive[2] & 4) || (RegS & 0x20)))
			PORTC &=0xfe;//KG=on
	}

void ControlLed(void)
	{
	if(Error & 0x8000)//I>290
		Led2 |=0x10;
	else
		Led2 &=0xef;

 	if(Error & 0x900)
		{
		Led1 |=1;
		}
	else if(Error & 0x200)//TM>105
		{
		Led1 &=0xfe;
		}
	else
		{
		Led1 &=0xfe;
		}
	if(Error & 0x400)//DM
		Led1 |=2;
	else
		Led1 &=0xfd;

	if(Error & 0x2000)//DT
		Led1 |=4;
	else
		Led1 &=0xfb;

	if(Error & 3)//NDiz
		Led1 |=8;
	else
		Led1 &=0xf7;

	if(Error & 0xc)//UGen
		Led2 |=8;
	else
		Led2 &=0xf7;

	if(TBap>=TBap85)
		Led1 |=0x10;
	else
		Led1 &=0xef;

	if(Error & 0x4000)//Vozd Filtr
		Led1 |=0x20;
	else
		Led1 &=0xdf;

	if(RegimWork & 5)//STOP || Temp Stop
		{
		Led2 |=0x2;
		Led2 &=0b11111110;
		}
	else
		{
		Led2 &=0xfd;
		if(RegimWork & 0x32)
			Led2 |=1;
		else
			Led2 &=0xfe;
		}

	if(Led1 & 0x80)//PNR
		PORTD &=0b11011111;
	else
		PORTD |=0b100000;

	if(RegimWork & 0x10)//TPN pusk
		PORTC &=0x5f;//TPN,PV
	else if(RegimWork & 0x20)//PV
		{
		PORTC &=0xdf;
		PORTC |=0x80;
		}
		else
			PORTC |=0xa0;
	}

void ControlVent(void)
	{
	if((NDiz<400) ||(TM<=90))
		PORTC |=0x40;
	else if(TM>=95)//65gr
		PORTC &=0xbf;

	}

void ChangeCtKalib(void)
	{
	if(Kalib==55)
		{
		if(RomReceive[2] & 0x4c)
			CtKalib=CtKalib0;//CtKalib0=300;
		}
	else
		CtKalib=CtKalib0;//CtKalib0=300;
	if(CtKalib)
		--CtKalib;
	if(!CtKalib)
		Kalib=0;
	}

void ControlZvonok(void)
	{
	if(!(RegimWork & 0x10))
		Error &=0xffbf;
	else if((CtTime[9]==0)||(CtTime[10]==0))
		Error &=0xffbf;
	else if((CtTime[9]<45)||(CtTime[10]<45))
		Error |=0x40;
	else
		Error &=0xffbf;
	}

/*++++++++++++++++++++++++++++++++++++++++++*/


int main(void)
{
	DDRB=0b00010000;
	PORTB = 0b00010000;// DG alarm off
	DDRC=0xff;//ALL OUT
	PORTC =0xff;
	DDRD=0b10111110;
	PORTD=0b10111011;//transmit=off
	DDRA=0;
	PORTA=0;

	wdt_reset();

	InitAd();
	ADCSRA |=(1<<ADIE);//enable interrupt AD

	/*TIMER1*/
	TCCR1B=0b00000111;//External clock source on T1 pin. Clock on rising edge.

	/*TIMER2*/
	TCCR2 =0b00000101;//clkT2S/128 (From prescaler)//4ms
	TIFR =0b00000000;
	TIMSK=0b01000000;//enable Overflof TMR2 enable Overflof TMR1

	LoadKoef();
	//LoadIKoef(); //загрузка поправок для тока из Eeprom
	ReadKn();
	RegWait=300;

	while(RegWait--)
		wdt_reset();

	ReadKn();
	InitUsart();
	UCSRB |=0b10000000;//enable int receive
	RegSOld=RegS;

	sei();
	Kalib=44;
	RomReceive[1]=0xff;
	ResetError();

	RegimTime=0;
	RegimWork=0;
	ResetCtTime();

	RomReceive[2]=0;
	CtTDiz=15;

	/*Work program*/
	while(1)
	{
		wdt_reset();
		if(NDiz<400)
		{
			CtTime[15]=CtTime0[15];
			RegimTime &=0x7fff;
		}

		//DeltaI=KoefEeprom[6];

		if(RegS & 0x10)//вкл МУ
		{
		if((RomReceive[1]==1)||(RomReceive[1]==2))
			{
			if((RomReceive[2] & 0x50)==0x50)//пришла ком калибровки
				Kalib=55;
			}
		}

		if(Kalib==55)
			ChangeKoef();

		SetRegimError();
		SetError();
		ControlLed();
		ControlZvonok();


		if(!(RegS & 0x10))//Dist upr
		{
			SetRegimWork();
			ControlStarter();
			ControlVozb();
			ControlKG();
			ControlStop();
			ControlVent();

			Led2 &=0b11111011;
		}
		else
		{
			if(Kalib==55)
				Error|=0x40;
			else
				Error &=0b1111111110111111;
			//	ResetError();
			///	RegimTime=0;
			RegimWork=0;
			Led1 &=0b01111111;
			Led2 |=4;
			Led2 &=0b11011111;
			//	Led1Mig &=0x70;
			PORTB |=0b00010000;
			PORTC =0xff;
			PORTD |=0b10110000;
		}

		if(RomReceive[2] & 0x10)
			ResetError();
		//	else if(RegS & 0x10)
		//	ResetError();

		if(MyStatus)
		{
			//AccountNDiz();
			AccountUGen();
			if(UGen>=200)
				AccountIGen();
			else
				IGen=0;

			DT=6; //заглушка чтобы не использовать датчик давления топлива.
			DM=AccountP();
			AccountTM();
			CalcTBap();
			MyStatus=0;
		}

		RegTransmit[15]=Led2;
		RegTransmit[14]=Led1;
		RegTransmit[13]=TBap;
		RegTransmit[12]=NDiz>>8;
		RegTransmit[11]=NDiz;
		RegTransmit[10]=TM;
		RegTransmit[9]=DM;
		RegTransmit[8]=DT;

		if(NDiz<150)//||(PORTC & 1))
		{
//			RabReg0=KoefEeprom[7]<<1;
//			RegTransmit[7]=RabReg0>>8;
//			RegTransmit[6]=RabReg0;
			RegTransmit[7]=0;
			RegTransmit[6]=0;
		}
		else
		{
			RegTransmit[7]=IGen>>8;
			RegTransmit[6]=IGen;
		}

		RegTransmit[5]=UGen>>8;
		RegTransmit[4]=UGen;

		RabReg0=Error>>8;//

		if(TBap>=TBap85)
			RabReg0 |=0x10;

		RegTransmit[3]=RabReg0;
		RegTransmit[2]=Error;
		//	RegTransmit[1]=Error;
		//	RegTransmit[0]=20;
		ReadKn();
	}
}

/* Timer/Counter1 Overflow */
 	ISR(TIMER1_OVF_vect)
{
	++CtTmr1;
}

/* Timer/Counter2 Overflow */
 	ISR(TIMER2_OVF_vect)//4 Mc!!!
{
	static unsigned char Cttmr2=25,cnt_n_tmr1=0;
	unsigned char b_sreg=0;
	unsigned int tcnt_tmr1=0;

	if(cnt_n_tmr1>=132)
		{
		cnt_n_tmr1=0;
		b_sreg=SREG;
		cli();
		tcnt_tmr1=TCNT1;
		TCNT1=0;
		SREG=b_sreg;
		sei();
		/*
		tcnt_tmr1=22 при 5,5кГц т.е. 3000 об/мин
		*/
		NDiz=tcnt_tmr1;//*136;
		}
		else
			++cnt_n_tmr1;

	if(Cttmr2)
		--Cttmr2;
	else
		{//25*4=100ms
		ChangeCtError();
		ChangeCtTime();
		ChangeCtKalib();
		Cttmr2=25;
		EnableKoef=55;
		}

	if(CtErrorLink)
		--CtErrorLink;
	else
		{
		InitUsart();
		UCSRB |=0x80;//enable int receive
		}
}

/* USART, RX Complete */
	ISR(USART_RX_vect)
{
	LinkPult();
}

/* ADC Conversion Complete */
	ISR(ADC_vect)
{
	static unsigned char count_ch=0,i=0;
 	unsigned int ADC_data;
// unsigned int RegInt1;
 	ADC_data=ADC;

	if(CtAd<CtAd0)//CtAd0=64
		{
		if(count_ch==0)
			{

#ifdef trans_toka //0.4ms
			if(!read)
				{
					I_Arr[i]=ADC_data;
					if(i<64)
						{
							i++;
						}
					else
						{
							i=0;
							read=1;
						}
				}
#else
				//ADC_data=(ADC_data>>2)&0xFF;
				if(ADC_data>767)
					ADC_data-=768;
				else
					ADC_data=0;
#endif
				/*
				if(ADC_data>771)
					{
						ADC_data-=771;

						if(ADC_data>KoefEeprom[6])
							ADC_data=ADC_data-KoefEeprom[6];//уст 0
						else
							ADC_data=0;
					}
				else
					ADC_data=0;
					*/
			}

		AdTemp +=ADC_data;
		++CtAd;
		}
	else //делаем 64 преобразования по каждому каналу
		{
			AdResult[count_ch]=AdTemp;
			AdTemp=0;
			CtAd=0;


			if(count_ch<MAX_AD_CH)
				++count_ch;
			else
				{
					count_ch=0;
					MyStatus=MyStatus | 1; //сформировать отчет
				}
			NumberAd=NumberAdNew[count_ch];//выбор след канала из списка
		}


	ADC_data=ADMUX;
	ADC_data &=0b11100000;
	ADC_data |=NumberAd;
	ADMUX=ADC_data;

	ADCSRA |=(1<<ADSC);//старт преобразов
}






