/**
  ******************************************************************************
  * @file     menu.c
  * @author   MCD Application Team
  * @version  V1.1.0
  * @date     11-March-2011
  * @brief    This file contains all the functions for the menu demo.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYJOY_RIGHT 2009 STMicroelectronics</center></h2>
  * @image html logo.bmp
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "menu.h"
#include "buzzer.h"
#include "lcd.h"	


extern void WaitDelay(uint16_t Delay);

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
//MAIN Menu
#define MENU_TOP_TEXT       ("   ** MENU **   ")
#define DMXRX_TEXT        	("    DMX512 RX   ")
#define DMX512_RT_TEXT		("  DMX512 RX TX  ")
#define TERMINAL_TEXT		(" Terminal RS485 ")
#define PLAY_M_TEXT			("   Play music   ")
//Target Menu
#define DMXRX_TEXT1        	("   Read DMX512  ")
#define DMX512_RT_TEXT1		("   Send DMX512  ")
#define TERMINAL_TEXT1		("  Config RS485  ")
#define PLAY_M_TEXT1		("  Select music  ")
#define EXIT_TEXT			("      exit      ")

CONST MENU_NEXTSTATE_T MENU_NEXTSTATE[] =
{
	/* STATE                     INPUT           NEXT STATE */
	/* MENU Level 1 */         /* Key */
	{ DMX512_RX,				JOY_SEL,		DMX512_RX1 	},
	{ DMX512_RXTX,				JOY_SEL,		DMX512_RXTX1},
	{ TERMINAL,					JOY_SEL,		TERMINAL1	},
	{ PLAY_M,					JOY_SEL,		PLAY_M1		},
	/* DMX512_RX menu Level 2 */
	{ DMX512_RX1,				JOY_SEL,		DMX512_RX	},
	/* DMX512_RXTX menu Level 2 */
	{ DMX512_RXTX1,				JOY_SEL,		DMX512_RXTX	},
	/* TERMINAL menu Level 2 */
	{ TERMINAL1,				JOY_SEL,		TERMINAL	},
	/* PLAY_M menu Level 2 */
	{ PLAY_M1,					JOY_SEL,		PLAY_M		},
	{ EXIT,						JOY_SEL,		TERMINAL	},
	/* End of table */
	{ (STATE_ID_T)0,	(JOYState_TypeDef)0,	(STATE_ID_T)0, }
};

CONST MENU_STATE_T MAIN_MENU_STATE[] =
{
	/* STATE                     STATE TEXT            FUNCTION */
	/*MAIN MENU Level 1 */
	{ DMX512_RX,				DMXRX_TEXT,				0 },
	{ DMX512_RXTX,				DMX512_RT_TEXT,			0 },
	{ TERMINAL,					TERMINAL_TEXT,			0 },
	{ PLAY_M,					PLAY_M_TEXT,			0 },
	/* End of table */
	{ (STATE_ID_T)0,				0,					0 }
};

