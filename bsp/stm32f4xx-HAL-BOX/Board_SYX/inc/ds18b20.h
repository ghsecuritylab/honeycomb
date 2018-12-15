#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f4xx_hal.h"

#define DS18B20_DQ_OUT_H		HAL_GPIO_WritePin(GPIOG,GPIO_PIN_5,GPIO_PIN_SET) 
#define DS18B20_DQ_OUT_L  	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_5,GPIO_PIN_RESET)

#define	DS18B20_DQ_IN  HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_5)


void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);
	
uint8_t DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(uint8_t dat);//д��һ���ֽ�
uint8_t DS18B20_Read_Byte(void);		//����һ���ֽ�
uint8_t DS18B20_Read_Bit(void);		//����һ��λ
uint8_t DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20 
#endif
