/*-------------------------------------------------*/
/*           ����˵������STM32ϵ�п�����           */
/*-------------------------------------------------*/
/*                                                 */
/*          ʵ�ָ���SPI�ӿڹ��ܵ�ͷ�ļ�            */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __SPI_H
#define __SPI_H

extern SPI_HandleTypeDef SPI2_Handler;  //SPI2���

void SPI2_Init(void);                          //������������ʼ��SPI2�ӿ�
uint8_t SPI2_ReadWriteByte(uint8_t);           //����������SPI�ӿڶ�д����
void SPI2_WriteByte(uint8_t);                         //����������SPI2�ӿ�д����
uint8_t SPI2_ReadByte(void);                          //����������SPI2�ӿڶ�����
void SPI2_WriteRead(uint8_t *, uint8_t *, uint16_t);  //��������, SPI2�ӿڶ�дָ����������  
void SPI2_Write(uint8_t *, uint16_t);                 //��������, SPI2�ӿ�дָ����������  
void SPI2_Read(uint8_t *, uint16_t);                  //��������, SPI2�ӿڶ�ָ����������  

#endif


