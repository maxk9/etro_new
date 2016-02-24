/*
*!!!!!!־בשטי פאיכ הכÿ ֿÊִ׃ ט ֿÊְֱ!!!!!!
*
*/

#ifndef __LINK_PULT_H
#define __LINK_PULT_H

#include "stdint.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include <stdbool.h>


#define RS485_TX_ON		PORT_SetBits(MDR_PORTA, PORT_Pin_0)
#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTA, PORT_Pin_0)


typedef struct
{
	bool calibr_on;
	uint8_t reg0;
	uint8_t reg1;
volatile uint16_t Error;	//error
volatile uint8_t RabReg0;
volatile uint16_t UGen;
volatile uint16_t IGen;
volatile uint8_t DT;
volatile uint8_t DM;
volatile uint8_t TM;
volatile uint16_t NDiz;
volatile uint8_t TBapEx;
volatile uint8_t TBapIn;
volatile uint8_t Led1;
volatile uint8_t Led2;
volatile uint8_t RegWrk;
volatile uint16_t RegimTime;
	bool protOFF;
volatile uint8_t Load;
}RegS;

extern volatile uint8_t time_out_byte;

void UART1_send_byte(uint8_t byte);
uint8_t ctrl_sum(uint8_t *data8,uint8_t cnt);
uint8_t UART1_receiv_data(void);



#endif 


