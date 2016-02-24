
/*
*
*Программа демо платы Milandr 91T эмулятор ПКДУ
*/

	#include "MDR32Fx.h"
	#include "MDR32F9Qx_port.h"
	#include "MDR32F9Qx_rst_clk.h"
	#include "MDR32F9Qx_timer.h"
	#include "MDR32F9Qx_uart.h"
	#include "MDR32F9Qx_adc.h"
	#include "MDR32F9Qx_iwdg.h"
	#include <stdbool.h>
	#include "lcd.h"
	#include "leds.h"
	#include "text.h"
	#include "joystick.h"
	#include <stdio.h>

#define LED1_OFF	PORT_ResetBits(MDR_PORTD, PORT_Pin_10)
#define LED1_ON		PORT_SetBits(MDR_PORTD, PORT_Pin_10)

#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTB, PORT_Pin_7)
#define RS485_TX_ON		PORT_SetBits(MDR_PORTB, PORT_Pin_7)
// #define TX_ON		PORT_ResetBits(MDR_PORTB, PORT_Pin_5)
// #define TX_OFF		PORT_SetBits(MDR_PORTB, PORT_Pin_5)
// #define RX_ON		PORT_ResetBits(MDR_PORTB, PORT_Pin_6)
// #define RX_OFF		PORT_SetBits(MDR_PORTB, PORT_Pin_6)


#define SystemCoreClock (uint32_t)8000000//частота сист шины

#define ADC_COUNT_CH 8//число каналов АЦП

//#define LED_ON		PORT_SetBits(MDR_PORTB, PORT_Pin_5)
//#define LED_OFF		PORT_ResetBits(MDR_PORTB, PORT_Pin_5)

typedef struct
{
	bool open_str;
	uint8_t reg0;
	uint8_t reg1;
	uint8_t Error;	//error
	uint8_t RabReg0;
	uint16_t UGen;
	uint16_t IGen;
	uint8_t DT;
	uint8_t DM;
	uint8_t TM;
	uint16_t NDiz;
	uint8_t TBap;
	uint8_t Led1;
	uint8_t Led2;
}RegS;

volatile RegS StatusPKDU; //= {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

volatile uint16_t delay_count=0,
				error_pkdu=0;

#define TIME_OUT_LINK_ST	100 //*1ms ожидание ответа
#define LINK_COUNT	5	//число попыток связи
#define RX_FRAME_SIZE	19

uint8_t ArrayRX_PKBA[RX_FRAME_SIZE];
volatile uint16_t cnt_start=0;

volatile bool 	ADC_compl=false,
				RX_ok=false,
				TX_st=false;
		
extern uint16_t error_pkba=0;
extern volatile uint8_t time_out_byte=0;
ADC_InitTypeDef sADC;
ADCx_InitTypeDef sADCx;
PORT_InitTypeDef PORT_InitStructure;
TIMER_CntInitTypeDef sTIM_CntInit;
TIMER_ChnInitTypeDef sTIM_ChnInit;
//static UART_InitTypeDef UART_InitStructure;

struct
{
	uint8_t t1;
	uint8_t t2;
	uint8_t t3;
	uint8_t t4;
}time;



//==================================================================================
void GPIO_init(void);
void TIM1_init(void);
void TIM2_init(void);
void TIM3_init(void);
void ADC_init(void);
void UART_init(void);
void PLL_init(void);
void WDT_init(void);
void ERRORm(void);
void WDT_init(void);
void link_PKBA(RegS Status);

void UART1_send_byte(uint8_t byte);
uint8_t ctrl_sum(uint8_t *data8,uint8_t cnt);
uint8_t UART1_receiv_data(void);
extern void delay_ms(uint16_t delay);
void clear_Arr(u8 *arr,u8 cnt);

