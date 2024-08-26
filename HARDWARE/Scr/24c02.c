/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ��24c02���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "main.h"             //������Ҫ��ͷ�ļ�
#include "24c02.h"            //������Ҫ��ͷ�ļ�
#include "clock.h" 	          //������Ҫ��ͷ�ļ�			 
#include "iic.h"              //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�
#include "mqtt.h"             //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*����������24c02ָ���ĵ�ַ��һ�ֽ�����            */
/*��  ����ReadAddr:��Ҫ�����ݵĵ�ַ                */
/*����ֵ������������                               */
/*-------------------------------------------------*/
uint8_t M24C02_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t receive;               //����һ���������ڱ�����յ�������
		
	IIC_Start();                   //IIC��ʼ�ź�
	IIC_Send_Byte(M24C02_WADDR);   //����24c02������ַ+д����	   	
	IIC_Wait_Ack();                //�ȴ�Ӧ��
    IIC_Send_Byte(ReadAddr);       //������Ҫ�����ݵĵ�ַ 
	IIC_Wait_Ack();	               //�ȴ�Ӧ�� 
	IIC_Start();  	 	           //IIC��ʼ�ź�
	IIC_Send_Byte(M24C02_RADDR);   //����24c02������ַ+������				   
	IIC_Wait_Ack();	               //�ȴ�Ӧ��
    receive=IIC_Read_Byte(0);	   //��һ�ֽ����ݣ�������receive������   
    IIC_Stop();                    //ֹͣ�ź�
	HAL_Delay(5);                   //�ӳ�
	return receive;                //���ض�ȡ������
}
/*-------------------------------------------------*/
/*��������ָ����ַд��һ������                     */
/*��  ����WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ           */
/*��  ����DataToWrite:Ҫд�������                 */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void M24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();                    //��ʼ�ź�
	IIC_Send_Byte(M24C02_WADDR);    //����������ַ+д����	 
	IIC_Wait_Ack();	                //�ȴ�Ӧ��
    IIC_Send_Byte(WriteAddr);       //����д�����ݵ�Ŀ�ĵ�ַ
	IIC_Wait_Ack(); 	 	        //�ȴ�Ӧ��									  		   
	IIC_Send_Byte(DataToWrite);     //����Ҫд�������							   
	IIC_Wait_Ack();  		        //�ȴ�Ӧ��	   
    IIC_Stop();                     //ֹͣ�ź�  
	HAL_Delay(5);                    //�ӳ�
}
/*-------------------------------------------------*/
/*����������ָ���ĵ�ַ��ָ����������               */
/*��  ����ReadAddr:��Ҫ�����ݵĵ�ַ                */
/*��  ����Redata:�������ݵĻ�����                  */
/*��  ����Redatalen:����������                     */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void M24C02_ReadSequential(uint8_t ReadAddr,uint8_t *Redata,uint16_t Redatalen)
{				  		
	uint16_t i;                                //����һ����������forѭ��   
	
	IIC_Start();                               //IIC��ʼ�ź�
	IIC_Send_Byte(M24C02_WADDR);               //����������ַ+д����	   	
	IIC_Wait_Ack();                            //�ȴ�Ӧ��
    IIC_Send_Byte(ReadAddr);                   //������Ҫ�����ݵĵ�ַ 
	IIC_Wait_Ack();	                           //�ȴ�Ӧ�� 
	IIC_Start();  	 	                       //IIC��ʼ�ź�
	IIC_Send_Byte(M24C02_RADDR);               //����������ַ+������				   
	IIC_Wait_Ack();	                           //�ȴ�Ӧ��
	for(i=0;i<(Redatalen-1);i++){              //ѭ����ȡǰRedatalen-1���ֽ�	
		Redata[i]=IIC_Read_Byte(1);            //��һ�ֽ����ݣ�����ACK�ź� 
    }	
	Redata[Redatalen-1]=IIC_Read_Byte(0);      //�����һ�ֽ����ݣ�������ACK�ź� 
    IIC_Stop();                                //ֹͣ�ź�
	HAL_Delay(5);                               //�ӳ�
}
/*-------------------------------------------------*/
/*����������EEpromָ���ĵ�ַ������                 */
/*��  ����addr:��Ҫ�����ݵĵ�ַ                    */
/*��  ����data:�������ݻ�����                      */
/*��  ����len:Ҫ��ȡ�����ݳ���                     */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void EEprom_ReadData(uint8_t addr,void *data, uint16_t len)
{				    
    uint8_t *ptr;                                 //����һ��ָ�����ڶ�ȡ����
	
	ptr = data;                                   //ptrָ����Ҫ��ȡ���ݵĻ�����
	M24C02_ReadSequential(addr,ptr,len);          //��ȡ����
}
/*-------------------------------------------------*/
/*��������EEpromָ����ַд��ָ���ֽڵ�����         */
/*��  ����addr����ַ                               */
/*��  ����data��д�������                         */
/*��  ����len��д���������                        */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void EEprom_WriteData(uint8_t addr, void *data, uint16_t len)
{
	uint16_t i;                                    //����forѭ��
	uint8_t *ptr;                                  //����һ��ָ������д������
	  
	ptr = data;                                    //ptrָ����Ҫд�����ݵĻ�����
	for(i=0;i<len;i++){                            //ѭ��д������
		M24C02_WriteOneByte(addr,ptr[i]);          //дһ�ֽ�����
		addr++;                                    //��ַ+1
	}
}
/*-------------------------------------------------*/
/*����������EEprom��ȡ����֤�������Ϣ             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void EEprom_ReadInfo(void)
{				 
    memset(&AliInfoCB,0,ALIINFO_STRUCT_LEN);                              //��սṹ��	
	EEprom_ReadData(0,&AliInfoCB,ALIINFO_STRUCT_LEN);                     //�ӵ�ַ0��ʼ����ȡ���������
	if(AliInfoCB.valid_cer != VALID_ALICER_DATA){                         //��������� ˵��֤����Ϣ��Ч����Ҫ��������
		u1_printf("֤����Ϣ��Ч ����BootLoader����������\r\n");           //����1���  ��ʾ��Ϣ
        NVIC_SystemReset();                                               //����
	}else{                                                                //��֮ ˵��֤����Ϣ��Ч ����else
		IoT_Parameter_Init();                                             //�����Ƴ�ʼ���������õ��ͻ���ID���û���������
		u1_printf("--------------------------�������豸֤����Ϣ-------------------------\r\n");          //����1�����Ϣ
		u1_printf("ProductKey��%s\r\n",AliInfoCB.ProductKeyBuff);                                        //����1�����Ϣ
		u1_printf("DeviceName��%s\r\n",AliInfoCB.DeviceNameBuff);                                        //����1�����Ϣ
		u1_printf("DeviceSecret��%s\r\n",AliInfoCB.DeviceSecretBuff);                                    //����1�����Ϣ
		u1_printf("�̼��汾��%s\r\n",AliInfoCB.Version_ABuff);                                           //����1�����Ϣ		
		u1_printf("�ͻ���ID��%s\r\n",Aliyun_mqtt.ClientID);                                              //����1�����Ϣ
		u1_printf("�� �� ����%s\r\n",Aliyun_mqtt.Username);               							     //����1�����Ϣ
		u1_printf("��    �룺%s\r\n",Aliyun_mqtt.Passward);              							     //����1�����Ϣ
		u1_printf("�� �� ����%s:%d\r\n",Aliyun_mqtt.ServerIP,Aliyun_mqtt.ServerPort);                    //����1�����Ϣ
		u1_printf("---------------------------------------------------------------------\r\n\r\n");      //����1�����Ϣ					
	}
}

