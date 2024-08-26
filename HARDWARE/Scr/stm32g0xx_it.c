/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*          实现各种中断服务函数的源文件           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "mqtt.h"             //包含需要的头文件
#include "cat1.h"             //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：串口3-4中断处理函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART3_4_IRQHandler(void)
{	
	HAL_UART_IRQHandler(&UART3_ControlCB.USART_Handler);			
	if(__HAL_UART_GET_FLAG(&UART3_ControlCB.USART_Handler,UART_FLAG_IDLE) != RESET){	                         //如果发生空闲中断      	
		__HAL_UART_CLEAR_IDLEFLAG(&UART3_ControlCB.USART_Handler);                                               //清除空闲中断标志 												
		UART3_ControlCB.Usart_RxCounter += (U3_RXMAX_SIZE - __HAL_DMA_GET_COUNTER(&UART3_ControlCB.USART_RxDMA));//累计当前缓冲区已经存放的数据量		
		HAL_UART_AbortReceive_IT(&UART3_ControlCB.USART_Handler);                                                // 中止上一次的接收工作                          
	}
}
/*-------------------------------------------------*/
/*函数名：串口2中断处理函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)
{	
	HAL_UART_IRQHandler(&UART2_ControlCB.USART_Handler);			
	if(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_IDLE) != RESET){	                         //如果发生空闲中断      	
		__HAL_UART_CLEAR_IDLEFLAG(&UART2_ControlCB.USART_Handler);                                               //清除空闲中断标志 												
		UART2_ControlCB.Usart_RxCounter += (U2_RXMAX_SIZE - __HAL_DMA_GET_COUNTER(&UART2_ControlCB.USART_RxDMA));//累计当前缓冲区已经存放的数据量		
		HAL_UART_AbortReceive_IT(&UART2_ControlCB.USART_Handler);                                                // 中止上一次的接收工作                          
	}
}
/*-------------------------------------------------*/
/*函数名：串口3发送DMA通道中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&UART3_ControlCB.USART_TxDMA);
}
/*-------------------------------------------------*/
/*函数名：串口3接收DMA通道中断处理函数            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel2_3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&UART3_ControlCB.USART_RxDMA);
}
/*-------------------------------------------------*/
/*函数名：串口2收发DMA通道中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&UART2_ControlCB.USART_TxDMA);
	HAL_DMA_IRQHandler(&UART2_ControlCB.USART_RxDMA);
}
/*-------------------------------------------------*/
/*函数名：不可屏蔽中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：硬件出错后进入的中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}
/*-------------------------------------------------*/
/*函数名：软中断，SWI 指令调用的处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}
/*-------------------------------------------------*/
/*函数名：可挂起的系统服务处理函数                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
	
}
/*-------------------------------------------------*/
/*函数名：SysTic系统嘀嗒定时器处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{  
	HAL_IncTick();	
}
