#include "ST7735R.h"
#include "bmp.h"
#include "lcdfont.h"
/******************************************************************************
//������������STM32F407ZGT6
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PB10
//              SDA   ��PC3
//              RES   ��PB15  PG2
//              DC    ��B14		PG3           
//							BL		��PB13  PG4
*******************************************************************************/



uint8_t ref=0;//ˢ����ʾ
uint16_t BACK_COLOR, POINT_COLOR;   //����ɫ������ɫ

//void lcd_spi_gpio_init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStruct;

//  /* GPIO Ports Clock Enable */
// 
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	__HAL_RCC_GPIOC_CLK_ENABLE();
//	/*Configure GPIO pin : PtPin */
//  GPIO_InitStruct.Pin = GPIO_PIN_3;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//	
//	GPIO_InitStruct.Pin = GPIO_PIN_10;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
//}

////��SPI���ߴ���һ��8λ����
//void  SPI_WriteData(uint8_t Data)
//{
//	unsigned char i=0;
//	for(i=0;i<8;i++)
//	{
//		LCD_SCL_CLR;
//		if(Data&0x80)	
//			LCD_SDA_SET; //�������
//		else LCD_SDA_CLR;
//      
//		LCD_SCL_SET;
//		Data<<=1; 
//	}
//}

//��Һ����дһ��8λָ��
__inline void Lcd_WriteIndex(uint8_t Index)
{
   //SPI д����ʱ��ʼ
   LCD_DC_CLR;
	 HAL_SPI_Transmit(&hspi2,&Index,1,50);
}

//��Һ����дһ��8λ����
__inline void Lcd_WriteData(uint8_t Data)
{
   LCD_DC_SET;
   HAL_SPI_Transmit(&hspi2,&Data,1,50);
}
//��Һ����дһ��16λ����
__inline void LCD_WriteData_16Bit(uint16_t Data)
{
	uint8_t data1,data2;
	data1 = Data>>8;
	data2 = Data&0xff;
  LCD_DC_SET;

	 HAL_SPI_Transmit(&hspi2,&data1,1,50); 	//д���8λ����
	 HAL_SPI_Transmit(&hspi2,&data2,1,50);	//д���8λ����
 
}

void LCD_WriteBuf_16(uint16_t *buf, uint32_t length)
{
	LCD_DC_SET;

	HAL_SPI_Transmit(&hspi2,(uint8_t*)&buf,length,50); 	//д���8λ����
}




void LCD_WriteIndexAndData(uint16_t Index,uint16_t Data)
{
		Lcd_WriteIndex(Index);
		LCD_WriteData_16Bit(Data);
}


void Lcd_Reset(void)
{
	LCD_RST_CLR;
	HAL_Delay(20);
	LCD_RST_SET;
	HAL_Delay(20);
}

//LCD Init For 1.44Inch LCD Panel with ST7735R.
void Lcd_Init(void)
{	
	
//	Lcd_Reset(); //Reset before LCD Init.
//	LCD_BL_SET;
	LCD_RST_CLR;
	HAL_Delay(20);
	LCD_RST_SET;
	HAL_Delay(20);
	//LCD_BL_SET;
	
//************* Start Initial Sequence **********// 
Lcd_WriteIndex(0x36); 
Lcd_WriteData(0x60);

Lcd_WriteIndex(0x3A); 
Lcd_WriteData(0x05);

Lcd_WriteIndex(0xB2);
Lcd_WriteData(0x0C);
Lcd_WriteData(0x0C);
Lcd_WriteData(0x00);
Lcd_WriteData(0x33);
Lcd_WriteData(0x33);

Lcd_WriteIndex(0xB7); 
Lcd_WriteData(0x35);  

Lcd_WriteIndex(0xBB);
Lcd_WriteData(0x19);

Lcd_WriteIndex(0xC0);
Lcd_WriteData(0x2C);

Lcd_WriteIndex(0xC2);
Lcd_WriteData(0x01);

Lcd_WriteIndex(0xC3);
Lcd_WriteData(0x12);   

Lcd_WriteIndex(0xC4);
Lcd_WriteData(0x20);  

Lcd_WriteIndex(0xC6); 
Lcd_WriteData(0x0F);    

Lcd_WriteIndex(0xD0); 
Lcd_WriteData(0xA4);
Lcd_WriteData(0xA1);

Lcd_WriteIndex(0xE0);
Lcd_WriteData(0xD0);
Lcd_WriteData(0x04);
Lcd_WriteData(0x0D);
Lcd_WriteData(0x11);
Lcd_WriteData(0x13);
Lcd_WriteData(0x2B);
Lcd_WriteData(0x3F);
Lcd_WriteData(0x54);
Lcd_WriteData(0x4C);
Lcd_WriteData(0x18);
Lcd_WriteData(0x0D);
Lcd_WriteData(0x0B);
Lcd_WriteData(0x1F);
Lcd_WriteData(0x23);

Lcd_WriteIndex(0xE1);
Lcd_WriteData(0xD0);
Lcd_WriteData(0x04);
Lcd_WriteData(0x0C);
Lcd_WriteData(0x11);
Lcd_WriteData(0x13);
Lcd_WriteData(0x2C);
Lcd_WriteData(0x3F);
Lcd_WriteData(0x44);
Lcd_WriteData(0x51);
Lcd_WriteData(0x2F);
Lcd_WriteData(0x1F);
Lcd_WriteData(0x1F);
Lcd_WriteData(0x20);
Lcd_WriteData(0x23);

Lcd_WriteIndex(0x21); 

Lcd_WriteIndex(0x11); 
//Delay (120); 

Lcd_WriteIndex(0x29); 
	
}


