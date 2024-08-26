/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*                 时钟配置源文件                  */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"  //包含需要的头文件
#include "clock.h"          //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：初始化时钟函数                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CLock_Init(void)
{  
	RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
	
	SysTick->CTRL &=~ (SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk);               		 //HAL_Init函数开启了SysTick和中断 在此关闭SysTick和关闭中断   	
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);                           		 //设置SysTick频率为HCLK 8分频
	
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;   		                         //使用内部主晶振
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;                                            		 //开内部晶振
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                                            		 //内部晶振 1分频
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;                 		 //默认的内部晶振校准微调值
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                                        		 //PLL开
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;                                		 //PLL源是内部晶振
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;                                         		 //M值   RCLK=16/M*N/R=16/1*8/2=64M
	RCC_OscInitStruct.PLL.PLLN = 8;                                                     		 //N值      
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;                                         		 //R值
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                                         		 //P值
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)                               			 //配置，如果失败进入if，重启
		NVIC_SystemReset();                                                             		 //重启
	
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;   //配置HCLK SYSCLK PCLK1
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;                                    //SYSCLK源是PLL
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                           //AHB分频1
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;                                            //APB1分频1
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)                      //配置，如果失败进入if，重启
		NVIC_SystemReset();                                                                      //重启
}
/*-------------------------------------------------*/
/*函数名：延迟微秒函数                             */
/*参  数：us：延时多少微秒                         */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Delay_Us(uint16_t us)
{		
	int32_t temp,load;
	
	load = SysTick->LOAD;                                                         //获取SysTick重载值
	temp = SysTick->VAL;                                                          //获取SysTick当前计数值
	
	if((temp - 64*us) >= 0){                                                      //如果延时后的计数量，不小于零，进入if
		while((SysTick->VAL > (temp - 64*us))&&(SysTick->VAL <=temp));            //等到延时时间到
	}else{                                                                        //反之，延时后的计数量，小于零，进入else
		while((SysTick->VAL <= temp)||(SysTick->VAL > (load - (64*us - temp))));  //等到延时时间到
	}	
}
