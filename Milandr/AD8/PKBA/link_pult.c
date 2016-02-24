/*
*èíòåðôåéñ ñâÿçè ñ ïóëüòîì ÀÄ8
*
*!!!!!!Îáùèé ôàéë äëÿ ÏÊÄÓ è ÏÊÁÀ!!!!!!
*
*
*/

#include "MDR32Fx.h"
#include "link_pult.h"

// typedef struct
// {
// 	uint8_t reg0;
// 	uint8_t reg1;
// 	uint8_t Error;	//error
// 	uint8_t RabReg0;
// 	uint16_t UGen;
// 	uint16_t IGen;
// 	uint8_t DT;
// 	uint8_t DM;
// 	uint8_t TM;
// 	uint8_t NDiz;
// 	uint8_t TBap;
// 	uint8_t Led1;
// 	uint8_t Led2;
// }RegS;

extern volatile uint8_t time_out_byte=0;

void UART1_send_byte(uint8_t byte)
{
	UART_SendData(MDR_UART1,byte);
	time_out_byte = 0;
	/*while(!UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	{
		if( time_out_byte>3)
			break;
	}
	UART_ReceiveData (MDR_UART1);
	*/
	
	while(UART_GetFlagStatus (MDR_UART1,UART_FLAG_BUSY) || !UART_GetFlagStatus (MDR_UART1,UART_FLAG_RXFF))
	{
// 		if(time_out_byte>3)
// 			break;
	}
	UART_ReceiveData (MDR_UART1);
	UART_ClearITPendingBit(MDR_UART1,UART_IT_RX);
}

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

