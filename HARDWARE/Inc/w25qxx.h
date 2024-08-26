/*-------------------------------------------------*/
/*           超子说物联网STM32系列开发板           */
/*-------------------------------------------------*/
/*                                                 */
/*          读写W25Qxx存储器芯片功能的头文件      */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __W25QXX_H
#define __W25QXX_H

#define CS_DISENABLE        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);    //CS无效信号
#define CS_ENABLE           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  //CS有效信号

#define W25QxxID	            0XEF16  //W25Qxx ID
#define W25Qxx_WriteEnable		0x06    //写使能
#define W25Qxx_WriteDisable     0x04    //写禁止
#define W25Qxx_ReadStatusReg1	0x05    //读状态寄存器1指令
#define W25Qxx_SectorErase		0x20    //扇区（4KB）擦除指令
#define W25Qxx_BlockErase	    0xD8    //块（64KB）擦除指令
#define W25Qxx_PageProgram		0x02    //页（256B）写入指令
#define W25Qxx_ReadData	        0x03    //读数据指令
#define W25Qxx_Powerdown        0xB9    //Powerdown指令

uint8_t W25Qxx_Init(void);                     //函数声明，初始化W25Qxx
void W25Qxx_Write_Enable(void);                //函数声明，W25Qxx擦写使能
void W25QXX_Write_Disable(void);                //函数声明，W25Qxx擦写禁止
uint16_t W25Qxx_ReadID(void);                  //函数声明，读取W25Qxx芯片ID
void W25Qxx_Wait_Busy(void);                   //函数声明，W25Qxx等待空闲
void W25Qxx_Erase_Sector(uint16_t);            //函数声明，W25Qxx擦除一个扇区（4KB）
void W25Qxx_Erase_Block(uint8_t);              //函数声明，W25Qxx擦除一个块（64KB）
void W25Qxx_Write_Page(uint8_t*, uint16_t);    //函数声明，W25Qxx写入一页数据（256字节）
void W25Qxx_Read(uint8_t*,uint32_t,uint32_t);  //函数声明，W25Qxx读取数据
	
#endif


