/**
  ******************************************************************************
  * @file    Demo_Init_MDR32F9Q1.c
  * @author  Phyton Application Team
  * @version V3.0.0
  * @date    09.09.2011
  * @brief   Initializes the demonstration application for MDR32F9Q1_Rev0 and
  *          MDR32F9Q1_Rev1 evaluation boards
  ******************************************************************************
  * <br><br>
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, PHYTON SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 Phyton</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "demo_init.h"
#include "leds.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_ebc.h"
#include "lcd.h"

#if defined (USE_MDR32F9Q1_Rev0) || defined (USE_MDR32F9Q1_Rev1)

/** @addtogroup __MDR32F9Qx_Eval_Demo MDR32F9Qx Demonstration Example
  * @{
  */

/** @addtogroup Demo_Setup Demo Setup
  * @{
  */

/** @defgroup __MDR32F9Q1_Demo_Setup MDR32F9Q1 evaluation boards specific
  * @{
  */

/** @defgroup __MDR32F9Q1_Demo_Setup_Private_Constants MDR32F9Q1 Demo Setup Private Constants
  * @{
  */

#define ALL_PORTS_CLK   (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                         RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                         RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

/** @} */ /* End of group __MDR32F9Q1_Demo_Setup_Private_Constants */

/** @defgroup __MDR32F9Q1_Demo_Setup_Private_Variables MDR32F9Q1 Demo Setup Private Variables
  * @{
  */

/** @} */ /* End of group __MDR32F9Q1_Demo_Setup_Private_Variables */

/** @defgroup __MDR32F9Q1_Demo_Setup_Private_Functions MDR32F9Q1 Demo Setup Private Functions
  * @{
  */



/*******************************************************************************
* Function Name  : UART_init
* Description    : Configures the UART1.
*                : Configures the HCLK division factor and RTCHSE clock for UART1.
* Input          :
*                :
* Output         : None
* Return         : None
*******************************************************************************/
void UART1_init(void)
{
  /* Enable the RTCHSE clock on UART1 */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

  /* Set the UART1 HCLK division factor */
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);

  UART_DeInit(MDR_UART1);

    /* Enable interrupt on UART1 */
   // NVIC_EnableIRQ(UART1_IRQn);

    /* Disable interrupt on UART1 */
    NVIC_DisableIRQ(UART1_IRQn);

  /* UART1 configuration */
  UARTInitStructure.UART_BaudRate			= 9600;
  UARTInitStructure.UART_WordLength			= UART_WordLength8b;
  UARTInitStructure.UART_StopBits			= UART_StopBits2;
  UARTInitStructure.UART_Parity				= UART_Parity_No;
  UARTInitStructure.UART_FIFOMode			= UART_FIFO_OFF;
  UARTInitStructure.UART_HardwareFlowControl= UART_HardwareFlowControl_RXE| UART_HardwareFlowControl_TXE;

  UART_Init (MDR_UART1,&UARTInitStructure);
  
	NVIC_EnableIRQ(UART1_IRQn);
	UART_ITConfig(MDR_UART1,UART_IT_RX, ENABLE);
	
  UART_Cmd(MDR_UART1,ENABLE);
}


/*******************************************************************************
* Function Name  : ExtBus_Setup
* Description    : Configures the External Bus.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ExtBus_Setup(void)
{
	EBC_InitTypeDef EBC_InitStruct;

	/* Enables the RTCHSE clock on the EBC */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_EBC, ENABLE);

  EBC_StructInit(&EBC_InitStruct);

  /* Configure External Bus */
  EBC_InitStruct.EBC_WaitState = EBC_WAIT_STATE_12HCLK;
  EBC_InitStruct.EBC_Mode          = EBC_MODE_ROM;

  EBC_Init(&EBC_InitStruct);
}

/*******************************************************************************
* Function Name  : ClockConfigure
* Description    : Configures the CPU_PLL and RTCHSE clock.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClockConfigure(void)
{
	/* Enable HSE clock oscillator */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	while(!RST_CLK_HSEstatus());
	
	RST_CLK_HSIcmd(DISABLE);
	
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	MDR_RST_CLK->CPU_CLOCK |=2;//CPU_C1 set HSE
  /* Enables the RTCHSE clock on all ports */
  RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);
}

