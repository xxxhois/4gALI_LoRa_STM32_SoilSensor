/*-------------------------------------------------*/
/*           ����˵������STM32ϵ�п�����           */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ�ָ������ڹ��ܵ�ͷ�ļ�            */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __USART_H
#define __USART_H

#include "stdio.h"            
#include "stdarg.h"		   
#include "string.h"          

#define U1_TXBUFF_SIZE     1024                    //���崮��1 ���ͻ�������С

#define U3_TXBUFF_SIZE     2048                    //���崮��3 ���ͻ�������С
#define U3_RXBUFF_SIZE     2048                    //���崮��3 ���ջ�������С
#define U3_RXMAX_SIZE      256                     //���崮��3 ���ν������������

#define U2_TXBUFF_SIZE     1024                    //���崮��2 ���ͻ�������С
#define U2_RXBUFF_SIZE     1024*8                  //���崮��2 ���ջ�������С
#define U2_RXMAX_SIZE      512                     //���崮��2 ���ν������������

#define TX_NUM  10                                 //��ǽ���������ʼ����ֹλ�õĽṹ�������Ա����
#define RX_NUM  10                                 //��Ƿ���������ʼ����ֹλ�õĽṹ�������Ա����

typedef struct{       
	uint8_t *StartPtr;                             //������ݵ���ʼλ��
	uint8_t *EndPtr;                               //������ݵĽ�ֹλ��
	uint32_t Timecounter;                          //��¼���ݽ��յ�ʱ��
}USART_LocationPtrCB;                              //���������ʼ����ֹλ�õĽṹ��

typedef struct{        
	uint32_t Usart_RxCounter;                      //�ۼƽ��ջ�������ŵ�������
	uint32_t Usart_TxCounter;                      //�ۼƷ��ͻ�������ŵ�������
	uint32_t Usart_TxTimer;                        //���ͼ��ʱ�����
	uint8_t  Usart_TxCpltflag;                     //������ɵı�־ 0������  1��������
	USART_LocationPtrCB  Usart_RxLocation[RX_NUM]; //��ǽ���������ʼ����ֹλ�õĽṹ�������
	USART_LocationPtrCB *UsartRxDataInPtr;         //ָ���ǽ���������ʼ����ֹλ�õĽṹ�������Ĳ������ݵ�λ��
	USART_LocationPtrCB *UsartRxDataOutPtr;        //ָ���ǽ���������ʼ����ֹλ�õĽṹ����������ȡ���ݵ�λ��
	USART_LocationPtrCB *UsartRxDataEndPtr;        //ָ���ǽ���������ʼ����ֹλ�õĽṹ�������Ľ�β��Ա��λ��
	USART_LocationPtrCB  Usart_TxLocation[TX_NUM]; //��Ƿ���������ʼ����ֹλ�õĽṹ�������
	USART_LocationPtrCB *UsartTxDataInPtr;         //ָ���Ƿ���������ʼ����ֹλ�õĽṹ�������Ĳ������ݵ�λ��
	USART_LocationPtrCB *UsartTxDataOutPtr;        //ָ���Ƿ���������ʼ����ֹλ�õĽṹ����������ȡ���ݵ�λ��
	USART_LocationPtrCB *UsartTxDataEndPtr;        //ָ���Ƿ���������ʼ����ֹλ�õĽṹ�������Ľ�β��Ա��λ��
	UART_HandleTypeDef USART_Handler;              //�������þ��
	DMA_HandleTypeDef  USART_RxDMA;                //���ý��� DMA�ľ��	
	DMA_HandleTypeDef  USART_TxDMA;                //���÷��� DMA�ľ��
}USART_ControlCB;     

extern USART_ControlCB UART1_ControlCB;            //�ⲿ��������������1���ƽṹ��
extern USART_ControlCB UART3_ControlCB;            //�ⲿ��������������3���ƽṹ��
extern USART_ControlCB UART2_ControlCB;            //�ⲿ��������������2���ƽṹ��
extern uint8_t  U1_TxBuff[U1_TXBUFF_SIZE];         //�ⲿ��������������1���ͻ����� 
extern uint8_t  U3_TxBuff[U3_TXBUFF_SIZE];         //�ⲿ��������������3���ͻ����� 
extern uint8_t  U3_RxBuff[U3_RXBUFF_SIZE];         //�ⲿ��������������3�����ճ��� 
extern uint8_t  U2_TxBuff[U2_TXBUFF_SIZE];         //�ⲿ��������������2���ͻ����� 
extern uint8_t  U2_RxBuff[U2_RXBUFF_SIZE];         //�ⲿ��������������2�����ճ��� 
extern uint8_t  U2_CopyBuff[U2_RXMAX_SIZE + 1];    //�ⲿ��������������2������������ 

void U1_Init(uint32_t);                            //������������ʼ������1
void U3_Init(uint32_t);                            //������������ʼ������3
void U2_Init(uint32_t);                            //������������ʼ������2
void u1_printf(char* fmt,...);                     //��������������1 printf����
void u2_printf(char* fmt,...);                     //��������������2 printf����
void u3_TxData(uint8_t *, uint16_t);               //��������������3��������
void u2_TxData(uint8_t *, uint16_t);               //��������������2��������
void u3_BuffInit(void);                            //������������ʼ������3�շ��������Լ�����ָ��
void u2_BuffInit(void);                            //������������ʼ������2�շ��������Լ�����ָ��
void u3_TxDataBuf(uint8_t *, uint32_t);            //�������������ͻ������������
void u2_TxDataBuf(uint8_t *, uint32_t);            //�������������ͻ������������

#endif


