/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*            实现IIc总线功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __IIC_H
#define __IIC_H

/*--------------------------------------------------------------------------*/
/*                      控制LED的IO引脚 和 时钟使能                         */
/*                        可以根据需要修改和增减                            */
/*--------------------------------------------------------------------------*/
#define SDA_GROUP             GPIOA                                               //SDA 对应的IO分组
#define SDA_PIN               GPIO_PIN_12                                          //SDA 对应的IO
#define SDA_GROUP_CLK_ENABLE  __HAL_RCC_GPIOA_CLK_ENABLE();                       //SDA 对应的IO分组 时钟使能

#define SCL_GROUP             GPIOA                                               //SCL 对应的IO分组
#define SCL_PIN               GPIO_PIN_11                                          //SCL 对应的IO
#define SCL_GROUP_CLK_ENABLE  __HAL_RCC_GPIOA_CLK_ENABLE();                       //SCL 对应的IO分组 时钟使能
/*--------------------------------------------------------------------------*/
  		   

#define READ_SDA        HAL_GPIO_ReadPin(SDA_GROUP, SDA_PIN)                               //读取SDA电平 
#define IIC_SDA_OUT(x)  HAL_GPIO_WritePin(SDA_GROUP, SDA_PIN, (GPIO_PinState)x)            //设置SDA电平 

#define	IIC_SCL_H       HAL_GPIO_WritePin(SCL_GROUP, SCL_PIN, GPIO_PIN_SET)                //SCL拉高
#define	IIC_SDA_H       HAL_GPIO_WritePin(SDA_GROUP, SDA_PIN, GPIO_PIN_SET)                //SDA拉高
 
#define	IIC_SCL_L       HAL_GPIO_WritePin(SCL_GROUP, SCL_PIN, GPIO_PIN_RESET)              //SCL拉低
#define	IIC_SDA_L       HAL_GPIO_WritePin(SDA_GROUP, SDA_PIN, GPIO_PIN_RESET)              //SDA拉低
 
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
char IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t);
unsigned char IIC_Read_Byte(uint8_t);

#endif
















