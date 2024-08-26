/*-------------------------------------------------*/
/*           ����˵������STM32ϵ�п�����           */
/*-------------------------------------------------*/
/*                                                 */
/*          ��дW25Qxx�洢��оƬ���ܵ�ͷ�ļ�      */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __W25QXX_H
#define __W25QXX_H

#define CS_DISENABLE        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);    //CS��Ч�ź�
#define CS_ENABLE           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  //CS��Ч�ź�

#define W25QxxID	            0XEF16  //W25Qxx ID
#define W25Qxx_WriteEnable		0x06    //дʹ��
#define W25Qxx_WriteDisable     0x04    //д��ֹ
#define W25Qxx_ReadStatusReg1	0x05    //��״̬�Ĵ���1ָ��
#define W25Qxx_SectorErase		0x20    //������4KB������ָ��
#define W25Qxx_BlockErase	    0xD8    //�飨64KB������ָ��
#define W25Qxx_PageProgram		0x02    //ҳ��256B��д��ָ��
#define W25Qxx_ReadData	        0x03    //������ָ��
#define W25Qxx_Powerdown        0xB9    //Powerdownָ��

uint8_t W25Qxx_Init(void);                     //������������ʼ��W25Qxx
void W25Qxx_Write_Enable(void);                //����������W25Qxx��дʹ��
void W25QXX_Write_Disable(void);                //����������W25Qxx��д��ֹ
uint16_t W25Qxx_ReadID(void);                  //������������ȡW25QxxоƬID
void W25Qxx_Wait_Busy(void);                   //����������W25Qxx�ȴ�����
void W25Qxx_Erase_Sector(uint16_t);            //����������W25Qxx����һ��������4KB��
void W25Qxx_Erase_Block(uint8_t);              //����������W25Qxx����һ���飨64KB��
void W25Qxx_Write_Page(uint8_t*, uint16_t);    //����������W25Qxxд��һҳ���ݣ�256�ֽڣ�
void W25Qxx_Read(uint8_t*,uint32_t,uint32_t);  //����������W25Qxx��ȡ����
	
#endif


