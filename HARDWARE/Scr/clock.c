/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*                 ʱ������Դ�ļ�                  */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"  //������Ҫ��ͷ�ļ�
#include "clock.h"          //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*����������ʼ��ʱ�Ӻ���                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CLock_Init(void)
{  
	RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
	
	SysTick->CTRL &=~ (SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk);               		 //HAL_Init����������SysTick���ж� �ڴ˹ر�SysTick�͹ر��ж�   	
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);                           		 //����SysTickƵ��ΪHCLK 8��Ƶ
	
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;   		                         //ʹ���ڲ�������
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;                                            		 //���ڲ�����
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                                            		 //�ڲ����� 1��Ƶ
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;                 		 //Ĭ�ϵ��ڲ�����У׼΢��ֵ
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                                        		 //PLL��
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;                                		 //PLLԴ���ڲ�����
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;                                         		 //Mֵ   RCLK=16/M*N/R=16/1*8/2=64M
	RCC_OscInitStruct.PLL.PLLN = 8;                                                     		 //Nֵ      
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;                                         		 //Rֵ
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                                         		 //Pֵ
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)                               			 //���ã����ʧ�ܽ���if������
		NVIC_SystemReset();                                                             		 //����
	
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;   //����HCLK SYSCLK PCLK1
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;                                    //SYSCLKԴ��PLL
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                           //AHB��Ƶ1
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;                                            //APB1��Ƶ1
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)                      //���ã����ʧ�ܽ���if������
		NVIC_SystemReset();                                                                      //����
}
/*-------------------------------------------------*/
/*���������ӳ�΢�뺯��                             */
/*��  ����us����ʱ����΢��                         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Delay_Us(uint16_t us)
{		
	int32_t temp,load;
	
	load = SysTick->LOAD;                                                         //��ȡSysTick����ֵ
	temp = SysTick->VAL;                                                          //��ȡSysTick��ǰ����ֵ
	
	if((temp - 64*us) >= 0){                                                      //�����ʱ��ļ���������С���㣬����if
		while((SysTick->VAL > (temp - 64*us))&&(SysTick->VAL <=temp));            //�ȵ���ʱʱ�䵽
	}else{                                                                        //��֮����ʱ��ļ�������С���㣬����else
		while((SysTick->VAL <= temp)||(SysTick->VAL > (load - (64*us - temp))));  //�ȵ���ʱʱ�䵽
	}	
}
