/*
*��������� ��23 ������� ���100�
*/


#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_ssp.h"
#include <stdbool.h>
#include "MDR32F9Qx_power.h"
#include <string.h>
#include "font5x7.h"


#define NUMBER_PDG	(uint8_t)0xA0
#define NUMBER_PZF	(uint8_t)0xA1

#define MAX_MESSAGE	6


//==================================================================================

#define  LCD_DIS 		PORT_ResetBits(MDR_PORTA, (PORT_Pin_1 ))

#define  LCD_EN 		PORT_SetBits(MDR_PORTA, (PORT_Pin_1 ))

#define  LCD_ALL_ROW_OFF 	{PORT_ResetBits(MDR_PORTD, (PORT_Pin_4));	\
							PORT_ResetBits(MDR_PORTE, (PORT_Pin_6|PORT_Pin_7|PORT_Pin_0|PORT_Pin_1));}

							
#define  LCD_ROW1_ON 	PORT_SetBits(MDR_PORTD, (PORT_Pin_4))
#define  LCD_ROW2_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_6))
#define  LCD_ROW3_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_7))					
#define  LCD_ROW4_ON 	PORT_SetBits(MDR_PORTE, ( PORT_Pin_0))
#define  LCD_ROW5_ON 	PORT_SetBits(MDR_PORTE, (PORT_Pin_1))

						
#define  LIN_LOW	 	PORT_ResetBits(MDR_PORTF, (PORT_Pin_2))
#define  LIN_HIGHT	 	PORT_SetBits(MDR_PORTF, (PORT_Pin_2))
#define  LIN_READ	 	PORT_ReadInputDataBit(MDR_PORTF, (PORT_Pin_3))

#define  TEST_MODE	 	!PORT_ReadInputDataBit(MDR_PORTA, (PORT_Pin_4))



typedef struct
{
	uint16_t	KN_DG;				//1
	uint16_t	Error;				//2
	uint16_t	UUg;				//3
	uint16_t	UVg;				//4
	uint16_t	UWg;				//5
	uint16_t	UF;					//6
	uint16_t	IUg;				//7
	uint16_t	IVg;				//8
	uint16_t	IWg;				//9
	uint8_t		ISL;				//10
	uint8_t		TG;					//11
	uint16_t	NDiz;				//12	
	uint8_t		KN_ZF;				//13
	uint16_t	IAf1;				//14
	uint16_t	IBf1;				//15
	uint16_t	ICf1;				//16
	uint16_t	IAf2;				//17
	uint16_t	IBf2;				//18
	uint16_t	ICf2;				//19
	uint16_t	RegWrk;				//20
	uint8_t		cal_b;				//21
}RegS;
	
volatile RegS Status = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	

const uint16_t 			Yanv_2015_ETRO[] = { 64*5,46*5,35*5,33*5,49*5,61*5,00*5,18*5,16*5,17*5,21*5,00*5,62*5,51*5,49*5,47*5,6*5},
						Err_link[] = { 46*5,38*5,51*5,0,50*5,35*5,64*5,40*5,41*5},
						PDG[] = { 48*5,37*5,36*5},	
						PZF[] = { 48*5,40*5,53*5},
						OK[] = { 47*5,43*5},
						ALARM_TF[] = {0,0,0,33*5,35*5,33*5,49*5,14*5,0,49*5,51*5},
						PN_RAZR[] = {0,0,0,0,0,48*5,46*5,0,49*5,33*5,40*5,49*5,14*5},
						Mu_upr[] = {0,0,0,45*5,38*5,50*5,51*5,14*5,0,52*5,48*5,49*5,14*5},
						Du_upr[] = {0,0,0,37*5,41*5,50*5,51*5,14*5,0,52*5,48*5,49*5,14*5},
						Otk_KZF[] = {47*5,51*5,43*5,33*5,40*5,0,43*5,40*5,53*5},
						Prot_OFF[] = {40*5,33*5,58*5,41*5,51*5,33*5,0,47*5,51*5,43*5,44*5,14*5},
						Alarm_OG[] = {0,0,0,33*5,35*5,33*5,49*5,14*5,0,51*5,47*5,39*5},
						Alarm_PM[] = {0,0,0,0,33*5,35*5,33*5,49*5,14*5,0,49*5,45*5},
						Raznos[] = {49*5,33*5,40*5,46*5,47*5,50*5},
						Ug_b_dop[] = {0,0,0,0,0,65*5,36*5,30*5,37*5,47*5,48*5},
						Ug_m_dop[] = {0,0,0,0,0,65*5,36*5,28*5,37*5,47*5,48*5},
						Overload[] = {48*5,38*5,49*5,38*5,36*5,49*5,52*5,40*5,43*5,33*5},
						KZ_load[] = {43*5,40*5,0,46*5,33*5,36*5,49*5,52*5,40*5,43*5,41*5},
						PUSK[] = {0,0,0,0,0,0,48*5,52*5,50*5,43*5},
						OG_m_37[] = {0,0,0,0,0,51*5,47*5,39*5,28*5,19*5,23*5},
						OSTANOV[] = {0,0,0,0,47*5,50*5,51*5,33*5,46*5,47*5,35*5},
						NO_PUSK[] = {0,0,0,46*5,38*5,51*5,0,48*5,52*5,50*5,43*5,33*5},
						Rashol[] = {0,0,0,0,49*5,33*5,50*5,54*5,47*5,44*5,14*5},
						NO_STOP[] = {0,0,46*5,38*5,51*5,0,47*5,50*5,51*5,33*5,46*5,47*5,35*5,33*5},
						NO_Vozb[] = {0,0,0,46*5,38*5,51*5,0,35*5,47*5,40*5,34*5,14*5};	
						