//==================================================================================
void delay_ms(uint16_t delay)
{
	delay_count=delay;
	while(delay_count);
}
//==================================================================================
void Timer1_IRQHandler(void)
{
	if(TIMER_GetITStatus(MDR_TIMER1,TIMER_STATUS_CNT_ARR) == SET)
	{
		TIMER_ClearITPendingBit(MDR_TIMER1,TIMER_STATUS_CNT_ARR);
	}
}
//==================================================================================
void Timer2_IRQHandler(void)
{
 if(TIMER_GetITStatus(MDR_TIMER2,TIMER_STATUS_CNT_ARR) == SET)
	{
		TIMER_ClearITPendingBit(MDR_TIMER2,TIMER_STATUS_CNT_ARR);
	}
}
//==================================================================================
void Timer3_IRQHandler(void)
{
 if(TIMER_GetITStatus(MDR_TIMER3,TIMER_STATUS_CNT_ARR) == SET)
	{
		TIMER_ClearITPendingBit(MDR_TIMER3,TIMER_STATUS_CNT_ARR);
	}
}
//==================================================================================
void UART1_IRQHandler(void)
{
	static uint8_t cnt_rx_byte=0;
	static bool start=false, ld=0;
	uint8_t tmp=0; 
	
	
	if(UART_GetITStatus(MDR_UART1,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
		tmp=UART_ReceiveData(MDR_UART1);
// 		if(tmp==0xAA)
// 			RX_ok=true;
		
		if((tmp==0xAA) && (!start))//start byte
		{
			start=true;
		}
		else
		{
			if(start)
			{
				if(!RX_ok)//предыдущие данные обработаны
					ArrayRX_PKBA[cnt_rx_byte]=tmp; 
					
				++cnt_rx_byte;
				if(cnt_rx_byte==RX_FRAME_SIZE)
				{	
					RX_ok=true;
					start=false;
					cnt_rx_byte=0;
				}
				
				if(ld)
					LED1_ON;
				else
					LED1_OFF;
				
				ld=!ld;
			}
		}
	}
	MDR_UART1->RSR_ECR = 0;	
}
//==================================================================================
void UART2_IRQHandler(void)
{
	if(UART_GetITStatus(MDR_UART2,UART_IT_RX) == SET)
	{
		UART_ClearITPendingBit(MDR_UART2,UART_IT_RX);
	}	
}
//==================================================================================
void ADC_IRQHandler(void)
{
//	static uint8_t cnt_ch=0;
	
	if(ADC_GetITStatus(ADC1_IT_END_OF_CONVERSION) == SET)
	{
// 		ADC_Arr[cnt_ch]=(uint8_t)(ADC1_GetResult()>>4);
// 		
// 		if(cnt_ch>(ADC_COUNT_CH-1))
// 		{
// 			cnt_ch=0;
// 			ADC_compl=true;
// 		}
// 		else
// 			++cnt_ch;
		//ADC1_ClearOverwriteFlag();
	}		
}
//==================================================================================

void SysTick_Handler(void)//~1ms
{
//	static uint16_t cnt_link=0;
	//static bool st=0;
	
	if(delay_count)
	{
		delay_count--;
	}
	
	++time_out_byte;
// 	if(st)
// 	{
// 		TX_OFF;
// 	}
// 	else
// 	{
// 		TX_ON;
// 	}
// 	st=!st;
		++cnt_start;
}
//==================================================================================
//==================================================================================
//==================================================================================

int main()
{
	uint8_t cnt_link=0,
			strlen[20],
			sum=0;
	
	Demo_Init();
	//WDT_init();
	RS485_TX_OFF;
	
	SysTick_Config(SystemCoreClock/1200);//~1 ms
	
	LCD_CLS();
	CurrentMethod = MET_AND;
	CurrentFont = &Font_6x8;
	LCD_PUTS(0, 0,"TX PKBA:");
	LCD_PUTS(0, CurrentFont->Height,"RX PKBA:");
//   LCD_PUTS(0, CurrentFont->Height * 2 + 4, "start sending.       ");
//   LCD_PUTS(0, CurrentFont->Height * 3 + 6, "Press SEL for return.");
//   LCD_PUTS(0, CurrentFont->Height * 4 + 8, "Send:                ");
//   LCD_PUTS(0, CurrentFont->Height * 5 + 8, "Rec.:                ");
//	LCD_PUTS(CurrentFont->Width*8, 0,"AA ");
	
	StatusPKDU.reg0=0x00;
	StatusPKDU.reg1=0x00;
	
	LED1_ON;
	delay_ms(100);
	
	while(1)
	{
		if(RX_ok)//100ms//1s
		{
			uint8_t cnt_byte=0;
			sum=0;
			while(cnt_byte<RX_FRAME_SIZE)
			{
				sum+=ArrayRX_PKBA[cnt_byte];
				++cnt_byte;
			}
			sum+=0xAA;
			
			if(!sum)
			{
				uint16_t tmp=0;
			//link_PKBA(StatusPKBA);
			tmp=(ArrayRX_PKBA[8]<<8)|ArrayRX_PKBA[7];
			sprintf(strlen,"Tok=%4d |",tmp);
			LCD_PUTS(0, CurrentFont->Height * 2 + 2, strlen);
			clear_Arr(strlen,20);
			
			tmp=ArrayRX_PKBA[11];
			if(tmp>29)
			{
				tmp-=30;
				sprintf(strlen,"TM = %3dC|",ArrayRX_PKBA[11]);
			}
			else
			{
				tmp=30-tmp;
				sprintf(strlen,"TM =-%dC |",ArrayRX_PKBA[11]);
			}
			LCD_PUTS(0, CurrentFont->Height * 3 + 2, strlen);
			clear_Arr(strlen,20);
			
			tmp=ArrayRX_PKBA[10];
			sprintf(strlen,"DM = %1d.%1d |",tmp/10,tmp%10);
			LCD_PUTS(0, CurrentFont->Height * 4 + 2, strlen);
			clear_Arr(strlen,20);
				
			sprintf(strlen,"TBEx=%dC |",ArrayRX_PKBA[14]);
			LCD_PUTS(0, CurrentFont->Height * 5 + 2, strlen);
			clear_Arr(strlen,20);
			
			sprintf(strlen,"TBIn=%dC |",ArrayRX_PKBA[15]);
			LCD_PUTS(0, CurrentFont->Height * 6 + 2, strlen);
			clear_Arr(strlen,20);
			
									//		reg1				reg0
			sprintf(strlen,"Date:%d/%d",ArrayRX_PKBA[1],ArrayRX_PKBA[0]);
			//sprintf(strlen,"Day: %1d%1d",1,2);
			LCD_PUTS(CurrentFont->Width*11, CurrentFont->Height * 2 + 2, strlen);
			clear_Arr(strlen,20);
			
			tmp=(ArrayRX_PKBA[6]<<8)|ArrayRX_PKBA[5];
			sprintf(strlen,"Ug = %2d.%1dV",tmp/10,tmp%(tmp/10));
			//sprintf(strlen,"Ug = %dV",tmp);
			LCD_PUTS(CurrentFont->Width*11, CurrentFont->Height * 3 + 2, strlen);
			clear_Arr(strlen,20);
			
			//sprintf(strlen,"ADC1= %02Xh",ArrayRX_PKBA[0]);
			sprintf(strlen,"Kn%1d%1d%1d%1d%1d%1d%1d%1d",(ArrayRX_PKBA[4]>>7)&1,(ArrayRX_PKBA[4]>>6)&1,
														(ArrayRX_PKBA[4]>>5)&1,(ArrayRX_PKBA[4]>>4)&1,
														(ArrayRX_PKBA[4]>>3)&1,(ArrayRX_PKBA[4]>>2)&1,
														(ArrayRX_PKBA[4]>>1)&1,(ArrayRX_PKBA[4]>>0)&1);
			LCD_PUTS(CurrentFont->Width*11, CurrentFont->Height * 4 + 2, strlen);
			clear_Arr(strlen,20);
			
// 			sprintf(strlen,"ADC2= %02Xh",ArrayRX_PKBA[1]);
// 			LCD_PUTS(CurrentFont->Width*11, CurrentFont->Height * 5 + 2, strlen);
// 			clear_Arr(strlen,20);
			
			tmp=(ArrayRX_PKBA[13]<<8)|ArrayRX_PKBA[12];
			sprintf(strlen,"Fdv= %4d",tmp);
			LCD_PUTS(CurrentFont->Width*11, CurrentFont->Height * 6 + 2, strlen);
			clear_Arr(strlen,20);
			
// 			sprintf(strlen,"%02X %02X %02X %02X %02X %02X",ArrayRX_PKBA[6],ArrayRX_PKBA[7],ArrayRX_PKBA[8],	\
// 														ArrayRX_PKBA[9],ArrayRX_PKBA[10],ArrayRX_PKBA[11]);
// 			LCD_PUTS(0, CurrentFont->Height * 3 + 2, strlen);
// 			clear_Arr(strlen,20);
// 			
// 			sprintf(strlen,"%02X %02X %02X",ArrayRX_PKBA[12],ArrayRX_PKBA[13],ArrayRX_PKBA[14]);
// 			LCD_PUTS(0, CurrentFont->Height * 4 + 2, strlen);
 			error_pkdu&=!(1<<0);
			LCD_PUTS(CurrentFont->Width*8, CurrentFont->Height," PKBA Ok   ");
				
			cnt_link=0;
			}
			else 
				LCD_PUTS(CurrentFont->Width*8, CurrentFont->Height," CRC err ");
			
			RX_ok=0;
		}

		if(cnt_start>TIME_OUT_LINK_ST)//TX_st *1.5ms
		//if(GetKey()==DOWN)
		{
			cnt_start=0;
			
			if(cnt_link<LINK_COUNT)
			{
			sum=StatusPKDU.reg0+StatusPKDU.reg1+0xAA;
			sum=255-sum;
			++sum;	
			sprintf(strlen,"%02X %02X %02X %02X",0xAA,StatusPKDU.reg0,StatusPKDU.reg1,sum);
			LCD_PUTS(CurrentFont->Width*8, 0,strlen);
			//NVIC_DisableIRQ(UART1_IRQn);
			UART_ITConfig(MDR_UART1,UART_IT_RX, DISABLE);
			RS485_TX_ON;
			UART1_send_byte(0xAA);
			UART1_send_byte(StatusPKDU.reg0);
			UART1_send_byte(StatusPKDU.reg1);
			UART1_send_byte(sum);
			RS485_TX_OFF;
				
			while(GetKey()==DOWN);

			//RS485_TX_OFF;

			UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);
				
			++cnt_link;
			}
			else
			{
			LCD_CLS();
			LCD_PUTS(0, 0,"TX PKBA:");		
			sprintf(strlen,"%2d cnt",cnt_link);
			LCD_PUTS(CurrentFont->Width*8, 0,strlen);
			LCD_PUTS(0, CurrentFont->Height ,"RX PKBA: PKBA Error");
			error_pkdu|=1<<0;
			cnt_link=0;

			while(!(GetKey()==DOWN))
				{
				IWDG_ReloadCounter();//WDT reset //409.5ms
				}
			}
		//while(GetKey()==DOWN);
		}

// 		if(TX_st)//100ms
// 		{
// 			TX_st=0;
// 			link_PKBA(StatusPKBA);
// 			LCD_PUTS(CurrentFont->Width*8, CurrentFont->Height + 1," wait...");
// 		}
// 		
// 		if(RX_ok)
// 		{
// 			RX_ok=false;
// 			LCD_PUTS(CurrentFont->Width*8, CurrentFont->Height + 1,"PKBA ok");
// 		}

		IWDG_ReloadCounter();//WDT reset //409.5ms
		delay_ms(1);
	}
}