/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void Lcd_SetRegion(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end)
{		
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(x_start>>8);
	Lcd_WriteData(x_start);
	Lcd_WriteData(x_end>>8);
	Lcd_WriteData(x_end);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(y_start>>8);
	Lcd_WriteData(y_start);
	Lcd_WriteData(y_end>>8);
	Lcd_WriteData(y_end);
	
	Lcd_WriteIndex(0x2c);

}

void Lcd_SetCursor(uint8_t x_start,uint8_t y_start)
{		
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(x_start>>8);
	Lcd_WriteData(x_start);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(y_start>>8);
	Lcd_WriteData(y_start);
}

/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(uint16_t	Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
  
   for(i=0;i<X_MAX_PIXEL;i++)
	{
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    }
	}		
}

/*************************************************
��������LCD_draw_point
���ܣ���һ����
��ڲ�����
����ֵ����
*************************************************/
void LCD_draw_point(uint8_t x,uint8_t y,uint16_t Color)
{
	Lcd_SetRegion(x,y,x,y);
	LCD_WriteData_16Bit(Color);

}    

/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(uint8_t x,uint8_t y)
{
  	Lcd_SetRegion(x,y,x,y);
}


void LCD_show_image(void) //��ʾ240*240ͼƬ
{ 
		int i;
	
			Lcd_SetRegion(0,0,240-1,240-1);		//��������
		    for(i=0;i<57600;i++)
			 { 	
				 		
			  	 Lcd_WriteData(gImage_123[i*2+1]);	 
				 Lcd_WriteData(gImage_123[i*2]);				
			 }	
	
	ref=0;		
}