#define  PDG_FRAME_SIZE 26
#define  PZF_FRAME_SIZE 16

#define LINK_PDG_PZF_OK	1<<0
#define LINK_PDG_OK		1<<1
#define LINK_PZF_OK		1<<2
#define NO_LINK_PDG		1<<3
#define NO_LINK_PZF		1<<4
							
#define CNT_LINK	5							

volatile uint32_t index_char[16],
				  led_reg[7][5];
volatile uint16_t delay_count = 0;
volatile uint8_t ArrayRX[PDG_FRAME_SIZE], IndData[5], status_link = 0;
uint8_t time_out_byte = 0, 
		num_par = 0,
		cur_dev = NUMBER_PDG;

bool RX_Ok = false, 
		F1_Off = false, 
		F2_Off = false,
		flash_pos = false,
		rst_alarm = false;
//CAN_DataTypeDef CAN_RX;	
	
	
	
void GPIO_init(void);
void TIM1_init(void);
void TIM2_init(void);
void TIM3_init(void);
void ADC_init(void);
void DAC_init(void);
void UART_init(void);
void PLL_init(void);
void WDT_init(void);
void EEPROM_init(void);
void SPI_init(void);
void sys_init(void);
void TX_Data(uint8_t);
void UART1_send_byte(uint8_t byte);
void scroll_mess_left(void);
void no_link_PDG(void);
void no_link_PZF(void);
void OK_PDG(void);
void OK_PZF(void);
void fill_reg_led(void);
bool Read_Data(uint8_t add_dev);
void clear_lcd(void);
void show_led_data(uint8_t);
void DecSegm(uint16_t R0);
void link_dev(void);
void Show_led_str(const uint16_t *str,uint8_t len);


//==================================================================================
__inline void delay_ms(uint16_t delay)//1ms
{
	delay_count = delay;
	while(delay_count);
}
//==================================================================================
void UART1_IRQHandler(void)
{
	uint8_t tmp = 0;
	static bool start = false;
	static uint8_t cnt_rx_byte = 0;
	
	tmp = UART_ReceiveData(MDR_UART1);
	
	if((tmp == 0xAA)&&(!start))
	{
		start = true;
	}
	else
	{
		if(start)
		{
			if(!RX_Ok)
				ArrayRX[cnt_rx_byte] = tmp; 
			
			++cnt_rx_byte;
			
			switch(cur_dev)
			{
				case NUMBER_PDG:
					if(cnt_rx_byte == PDG_FRAME_SIZE)
						{	
							RX_Ok = true;
							start = false;
							cnt_rx_byte = 0;
						}
					break;
						
				case NUMBER_PZF:
					if(cnt_rx_byte == PZF_FRAME_SIZE)
						{	
							RX_Ok = true;
							start = false;
							cnt_rx_byte = 0;
						}
					break;
						
				default: break;
			}	
		}
	}
}
//==================================================================================
void Timer1_IRQHandler(void)//100ms
{
	static uint8_t cnt_sec=0,
					cnt_link = CNT_LINK;
	static bool RX_PDG = false,
				RX_PZF = true;
	
	MDR_TIMER1->CNT = 0x0000;
	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
	 

	if(cnt_sec > 9)
	{
		flash_pos ^= true;
		cnt_sec = 0;
	}
	else
		++cnt_sec;
	
	++time_out_byte;
	
	
	if(RX_Ok)
		{		
			if(Read_Data(cur_dev))
			{
				status_link |= (1<<(cur_dev&1));
			}
			else
			{
				status_link &= ~(1<<(cur_dev&1));
			}

			if(status_link == 3)
			{
				cnt_link = CNT_LINK;
			}
			RX_Ok = false;
			
			if(RX_PDG)
			{
				RX_PDG = false;
				RX_PZF = true;
				cur_dev = NUMBER_PZF;
			}
			else
			if(RX_PZF)
			{
				RX_PZF = false;
				RX_PDG = true;
				cur_dev = NUMBER_PDG;
			}
		}
		else
		{
			if(cnt_link)
				--cnt_link;
			else
			{
				cnt_link = CNT_LINK;
				status_link &= ~(1<<(cur_dev&1));
				
				if(RX_PDG)
				{
					RX_PDG = false;
					RX_PZF = true;
					cur_dev = NUMBER_PZF;
				}
				else
				if(RX_PZF)
				{
					RX_PZF = false;
					RX_PDG = true;
					cur_dev = NUMBER_PDG;
				}
			}
		}
		TX_Data(cur_dev);
}

//==================================================================================
//extern const char font5x7[];
void Timer2_IRQHandler(void)//2.5ms
{
	static uint8_t 	cnt_row = 0;
	uint8_t reg = 7;
	
	MDR_TIMER2 -> CNT = 0x0000;
	MDR_TIMER2 -> STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ( Timer2_IRQn );
	
	LCD_DIS;
	LCD_ALL_ROW_OFF;
	
	while( reg )
	{
		--reg;
		SSP_SendData( MDR_SSP1, (uint16_t)led_reg[reg][cnt_row] );
		while( !SSP_GetFlagStatus( MDR_SSP1, SSP_FLAG_TNF ) );
	}
	while( SSP_GetFlagStatus( MDR_SSP1, SSP_FLAG_BSY ) );
	
	LCD_EN;
	
	switch( cnt_row )
	{
		case 0:
			LCD_ROW1_ON;
			break;
		case 1:
			LCD_ROW2_ON;
			break;
		case 2:
			LCD_ROW3_ON;
			break;
		case 3:
			LCD_ROW4_ON;
			break;
		case 4:
			LCD_ROW5_ON;
			break;
		default: break;
	}


	if( cnt_row < 4 )
		++cnt_row;
	else
		cnt_row = 0;
}
//==================================================================================
void SysTick_Handler(void)//1 ms
{
 //	static uint8_t cnt_s = 0;
// 	static uint32_t	cnt_sec=0;
	
// 	if(cnt_s>2)
// 	{
		if(delay_count)
		{
			delay_count--;
		}
// 		cnt_s = 0;
// 	}
// 	else
// 		++cnt_s;
}
//==================================================================================
//==================================================================================
//==================================================================================


