#include "ds18b20.h"
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/4/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void DS18B20_IO_IN(void)
{
		GPIO_InitTypeDef GPIO_Initure;	
	
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PG5
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //初始化
}

void DS18B20_IO_OUT(void)
{
		GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PG5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //初始化
}

//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   //设置为输出
	//DS18B20_DQ_OUT=0;  	//拉低DQ
	DS18B20_DQ_OUT_L;
	HAL_Delay_us(750);      //拉低750us
	//DS18B20_DQ_OUT=1;  	//DQ=1 
	DS18B20_DQ_OUT_H;
	HAL_Delay_us(15);       //15US
}

//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    //设置为输入
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

//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   //设置为输出
	//DS18B20_DQ_OUT=0; 
	DS18B20_DQ_OUT_L;
	HAL_Delay_us(2);
	//DS18B20_DQ_OUT=1;
	DS18B20_DQ_OUT_H;	
	DS18B20_IO_IN();    //设置为输入
	HAL_Delay_us(12);
	if(DS18B20_DQ_IN)data=1;
	else data=0;	 
	HAL_Delay_us(50);           
	return data;
}

//从DS18B20读取一个字节
//返回值：读到的数据
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

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();     //设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // 写1
        {
            //DS18B20_DQ_OUT=0;
						DS18B20_DQ_OUT_L;
            HAL_Delay_us(2);                            
            //DS18B20_DQ_OUT=1;
						DS18B20_DQ_OUT_H;
            HAL_Delay_us(60);             
        }
        else            //写0
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
 
//开始温度转换
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DS18B20_Init(void)
{
	  GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();			
	
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PG5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //初始化
 
	DS18B20_Rst();
	return DS18B20_Check();
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
	uint8_t temp;
	uint8_t TL,TH;
	short tem;

	rt_enter_critical();

	DS18B20_Start ();           //开始转换
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
			temp=0;//温度为负  
	}else temp=1;//温度为正	  	  
	tem=TH; //获得高八位
	tem<<=8;    
	tem+=TL;//获得底八位
	tem=(double)tem*0.625;//转换   
	if(temp)return tem; //返回温度值
	else return -tem;    
}
