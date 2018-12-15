#include "ds18b20.h"
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/4/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void DS18B20_IO_IN(void)
{
		GPIO_InitTypeDef GPIO_Initure;	
	
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PG5
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //��ʼ��
}

void DS18B20_IO_OUT(void)
{
		GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PG5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //��ʼ��
}

//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   //����Ϊ���
	//DS18B20_DQ_OUT=0;  	//����DQ
	DS18B20_DQ_OUT_L;
	HAL_Delay_us(750);      //����750us
	//DS18B20_DQ_OUT=1;  	//DQ=1 
	DS18B20_DQ_OUT_H;
	HAL_Delay_us(15);       //15US
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    //����Ϊ����
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		HAL_Delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		HAL_Delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

//��DS18B20��ȡһ��λ
//����ֵ��1/0
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   //����Ϊ���
	//DS18B20_DQ_OUT=0; 
	DS18B20_DQ_OUT_L;
	HAL_Delay_us(2);
	//DS18B20_DQ_OUT=1;
	DS18B20_DQ_OUT_H;	
	DS18B20_IO_IN();    //����Ϊ����
	HAL_Delay_us(12);
	if(DS18B20_DQ_IN)data=1;
	else data=0;	 
	HAL_Delay_us(50);           
	return data;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
uint8_t DS18B20_Read_Byte(void)   
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
	return dat;
}

//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();     //����Ϊ���
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // д1
        {
            //DS18B20_DQ_OUT=0;
						DS18B20_DQ_OUT_L;
            HAL_Delay_us(2);                            
            //DS18B20_DQ_OUT=1;
						DS18B20_DQ_OUT_H;
            HAL_Delay_us(60);             
        }
        else            //д0
        {
            //DS18B20_DQ_OUT=0;
						DS18B20_DQ_OUT_L;
            HAL_Delay_us(60);             
            //DS18B20_DQ_OUT=1;
						DS18B20_DQ_OUT_H;
            HAL_Delay_us(2);                          
        }
    }
}
 
//��ʼ�¶�ת��
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
uint8_t DS18B20_Init(void)
{
	  GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();			
	
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PG5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //��ʼ��
 
	DS18B20_Rst();
	return DS18B20_Check();
}

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp(void)
{
	uint8_t temp;
	uint8_t TL,TH;
	short tem;

	rt_enter_critical();

	DS18B20_Start ();           //��ʼת��
	DS18B20_Rst();
	DS18B20_Check();	 
	DS18B20_Write_Byte(0xcc);   // skip rom
	DS18B20_Write_Byte(0xbe);   // convert	    
	TL=DS18B20_Read_Byte();     // LSB   
	TH=DS18B20_Read_Byte();     // MSB   
	
	rt_exit_critical();
	
	if(TH>7)
	{
			TH=~TH;
			TL=~TL; 
			temp=0;//�¶�Ϊ��  
	}else temp=1;//�¶�Ϊ��	  	  
	tem=TH; //��ø߰�λ
	tem<<=8;    
	tem+=TL;//��õװ�λ
	tem=(double)tem*0.625;//ת��   
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
}
