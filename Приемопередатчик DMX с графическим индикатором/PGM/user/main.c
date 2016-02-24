/**
  ******************************************************************************
  * @file     main.c
  * @author   Max K
  * @version  V1.0.0
  * @date     6-10-2014
  * @brief    This file contains the main function.
  ******************************************************************************

  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "buzzer.h"
#include "lcd.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <iostm8s207mb.h>
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LED_REVERS GPIO_WriteReverse(GPIOA,GPIO_PIN_6)    
#define POWER_ON	GPIO_WriteHigh(GPIOG,GPIO_PIN_0)   
#define POWER_OFF	GPIO_WriteLow(GPIOG,GPIO_PIN_0)

#define ENC_A	(GPIO_ReadInputPin(GPIOD,GPIO_PIN_2))
#define ENC_B	(GPIO_ReadInputPin(GPIOD,GPIO_PIN_3))
#define ENC_SW	!(GPIO_ReadInputPin(GPIOD,GPIO_PIN_0))
#define R_STOP	0
#define R_LEFT	1
#define R_RIGHT	2
#define R_SW	3
/* Evalboard I/Os configuration */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t State_MelodyCallBack, enc_rot;
volatile uint16_t GTimeStamp = 0, ADC_V = 0;
volatile bool adc_cmp = 0,
				new_data = 0;		//флаг что прин¤ты новые данные;
volatile uint8_t	data_receiv;	//прин¤тый пакет		
					
volatile uint16_t	adres_dev = 10,	//адрес ус-ва
					receiv_add = 0;	//принятый адр
/* Private function prototypes -----------------------------------------------*/
void Init_Application(void);
void WaitDelay(uint16_t Delay);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief Timer1 Update/Overflow Interrupt routine. 1ms
  * @param  None
  * @retval None
  */
#pragma vector = TIM1_OVR_UIF_vector
 __interrupt void IRQ_TMR1_OVF(void)
{
  	static uint8_t cnt = 0, prev_cnt = 0;
	static bool start = false, end = false;
	
    TIM1_ClearFlag( TIM1_FLAG_UPDATE );
	 
	if( (!ENC_A && !ENC_B) && !start)
	{
	  start = true;
	  cnt = 0;
	  nop();
	}
	else
	{
	  if(start && !end)
	  {
			if(ENC_A && !ENC_B)
			{
				enc_rot = R_RIGHT;
				end = true;
				//start = false;
				cnt = 0;
			}
			if(!ENC_A && ENC_B)
			{
				enc_rot = R_LEFT;
				end = true;
				cnt = 0;
			}
	  }
	}
	
	if(ENC_SW)
	{
	  enc_rot = R_SW;
	}
	
	if(end)
	{
	  ++cnt;
		if(cnt > 50) //50 ms
		{
			cnt = 0;
			start = false;
			end = false;
			enc_rot = R_STOP;
		}
	}
}
/**
  * @brief Timer4 Update/Overflow Interrupt routine. 2ms
  * @param  None
  * @retval None
  */
#pragma vector = TIM4_OVR_UIF_vector
 __interrupt void IRQ_TMR4_OVF(void)
{
  static uint8_t cnt = 0;
    TIM4_ClearFlag( TIM4_FLAG_UPDATE );
//    GPIO_WriteReverse(GPIOH, GPIO_PIN_4); /* Debug */
//    GTimeStamp++;
    if( cnt>100 )
    {
      LED_REVERS;
	  cnt = 0;
    }
	else
	  ++cnt;
	
	++GTimeStamp;
}
/**
  * @brief ADC2 interrupt routine.
  * @param  None
  * @retval None
  */
#pragma vector = ADC2_EOC_vector
 __interrupt void IRQ_ADC2(void)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
   
   if(!adc_cmp)
   {
	   ADC_V = ADC2_GetConversionValue();
	   adc_cmp = true;
   }
   else
	 ADC2_GetConversionValue();
   
   ADC2_ClearITPendingBit();
 }

/**
  * @brief UART1 RX Interrupt routine.
  * @param  None
  * @retval None
  */