/*
void LCD_show_char(uint8_t x,uint8_t y,uint16_t color,uint8_t size,uint8_t data)
{
	uint8_t temp,t,pos;
//	uint16_t x0=x;
	uint16_t tempcolor=WHITE;
	if(x>X_MAX_PIXEL-16||y>Y_MAX_PIXEL-16)return;	  
	data=data-' ';//�õ�ƫ�ƺ��ֵ
	
				Lcd_SetRegion(x,y,x+8-1,y+16-1);
				for(pos=0;pos<16;pos++)
				{
					 temp=asc2_1608[(uint16_t)data*16+pos];		 //����1608����
						for(t=0;t<8;t++)
							{                 
									if(temp&0x01)
										LCD_draw_point(x+t,y+pos,color);//��һ����     
									temp>>=1; 
							}
				}
		
	POINT_COLOR=tempcolor;
}

void LCD_show_char24(uint8_t x,uint8_t y,uint16_t color,uint8_t size,uint8_t data)
{
	uint8_t temp,t,pos;
//	uint16_t x0=x;
	uint16_t tempcolor=WHITE;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);  
	data=data-' ';//�õ�ƫ�ƺ��ֵ
	
				Lcd_SetRegion(x,y,x+24-1,y+24-1);
				for(pos=0;pos<csize;pos++)
				{
					 temp=asc2_2412[data][pos];	 //����2412����
						for(t=0;t<8;t++)
							{                 
									if(temp&0x80)
									{
										LCD_draw_point(x,y+pos,color);//��һ����     
									}
									temp<<=1; 
									
							}
				}
		
	POINT_COLOR=tempcolor;
}


//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����
void LCD_show_string(uint8_t x,uint8_t y,uint16_t color,uint8_t size,const uint8_t *p)
{    
		while(*p!='\0')
    {       
//        if(x>X_MAX_PIXEL-16){x=0;y+=16;}
//        if(y>Y_MAX_PIXEL-16){y=x=0;Lcd_Clear(WHITE);}
			if(size == 16){
        LCD_show_char(x,y,color,size,*p);
			}
			if(size == 24){
				LCD_show_char24(x,y,color,size,*p);
			}
				x+=24;
        p++;
    }  
}


uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}




//���ߺ���
//������x1��x2��y1��y2��һ���߶Σ�color����ɫ
int LCD_draw_line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint16_t color)
{
	if((x1>132)|(x2>132)|(y1>132)|(y2>132))
	{
		return 0;
	}
	uint16_t tempcolor = color;
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1;
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else{incx=-1;delta_x=-delta_x;}
	
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;}
	
	if(delta_x>delta_y)distance=delta_x;
	else distance=delta_y;
	for(t=0;t<=distance+1;t++)
		{
			LCD_draw_point(uRow,uCol,tempcolor);
			xerr+=delta_x;
			yerr+=delta_y;
			if(xerr>distance)
			{
				xerr-=distance;
				uRow+=incx;
			}
			if(yerr>distance)
			{
				yerr-=distance;
				uCol+=incy;
			}
		}
	
}

//��Բ����
int LCD_draw_circle(uint8_t x,uint8_t y,uint8_t r,uint16_t color)
{
	if((x>132)|(y>132)|(r>66))
	{
		return 0;
	}
	
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);
	while(a<=b)
	{
		LCD_draw_point(x-b,y-a,color);
		LCD_draw_point(x+b,y-a,color);
		LCD_draw_point(x-a,y+b,color);
		LCD_draw_point(x-b,y-a,color);
		LCD_draw_point(x-a,y-b,color);
		LCD_draw_point(x+b,y+a,color);
		LCD_draw_point(x+a,y-b,color);
		LCD_draw_point(x+a,y+b,color);
		LCD_draw_point(x-b,y+a,color);
		a++;
		if(di<0)di+=4*a+6;
		else
		{
				di+=10+4*(a-b);
			  b--;
		}
		LCD_draw_point(x+a,y+b,color);
		
	}
}

//������
int LCD_draw_rect(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint16_t color,int fill)
{
	uint16_t temp;
	uint16_t i,m;

	if((x1>132)|(x2>132)|(y1>132)|(y2>132))
	{
		return 0;
	}
	
	if(x1>x2)
	{
		temp=x1;
		x1=x2;
		x2=temp;
	}
	
	if(y1>y2)
	{
		temp=y1;
		y1=y2;
		y2=temp;
	}
	
	LCD_draw_line(x1,y1,x1,y2,color);
	LCD_draw_line(x2,y1,x2,y2,color);
	LCD_draw_line(x1,y1,x2,y1,color);
	LCD_draw_line(x1,y2,x2,y2,color);
	if(fill)
	{
	 
		Lcd_SetRegion(x1,y1,x2,y2+1);
   Lcd_WriteIndex(0x2C);
   for(i=x1;i<x2;i++)
    for(m=y1;m<y2+1;m++)
    {	
	  	LCD_WriteData_16Bit(color);
    }   
		
		
	}
}

//��ָ��λ����ʾһ������(32*33��С)
//dcolorΪ������ɫ��gbcolorΪ������ɫ
void LCD_show_hanzi(unsigned int x,unsigned int y,unsigned char index)	
{  
	unsigned char i,j;
	unsigned char *temp=hanzi;    
    Lcd_SetRegion(x,y,x+31,y+31); //��������      
	temp+=index*128;	
	for(j=0;j<128;j++)
	{
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<i))!=0)
			{
				LCD_WriteData_16Bit(BLACK);
			} 
			else
			{
				LCD_WriteData_16Bit(WHITE);
			}   
		}
		temp++;
	 }
}

*/
void LCD_Pin_Conf(void)
{
	  
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