int main(void)
{
	uint8_t cnt=255, read_kal=0, cnt_b=0;
	uint16_t prev_KN_DG = 0;
	bool link_ok = false;
	
	cnt=255;
	while(--cnt)
	__nop();
	
	sys_init();

	IWDG_ReloadCounter();//WDT reset //409.5ms
	delay_ms(30);
	
	
	
	if(TEST_MODE)
	//if(1)
	{
		Show_led_str( Yanv_2015_ETRO, 17 );
		fill_reg_led();
		while(1)
		{
			delay_ms( 100 );
			scroll_mess_left();
			IWDG_ReloadCounter();//WDT reset //409.5ms
		}
	}
	
	
	cnt = 0;
	
	delay_ms( 1000 );
	
	while(1)
	{
		if(status_link != 3)
		{
			link_ok = false;
			if(status_link&1)
			{
				OK_PDG();
				delay_ms(1000);
			}
			else
			{
				
				link_ok = false;
				no_link_PDG();
				delay_ms(1000);
			}
			
			if(status_link&2)
			{
				OK_PZF();
				delay_ms(1000);
			}
			else
			{
				no_link_PZF();
				delay_ms(1000);
			}
		}
		else
		{
			link_ok = true;
			if(Status.cal_b & 0x80)
			{
				if(Status.KN_DG != prev_KN_DG)
				{
					prev_KN_DG = Status.KN_DG;
					
					if(Status.KN_DG & ((uint16_t)1<<8))//sel +
					{
						if(num_par < 16)
							++num_par;
						else 
							num_par = 1;
					}
					
					if(Status.KN_DG & ((uint16_t)1<<9))//sel -
					{
						if(num_par > 1)
							--num_par;
						else 
							num_par = 16;
					}
				}
			}
		}
			
		
		IWDG_ReloadCounter();//WDT reset //409.5ms
		
		if(link_ok && !(Status.cal_b & 0x80))
		{
			if((Status.KN_DG & ((uint16_t)1<<3)))//CA
			{
				F1_Off = false;
				F2_Off = false;
				rst_alarm = true;
			}
			else
				if(!Status.Error)
					rst_alarm = false;
			
			if(Status.RegWrk == 0) //DG ostanov
			{
				F1_Off = false;
				F2_Off = false;
				
				clear_lcd();
				if((Status.KN_DG & ((uint16_t)1<<2)))//��� 
				{
					Show_led_str(Mu_upr,13);
				}
				else	//DU
				{
					Show_led_str(Du_upr,13);
					
					if((Status.KN_DG & ((uint16_t)1<<1))) //������ ���������
					{
						Show_led_str(Prot_OFF,12);
					}
				}
			}
			else
			if(Status.RegWrk&0x40) //DG work
			{
//				if(set_kalibr)
//					show_led_data((num_par&0x1F));
//				else
				{
					if(!Status.Error)
					{
						F1_Off = false;
						F2_Off = false;
						
						if(Status.TG > 37)
						{
							if((Status.KN_ZF&(1<<4))||(Status.KN_ZF&(1<<5))) //PZF K1 K2 ON
								show_led_data(0);
							else
							{
								Show_led_str(PN_RAZR,13);
							}
						}
						else
						{
							Show_led_str(OG_m_37,11);
						}
					}
				}
			}
			else
			if(Status.RegWrk&4) //DG start
			{
				Show_led_str(PUSK,10);
			}
			else
			if(Status.RegWrk&0x10) //DG stop
			{
				F1_Off = true;
				F2_Off = true;
				Show_led_str(OSTANOV,11);
			}
			else
			if(Status.RegWrk&0x80) //DG rashol
			{
				F1_Off = true;
				F2_Off = true;
				Show_led_str(Rashol,11);
			}
			
			fill_reg_led();
			delay_ms(1000);

			
			if(Status.Error)
 			{
				cnt_b=0;
				while(cnt_b < 16)
				{
					if(Status.Error	&(1<<cnt_b))
					{
						switch( cnt_b )
						{
							case 0://no pusk
									Show_led_str( NO_PUSK,12 );
								break;
							case 1://DG no stop
									Show_led_str( NO_STOP,14 );
								break;
							case 2://raznos
									Show_led_str( Raznos,6 );
									F1_Off = true;
									F2_Off = true;
								break;
							case 3:	//n<400
								break;
							case 4://��
									Show_led_str( Alarm_OG,12 );
								break;
							case 5://���
									F1_Off = true;
									F2_Off = true;
									Show_led_str( Alarm_PM,12 );
								break;
							case 6://no vozb
									if(!((Status.Error>>5)&1))//no ddm
										Show_led_str( NO_Vozb,12 );
									
									F1_Off = true;
									F2_Off = true;
								break;
							case 7: //Ug >< DOP
									if((Status.UUg > 253) || (Status.UWg > 253) || (Status.UVg > 253))
									{
										Show_led_str(Ug_b_dop,11);
									}
									else
									{
										Show_led_str(Ug_m_dop,11);
									}
									
									F1_Off = true;
									F2_Off = true;
								break;
							case 8: //> 120%
									Show_led_str(Overload,10);
								break;
							case 9://> 50%
									Show_led_str(Overload,10);
								break;
							case 10://> 200%
									Show_led_str(KZ_load,11);
								break;
							case 11:
								Show_led_str(ALARM_TF,11);
								
								break;
							case 12:
								break;
							case 13:
								break;
							case 14:
								break;
							case 15:
								break;
						}
						fill_reg_led();
						delay_ms(1000);
					}
					++cnt_b;
				}
			}
		}
		else
		{
			if(Status.cal_b & 0x80)
				show_led_data((num_par&0x1F));
		}

// 		if(cnt<95)
// 		{
// 			delay_ms(100);
// 			//scroll_mess_left();
// 			++cnt;
// 		}
// 		else
// 		{
// 			delay_ms(5000);
// 			cnt=0;
// 		}
	}
}

