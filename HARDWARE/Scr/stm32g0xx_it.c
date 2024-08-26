/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*          ʵ�ָ����жϷ�������Դ�ļ�           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "main.h"             //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�
#include "mqtt.h"             //������Ҫ��ͷ�ļ�
#include "cat1.h"             //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*������������3-4�жϴ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART3_4_IRQHandler(void)
{	
	HAL_UART_IRQHandler(&UART3_ControlCB.USART_Handler);			
	if(__HAL_UART_GET_FLAG(&UART3_ControlCB.USART_Handler,UART_FLAG_IDLE) != RESET){	                         //������������ж�      	
		__HAL_UART_CLEAR_IDLEFLAG(&UART3_ControlCB.USART_Handler);                                               //��������жϱ�־ 												
		UART3_ControlCB.Usart_RxCounter += (U3_RXMAX_SIZE - __HAL_DMA_GET_COUNTER(&UART3_ControlCB.USART_RxDMA));//�ۼƵ�ǰ�������Ѿ���ŵ�������		
		HAL_UART_AbortReceive_IT(&UART3_ControlCB.USART_Handler);                                                // ��ֹ��һ�εĽ��չ���                          
	}
}
/*-------------------------------------------------*/
/*������������2�жϴ�����                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)
{	
	HAL_UART_IRQHandler(&UART2_ControlCB.USART_Handler);			
	if(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_IDLE) != RESET){	                         //������������ж�      	
		__HAL_UART_CLEAR_IDLEFLAG(&UART2_ControlCB.USART_Handler);                                               //��������жϱ�־ 												
		UART2_ControlCB.Usart_RxCounter += (U2_RXMAX_SIZE - __HAL_DMA_GET_COUNTER(&UART2_ControlCB.USART_RxDMA));//�ۼƵ�ǰ�������Ѿ���ŵ�������		
		HAL_UART_AbortReceive_IT(&UART2_ControlCB.USART_Handler);                                                // ��ֹ��һ�εĽ��չ���                          
	}
}
/*-------------------------------------------------*/
/*������������3����DMAͨ���жϴ�����             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&UART3_ControlCB.USART_TxDMA);
}
/*-------------------------------------------------*/
/*������������3����DMAͨ���жϴ�����            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DMA1_Channel2_3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&UART3_ControlCB.USART_RxDMA);
}
/*-------------------------------------------------*/
/*������������2�շ�DMAͨ���жϴ�����             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&UART2_ControlCB.USART_TxDMA);
	HAL_DMA_IRQHandler(&UART2_ControlCB.USART_RxDMA);
}
/*-------------------------------------------------*/
/*�����������������жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ӳ������������жϴ�����             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}
/*-------------------------------------------------*/
/*�����������жϣ�SWI ָ����õĴ�����           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}
/*-------------------------------------------------*/
/*���������ɹ����ϵͳ��������                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
	
}
/*-------------------------------------------------*/
/*��������SysTicϵͳ��શ�ʱ��������             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{  
	HAL_IncTick();	
}
