/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��IIc���߹��ܵ�ͷ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __IIC_H
#define __IIC_H

/*--------------------------------------------------------------------------*/
/*                      ����LED��IO���� �� ʱ��ʹ��                         */
/*                        ���Ը�����Ҫ�޸ĺ�����                            */
/*--------------------------------------------------------------------------*/
#define SDA_GROUP             GPIOA                                               //SDA ��Ӧ��IO����
#define SDA_PIN               GPIO_PIN_12                                          //SDA ��Ӧ��IO
#define SDA_GROUP_CLK_ENABLE  __HAL_RCC_GPIOA_CLK_ENABLE();                       //SDA ��Ӧ��IO���� ʱ��ʹ��

#define SCL_GROUP             GPIOA                                               //SCL ��Ӧ��IO����
#define SCL_PIN               GPIO_PIN_11                                          //SCL ��Ӧ��IO
#define SCL_GROUP_CLK_ENABLE  __HAL_RCC_GPIOA_CLK_ENABLE();                       //SCL ��Ӧ��IO���� ʱ��ʹ��
/*--------------------------------------------------------------------------*/
  		   

#define READ_SDA        HAL_GPIO_ReadPin(SDA_GROUP, SDA_PIN)                               //��ȡSDA��ƽ 
#define IIC_SDA_OUT(x)  HAL_GPIO_WritePin(SDA_GROUP, SDA_PIN, (GPIO_PinState)x)            //����SDA��ƽ 

#define	IIC_SCL_H       HAL_GPIO_WritePin(SCL_GROUP, SCL_PIN, GPIO_PIN_SET)                //SCL����
#define	IIC_SDA_H       HAL_GPIO_WritePin(SDA_GROUP, SDA_PIN, GPIO_PIN_SET)                //SDA����
 
#define	IIC_SCL_L       HAL_GPIO_WritePin(SCL_GROUP, SCL_PIN, GPIO_PIN_RESET)              //SCL����
#define	IIC_SDA_L       HAL_GPIO_WritePin(SDA_GROUP, SDA_PIN, GPIO_PIN_RESET)              //SDA����
 
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
char IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t);
unsigned char IIC_Read_Byte(uint8_t);

#endif
