CONST MENU_STATE_T TARG_MENU_STATE[] =
{
	/* STATE                     STATE TEXT            FUNCTION */
	/*TARGET MENU Level 2 */
	{ PLAY_M1,					PLAY_M_TEXT1,			0 },
	{ DMX512_RX1,				DMXRX_TEXT1,			0 },
	{ DMX512_RXTX1,				DMX512_RT_TEXT1,		0 },
	{ TERMINAL1,				TERMINAL_TEXT1,			0 },
	{ PLAY_M1,					PLAY_M_TEXT1,			0 },
	{ EXIT,						EXIT_TEXT,				0 },
	/* End of table */
	{ (STATE_ID_T)0,				0,					0 }
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
STATE_ID_T Gl_State;
bool TargetMenu = false;
void (*pStateFunc)(void);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the navigation menu.
  * @param  None
  * @retval None
  */
void MenuInit(void)
{
  Gl_State = TERMINAL;
  pStateFunc = 0;
}

/**
  * @brief  Updates the menu states and displays the new menu on the LCD.
  * @param  ujoystick: new joystick state
  * @retval None
  */
void MenuUpdate(JOYState_TypeDef ujoystick)
{
  static uint8_t i = TERMINAL;
  STATE_ID_T NextState = (STATE_ID_T)0;
  
  if (!TargetMenu)
	{
	  switch (ujoystick)
		{
			case JOY_RIGHT:
				
			  if(i < 4)
				++i;
				else
				  i = 1;
				//Gl_State = (STATE_ID_T)i; 
				break;
				
			case JOY_LEFT:
				
			  if(i != 1)
				--i;
				else
				  i = 4;
				
				break;
		  case JOY_SEL:
			/* If a sub-menu is entered, the TargetState is the current state */
			TargetMenu = true;
			//Gl_State = MENU_NEXTSTATE[i].NextState;
				break;
		  default:
				break;
		}
	  
	 Gl_State = (STATE_ID_T)i; 
	}
  else
	if(ujoystick == JOY_SEL)
	{
	  TargetMenu = false;
	  Gl_State = (STATE_ID_T)i;
	}
  
	
  
    MenuDisplay(); /* Display the new menu on LCD */

//    /* Execute the function of state if there is one MAIN MENU*/
//    for (i = 0; MAIN_MENU_STATE[i].MainState; i++)
//    {
//      if (MAIN_MENU_STATE[i].MainState == MainState)
//      {
//        pStateFunc = MAIN_MENU_STATE[i].pFunc;
//        if (pStateFunc) /* Check if there is a state function to be executed */
//        {
//          pStateFunc(); /* Execute the state function */
//          pStateFunc = 0;
//        }
//      }
//    }
//	 /* Execute the function of state if there is one Targ MENU*/
//    for (i = 0; TARG_MENU_STATE[i].MainState; i++)
//    {
//      if (TARG_MENU_STATE[i].MainState == MainState)
//      {
//        pStateFunc = TARG_MENU_STATE[i].pFunc;
//        if (pStateFunc) /* Check if there is a state function to be executed */
//        {
//          pStateFunc(); /* Execute the state function */
//          pStateFunc = 0;
//        }
//      }
//    }
//  }
}

/**
  * @brief  Displays the current menu level on the LCD
  * @param  None
  * @retval None
  */
void MenuDisplay(void)
{
  uint8_t i = 0;

  /* Clear LCD */
  
	WaitDelay(80);
	
  //main menu
  	if (!TargetMenu)
	{
	for (i = 0; MAIN_MENU_STATE[i].State; i++)
		{
			if(MAIN_MENU_STATE[i].pText && i < 6)
			{
			  GLCD_GoTo(5, 1+i);
			  if(MAIN_MENU_STATE[i].State == Gl_State)
				GLCD_WriteStringBlack((char *)MAIN_MENU_STATE[i].pText);
			  else
			  GLCD_WriteString((char *)MAIN_MENU_STATE[i].pText);
			}
		}
	}
  	else //target menu
	  {
		GLCD_GoTo(5, 1);
		GLCD_WriteString((char *)TARG_MENU_STATE[Gl_State].pText);
		
		for (i = 2; i < 8; i++)
		{
			GLCD_GoTo(5, i);
			GLCD_WriteString("                ");//clear menu
		}
	  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
//  
//  /* Display menu line 1 */
//  if (TargetState)
//  {
//    for (i = 0; MENU_STATE[i].MainState; i++)
//    {
//      if (MENU_STATE[i].MainState == TargetState)
//      {
//        if(MENU_STATE[i].pText)
//        {
//          GLCD_GoTo(5, 1);
//		  
//          GLCD_WriteString((char *)MENU_STATE[i].pText);
//        }
//        break;
//      }
//    }
//  }
//  else
//  {
//    /* Menu top level reached */
//    GLCD_GoTo(5, 1);
//	GLCD_WriteStringBlack(MENU_TOP_TEXT);
//    //GLCD_WriteString(MENU_TOP_TEXT);
//  }
//  
//  /* Display menu line 2 */
//  for (i = 0; MENU_STATE[i].MainState; i++)
//  {
//    if (MENU_STATE[i].MainState == MainState)
//    {
//      if (MENU_STATE[i].pText)
//      {
//        GLCD_GoTo(5, 2);
//        GLCD_WriteString((char *)MENU_STATE[i].pText);
//      }
//      break;
//    }
//  }
}

/******************* (C) COPYJOY_RIGHT 2009 STMicroelectronics *****END OF FILE****/
