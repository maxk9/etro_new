
//-------------------------------------------------< Библиотека для работы с шиной I2C >----------------------------------------------------



#ifndef I2C_H_
#define I2C_H_

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_lib.h"
#include "MDR32F9Qx_port.h"

#define BYTE unsigned char

#define I2C_PORT	MDR_PORTC	// Порт на котором реализуется I2C
#define I2C_SCL		PORT_Pin_0	// Пин SCL
#define I2C_SDA		PORT_Pin_1	// Пин SDA

#define ACK 0		// Ответ удачный
#define NACK 1		// Ответ не удачный

#define I2C_DELAY()	{ __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop();}	// Общая пауза на шине

#define ONE_SCL()	{ PORT_SetBits(I2C_PORT,I2C_SCL); } // Установка единицы на линии SCL
#define NULL_SCL()	{ PORT_ResetBits(I2C_PORT,I2C_SCL); } // Установка нуля на линии SCL
#define ONE_SDA()	{ PORT_SetBits(I2C_PORT,I2C_SDA); } // Установка единицы на линии SDA
#define NULL_SDA()	{ PORT_ResetBits(I2C_PORT,I2C_SDA); } // Установка нуля на линии SDA
	

// Прототипы функций

BYTE i2c_stop(void);			// Функция стопа/ Возврашает 0 если все нармально. Возвращает 1 если ошибка на SDA. Возвращает 2 если ошибка на SCL. Если 3, ошибка на обеих линиях.
void i2c_start(void);			// Функция старта
void i2c_restart(void);			// Функция рестарта
void i2c_init(void);			// Фуекция инициализации шины
BYTE i2c_send_byte(BYTE data);	// Функция передачи байта. Возвращает ACK данные переданы, NACK данные не переданы
BYTE i2c_read_byte(BYTE ask);	// Функция читает байт. Если аргумент передается ACK, значит будет читатся еще один байт. Если аргумент NACK, значит принят последний байт

#endif /* I2C_H_ */







