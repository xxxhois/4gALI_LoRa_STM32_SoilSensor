/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*             操作4G模块功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __CAT1_H
#define __CAT1_H

#include "usart.h"	    

#define POWER_KEY(x)      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (GPIO_PinState)x) //PA15控制复位IO
#define POWER_STA         HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)                     //PA0读取电平状态,可以判断是开关机状态
#define NET_STA           HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)                     //PA1读取电平状态,可以判断网络注册状态

#define CAT1_printf       u2_printf                                               //串口2控制 CAT1

void CAT1_Reset(void);
void SysInit(void);                               
void U2PassiveEvent(uint8_t *, uint16_t);
void ActiveEvent(void);                        
void CAT1_PropertyVersion(uint8_t *,uint8_t *);
void CAT1_OTADownload(int, int);
void CAT1_SubDeviceLogin(char *);
void CAT1_SubDevicePost(char *);
	
#endif