//==================================================================================
//==================================================================================
//==================================================================================
void sys_init(void)
{
	PLL_init();
	GPIO_init();
	TIM1_init();
	TIM2_init();
	UART_init();
	SPI_init();
	//SPI_init();
	//WDT_init();
	
	SysTick_Config(HSE_Value/1000);//1 ms
}
//==================================================================================
#define ALL_PORTS_CLK ( RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
						RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
						RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF )
void GPIO_init(void)
{
	PORT_InitTypeDef PORT_InitStructure;
	
/* Enable the RTCHSE clock on all ports */
	RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
	PORT_StructInit(&PORT_InitStructure);//reset struct	
/* Configure PORTA pins */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_1 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTA, ( PORT_Pin_1 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7), Bit_RESET );
	
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;

	PORT_Init(MDR_PORTA, &PORT_InitStructure);
	
	
PORT_StructInit(&PORT_InitStructure);//reset struct	
/* Configure PORTB pin */
	PORT_InitStructure.PORT_Pin   = (PORT_Pin_5|PORT_Pin_6);//UART1 RX TX
	PORT_InitStructure.PORT_PD 	= PORT_PD_OPEN;
	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTB, &PORT_InitStructure);

PORT_StructInit(&PORT_InitStructure);//reset struct	
	
/* Configure PORTC pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTC, (PORT_Pin_0),Bit_RESET);//close optr
	
	
/* Configure PORTD pin */

	PORT_InitStructure.PORT_Pin  = (PORT_Pin_4);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTD, (PORT_Pin_4),Bit_RESET);
	
	
	
/* Configure PORTE pin */

	PORT_InitStructure.PORT_Pin   = (PORT_Pin_0|PORT_Pin_1|PORT_Pin_6|PORT_Pin_7);
	PORT_InitStructure.PORT_PD 	  = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_OE	  = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	PORT_WriteBit(MDR_PORTE, (PORT_Pin_0|PORT_Pin_1|PORT_Pin_6|PORT_Pin_7),Bit_RESET);

	//PORT_StructInit(&PORT_InitStructure);//reset struct	

}
//==================================================================================
void TIM1_init(void)
{
	TIMER_CntInitTypeDef sTIM_CntInit;
	
	TIMER_CntStructInit(&sTIM_CntInit);
	
	// TIMER1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1,ENABLE);
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER1);

	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	/* TIM1 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
	sTIM_CntInit.TIMER_Prescaler                = 8000;//100ms
	//sTIM_CntInit.TIMER_Prescaler                = 31996;//100ms
	sTIM_CntInit.TIMER_Period                   = 100;
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

	NVIC_EnableIRQ(Timer1_IRQn);
	TIMER_ITConfig(MDR_TIMER1,TIMER_STATUS_CNT_ARR, ENABLE);
	
  
	/* TMR1 enable */
	TIMER_Cmd (MDR_TIMER1,ENABLE);
}
//==================================================================================
void TIM2_init(void)
{
	TIMER_CntInitTypeDef sTIM_CntInit;
	
	TIMER_CntStructInit(&sTIM_CntInit);

	
	// TIMER2
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2,ENABLE);
	
/* Reset all TIMER2 settings */
	TIMER_DeInit(MDR_TIMER2);

	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	/* TIM2 configuration ------------------------------------------------*/
	/* Initializes the TIMERx Counter ------------------------------------*/
	sTIM_CntInit.TIMER_IniCounter				=0;
	//sTIM_CntInit.TIMER_Prescaler                = 4000;//50ms
	sTIM_CntInit.TIMER_Prescaler                = 160;
	sTIM_CntInit.TIMER_Period                   = 300;//
	sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
	sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER2,&sTIM_CntInit);

	NVIC_EnableIRQ(Timer2_IRQn);
	TIMER_ITConfig(MDR_TIMER2,TIMER_STATUS_CNT_ARR, ENABLE);
	
  
	/* TMR2 enable */
	TIMER_Cmd (MDR_TIMER2,ENABLE);
}
//==================================================================================
void PLL_init(void)
{	
	/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig( RST_CLK_HSE_ON );
	
	/* Enable CPU_PLL */
   //   RST_CLK_CPU_PLLcmd(ENABLE);
	
	while( RST_CLK_HSEstatus() == ERROR );

	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );
	MDR_RST_CLK->CPU_CLOCK |= 2;//CPU_C1 set HSE
	RST_CLK_HSIcmd( DISABLE );
	/* Configure CPU_PLL clock */
