/*
Программа для электронных часов
начата: неизв
изменена: Копусов МА 15:04:11
Fosc=14.119MHz
Fosc/4 = 3.53MHz
*/

#include <htc.h>
//#include "usart.h"

//__CONFIG(DEBUGDIS & WDTDIS & LVPDIS & HS & PWRTEN & BORDIS & UNPROTECT);
__CONFIG(DEBUGDIS & WDTE_OFF & WRTDIS & LVPDIS & HS & PWRTEN & FBOREN & UNPROTECT);


#define UCHAR unsigned char
#define UINT unsigned int
#define DATA 	RB7
#define CLOCK 	RB6
#define R_DATA 	RC7
#define R_CLOCK	RC6

#define TEST_PIN RC4

void main()
{
	while(1)
	{
	}
}