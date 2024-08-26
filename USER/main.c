/*-----------------------------------------------------*/
/*              超子说物联网STM32系列开发板            */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*STM32G0xx_HAL_Driver  ：官方提供的库文件             */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "clock.h"            //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "lora.h"	          //包含需要的头文件
#include "iic.h"              //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件
#include "spi.h"              //包含需要的头文件
#include "cat1.h"	          //包含需要的头文件

Sys_CB  SysCB;                                     //用于各种系统参数的结构体
Attribute_CB    AttributeCB[SUB_DEVICE_NUM];       //用于保存各种属性功能的结构体
AliyunInfo_CB   AliInfoCB;                         //EEPROM内保存的网关板的阿里云证书信息结构体
AliyunInfo_CB   AliInfoCB_SUB[SUB_DEVICE_NUM];     //用于保存子设备阿里云证书信息结构体
OTA_CB AliOTA;                                     //用于阿里云OTA升级的结构体

int main(void) 
{    			
//	uint16_t i;

	HAL_Init();                        //初始化HAL	
	CLock_Init();                      //初始化时钟
	U1_Init(921600);                   //初始化串口1	
	U3_Init(9600);                     //初始化串口3
	U2_Init(115200);                   //初始化串口2	
	IIC_Init();                        //初始化IIC接口
	W25Qxx_Init();                     //初始化W25Qxx
	EEprom_ReadInfo();                 //EEprom读取阿里云证书信息 
    LoRa_Init();                       //初始化LoRa模块	
    
	while(1){                          //主循环
		/*-----------------------------------------------------------------------------------------*/  
		/*                             处 理 串 口 2 接 收 到 的 数 据                             */
		/*-----------------------------------------------------------------------------------------*/
		//如果 UsartRxDataOutPtr 指针 和 UsartRxDataInPtr 指针不相等，则if成立，说明有数据接收到了
		if(UART2_ControlCB.UsartRxDataOutPtr != UART2_ControlCB.UsartRxDataInPtr){                           														    					   						
			u1_printf("串口2本次接收%d字节报文数据：\r\n",UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr);	                //串口输出数据	                    
//			for(i=0;i<UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART2_ControlCB.UsartRxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");		                                                                  				
			memset(U2_CopyBuff,0,U2_RXMAX_SIZE + 1);                                                         													    //清空缓冲区
			memcpy(U2_CopyBuff,UART2_ControlCB.UsartRxDataOutPtr->StartPtr,UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr);//拷贝数据
			U2PassiveEvent(U2_CopyBuff,UART2_ControlCB.UsartRxDataOutPtr->EndPtr - UART2_ControlCB.UsartRxDataOutPtr->StartPtr);                                    //处理串口2被动事件数据							
			UART2_ControlCB.UsartRxDataOutPtr ++;                                                                                                                   //Out指针下移
			if(UART2_ControlCB.UsartRxDataOutPtr==UART2_ControlCB.UsartRxDataEndPtr)                        													    //如果下移到End指针标记的结尾位置
				UART2_ControlCB.UsartRxDataOutPtr = &UART2_ControlCB.Usart_RxLocation[0];                   											            //接收缓冲区Out插入指针归位到位置结构体数组0号成员 
		}
		/*-----------------------------------------------------------------------------------------*/  
		/*                              处 理 串 口 2 发 送 的 数 据                               */
		/*-----------------------------------------------------------------------------------------*/
		//如果 UsartTxDataOutPtr指针 和 UsartTxDataInPtr指针不相等 且 发送空闲 且 距离上次发送间隔超过100ms 则if成立，说明有数据要发送            
		if((UART2_ControlCB.UsartTxDataOutPtr != UART2_ControlCB.UsartTxDataInPtr)&&(UART2_ControlCB.Usart_TxCpltflag == 0)){               					   						
		    u1_printf("串口2本次发送%d字节数据\r\n",UART2_ControlCB.UsartTxDataOutPtr->EndPtr - UART2_ControlCB.UsartTxDataOutPtr->StartPtr);		                                         
//			for(i=0;i<UART2_ControlCB.UsartTxDataOutPtr->EndPtr - UART2_ControlCB.UsartTxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART2_ControlCB.UsartTxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");		                                                                  				
			if(HAL_UART_Transmit_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartTxDataOutPtr->StartPtr,UART2_ControlCB.UsartTxDataOutPtr->EndPtr - UART2_ControlCB.UsartTxDataOutPtr->StartPtr)== HAL_OK){
				UART2_ControlCB.Usart_TxCpltflag = 1;                                                         //发送中
				UART2_ControlCB.UsartTxDataOutPtr ++;                                                         //Out指针下移
				if(UART2_ControlCB.UsartTxDataOutPtr==UART2_ControlCB.UsartTxDataEndPtr)                      //如果下移到End指针标记的结尾位置
					UART2_ControlCB.UsartTxDataOutPtr = &UART2_ControlCB.Usart_TxLocation[0];                 //发送缓冲区Out插入指针归位到位置结构体数组0号成员
			}			
		}
		/*-----------------------------------------------------------------------------------------*/  
		/*                             处 理 串 口 3 接 收 到 的 数 据                             */
		/*-----------------------------------------------------------------------------------------*/
		//如果 UsartRxDataOutPtr指针 和 UsartRxDataInPtr指针不相等，则if成立，说明有数据接收到了    			
		if(UART3_ControlCB.UsartRxDataOutPtr != UART3_ControlCB.UsartRxDataInPtr){                           		   						
			u1_printf("串口3本次接收%d字节报文数据：\r\n",UART3_ControlCB.UsartRxDataOutPtr->EndPtr - UART3_ControlCB.UsartRxDataOutPtr->StartPtr);		                    	
//			for(i=0;i<UART3_ControlCB.UsartRxDataOutPtr->EndPtr - UART3_ControlCB.UsartRxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART3_ControlCB.UsartRxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");			
			U3PassiveEvent(UART3_ControlCB.UsartRxDataOutPtr->StartPtr,UART3_ControlCB.UsartRxDataOutPtr->EndPtr - UART3_ControlCB.UsartRxDataOutPtr->StartPtr);  //处理串口4被动事件数据							
			UART3_ControlCB.UsartRxDataOutPtr ++;                                                            //Out指针下移
			if(UART3_ControlCB.UsartRxDataOutPtr==UART3_ControlCB.UsartRxDataEndPtr)                         //如果下移到End指针标记的结尾位置
				UART3_ControlCB.UsartRxDataOutPtr = &UART3_ControlCB.Usart_RxLocation[0];                    //接收缓冲区Out插入指针归位到位置结构体数组0号成员 
		}
		/*-----------------------------------------------------------------------------------------*/  
		/*                              处 理 串 口 3 发 送 的 数 据                               */
		/*-----------------------------------------------------------------------------------------*/
		//如果 UsartTxDataOutPtr指针 和 UsartTxDataInPtr指针不相等 且 发送空闲 且 距离上次发送间隔超过200ms 则if成立，说明有数据要发送            
		if((UART3_ControlCB.UsartTxDataOutPtr != UART3_ControlCB.UsartTxDataInPtr)&&(UART3_ControlCB.Usart_TxCpltflag == 0)&&(HAL_GetTick() - UART3_ControlCB.Usart_TxTimer>100)){               					   						
		    u1_printf("串口3本次发送%d字节数据\r\n",UART3_ControlCB.UsartTxDataOutPtr->EndPtr - UART3_ControlCB.UsartTxDataOutPtr->StartPtr);		                                         
//			for(i=0;i<UART3_ControlCB.UsartTxDataOutPtr->EndPtr - UART3_ControlCB.UsartTxDataOutPtr->StartPtr;i++)
//				u1_printf("%02x ",UART3_ControlCB.UsartTxDataOutPtr->StartPtr[i]);		                            
//            u1_printf("\r\n\r\n");				
			if(HAL_UART_Transmit_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartTxDataOutPtr->StartPtr,UART3_ControlCB.UsartTxDataOutPtr->EndPtr - UART3_ControlCB.UsartTxDataOutPtr->StartPtr)== HAL_OK){
				UART3_ControlCB.Usart_TxCpltflag = 1;                                                        //发送中
				UART3_ControlCB.UsartTxDataOutPtr ++;                                                        //Out指针下移
				if(UART3_ControlCB.UsartTxDataOutPtr==UART3_ControlCB.UsartTxDataEndPtr)                     //如果下移到End指针标记的结尾位置
					UART3_ControlCB.UsartTxDataOutPtr = &UART3_ControlCB.Usart_TxLocation[0];                //发送缓冲区Out插入指针归位到位置结构体数组0号成员
			}			
		}
		/*----------------------------------------------------------------------------------------*/
		/*                                     主 动 事 件                                        */
		/*----------------------------------------------------------------------------------------*/	
		ActiveEvent();
	}
}
