/*-------------------------------------------------*/
/*           超子说物联网STM32系列开发板           */
/*-------------------------------------------------*/
/*                                                 */
/*          实现各个SPI接口功能的头文件            */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __SPI_H
#define __SPI_H

extern SPI_HandleTypeDef SPI2_Handler;  //SPI2句柄

void SPI2_Init(void);                          //函数声明，初始化SPI2接口
uint8_t SPI2_ReadWriteByte(uint8_t);           //函数声明，SPI接口读写数据
void SPI2_WriteByte(uint8_t);                         //函数声明，SPI2接口写数据
uint8_t SPI2_ReadByte(void);                          //函数声明，SPI2接口读数据
void SPI2_WriteRead(uint8_t *, uint8_t *, uint16_t);  //函数声明, SPI2接口读写指定数量数据  
void SPI2_Write(uint8_t *, uint16_t);                 //函数声明, SPI2接口写指定数量数据  
void SPI2_Read(uint8_t *, uint16_t);                  //函数声明, SPI2接口读指定数量数据  

#endif