// 	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSEdiv1,RST_CLK_CPU_PLLmul4); //48MHz
// 	RST_CLK_CPU_PLLuse(ENABLE);
	//RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
}
//==================================================================================
void SPI_init(void)
{
	PORT_InitTypeDef PORT_InitStructure;
	SSP_InitTypeDef SPI_InitStructure;
	
	PORT_StructInit( &PORT_InitStructure );//reset struct

	PORT_InitStructure.PORT_Pin = ( PORT_Pin_0 | PORT_Pin_1 );//spi
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init( MDR_PORTF, &PORT_InitStructure );

 	RST_CLK_PCLKcmd( RST_CLK_PCLK_SSP1, ENABLE );

	/* Reset all SSP settings */
	SSP_DeInit( MDR_SSP1 );	
	
	SSP_BRGInit( MDR_SSP1, SSP_HCLKdiv8 );
	

	/* SSP1 MASTER configuration ------------------------------------------------*/
	SSP_StructInit (&SPI_InitStructure);
	
	SPI_InitStructure.SSP_SCR = 0x00;
	SPI_InitStructure.SSP_CPSDVSR = 10;
	SPI_InitStructure.SSP_Mode = SSP_ModeMaster;
	SPI_InitStructure.SSP_WordLength = SSP_WordLength16b;
	SPI_InitStructure.SSP_SPH = SSP_SPH_2Edge;
	SPI_InitStructure.SSP_SPO = SSP_SPO_Low;
	SPI_InitStructure.SSP_FRF = SSP_FRF_SPI_Motorola;
	SPI_InitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;

	SSP_Init( MDR_SSP1, &SPI_InitStructure );
	  /* Enable SSP1 */
	SSP_Cmd( MDR_SSP1, ENABLE );
}
//==================================================================================
void UART_init(void)
{
	
	UART_InitTypeDef UART_InitStructure;

	UART_StructInit(&UART_InitStructure);

	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1,ENABLE);
	/* Set the HCLK division factor = 1 for UART1,UART2*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_DeInit(MDR_UART1);

	/* Disable interrupt on UART1 */
	NVIC_DisableIRQ(UART1_IRQn);

	/* Initialize UART_InitStructure */
	UART_InitStructure.UART_BaudRate			= 9600;
	//UART_InitStructure.UART_BaudRate			= 38400;
	UART_InitStructure.UART_WordLength			= UART_WordLength8b;
	UART_InitStructure.UART_StopBits			= UART_StopBits2;
	UART_InitStructure.UART_Parity				= UART_Parity_No;
	UART_InitStructure.UART_FIFOMode			= UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl	= UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

	/* Configure UART1 parameters*/
	UART_Init(MDR_UART1,&UART_InitStructure);

	NVIC_EnableIRQ(UART1_IRQn);
	UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);
	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART1,ENABLE);
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
//==================================================================================
void TX_Data(uint8_t add_dev)
{
	//static uint8_t cnt_link=0;
	uint8_t sum=0,tmp1=0, tmp=0;
// 	PKDU.byte1=(PKDU.StatusKN)&0xFF;
// 	PKDU.byte2=((PKDU.StatusKN>>8)&3)|((PKDU.SchowPar<<4)&0xF0);
	tmp = ((rst_alarm<<5)|(F1_Off<<6)|(F2_Off<<7)) & 0xE0;
	tmp1 = (num_par & 0x0F) | (Status.cal_b & 0xF0);
	sum = tmp1+tmp+add_dev+0xAA;
	sum = 255-sum;
	++sum;	

	UART1_send_byte(0xAA);
	UART1_send_byte(tmp);
	UART1_send_byte(add_dev);
	UART1_send_byte(tmp1);
	UART1_send_byte(sum);
}
//==================================================================================
bool Read_Data(uint8_t add_dev)
{
	uint8_t sum = 0, cnt = 0;
	
	switch(add_dev)
	{
		case NUMBER_PDG:
			while(cnt<(PDG_FRAME_SIZE))
			{
				sum += ArrayRX[cnt];
				++cnt;
			}
			break;
			
		case NUMBER_PZF:
			while(cnt<(PZF_FRAME_SIZE))
			{
				sum += ArrayRX[cnt];
				++cnt;
			}
			break;
		default: return false;
	}

	sum += 0xAA;//����� ����
	
	if(sum)//check CRC
	{
		return false;
	}
	else
	{
		uint8_t numb = ArrayRX[0];
		if(numb != add_dev)
			return false;
	}
	
	if(add_dev == NUMBER_PDG)
	{
		Status.KN_DG = (ArrayRX[2]<<8)|ArrayRX[1];
		Status.Error &= 0xFF00;
		Status.Error |= (ArrayRX[3] & 0xFF);
		Status.UUg = (ArrayRX[5]<<8)|ArrayRX[4];
		Status.UVg = (ArrayRX[7]<<8)|ArrayRX[6];
		Status.UWg = (ArrayRX[9]<<8)|ArrayRX[8];
		Status.UF =  (ArrayRX[11]<<8)|ArrayRX[10];
		Status.IUg = (ArrayRX[13]<<8)|ArrayRX[12];
		Status.IVg = (ArrayRX[15]<<8)|ArrayRX[14];
		Status.IWg = (ArrayRX[17]<<8)|ArrayRX[16];
		Status.ISL = ArrayRX[18];
		Status.TG = ArrayRX[19];
		Status.NDiz = (ArrayRX[21]<<8)|ArrayRX[20];
		Status.RegWrk = (ArrayRX[23]<<8)|ArrayRX[22];
		Status.cal_b = ArrayRX[24];
	}
	else
		if(add_dev == NUMBER_PZF)
		{
			Status.KN_ZF =ArrayRX[1];
			Status.Error &= 0x00FF;
			Status.Error |= ((uint16_t)ArrayRX[2]<<8)&0xFF00;
			Status.IAf1 =(ArrayRX[4]<<8)|ArrayRX[3];
			Status.IBf1 =(ArrayRX[6]<<8)|ArrayRX[5];
			Status.ICf1 =(ArrayRX[8]<<8)|ArrayRX[7];
			Status.IAf2 =(ArrayRX[10]<<8)|ArrayRX[9];
			Status.IBf2 =(ArrayRX[12]<<8)|ArrayRX[11];
			Status.ICf2 =(ArrayRX[14]<<8)|ArrayRX[13];
		}
		
	return true;
}
//==================================================================================
uint8_t Calc_CRC(const RegS* data_crc)
{
	return 0;
}
//==================================================================================
void UART1_send_byte(uint8_t byte)
{
//	uint8_t time_out_byte=0;
	
	UART_SendData(MDR_UART1,byte);
	time_out_byte=0;
	
	while(UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFF));

	UART_ReceiveData (MDR_UART1);
	
	UART_ClearITPendingBit( MDR_UART1,UART_IT_RX );
	
