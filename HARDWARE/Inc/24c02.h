/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板           */
/*-------------------------------------------------*/
/*                                                 */
/*             实现24c02功能的头文件               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __24C02_H
#define __24C02_H

#define M24C02_WADDR   0xA0                               //写24c02的地址
#define M24C02_RADDR   0xA1                               //读24c02的地址


uint8_t M24C02_ReadOneByte(uint8_t );                     //函数声明，从24c02指定的地址读一字节数据
void M24C02_WriteOneByte(uint8_t ,uint8_t );              //函数声明，指定地址写入一个数据 
void M24C02_ReadSequential(uint8_t,uint8_t *,uint16_t);   //函数声明，从指定的地址读指定数量数据
void EEprom_ReadData(uint8_t ,void *, uint16_t);          //函数声明，从EEprom指定的地址读数据
void EEprom_WriteData(uint8_t, void *, uint16_t);         //函数声明，从EEprom指定的地址写数据
void EEprom_ReadInfo(void);                               //函数声明，从EEprom读取所有证书参数信息

#endif
