//==================================================================================
//==================================================================================
//==================================================================================
void ERRORm(void)
{
	while(1);
}
//==================================================================================

//==================================================================================
void UART1_send_byte(uint8_t byte)
{
	UART_SendData(MDR_UART1,byte);
	time_out_byte=0;
	while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	{
		if(time_out_byte>3)
			break;
	}
	UART_ReceiveData (MDR_UART1);
	UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
// 	while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY))
// 	{
// 		UART_ReceiveData (MDR_UART1);
// 	}
}
//==================================================================================
uint8_t ctrl_sum(uint8_t *data8,uint8_t cnt)
{
	uint8_t i=0,sum=0;
	
	while(i<cnt)
	{
		sum+=data8[i];
		++i;
	}
	return sum;
}
//==================================================================================
uint8_t UART1_receiv_data(void)
{
	uint8_t rec_data=0;
	/* Check RXFF flag */
    while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_RXFF)!= SET)
    {
		__nop();
    }
	rec_data = UART_ReceiveData (MDR_UART1);
	
	return rec_data;
}
//==================================================================================
void clear_Arr(u8 *arr,u8 cnt)
{
	while(cnt)
	{
		arr[cnt]=0;
		cnt--;
	}
}
//==================================================================================
void WDT_init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_IWDG,ENABLE);
	IWDG_WriteAccessEnable();
	IWDG_SetPrescaler(IWDG_Prescaler_4);//409.5ms
	IWDG_SetReload(0xFFF);
	IWDG_Enable();
	IWDG_ReloadCounter();
}

