#pragma vector = UART1_R_RXNE_vector
 __interrupt void IRQ_UART1_RX(void)
 {
   u8 tmp = 0;	
   
	if(UART1_SR_bit.FE)	//if(UART2_GetFlagStatus(UART2_FLAG_FE))
	{
	  receiv_add = 0;
	}
	else
	{
	  ++receiv_add;
	}
	
	if(receiv_add == adres_dev)
	{
		 data_receiv = (uint8_t)UART1_DR;
		 UART1_ITConfig(UART1_IT_RXNE, DISABLE);//вырубаем уарт
		 new_data = 1;
	}		
	tmp = UART1_DR; 
 }

/* Public functions ----------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    Melody_TypeDef *pMelody;
    char str[20];
	u8 cnt = 0;
	uint32_t Vbat = 0;

    /* Demo Initialization */
    Init_Application();

    /* Display the welcome text on first LCD line */
    GLCD_ClearScreen();
    GLCD_GoTo(0, 0);
    //GLCD_WriteString("STM8s207S6");
	
	GLCD_DrawBrd();
	/* Display the menu */
 	MenuDisplay();
	
    /* Play melody */

	//sprintf(str,"Melody: %d",6);
//	GLCD_GoTo(0, 1);
//	GLCD_WriteString(str);
    State_MelodyCallBack = 0;
	//pMelody = (Melody_TypeDef *)MELODIES+6;
	pMelody = (Melody_TypeDef *)MELODIES+5;
	//PlayMelody(pMelody->Notes);

    State_MelodyCallBack = 1;
	

	
    while(1)
	{
	  	if(enc_rot != R_STOP)
		{
			switch(enc_rot)
			{
				case R_RIGHT:
					MenuUpdate(JOY_RIGHT);
				break;
				case R_LEFT:
					MenuUpdate(JOY_LEFT);
				break;
				case R_SW:
					MenuUpdate(JOY_SEL);
				break;
				default: break;
			}
			enc_rot = R_STOP;
			while(ENC_SW);
		 // while(enc_rot != R_STOP || ENC_SW);
		}
		if(adc_cmp)
		{
			
			Vbat += ((uint32_t)ADC_V*1000)/(uint32_t)1023;

			++cnt;
			if(cnt>150)
			{
				u16 prc = 0;
				
				Vbat /= 151;  
				cnt = 0;
				
				prc = (Vbat - 380)*10/4;
				GLCD_bat_chg(prc);
				
				//sprintf(str,"Vbat = %u.%02uV",(u16)Vbat/100,(u16)Vbat%100);
				//GLCD_GoTo(0, 1);
				//GLCD_WriteString(str);
				
//				if(enc_rot == R_STOP)
//				{
//					GLCD_GoTo(4, 2);
//					GLCD_WriteString("Encoder STOP ");
//				}
//				if(enc_rot == R_LEFT)
//				{
//					GLCD_GoTo(4, 2);
//					GLCD_WriteString("Encoder LEFT ");
//				}
//				if(enc_rot == R_RIGHT)
//				{
//					GLCD_GoTo(4, 2);
//					GLCD_WriteString("Encoder RIGHT");
//				}
				WaitDelay(100);
			}

			adc_cmp = false;
			ADC2_StartConversion();
		}
	}
}



/** Initialize the application.
  * @param[in] None
  * @return None
  **/
