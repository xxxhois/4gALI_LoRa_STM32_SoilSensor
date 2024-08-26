/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*         ��дW25Qxx�洢��оƬ���ܵ�Դ�ļ�       */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "w25qxx.h"          //������Ҫ��ͷ�ļ�
#include "spi.h"              //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*����������ʼ��W25Qxx                            */
/*��  ������                                       */
/*����ֵ��0����ȷ                                  */
/*-------------------------------------------------*/
uint8_t W25Qxx_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();                             //ʹ�ܶ˿�Dʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_12;                              //����PD0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;                    //�������
    GPIO_Initure.Pull=GPIO_PULLUP;                            //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;                  //����ģʽ
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);                       //����	
    CS_DISENABLE;                                             //CS��Ч�ź�	
	
	SPI2_Init();                                              //��ʼ��SPI2�ӿ�
    if(W25Qxx_ReadID()==W25QxxID){                            //��ȡID����ȷ����if
		W25Qxx_Write_Enable();                                //дʹ��
	}else return 1;                                           //��ȡID���󣬷���1
	
	return 0;                                                 //��ȷ ����0
}
/*-------------------------------------------------*/
/*��������W25Qxx��дʹ��                          */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W25Qxx_Write_Enable(void)   
{
	W25Qxx_Wait_Busy();                      //�ȴ�����
	CS_ENABLE;                               //CS��Ч�ź�
    SPI2_WriteByte(W25Qxx_WriteEnable);      //����дʹ��  
	CS_DISENABLE;                            //CS��Ч�ź�	    	      
}
/*-------------------------------------------------*/
/*��������W25Qxx��д��ֹ                          */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W25QXX_Write_Disable(void)
{
	W25Qxx_Wait_Busy();                     //�ȴ�����
	CS_ENABLE;                              //CS��Ч�ź�
    SPI2_WriteByte(W25Qxx_WriteDisable);    //����д��ֹ 
	CS_DISENABLE;                           //CS��Ч�ź�
}
/*-------------------------------------------------*/
/*����������ȡW25QxxоƬID                        */
/*��  ������                                       */
/*����ֵ��0XEF17 ��W25Qxx��ID                     */
/*-------------------------------------------------*/
uint16_t W25Qxx_ReadID(void)
{  
	uint8_t wdata[6] = {0x90,0x00,0x00,0x00,0x00,0x00};             //��ȡIDָ������
	uint8_t rdata[6];                                               //�������ݻ�����
	
	W25Qxx_Wait_Busy();                                             //�ȴ�����
	CS_ENABLE;                                                      //CS��Ч�ź�				    
	SPI2_WriteRead(wdata,rdata,6);                                  //���Ͳ���������
	CS_DISENABLE;                                                   //CS��Ч�ź�				    
	return (rdata[4]*256 + rdata[5]);                               //����ID                           
}  
/*-------------------------------------------------*/
/*��������W25Qxx�ȴ�����                          */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W25Qxx_Wait_Busy(void)   
{   
	uint8_t res;
	
	do{
		CS_ENABLE;                                  //CS��Ч�ź�				    
		SPI2_WriteByte(W25Qxx_ReadStatusReg1);      //���Ͷ�ȡ״̬�Ĵ���1����	    
		res = SPI2_ReadByte(); 	                    //��ȡ״̬�Ĵ���1    
		CS_DISENABLE;                               //CS��Ч�ź�
	}while((res&0x01)==0x01);                       //�ȴ�BUSYλ��գ���ʾ����
} 
/*-------------------------------------------------*/
/*��������W25Qxx����һ��������4KB��               */
/*��  ����NBsector:�ڼ������� ��0~4095��           */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W25Qxx_Erase_Sector(uint16_t NBsector)   
{  
    uint8_t wdata[4];                                    //ָ�����ݻ�����
	
	wdata[0] = W25Qxx_SectorErase;                       //��������ָ�� 
	wdata[1] = (NBsector*4096)>>16;                      //A23~A16��ַλ    
	wdata[2] = (NBsector*4096)>>8;                       //A15~A8��ַλ   
	wdata[3] = (NBsector*4096)>>0;                       //A7~A0��ַλ
	
	W25Qxx_Write_Enable();                               //дʹ��   
	W25Qxx_Wait_Busy();                                  //�ȴ�����
	CS_ENABLE;                                           //CS��Ч�ź�	  
	SPI2_Write(wdata,4);                                 //��������    
	CS_DISENABLE;                                        //CS��Ч�ź�	
	W25Qxx_Wait_Busy();                                  //�ȴ��������	
}
/*-------------------------------------------------*/
/*��������W25Qxx����һ���飨64KB��                */
/*��  ����NBsector:�ڼ����� ��0~255��              */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W25Qxx_Erase_Block(uint8_t NBBlock)   
{  
    uint8_t wdata[4];                                    //ָ�����ݻ�����
	
	wdata[0] = W25Qxx_BlockErase;                        //�����ָ�� 
	wdata[1] = (NBBlock*16*4096)>>16;                    //A23~A16��ַλ    
	wdata[2] = (NBBlock*16*4096)>>8;                     //A15~A8��ַλ   
	wdata[3] = (NBBlock*16*4096)>>0;                     //A7~A0��ַλ
	
	W25Qxx_Write_Enable();                               //дʹ��   
	W25Qxx_Wait_Busy();                                  //�ȴ�����
	CS_ENABLE;                                           //CS��Ч�ź�	  
    SPI2_Write(wdata,4);                                 //�������� 
	CS_DISENABLE;                                        //CS��Ч�ź�
    W25Qxx_Wait_Busy();                                  //�ȴ��������	
}
/*-------------------------------------------------*/
/*��������W25Qxxд��һҳ���ݣ�256�ֽڣ�           */
/*��  ����wdata:��д������                         */
/*��  ����NBPage:�ڼ���ҳ��0~65535��               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W25Qxx_Write_Page(uint8_t* wbuff, uint16_t NBPage)
{   	
	uint8_t wdata[4];                                        //ָ�����ݻ�����
	
	wdata[0] = W25Qxx_PageProgram;                           //ҳ��256B��д��ָ��
	wdata[1] = (NBPage*256)>>16;                             //A23~A16��ַλ    
	wdata[2] = (NBPage*256)>>8;                              //A15~A8��ַλ   
	wdata[3] = (NBPage*256)>>0;                              //A7~A0��ַλ
   
	W25Qxx_Write_Enable();                                   //дʹ��
	W25Qxx_Wait_Busy();                                      //�ȴ�����
	CS_ENABLE;                                               //CS��Ч�ź�	  
    SPI2_Write(wdata,4);                                     //�������� 
	SPI2_Write(wbuff,256);                                   //��������       
	CS_DISENABLE;                                            //CS��Ч�ź�	     	         
} 
/*-------------------------------------------------*/
/*��������W25Qxx��ȡ����                          */
/*��  ����rdata:������ݻ�����                     */
/*��  ����rAddr:��ȡ�Ŀ�ʼ��ַ                     */
/*��  ����len:��ȡ�����ֽ�����                     */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void W25Qxx_Read(uint8_t* rdata,uint32_t rAddr,uint32_t len)   
{ 
    uint8_t wdata[4];                                     //ָ�����ݻ�����
	
	wdata[0] = W25Qxx_ReadData;                           //������ָ��
	wdata[1] = rAddr>>16;                                 //A23~A16��ַλ    
	wdata[2] = rAddr>>8;                                  //A15~A8��ַλ   
	wdata[3] = rAddr>>0;                                  //A7~A0��ַλ
	
	W25Qxx_Wait_Busy();                                   //�ȴ�����
	CS_ENABLE;                                            //CS��Ч�ź�	  
    SPI2_Write(wdata,4);                                  //�������� 
	SPI2_Read(rdata,len);                                 //��������            
	CS_DISENABLE;                                         //CS��Ч�ź�	     	      
}
