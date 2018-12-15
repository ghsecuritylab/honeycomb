#include "dht11.h"

void DHT11_IO_IN(void)
{
		GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin=GPIO_PIN_15;           	//PD15
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//��������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��
}

void DHT11_IO_OUT(void)
{
		GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin=GPIO_PIN_15;           	//PD15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��
}


//��λDHT11
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//����Ϊ���
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET); 	//����DQ
	HAL_Delay_us(20000);    	//��������18ms
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET); 	//DQ=1 
	HAL_Delay_us(30);     	//��������20~40us
}

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	DHT11_IO_IN();      //����Ϊ���	
	while (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15)&&retry<100)//DHT11������40~80us
	{
		retry++;
		HAL_Delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
	while ((!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15)) && (retry<100))//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		HAL_Delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

//��DHT11��ȡһ��λ
//����ֵ��1/0
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15)&&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		HAL_Delay_us(1);
	}
	retry=0;
	while(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15)&&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		HAL_Delay_us(1);
	}
	HAL_Delay_us(40);//�ȴ�40us
	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15))return 1;
	else return 0;		   
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
uint8_t DHT11_Read_Byte(void)    
{        
	uint8_t i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)    
{        
 	uint8_t buf[5];
	uint8_t i;
	uint8_t result = 0;
	
	//rt_enter_critical();
	
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
		else
		{
			result = 1;
		}
	}else {
		result = 1;
	}
	
	//rt_exit_critical();
	
	return result;	    
}

//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����     	 
uint8_t DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();			//����GPIODʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_15;           	//PD15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��
 
    DHT11_Rst();
	return DHT11_Check();
}
