/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*             ����4Gģ�鹦�ܵ�ͷ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __CAT1_H
#define __CAT1_H

#include "usart.h"	    

#define POWER_KEY(x)      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (GPIO_PinState)x) //PA15���Ƹ�λIO
#define POWER_STA         HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)                     //PA0��ȡ��ƽ״̬,�����ж��ǿ��ػ�״̬
#define NET_STA           HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)                     //PA1��ȡ��ƽ״̬,�����ж�����ע��״̬

#define CAT1_printf       u2_printf                                               //����2���� CAT1

void CAT1_Reset(void);
void SysInit(void);                               
void U2PassiveEvent(uint8_t *, uint16_t);
void ActiveEvent(void);                        
void CAT1_PropertyVersion(uint8_t *,uint8_t *);
void CAT1_OTADownload(int, int);
void CAT1_SubDeviceLogin(char *);
void CAT1_SubDevicePost(char *);
	
#endif


