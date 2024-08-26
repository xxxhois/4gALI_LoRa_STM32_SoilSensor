/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����           */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ��24c02���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __24C02_H
#define __24C02_H

#define M24C02_WADDR   0xA0                               //д24c02�ĵ�ַ
#define M24C02_RADDR   0xA1                               //��24c02�ĵ�ַ


uint8_t M24C02_ReadOneByte(uint8_t );                     //������������24c02ָ���ĵ�ַ��һ�ֽ�����
void M24C02_WriteOneByte(uint8_t ,uint8_t );              //����������ָ����ַд��һ������ 
void M24C02_ReadSequential(uint8_t,uint8_t *,uint16_t);   //������������ָ���ĵ�ַ��ָ����������
void EEprom_ReadData(uint8_t ,void *, uint16_t);          //������������EEpromָ���ĵ�ַ������
void EEprom_WriteData(uint8_t, void *, uint16_t);         //������������EEpromָ���ĵ�ַд����
void EEprom_ReadInfo(void);                               //������������EEprom��ȡ����֤�������Ϣ

#endif
















