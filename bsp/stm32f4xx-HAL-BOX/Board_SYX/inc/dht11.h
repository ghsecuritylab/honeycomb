#ifndef __DHT11_H
#define __DHT11_H


#include "stm32f4xx_hal.h"
////////////////////////////////////////////////////////////////////////////////// 	


void DHT11_IO_OUT(void);
void DHT11_IO_IN(void);
   	
uint8_t DHT11_Init(void);//��ʼ��DHT11
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);//��ȡ��ʪ��
uint8_t DHT11_Read_Byte(void);//����һ���ֽ�
uint8_t DHT11_Read_Bit(void);//����һ��λ
uint8_t DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11  
#endif
