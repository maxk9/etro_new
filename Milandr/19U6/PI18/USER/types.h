/*=============================================================================
 *
 *  (C) 2010, Phyton
 *
 *  Демонстрационный проект для 1986BE91 testboard
 *
 *  Данное ПО предоставляется "КАК ЕСТЬ", т.е. исключительно как пример, призванный облегчить 
 *  пользователям разработку их приложений для процессоров Milandr 1986BE91T1. Компания Phyton 
 *  не несет никакой ответственности за возможные последствия использования данного, или 
 *  разработанного пользователем на его основе, ПО.
 *
 *-----------------------------------------------------------------------------
 *
 *  Файл type.h: алиасы базовых типов и макросы
 *
 *=============================================================================*/
//#include "1986BE91T1.h"
#ifndef __TYPES_H
#define __TYPES_H

typedef signed long s32;
typedef signed short s16;
typedef signed char s8;

typedef signed long const sc32;
typedef signed short const sc16;
typedef signed char const sc8;

typedef volatile signed long vs32;
typedef volatile signed short vs16;
typedef volatile signed char vs8;

typedef volatile signed long const vsc32;
typedef volatile signed short const vsc16;
typedef volatile signed char const vsc8;

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef unsigned long const uc32;
typedef unsigned short const uc16;
typedef unsigned char const uc8;

typedef volatile unsigned long vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;

typedef volatile unsigned long const vuc32;
typedef volatile unsigned short const vuc16;
typedef volatile unsigned char const vuc8;

typedef unsigned char const    ucint8_t;
typedef volatile unsigned char vuint8_t;
typedef volatile unsigned long vuint32_t;

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;

#endif /* __TYPES_H */

/*=============================================================================
 * Конец файла types.h
 *=============================================================================*/