void Init_Application(void)
{
	/* Clock initialization */
	/* Select fCPU = 16MHz */
	CLK_SYSCLKConfig( CLK_PRESCALER_HSIDIV1 );

	/* For test purpose output Fcpu on MCO pin */
	//CLK_CCOConfig(CLK_OUTPUT_CPU);

	/* I/O Port initialization */
	//power pin
	GPIO_Init( GPIOG, GPIO_PIN_0, GPIO_MODE_OUT_PP_HIGH_SLOW );
	/* Initialize I/Os in Output Mode for LEDs */
	GPIO_Init( GPIOA, GPIO_PIN_6, GPIO_MODE_OUT_OD_HIZ_SLOW );
	//encoder
	GPIO_Init( GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3), GPIO_MODE_IN_FL_NO_IT );
	
	/* PWM pin (TIM2_CH1) */
	GPIO_Init( GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST );
	
	/* Chg pin (TIM2_CH1) */
	GPIO_Init( GPIOE, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT );

	/* TIMER1 initialization for generic timebase */

	/* TimerTick = 50 ms
	 Warning: fcpu must be equal to 16MHz
	 fck_cnt = fck_psc/presc = fcpu/16 = 1 kHz --> 1 tick every 1 ms
	 ==> 1 ms / 1 ms = 1 ticks
	*/
	TIM1_TimeBaseInit((uint16_t)16000, TIM1_COUNTERMODE_UP,  1, 1);
	TIM1_UpdateRequestConfig( TIM1_UPDATESOURCE_GLOBAL );

	TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE );
	TIM1_Cmd( ENABLE );
	
	/* TIMER4 initialization for generic timebase */

	/* TimerTick = 2 ms
	 Warning: fcpu must be equal to 16MHz
	 fck_cnt = fck_psc/presc = fcpu/128 = 125 kHz --> 1 tick every 8 µs
	 ==> 2 ms / 8 µs = 250 ticks
	*/
	TIM4_TimeBaseInit( TIM4_PRESCALER_128, 250 );
	TIM4_UpdateRequestConfig( TIM4_UPDATESOURCE_GLOBAL );

	TIM4_ITConfig( TIM4_IT_UPDATE, ENABLE );
	TIM4_Cmd( ENABLE );

	/* LCD Initialization */
	GLCD_Initialize();

	/* TIMER2 initialization for buzzer (PWM mode)  */
	/* PWM for buzzer: TIM2_CH1 */
	TIM2_PrescalerConfig((uint16_t)2, TIM2_PSCRELOADMODE_IMMEDIATE);
	TIM2_OC1Init( TIM2_OCMODE_PWM1,  TIM2_OUTPUTSTATE_ENABLE, 0xFFFF,  TIM2_OCPOLARITY_LOW);

	//TIM1_CtrlPWMOutputs(ENABLE);

	TIM2_Cmd(ENABLE);

	ADC2_DeInit();

	ADC2_Init(ADC2_CONVERSIONMODE_SINGLE, ADC2_CHANNEL_9, ADC2_PRESSEL_FCPU_D18,
			ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SCHMITTTRIG_CHANNEL9, DISABLE);

	ADC2_ITConfig(ENABLE);
	ADC2_Cmd(ENABLE); 
	
	ADC2_StartConversion();
	
	  /* Menu initialization */
 	MenuInit();
	
	/* Enable interrupts */
	enableInterrupts();

}

/** Wait a delay
  * @param[in] Delay based on timer Tick
  * @return None
  **/
void WaitDelay(uint16_t Delay)
{

    uint16_t DelayPeriodStart;
    __IO uint16_t TimeStamp;
    uint16_t PeriodStamp;

    sim();
    DelayPeriodStart = GTimeStamp;
    rim();

    while (1)
    {
        sim();
        TimeStamp = GTimeStamp;
        rim();
        PeriodStamp = TimeStamp - DelayPeriodStart;
        if (PeriodStamp >= Delay)
        {
            return;
        }
    }
}

#ifdef ENABLE_MELODY_CALLBACK
void MelodyCallBack(void)
{
 // static uint8_t logo_pos = 0x90;
 // static int8_t logo_pos_index = 1;

//  if (State_MelodyCallBack == 0)
//  {
//    LCD_SetCursorPos(LCD_LINE2, 0);
//    LCD_Print("               ");
//
//    LCD_DisplayLogo(logo_pos);
//    logo_pos += logo_pos_index;
//    if (logo_pos > 0x95)
//    {
//      logo_pos -= 2;
//      logo_pos_index = -1;
//    }
//    if (logo_pos < 0x90)
//    {
//      logo_pos += 2;
//      logo_pos_index = 1;
//    }
//  }
}
#endif



