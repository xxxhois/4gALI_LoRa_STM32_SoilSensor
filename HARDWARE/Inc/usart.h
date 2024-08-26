/*-------------------------------------------------*/
/*           超子说物联网STM32系列开发板           */
/*-------------------------------------------------*/
/*                                                 */
/*             实现各个串口功能的头文件            */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __USART_H
#define __USART_H

#include "stdio.h"            
#include "stdarg.h"		   
#include "string.h"          

#define U1_TXBUFF_SIZE     1024                    //定义串口1 发送缓冲区大小

#define U3_TXBUFF_SIZE     2048                    //定义串口3 发送缓冲区大小
#define U3_RXBUFF_SIZE     2048                    //定义串口3 接收缓冲区大小
#define U3_RXMAX_SIZE      256                     //定义串口3 单次接收最大数据量

#define U2_TXBUFF_SIZE     1024                    //定义串口2 发送缓冲区大小
#define U2_RXBUFF_SIZE     1024*8                  //定义串口2 接收缓冲区大小
#define U2_RXMAX_SIZE      512                     //定义串口2 单次接收最大数据量

#define TX_NUM  10                                 //标记接收数据起始、截止位置的结构体数组成员数量
#define RX_NUM  10                                 //标记发送数据起始、截止位置的结构体数组成员数量

typedef struct{       
	uint8_t *StartPtr;                             //标记数据的起始位置
	uint8_t *EndPtr;                               //标记数据的截止位置
	uint32_t Timecounter;                          //记录数据接收的时基
}USART_LocationPtrCB;                              //标记数据起始、截止位置的结构体

typedef struct{        
	uint32_t Usart_RxCounter;                      //累计接收缓冲区存放的数据量
	uint32_t Usart_TxCounter;                      //累计发送缓冲区存放的数据量
	uint32_t Usart_TxTimer;                        //发送间隔时间变量
	uint8_t  Usart_TxCpltflag;                     //发送完成的标志 0：空闲  1：发送中
	USART_LocationPtrCB  Usart_RxLocation[RX_NUM]; //标记接收数据起始、截止位置的结构体的数组
	USART_LocationPtrCB *UsartRxDataInPtr;         //指向标记接收数据起始、截止位置的结构体的数组的插入数据的位置
	USART_LocationPtrCB *UsartRxDataOutPtr;        //指向标记接收数据起始、截止位置的结构体的数组的提取数据的位置
	USART_LocationPtrCB *UsartRxDataEndPtr;        //指向标记接收数据起始、截止位置的结构体的数组的结尾成员的位置
	USART_LocationPtrCB  Usart_TxLocation[TX_NUM]; //标记发送数据起始、截止位置的结构体的数组
	USART_LocationPtrCB *UsartTxDataInPtr;         //指向标记发送数据起始、截止位置的结构体的数组的插入数据的位置
	USART_LocationPtrCB *UsartTxDataOutPtr;        //指向标记发送数据起始、截止位置的结构体的数组的提取数据的位置
	USART_LocationPtrCB *UsartTxDataEndPtr;        //指向标记发送数据起始、截止位置的结构体的数组的结尾成员的位置
	UART_HandleTypeDef USART_Handler;              //串口设置句柄
	DMA_HandleTypeDef  USART_RxDMA;                //配置接收 DMA的句柄	
	DMA_HandleTypeDef  USART_TxDMA;                //配置发送 DMA的句柄
}USART_ControlCB;     

extern USART_ControlCB UART1_ControlCB;            //外部变量声明，串口1控制结构体
extern USART_ControlCB UART3_ControlCB;            //外部变量声明，串口3控制结构体
extern USART_ControlCB UART2_ControlCB;            //外部变量声明，串口2控制结构体
extern uint8_t  U1_TxBuff[U1_TXBUFF_SIZE];         //外部变量声明，串口1发送缓冲区 
extern uint8_t  U3_TxBuff[U3_TXBUFF_SIZE];         //外部变量声明，串口3发送缓冲区 
extern uint8_t  U3_RxBuff[U3_RXBUFF_SIZE];         //外部变量声明，串口3发接收冲区 
extern uint8_t  U2_TxBuff[U2_TXBUFF_SIZE];         //外部变量声明，串口2发送缓冲区 
extern uint8_t  U2_RxBuff[U2_RXBUFF_SIZE];         //外部变量声明，串口2发接收冲区 
extern uint8_t  U2_CopyBuff[U2_RXMAX_SIZE + 1];    //外部变量声明，串口2拷贝接收数据 

void U1_Init(uint32_t);                            //函数声明，初始化串口1
void U3_Init(uint32_t);                            //函数声明，初始化串口3
void U2_Init(uint32_t);                            //函数声明，初始化串口2
void u1_printf(char* fmt,...);                     //函数声明，串口1 printf函数
void u2_printf(char* fmt,...);                     //函数声明，串口2 printf函数
void u3_TxData(uint8_t *, uint16_t);               //函数声明，串口3发送数据
void u2_TxData(uint8_t *, uint16_t);               //函数声明，串口2发送数据
void u3_BuffInit(void);                            //函数声明，初始化串口3收发缓冲区以及各个指针
void u2_BuffInit(void);                            //函数声明，初始化串口2收发缓冲区以及各个指针
void u3_TxDataBuf(uint8_t *, uint32_t);            //函数声明，向发送缓冲区添加数据
void u2_TxDataBuf(uint8_t *, uint32_t);            //函数声明，向发送缓冲区添加数据

#endif


