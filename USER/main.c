/*-----------------------------------------------------*/
/*              ����˵������STM32ϵ�п�����            */
/*-----------------------------------------------------*/
/*                     ����ṹ                        */
/*-----------------------------------------------------*/
/*USER     �����������main��������������������      */
/*HARDWARE ��������������ֹ����������������          */
/*STM32G0xx_HAL_Driver  ���ٷ��ṩ�Ŀ��ļ�             */
/*-----------------------------------------------------*/
/*                                                     */
/*           ����main��������ں���Դ�ļ�              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "main.h"             //������Ҫ��ͷ�ļ�
#include "clock.h"            //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�
#include "lora.h"	          //������Ҫ��ͷ�ļ�
#include "iic.h"              //������Ҫ��ͷ�ļ�
#include "24c02.h"            //������Ҫ��ͷ�ļ�
#include "w25qxx.h"           //������Ҫ��ͷ�ļ�
#include "spi.h"              //������Ҫ��ͷ�ļ�
#include "cat1.h"	          //������Ҫ��ͷ�ļ�

Sys_CB  SysCB;                                     //���ڸ���ϵͳ�����Ľṹ��
Attribute_CB    AttributeCB[SUB_DEVICE_NUM];       //���ڱ���������Թ��ܵĽṹ��
AliyunInfo_CB   AliInfoCB;                         //EEPROM�ڱ�������ذ�İ�����֤����Ϣ�ṹ��
AliyunInfo_CB   AliInfoCB_SUB[SUB_DEVICE_NUM];     //���ڱ������豸������֤����Ϣ�ṹ��
OTA_CB AliOTA;                                     //���ڰ�����OTA�����Ľṹ��

int main(void) 
{    			
//	uint16_t i;

	HAL_Init();                        //��ʼ��HAL	
	CLock_Init();                      //��ʼ��ʱ��
	U1_Init(921600);                   //��ʼ������1	
	U3_Init(9600);                     //��ʼ������3
	U2_Init(115200);                   //��ʼ������2	
	IIC_Init();                        //��ʼ��IIC�ӿ�
	W25Qxx_Init();                     //��ʼ��W25Qxx
	EEprom_ReadInfo();                 //EEprom��ȡ������֤����Ϣ 
    LoRa_Init();                       //��ʼ��LoRaģ��	
    
	while(1){                          //��ѭ��
		/*-----------------------------------------------------------------------------------------*/  
		/*                             �� �� �� �� 2 �� �� �� �� �� ��                             */
		/*-----------------------------------------------------------------------------------------*/
		//��� UsartRxDataOutPtr ָ�� �� UsartRxDataInPtr ָ�벻��ȣ���if������˵�������ݽ��յ���
		if(UART2_ControlCB.UsartRxDataOutPtr != UART2_ControlCB.UsartRxDataInPtr){                           														    					   						
			u1_printf("����2���ν���%d�ֽڱ������ݣ�\r\n",UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr);	                //�����������	                    
//			for(i=0;i<UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART2_ControlCB.UsartRxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");		                                                                  				
			memset(U2_CopyBuff,0,U2_RXMAX_SIZE + 1);                                                         													    //��ջ�����
			memcpy(U2_CopyBuff,UART2_ControlCB.UsartRxDataOutPtr->StartPtr,UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr);//��������
			U2PassiveEvent(U2_CopyBuff,UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr);                                    //������2�����¼�����							
			UART2_ControlCB.UsartRxDataOutPtr ++;                                                                                                                   //Outָ������
			if(UART2_ControlCB.UsartRxDataOutPtr==UART2_ControlCB.UsartRxDataEndPtr)                        													    //������Ƶ�Endָ���ǵĽ�βλ��
				UART2_ControlCB.UsartRxDataOutPtr = &UART2_ControlCB.Usart_RxLocation[0];                   											            //���ջ�����Out����ָ���λ��λ�ýṹ������0�ų�Ա 
		}
		/*-----------------------------------------------------------------------------------------*/  
		/*                              �� �� �� �� 2 �� �� �� �� ��                               */
		/*-----------------------------------------------------------------------------------------*/
		//��� UsartTxDataOutPtrָ�� �� UsartTxDataInPtrָ�벻��� �� ���Ϳ��� �� �����ϴη��ͼ������100ms ��if������˵��������Ҫ����            
		if((UART2_ControlCB.UsartTxDataOutPtr != UART2_ControlCB.UsartTxDataInPtr)&&(UART2_ControlCB.Usart_TxCpltflag == 0)){               					   						
		    u1_printf("����2���η���%d�ֽ�����\r\n",UART2_ControlCB.UsartTxDataOutPtr->EndPtr - UART2_ControlCB.UsartTxDataOutPtr->StartPtr);		                                         
//			for(i=0;i<UART2_ControlCB.UsartTxDataOutPtr->EndPtr - UART2_ControlCB.UsartTxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART2_ControlCB.UsartTxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");		                                                                  				
			if(HAL_UART_Transmit_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartTxDataOutPtr->StartPtr,UART2_ControlCB.UsartTxDataOutPtr->EndPtr - UART2_ControlCB.UsartTxDataOutPtr->StartPtr)== HAL_OK){
				UART2_ControlCB.Usart_TxCpltflag = 1;                                                         //������
				UART2_ControlCB.UsartTxDataOutPtr ++;                                                         //Outָ������
				if(UART2_ControlCB.UsartTxDataOutPtr==UART2_ControlCB.UsartTxDataEndPtr)                      //������Ƶ�Endָ���ǵĽ�βλ��
					UART2_ControlCB.UsartTxDataOutPtr = &UART2_ControlCB.Usart_TxLocation[0];                 //���ͻ�����Out����ָ���λ��λ�ýṹ������0�ų�Ա
			}			
		}
		/*-----------------------------------------------------------------------------------------*/  
		/*                             �� �� �� �� 3 �� �� �� �� �� ��                             */
		/*-----------------------------------------------------------------------------------------*/
		//��� UsartRxDataOutPtrָ�� �� UsartRxDataInPtrָ�벻��ȣ���if������˵�������ݽ��յ���    			
		if(UART3_ControlCB.UsartRxDataOutPtr != UART3_ControlCB.UsartRxDataInPtr){                           		   						
			u1_printf("����3���ν���%d�ֽڱ������ݣ�\r\n",UART3_ControlCB.UsartRxDataOutPtr->EndPtr - UART3_ControlCB.UsartRxDataOutPtr->StartPtr);		                    	
//			for(i=0;i<UART3_ControlCB.UsartRxDataOutPtr->EndPtr - UART3_ControlCB.UsartRxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART3_ControlCB.UsartRxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");			
			U3PassiveEvent(UART3_ControlCB.UsartRxDataOutPtr->StartPtr,UART3_ControlCB.UsartRxDataOutPtr->EndPtr - UART3_ControlCB.UsartRxDataOutPtr->StartPtr);  //������4�����¼�����							
			UART3_ControlCB.UsartRxDataOutPtr ++;                                                            //Outָ������
			if(UART3_ControlCB.UsartRxDataOutPtr==UART3_ControlCB.UsartRxDataEndPtr)                         //������Ƶ�Endָ���ǵĽ�βλ��
				UART3_ControlCB.UsartRxDataOutPtr = &UART3_ControlCB.Usart_RxLocation[0];                    //���ջ�����Out����ָ���λ��λ�ýṹ������0�ų�Ա 
		}
		/*-----------------------------------------------------------------------------------------*/  
		/*                              �� �� �� �� 3 �� �� �� �� ��                               */
		/*-----------------------------------------------------------------------------------------*/
		//��� UsartTxDataOutPtrָ�� �� UsartTxDataInPtrָ�벻��� �� ���Ϳ��� �� �����ϴη��ͼ������200ms ��if������˵��������Ҫ����            
		if((UART3_ControlCB.UsartTxDataOutPtr != UART3_ControlCB.UsartTxDataInPtr)&&(UART3_ControlCB.Usart_TxCpltflag == 0)&&(HAL_GetTick() - UART3_ControlCB.Usart_TxTimer>100)){               					   						
		    u1_printf("����3���η���%d�ֽ�����\r\n",UART3_ControlCB.UsartTxDataOutPtr->EndPtr - UART3_ControlCB.UsartTxDataOutPtr->StartPtr);		                                         
//			for(i=0;i<UART3_ControlCB.UsartTxDataOutPtr->EndPtr - UART3_ControlCB.UsartTxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART3_ControlCB.UsartTxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");				
			if(HAL_UART_Transmit_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartTxDataOutPtr->StartPtr,UART3_ControlCB.UsartTxDataOutPtr->EndPtr - UART3_ControlCB.UsartTxDataOutPtr->StartPtr)== HAL_OK){
				UART3_ControlCB.Usart_TxCpltflag = 1;                                                        //������
				UART3_ControlCB.UsartTxDataOutPtr ++;                                                        //Outָ������
				if(UART3_ControlCB.UsartTxDataOutPtr==UART3_ControlCB.UsartTxDataEndPtr)                     //������Ƶ�Endָ���ǵĽ�βλ��
					UART3_ControlCB.UsartTxDataOutPtr = &UART3_ControlCB.Usart_TxLocation[0];                //���ͻ�����Out����ָ���λ��λ�ýṹ������0�ų�Ա
			}			
		}
		/*----------------------------------------------------------------------------------------*/
		/*                                     �� �� �� ��                                        */
		/*----------------------------------------------------------------------------------------*/	
		ActiveEvent();
	}
}