/*******************************************************************************
* Function Name  : Demo_Init
* Description    : Initializes the demonstration application
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Demo_Init(void)
{
  ClockConfigure();

/************************ LCD Initialization *************************/

  /* Configure PORTA pins for data transfer to/from LCD */
  PortInitStructure.PORT_Pin = LCD_DATA_BUS_8;
  PortInitStructure.PORT_FUNC = PORT_FUNC_MAIN;
  PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
  PortInitStructure.PORT_OE = PORT_OE_IN;

  PORT_Init(MDR_PORTA, &PortInitStructure);

  /* Configure PORTE pin4 and pin5 for LCD crystals control */
  PortInitStructure.PORT_Pin = PORT_Pin_4 | PORT_Pin_5;

  PORT_Init(MDR_PORTE, &PortInitStructure);

  /* Configure PORTE pin11 for data/command mode switching */
  PortInitStructure.PORT_Pin = PORT_Pin_11;

  PORT_Init(MDR_PORTE, &PortInitStructure);

  /* Configure PORTC pin2 for read/write control */
  PortInitStructure.PORT_Pin = PORT_Pin_2;

  PORT_Init(MDR_PORTC, &PortInitStructure);

  /* Configure PORTC pin7 for CLOCK signal control */
  PortInitStructure.PORT_Pin = PORT_Pin_7;
  PortInitStructure.PORT_SPEED = PORT_SPEED_FAST;

  PORT_Init(MDR_PORTC, &PortInitStructure);

  /* Configure PORTC pin9 for RESET signal control */
  PortInitStructure.PORT_Pin = PORT_Pin_9;
  PortInitStructure.PORT_OE = PORT_OE_OUT;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTC, &PortInitStructure);

  ExtBus_Setup();

  LCD_INIT();
/************************ UART1 Initialization *************************/
	PORT_StructInit(&PortInitStructure);//reset struct

  PortInitStructure.PORT_Pin = PORT_Pin_5|PORT_Pin_6;//RX TX
  PortInitStructure.PORT_FUNC = PORT_FUNC_ALTER;
  PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
  PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
  PORT_Init(MDR_PORTB, &PortInitStructure);
  
  PortInitStructure.PORT_Pin = PORT_Pin_7;//EN RS485
  PortInitStructure.PORT_OE =PORT_OE_OUT;
  PortInitStructure.PORT_PD = PORT_PD_OPEN;
  PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
  PORT_Init(MDR_PORTB, &PortInitStructure);
  
  UART1_init();
// 	PORT_StructInit(&PortInitStructure);//reset struct
// 	
//   PortInitStructure.PORT_Pin = PORT_Pin_5|PORT_Pin_6;//RX TX
//   PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
//   PortInitStructure.PORT_OE =PORT_OE_OUT;
//   PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
//   PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
//   PORT_Init(MDR_PORTB, &PortInitStructure);
//   
//   PortInitStructure.PORT_Pin = PORT_Pin_7;//EN RS485
//   PortInitStructure.PORT_OE =PORT_OE_OUT;
//   PortInitStructure.PORT_PD = PORT_PD_OPEN;
//   PortInitStructure.PORT_FUNC = PORT_FUNC_PORT;
//   PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
//   PORT_Init(MDR_PORTB, &PortInitStructure);

/************************ Joystick Initialization *************************/

  /* Configure PORTC pins 10..14 for input to handle joystick events */
  PortInitStructure.PORT_Pin   = (PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 |
                                  PORT_Pin_13 | PORT_Pin_14);
  PortInitStructure.PORT_OE    = PORT_OE_IN;
  PortInitStructure.PORT_PD = PORT_PD_DRIVER;
  PortInitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PortInitStructure.PORT_SPEED = PORT_OUTPUT_OFF;

  PORT_Init(MDR_PORTC, &PortInitStructure);

/************************ LEDs Initialization *************************/

  /* Configure PORTD pins 10..14 for output to switch LEDs on/off */
  PortInitStructure.PORT_Pin   = LEDs_PINs;
  PortInitStructure.PORT_OE    = PORT_OE_OUT;
  PortInitStructure.PORT_PD = PORT_PD_DRIVER;
  PortInitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTD, &PortInitStructure);

  /* All LEDs switch off */
  PORT_ResetBits(MDR_PORTD, LEDs_PINs);
}

/** @} */ /* End of group __MDR32F9Q1_Demo_Setup_Private_Functions */

/** @} */ /* End of group __MDR32F9Q1_Demo_Setup */

/** @} */ /* End of group Demo_Setup */

/** @} */ /* End of group __MDR32F9Qx_Eval_Demo */

#endif /* USE_MDR32F9Q1_Rev0 || USE_MDR32F9Q1_Rev1 */

/******************* (C) COPYRIGHT 2011 Phyton *******************
*
* END OF FILE Demo_Init_MDR32F9Q1.c */

