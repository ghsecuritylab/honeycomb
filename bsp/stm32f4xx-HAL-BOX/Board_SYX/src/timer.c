#include "timer.h"
#include "SMG.h"


TIM_HandleTypeDef TIM3_Handler;      	//��ʱ����� 
TIM_OC_InitTypeDef TIM3_CH1Handler;

TIM_HandleTypeDef TIM4_Handler;      	//��ʱ����� 
TIM_OC_InitTypeDef TIM4_CH2Handler;

TIM_HandleTypeDef TIM2_Handler;      //��ʱ����� 


void TIM3_PWM_Init(uint16_t arr,uint16_t psc)
{  
    TIM3_Handler.Instance=TIM3;          	//��ʱ��3
    TIM3_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM3_Handler.Init.Period=arr;          //�Զ���װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler);       //��ʼ��PWM
    
    TIM3_CH1Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM3_CH1Handler.Pulse=0;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM3_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH1Handler,TIM_CHANNEL_1);//����TIM3ͨ��1
	
    HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_1);//����PWMͨ��1
}

void TIM4_PWM_Init(uint16_t arr,uint16_t psc)
{  
    TIM4_Handler.Instance=TIM4;          	//��ʱ��4
    TIM4_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM4_Handler.Init.Period=arr;          //�Զ���װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM4_Handler);       //��ʼ��PWM
    
    TIM4_CH2Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM4_CH2Handler.Pulse=arr;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM4_CH2Handler.OCPolarity=TIM_OCPOLARITY_LOW; //����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM4_Handler,&TIM4_CH2Handler,TIM_CHANNEL_2);//����TIM4ͨ��2
	
    HAL_TIM_PWM_Start(&TIM4_Handler,TIM_CHANNEL_2);//����PWMͨ��1
}


//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM3_CLK_ENABLE();			//ʹ�ܶ�ʱ��3
	__HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	
		__HAL_RCC_TIM4_CLK_ENABLE();			//ʹ�ܶ�ʱ��4
	__HAL_RCC_GPIOD_CLK_ENABLE();			//����GPIODʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_6;           	//PC6
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	GPIO_Initure.Alternate= GPIO_AF2_TIM3;	//PC6����ΪTIM3_CH1
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_13;           	//PD13
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	GPIO_Initure.Alternate= GPIO_AF2_TIM4;	//PD13����ΪTIM4_CH2
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
}

//����TIMͨ��4��ռ�ձ�
//compare:�Ƚ�ֵ
void TIM_SetTIM3Compare1(uint32_t compare)
{
	TIM3->CCR1=compare; 
}

void TIM_SetTIM4Compare1(uint32_t compare)
{
	TIM4->CCR2=compare; 
}

void TIM3_Set_freq(uint16_t arr)
{
	TIM3->ARR=arr;
	TIM3->CCR1=(uint16_t)(arr);
}

void TIM4_Set_freq(uint16_t arr)
{
	TIM4->ARR=arr;
	TIM4->CCR2=(uint16_t)(arr);
}



void MX_TIM2_Init(void)
{
    TIM2_Handler.Instance=TIM2;                          //ͨ�ö�ʱ��3
    TIM2_Handler.Init.Prescaler=84-1;                     //��Ƶϵ��
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM2_Handler.Init.Period=5000-1;                        //�Զ�װ��ֵ
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM2_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM2_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE  

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
  
		__HAL_RCC_TIM2_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM2_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //����ITM3�ж�   
	
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

 
//��ʱ��3�жϷ�����
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM2_Handler);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	 if (htim == (&TIM2_Handler))

	 {

		 SMG_show(45);
		//HAL_Delay(1000);
	 }

}

