/**
  ********************************************************************************
  * @file     menu.h
  * @author   MCD Application Team
  * @version  V1.1.0
  * @date     11-March-2011
  * @brief    TThis file contains functions prototypes for the menu.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

	
typedef enum
{
  JOY_NONE = 0,
  JOY_SEL = 1,
  JOY_DOWN = 2,
  JOY_LEFT = 3,
  JOY_RIGHT = 4,
  JOY_UP = 5
} JOYState_TypeDef;


/** State ID
  * @note Table element must not be equal to 0 as this element is
  * used to detect the end of table.
 */
typedef enum {
    /* Level 1 */
    DMX512_RX = 1,
	DMX512_RXTX,
	TERMINAL,
	PLAY_M,
	/* Level 2 */
	DMX512_RX1,
	DMX512_RXTX1,
	TERMINAL1, 
	PLAY_M1,
	EXIT
} STATE_ID_T;

/** Menu NextState */
typedef struct
{
    STATE_ID_T State;
    JOYState_TypeDef Key;
    STATE_ID_T NextState;
}
MENU_NEXTSTATE_T;

/** Menu state */
typedef struct
{
    STATE_ID_T State;
    char* pText;
    void (*pFunc)(void);
}
MENU_STATE_T;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern STATE_ID_T State;
/* Exported functions --------------------------------------------------------*/
void MenuInit(void);
void MenuUpdate(JOYState_TypeDef ujoystick);
void MenuDisplay(void);

#endif /* __MENU_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