//	while(UART_GetFlagStatus (MDR_UART1, UART_FLAG_BUSY)== SET)
//	{
//		UART_ReceiveData (MDR_UART1);
//	}
}
//==================================================================================
void scroll_mess_left(void)
{
	uint8_t digit=0, row=0, tmp_reg = 0;
	
	tmp_reg = led_reg[0][0];
	
	while(digit<18)
	{
		row = 0;
		while(row < 4)
		{
			led_reg[digit][row] = led_reg[digit][row + 1];
			++row;
		}
		
		led_reg[digit][row] = led_reg[digit+1][0];
		
		++digit;
	}
	led_reg[digit][0] = led_reg[digit][1];
	led_reg[digit][1] = led_reg[digit][2];
	led_reg[digit][2] = led_reg[digit][3];
	led_reg[digit][3] = led_reg[digit][4];
	led_reg[digit][4] = tmp_reg;
}
//==================================================================================
void no_link_PDG(void)
{
	uint8_t cnt_b=0, i = 0;
	
	while(cnt_b<9)
	{
		index_char[cnt_b] = Err_link[i];
		++cnt_b;
		++i;
	}
	index_char[cnt_b] = 0;
	++cnt_b;
	i = 0;
	while(cnt_b<13)
	{
		index_char[cnt_b] = PDG[i];
		++cnt_b;
		++i;
	}
	while(cnt_b<16)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	fill_reg_led();
}
//==================================================================================
void no_link_PZF(void)
{
	uint8_t cnt_b=0, i = 0;
	
	while(cnt_b<9)
	{
		index_char[cnt_b] = Err_link[i];
		++cnt_b;
		++i;
	}
	index_char[cnt_b] = 0;
	++cnt_b;
	i = 0;
	while(cnt_b<13)
	{
		index_char[cnt_b] = PZF[i];
		++cnt_b;
		++i;
	}
	while(cnt_b<16)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	fill_reg_led();
}
//==================================================================================
void clear_lcd(void)
{
	uint8_t reg=0, row=0, cnt_b=0;
	
	while(cnt_b<16)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	
	while(reg<7)
	{
		row=0;
		while(row<5)
		{
			led_reg[reg][row] = 0;
			++row;
		}
		++reg;
	}
}
//==================================================================================
void OK_PDG(void)
{
	uint8_t cnt_b=0, i = 0;
	
	clear_lcd();
	
	while(cnt_b<3)
	{
		index_char[cnt_b] = PDG[i];
		++cnt_b;
		++i;
	}

	++cnt_b;
	i = 0;
	while(cnt_b<6)
	{
		index_char[cnt_b] = OK[i];
		++cnt_b;
		++i;
	}

	fill_reg_led();
}
//==================================================================================
void OK_PZF(void)
{
	uint8_t cnt_b=0, i = 0;
	
	clear_lcd();
	
	while(cnt_b<3)
	{
		index_char[cnt_b] = PZF[i];
		++cnt_b;
		++i;
	}

	++cnt_b;
	i = 0;
	while(cnt_b<6)
	{
		index_char[cnt_b] = OK[i];
		++cnt_b;
		++i;
	}

	fill_reg_led();
}
//==================================================================================
void fill_reg_led(void)
{
	uint8_t reg = 0, 
			row = 0,
			digit = 0,
			bit8 = 0,
			bit16 = 0;
	
	
	while( row < 5 )
	{
		reg = 0;
		digit = 0;
		while( reg < 7 )
		{
			led_reg[reg][row] = 0;
			
			bit16 = 0;
			while( (bit16 < 16) && (reg < 7))
			{
				while( bit8 < 7 )
				{
					++bit8;	
					//led_reg[reg][row] |= ((uint16_t)((tmp >> bit8)&1)) << bit16;
					led_reg[reg][row] |= ((uint16_t)((font5x7[index_char[digit] + row] >> (8-bit8))&1)) << bit16;
					
					if( bit16<15 )
						++bit16;
					else
					{
						bit16 = 0;
						++reg;
						if(reg < 7)
							led_reg[reg][row] = 0;
					}
				}
				bit8 = 0;
				++digit;
			}
			++reg;
		}
		++row;
	}
}
//==================================================================================
void show_led_data(uint8_t num_par)
{
	static uint8_t cnt_b = 0;
	static uint32_t sum_nd = 0;
	
//	clear_lcd();
	
	switch(num_par)
	{
		case 0:	//IgA
			index_char[0] = (uint16_t)5*66;//I
			index_char[1] = (uint16_t)5*36;//�
			index_char[2] = (uint16_t)5*5;//%
			index_char[3] = 0;//
			index_char[4] = (uint16_t)33*5;//A
			DecSegm(((Status.IUg*77)>>8));
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
			index_char[8] = (uint16_t)35*5;//B
			DecSegm((Status.IVg*77)>>8);
			index_char[9] = (uint16_t)(IndData[2]+16)*5;
			index_char[10] = (uint16_t)(IndData[3]+16)*5;
			index_char[11] = (uint16_t)(IndData[4]+16)*5;
		
			index_char[12] = (uint16_t)50*5;//C
			DecSegm((Status.IWg*77)>>8);
			index_char[13] = (uint16_t)(IndData[2]+16)*5;
			index_char[14] = (uint16_t)(IndData[3]+16)*5;
			index_char[15] = (uint16_t)(IndData[4]+16)*5;

			break;
		case 1:	//IgA
		case 2:	//IgB
		case 3:	//IgC
			index_char[0] = (uint16_t)5*66;//I
			index_char[1] = (uint16_t)5*36;//�
			index_char[2] = 0;//
			index_char[3] = 0;//
		if(flash_pos && (num_par == 1))
		{
			index_char[4] = 0;
		}
		else
			index_char[4] = (uint16_t)33*5;//A
			
		
			DecSegm(Status.IUg);
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 2))
		{
			index_char[8] = 0;
		}
		else	
		index_char[8] = (uint16_t)35*5;//B
			DecSegm(Status.IVg);
			index_char[9] = (uint16_t)(IndData[2]+16)*5;
			index_char[10] = (uint16_t)(IndData[3]+16)*5;
			index_char[11] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 3))
		{
			index_char[12] = 0;
		}
		else	
		index_char[12] = (uint16_t)50*5;//C
			DecSegm(Status.IWg);
			index_char[13] = (uint16_t)(IndData[2]+16)*5;
			index_char[14] = (uint16_t)(IndData[3]+16)*5;
			index_char[15] = (uint16_t)(IndData[4]+16)*5;

			break;
		case 4:	//UgA
		case 5:	//UgB
		case 6:	//UgC
			//Ug
			index_char[0] = (uint16_t)5*65;//U
			index_char[1] = (uint16_t)5*36;//�
			index_char[2] = 0;//
			index_char[3] = 0;//
		if(flash_pos && (num_par == 4))
		{
			index_char[4] = 0;
		}
		else
			index_char[4] = (uint16_t)33*5;//A
			DecSegm(Status.UUg);
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 5))
		{
			index_char[8] = 0;
		}
		else	
		index_char[8] = (uint16_t)35*5;//B
			DecSegm(Status.UVg);
			index_char[9] = (uint16_t)(IndData[2]+16)*5;
			index_char[10] = (uint16_t)(IndData[3]+16)*5;
			index_char[11] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 6))
		{
			index_char[12] = 0;
		}
		else	
		index_char[12] = (uint16_t)50*5;//C
			DecSegm(Status.UWg);
			index_char[13] = (uint16_t)(IndData[2]+16)*5;
			index_char[14] = (uint16_t)(IndData[3]+16)*5;
			index_char[15] = (uint16_t)(IndData[4]+16)*5;
			break;
		case 7:
			//UabF
		if(flash_pos && (num_par == 7))
		{
			index_char[0] = 0;
			index_char[1] = 0;
			index_char[2] = 0;
			index_char[3] = 0;
		}
		else
		{
			index_char[0] = (uint16_t)5*65;//U
			index_char[1] = (uint16_t)5*33;//a
			index_char[2] = (uint16_t)5*34;//b
			index_char[3] = (uint16_t)5*53;//�
		}
			index_char[4] = 0;
			DecSegm(Status.UF);
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
			index_char[8] = 0;
			index_char[9] = 0;
			index_char[10] = 0;
			index_char[11] = 0;
		
			index_char[12] = 0;
			index_char[13] = 0;
			index_char[14] = 0;
			index_char[15] = 0;
			break;
		case 8:
		case 9:
		case 10:
			//If1
			index_char[0] = (uint16_t)5*66;//I
			index_char[1] = (uint16_t)5*53;//�
			index_char[2] = (uint16_t)5*17;//1
			index_char[3] = 0;//
		
			
		if(flash_pos && (num_par == 8))
		{
			index_char[4] = 0;
		}
		else
		index_char[4] = (uint16_t)33*5;//A
			DecSegm(Status.IAf1);
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 9))
		{
			index_char[8] = 0;
		}
		else	
		index_char[8] = (uint16_t)35*5;//B
			DecSegm(Status.IBf1);
			index_char[9] = (uint16_t)(IndData[2]+16)*5;
			index_char[10] = (uint16_t)(IndData[3]+16)*5;
			index_char[11] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 10))
		{
			index_char[12] = 0;
		}
		else	
		index_char[12] = (uint16_t)50*5;//C
			DecSegm(Status.ICf1);
			index_char[13] = (uint16_t)(IndData[2]+16)*5;
			index_char[14] = (uint16_t)(IndData[3]+16)*5;
			index_char[15] = (uint16_t)(IndData[4]+16)*5;

			break;
		case 11:
		case 12:
		case 13:
			//If2
			index_char[0] = (uint16_t)5*66;//I
			index_char[1] = (uint16_t)5*53;//�
			index_char[2] = (uint16_t)5*18;//2
			index_char[3] = 0;//
		
		if(flash_pos && (num_par == 11))
		{
			index_char[4] = 0;
		}
		else	
		index_char[4] = (uint16_t)33*5;//A
			DecSegm(Status.IAf2);
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 12))
		{
			index_char[8] = 0;
		}
		else	
		index_char[8] = (uint16_t)35*5;//B
			DecSegm(Status.IBf2);
			index_char[9] = (uint16_t)(IndData[2]+16)*5;
			index_char[10] = (uint16_t)(IndData[3]+16)*5;
			index_char[11] = (uint16_t)(IndData[4]+16)*5;
		
		if(flash_pos && (num_par == 13))
		{
			index_char[12] = 0;
		}
		else
		index_char[12] = (uint16_t)50*5;//C
			DecSegm(Status.ICf2);
			index_char[13] = (uint16_t)(IndData[2]+16)*5;
			index_char[14] = (uint16_t)(IndData[3]+16)*5;
			index_char[15] = (uint16_t)(IndData[4]+16)*5;
			break;
		case 14:
			if(flash_pos && (num_par == 14))
				{
				index_char[0] = 0;
				index_char[1] = 0;
				index_char[2] = 0;
				}
			else
				{
				//���
				index_char[0] = (uint16_t)5*56;//�
				index_char[1] = (uint16_t)5*37;//�
				index_char[2] = (uint16_t)5*35;//�
				}
			index_char[3] = 0;//
				
				sum_nd += Status.NDiz;
				++cnt_b;
				if(cnt_b>49)
				{
					cnt_b =0;
					sum_nd /=50;
					DecSegm(sum_nd);
				}
			index_char[4] = (uint16_t)(IndData[1]+16)*5;
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
			index_char[8] = 0;
			index_char[9] = 0;
			index_char[10] = 0;
			index_char[11] = 0;
		
			index_char[12] = 0;
			index_char[13] = 0;
			index_char[14] = 0;
			index_char[15] = 0;
			break;

		case 15:
			//���
			index_char[0] = (uint16_t)5*41;//�
			index_char[1] = (uint16_t)5*40;//�
			index_char[2] = (uint16_t)5*44;//�
			index_char[3] = 0;//
			DecSegm(Status.ISL);
			index_char[4] = (uint16_t)(IndData[2]+16)*5;
			index_char[5] = (uint16_t)(IndData[3]+16)*5;
			index_char[6] = (uint16_t)(IndData[4]+16)*5;
			index_char[7] = 0;
		
			index_char[8] = 0;
			index_char[9] = 0;
			index_char[10] = 0;
			index_char[11] = 0;
		
			index_char[12] = 0;
			index_char[13] = 0;
			index_char[14] = 0;
			index_char[15] = 0;
			break;
		case 16:
			//��
		if(flash_pos && (num_par == 16))
		{
			index_char[0] = 0;
			index_char[1] = 0;
		}
		else{
			index_char[0] = (uint16_t)5*47;//�
			index_char[1] = (uint16_t)5*39;//�
			}
			index_char[2] = 0;//
			index_char[3] = 0;//
			DecSegm(Status.TG);
			index_char[4] = 0;//(uint16_t)(IndData[3]+16)*5;
			index_char[5] = (uint16_t)(IndData[2]+16)*5;
			index_char[6] = (uint16_t)(IndData[3]+16)*5;
			index_char[7] = (uint16_t)(IndData[4]+16)*5;
		
			index_char[8] = 0;
			index_char[9] = 0;
			index_char[10] = 0;
			index_char[11] = 0;
		
			index_char[12] = 0;
			index_char[13] = 0;
			index_char[14] = 0;
			index_char[15] = 0;
			break;
		
		default: break;
	}
	
	fill_reg_led();
}
//==================================================================================
void DecSegm(uint16_t R0)
{
 	uint16_t R1=0;

	R1=R0/10000;
	IndData[0]=R1;
	R1=R0%10000;
	R0=R1/1000;
	IndData[1]=R0;
	R0=R1%1000;
	R1=R0/100;
	IndData[2]=R1;
	R1=R0%100;
	R0=R1/10;
	IndData[3]=R0;
	R1=R1%10;
	IndData[4]=R1;	
	
}
//==================================================================================
void link_dev(void)
{
// 	static bool RX_PDG = false,
// 				RX_PZF = false;
// 	
// 	if(!RX_PZF)
// 	{
// 		if(!RX_PDG) //read PDG
// 		{
// 			RX_PDG = true;
// 			cur_dev = NUMBER_PDG;
// 			TX_Data(NUMBER_PDG);
// 		}
// 		else
// 		{
// 			if((time_out_byte > 2) && (!RX_Ok)) // ��� ����� ���
// 			{
// 				RX_PDG = false;
// 			}
// 		}
// 	}
// 		
// 	if(!RX_PDG)
// 	{
// 		if(!RX_PZF)	//read PZF
// 		{
// 			RX_PZF = true;
// 			cur_dev = NUMBER_PZF;
// 			TX_Data(NUMBER_PZF);
// 		}
// 		else
// 		{
// 			if((time_out_byte > 2) && (!RX_Ok)) // ��� ����� ���
// 			{
// 				RX_PZF = false;
// 				--cnt_link;
// 			}
// 		}
// 	}
}
//==================================================================================
void Show_led_str(const uint16_t *str,uint8_t len)
{
	uint8_t cnt_b = 0;
	
	clear_lcd();
	
	while(cnt_b < len)
	{
		index_char[cnt_b] = str[cnt_b];
		++cnt_b;
	}
}







