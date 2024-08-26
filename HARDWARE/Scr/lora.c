/*-------------------------------------------------*/
/*           ����˵������STM32ϵ�п�����           */
/*-------------------------------------------------*/
/*                                                 */
/*            ����LoRaģ�鹦�ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "main.h"             //������Ҫ��ͷ�ļ�
#include "clock.h"            //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�
#include "lora.h"	          //������Ҫ��ͷ�ļ�
#include "clock.h"	          //������Ҫ��ͷ�ļ�
#include "utils_hmac.h"       //������Ҫ��ͷ�ļ�
#include "cat1.h"	          //������Ҫ��ͷ�ļ�
#include "mqtt.h"             //������Ҫ��ͷ�ļ�
#include "w25qxx.h"           //������Ҫ��ͷ�ļ�

LoRaParameter LoRaSetData = {            //ģ�鹤������
	0x00,                                //ģ���ַ���ֽ�
	0x00,                                //ģ���ַ���ֽ�
	0x01,                                //ģ�������ַ
	LoRa_9600,                           //ģ�鴮�� ������ 9600
	LoRa_8N1,                            //���ڹ���ģʽ 8����λ ��У�� 1ֹͣλ	
	LoRa_38_4,                           //ģ��������� 38.4K
	LoRa_Data240,                        //���ݷְ���С 240�ֽ�
	LoRa_RssiDIS,                        //�ر�RSSI����
	LoRa_FEC_22DBM,                      //���书��22dbm
	LoRa_CH23,                           //ģ���ŵ�
	LoRa_RssiByteDIS,                    //����RSSI�ֽڹ���
	LoRa_ModePOINT,                      //����ģʽ
	LoRa_RelayDIS,                       //�����м�ģʽ
	LoRa_LBTEN,                          //ʹ��LBT
	LoRa_WorTX,                          //Worģʽ����        ֻ��ģʽ1����Ч
	LoRa_Wor2000ms,                      //Wor����2000����    ֻ��ģʽ1����Ч
	0x22,                                //ģ�������Կ���ֽ�
	0x33,                                //ģ�������Կ���ֽ�	
};

/*-------------------------------------------------*/
/*����������ʼ��LoRaģ���IO                       */
/*��  ����timeout�� ��ʱʱ��                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LoRa_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_Initure;                //GPIO�˿����ñ���
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			      //ʹ��GPIOBʱ��
	__HAL_RCC_GPIOF_CLK_ENABLE();			      //ʹ��GPIOFʱ��
	
	GPIO_Initure.Pin = GPIO_PIN_7;                //׼������PB7
	GPIO_Initure.Mode = GPIO_MODE_INPUT;          //���빦��
	GPIO_Initure.Pull = GPIO_PULLUP;              //����
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);          //����
	
	GPIO_Initure.Pin = GPIO_PIN_1;                //׼������PF1
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;      //�������
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);          //����	
	
	GPIO_Initure.Pin = GPIO_PIN_5 | GPIO_PIN_6;   //׼������PB5 6
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;      //�������
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);          //����	
	
	LoRa_MODE2;                                   //LoRa����ģʽ2	
	HAL_Delay(200);                               //�ʵ���ʱ
}
/*-------------------------------------------------*/
/*��������LoRa�ϵ縴λ                             */
/*��  ����timeout����ʱʱ��                        */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
uint8_t LoRa_Reset(int16_t timeout)
{
    LoRa_PowerOFF;                  //�ȹرյ�Դ
	HAL_Delay(200);                 //��ʱ
	LoRa_PowerON;                   //�ٴ򿪵�Դ
	
	while(timeout--)                //�ȴ���λ�ɹ�
	{                           
		HAL_Delay(100);             //��ʱ100ms
		if(LoRa_AUX==1)             //��Ҫ�ȵ��ߵ�ƽ1����������while����ʾ��λ���
			break;       			//��������whileѭ��
		u1_printf("%d ",timeout);   //����������ڵĳ�ʱʱ��		
	} 
	u1_printf("\r\n");              //���������Ϣ
	if(timeout<=0)return 1;         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû�ܵȵ�LoRa_AUX�ߵ�ƽ������1
	return 0;                       //��ȷ������0
}
/*-------------------------------------------------*/
/*����������ʼ��ģ��                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LoRa_Init(void)
{
	LoRa_GPIO_Init();                                 //Loraģ�����ų�ʼ��
	u1_printf("׼����λLoraģ��\r\n");   	          //���������Ϣ
	if(LoRa_Reset(100)){                              //��λLoraģ�飬����ֵ������㣬��ʾ����
		u1_printf("��λLoraģ��ʧ�ܣ�����\r\n");   	  //���������Ϣ
		NVIC_SystemReset();                           //����
	}else u1_printf("Loraģ�鸴λ�ɹ�\r\n");   	      //���������Ϣ	
	u1_printf("׼������Loraģ��\r\n");   	          //���������Ϣ
	LoRa_Set();                                       //����Loraģ��
	HAL_Delay(200);                                   //��ʱ		
}
/*-------------------------------------------------*/
/*��������LoRa���ù�������                         */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LoRa_Set(void)
{
	uint8_t cmd[12];                                         //�������ò��������黺�����������12�ֽ�
	 
	cmd[0] = 0xC0;                                           //���ò���ָ�����ʼ�ֽڣ��̶�ֵ0xC0
	cmd[1] = 0x00;                                           //���ò�������ʼ�Ĵ�����ַ���ӼĴ���0��ʼ���ã�һ��9��
	cmd[2] = 0x09;                                           //һ������9���Ĵ���
	cmd[3] = LoRaSetData.LoRa_AddrH;                         //����ģ���ַ���ֽ�
	cmd[4] = LoRaSetData.LoRa_AddrL;                         //����ģ���ַ���ֽ�
	cmd[5] = LoRaSetData.LoRa_NetID;                         //����ģ�������ַ	
	cmd[6] = LoRaSetData.LoRa_Baudrate | LoRaSetData.LoRa_UartMode | LoRaSetData.LoRa_airvelocity;    //����ģ�� ������ ����ģʽ ��������
	cmd[7] = LoRaSetData.LoRa_DataLen | LoRaSetData.LoRa_Rssi | LoRaSetData.LoRa_TxPower;             //����ģ�� ���ݷְ����� RSSI�����Ƿ�ʹ�� ���书��
	cmd[8] = LoRaSetData.LoRa_CH;                            //����ģ���ŵ�	
	cmd[9] = LoRaSetData.LoRa_RssiByte | LoRaSetData.LoRa_DateMode | LoRaSetData.LoRa_Relay | LoRaSetData.LoRa_LBT | LoRaSetData.LoRa_WORmode | LoRaSetData.LoRa_WORcycle; //����ģ�� �Ƿ�ʹ��RSSI�ֽڹ��� ���䷽ʽ �Ƿ��м� �Ƿ�ʹ��LBT WORģʽ�շ����� WOR����
	cmd[10] = LoRaSetData.LoRa_KeyH;                         //����ģ�������Կ���ֽ�
	cmd[11] = LoRaSetData.LoRa_KeyL;                         //����ģ�������Կ���ֽ�
    u3_TxDataBuf(cmd,12);                                    //���ͻ�������������	
}
/*-------------------------------------------------*/
/*��������LoRa��ѯ��������                         */
/*��  ����data��������������                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LoRa_Get(uint8_t *data)
{
	uint8_t ch;                                                          //�����ŵ��ı���
	 		         			           	                         
	u1_printf("ģ���ַ:0x%02X%02X\r\n",data[3],data[4]);                //���������Ϣ
	u1_printf("ģ������ID:0x%02X\r\n",data[5]);                          //���������Ϣ
	
	switch(data[6]&0xE0){                                                //�жϴ��ڲ�����
		case LoRa_1200 : u1_printf("������ 1200\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_2400 : u1_printf("������ 2400\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_4800 : u1_printf("������ 4800\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_9600 : u1_printf("������ 9600\r\n");                   //���������Ϣ
						 break;                                          //����
		case LoRa_19200 : u1_printf("������ 19200\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_38400 : u1_printf("������ 38400\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_57600 : u1_printf("������ 57600\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_115200: u1_printf("������ 115200\r\n");                //���������Ϣ
						 break;                                          //����
	}	
	switch(data[6]&0x18){                                                //�жϴ��ڲ���
		case LoRa_8N1 :	 u1_printf("8����λ ��У�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		case LoRa_8O1 :	 u1_printf("8����λ ��У�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
		case LoRa_8E1 :	 u1_printf("8����λ żУ�� 1ֹͣλ\r\n");        //���������Ϣ
						 break;                                          //����
	}		
	switch(data[6]&0x07){                                                //�жϿ�������
		case LoRa_0_3 :  u1_printf("�������� 0.3K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_1_2 :  u1_printf("�������� 1.2K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_2_4 :  u1_printf("�������� 2.4K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_4_8 :  u1_printf("�������� 4.8K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_9_6 :  u1_printf("�������� 9.6K\r\n");                 //���������Ϣ
						 break;                                          //����
		case LoRa_19_2 : u1_printf("�������� 19.2K\r\n");                //���������Ϣ
						 break;                                          //����     
		case LoRa_38_4 : u1_printf("�������� 38.4K\r\n");                //���������Ϣ
						 break;  		                                 //����
		case LoRa_62_5 : u1_printf("�������� 62.5K\r\n");                //���������Ϣ
						 break;  		                                 //����
	}	
	switch(data[7]&0xC0){                                                //�ж����ݷְ���С
		case LoRa_Data240 : u1_printf("���ݷְ���С��240�ֽ�\r\n");      //���������Ϣ
							break;                                       //����
		case LoRa_Data128 : u1_printf("���ݷְ���С��128�ֽ�\r\n");      //���������Ϣ
							break;                                       //����
		case LoRa_Data64  : u1_printf("���ݷְ���С��64�ֽ�\r\n");       //���������Ϣ
							break;                                       //����
		case LoRa_Data32  : u1_printf("���ݷְ���С��32�ֽ�\r\n");       //���������Ϣ
							break;                                       //����
	}	
	switch(data[7]&0x20){                                                //�ж��Ƿ�����RSSI����
		case LoRa_RssiEN  :	u1_printf("����RSSI����\r\n");               //���������Ϣ
							break;                                       //����
		case LoRa_RssiDIS :	u1_printf("����RSSI����\r\n");               //���������Ϣ
							break;                                       //����
	}		
	switch(data[7]&0x03){                                                //�жϷ��书��
		case LoRa_FEC_22DBM :  u1_printf("���书�� 22dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_17DBM :  u1_printf("���书�� 17dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_13DBM :  u1_printf("���书�� 13dbm\r\n");          //���������Ϣ
							   break;                                    //����
		case LoRa_FEC_10DBM :  u1_printf("���书�� 10dbm\r\n");          //���������Ϣ
							   break;                                    //����
	}
	
	ch = data[8] & 0x7F;                                                 //�����ŵ�
	u1_printf("�ŵ�:0x%02X  ��ӦƵ��%dMHz\r\n",ch,410+ch);               //���������Ϣ
	
	switch(data[9]&0x80){                                                //�ж�RSSI�ֽڹ���
		case LoRa_RssiByteEN  : u1_printf("����RSSI�ֽڹ���\r\n");       //���������Ϣ
								break;                                   //����
		case LoRa_RssiByteDIS : u1_printf("����RSSI�ֽڹ���\r\n");       //���������Ϣ
								break;                                   //����
	}
	switch(data[9]&0x40){                                                //�жϴ���ģʽ
		case LoRa_ModeTRANS :  u1_printf("͸������\r\n");                //���������Ϣ
							   break;                                    //����
		case LoRa_ModePOINT :  u1_printf("���㴫��\r\n");                //���������Ϣ
							   break;                                    //����
	}
	switch(data[9]&0x20){                                               //�ж��м̹���
		case LoRa_RelayEN  :  u1_printf("�����м�\r\n");                //���������Ϣ
							  break;                                    //����
		case LoRa_RelayDIS :  u1_printf("�����м�\r\n");                //���������Ϣ
							  break;                                    //����
	}
	switch(data[9]&0x10){                                               //�ж�LBT����
		case LoRa_LBTEN  :  u1_printf("����LBT\r\n");                   //���������Ϣ
							  break;                                    //����
		case LoRa_LBTDIS :  u1_printf("����LBT\r\n");                   //���������Ϣ
							  break;                                    //����
	}
//	switch(data[9]&0x08){                                               //�ж�WORģʽ
//		case LoRa_WorTX  :  u1_printf("Worģʽ����\r\n");               //���������Ϣ
//							  break;                                    //����
//		case LoRa_WorRX :  u1_printf("Worģʽ����\r\n");                //���������Ϣ
//							  break;                                    //����
//	}
//	switch(data[9]&0x07){                                               //�ж�WOR����
//		case LoRa_Wor500ms  : u1_printf("WOR����ʱ�� 500����\r\n");     //���������Ϣ
//							  break;                                    //����
//		case LoRa_Wor1000ms : u1_printf("WOR����ʱ�� 1000����\r\n");    //���������Ϣ
//							  break;                                    //����
//		case LoRa_Wor1500ms : u1_printf("WOR����ʱ�� 1500����\r\n");    //���������Ϣ
//							  break;                                    //����
//		case LoRa_Wor2000ms : u1_printf("WOR����ʱ�� 2000����\r\n");    //���������Ϣ
//							  break;                                    //����
//		case LoRa_Wor2500ms : u1_printf("WOR����ʱ�� 2500����\r\n");    //���������Ϣ
//							  break;                                    //����
//		case LoRa_Wor3000ms : u1_printf("WOR����ʱ�� 3000����\r\n");    //���������Ϣ
//							  break;                                    //����
//		case LoRa_Wor3500ms : u1_printf("WOR����ʱ�� 3500����\r\n");    //���������Ϣ
//							  break;                                    //����
//		case LoRa_Wor4000ms : u1_printf("WOR����ʱ�� 4000����\r\n");    //���������Ϣ
//							  break;                                    //����
//	}
}
/*-------------------------------------------------*/
/*������������3�����¼�                            */
/*��  ����data ������                              */
/*��  ����datalen �����ݳ���                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
uint8_t tempdata[512];
uint8_t Sign[256];     
void U3PassiveEvent(uint8_t *data, uint16_t datalen)
{
	
	/*----------------------------------------------*/
	/*           ��������LoRaģ�鷵������           */
	/*----------------------------------------------*/
	if((data[0]==0xC1)&&(datalen==12)){                   
		u1_printf("��������LoRaģ�鷵������\r\n");        //���������Ϣ
		LoRa_Get(data);                                   //�������õĲ���
		LoRa_MODE0;                                       //�л���ģʽ0
		HAL_Delay(200);                                   //��ʱ
		CAT1_Reset();                                     //4Gģ������
    }	
	/*----------------------------------------------*/
	/*         ���յ����豸1�Ϸ���֤����Ϣ          */
	/*----------------------------------------------*/
	if((data[0]==VALID_ALICER1_DATA)&&(datalen==(ALIINFO_STRUCT_LEN+1))){
		u1_printf("���յ����豸1֤����Ϣ\r\n"); 
		memcpy(&AliInfoCB_SUB[0],&data[1],ALIINFO_STRUCT_LEN);
		memset(tempdata,0,512);                                                                                    //��ʱ������ȫ������
		memset(Sign,0,256);                                                                                        //��ʱ������ȫ������	
		sprintf((char *)tempdata,"clientId%s&%sdeviceName%sproductKey%s",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,AliInfoCB_SUB[0].DeviceNameBuff,AliInfoCB_SUB[0].ProductKeyBuff);     //��������ʱ������   
		utils_hmac_sha1((char *)tempdata,strlen((char *)tempdata),(char *)Sign,(char *)AliInfoCB_SUB[0].DeviceSecretBuff,DEVICESECRET_LEN);//��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������豸ǩ���������浽��������
		memset(tempdata,0,512);
		sprintf((char *)tempdata,"{\"id\": \"1\",\"params\": {\"productKey\": \"%s\",\"deviceName\": \"%s\",\"clientId\": \"%s&%s\",\"signMethod\": \"hmacSha1\",\"sign\": \"%s\",\"cleanSession\": \"true\"}}",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,Sign);  //�������豸1��������
		CAT1_SubDeviceLogin((char *)tempdata);                                                                     //�������豸�������ݱ���	
    }	
	/*----------------------------------------------*/
	/*         ���յ����豸2�Ϸ���֤����Ϣ           */
	/*----------------------------------------------*/
	if((data[0]==VALID_ALICER2_DATA)&&(datalen==(ALIINFO_STRUCT_LEN+1))){
		u1_printf("���յ����豸2֤����Ϣ\r\n"); 
		memcpy(&AliInfoCB_SUB[1],&data[1],ALIINFO_STRUCT_LEN);
		memset(tempdata,0,512);                                                                                    //��ʱ������ȫ������
		memset(Sign,0,256);                                                                                        //��ʱ������ȫ������	
		sprintf((char *)tempdata,"clientId%s&%sdeviceName%sproductKey%s",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,AliInfoCB_SUB[1].DeviceNameBuff,AliInfoCB_SUB[1].ProductKeyBuff);     //��������ʱ������   
		utils_hmac_sha1((char *)tempdata,strlen((char *)tempdata),(char *)Sign,(char *)AliInfoCB_SUB[1].DeviceSecretBuff,DEVICESECRET_LEN);//��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������豸ǩ���������浽��������
		memset(tempdata,0,512);
		sprintf((char *)tempdata,"{\"id\": \"1\",\"params\": {\"productKey\": \"%s\",\"deviceName\": \"%s\",\"clientId\": \"%s&%s\",\"signMethod\": \"hmacSha1\",\"sign\": \"%s\",\"cleanSession\": \"true\"}}",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,Sign);  //�������豸1��������
		CAT1_SubDeviceLogin((char *)tempdata);                                                                     //�������豸�������ݱ���	
    }
	/*----------------------------------------------*/
	/*         ���յ����豸3�Ϸ���֤����Ϣ           */
	/*----------------------------------------------*/
	if((data[0]==VALID_ALICER3_DATA)&&(datalen==(ALIINFO_STRUCT_LEN+1))){
		u1_printf("���յ����豸3֤����Ϣ\r\n"); 
		memcpy(&AliInfoCB_SUB[2],&data[1],ALIINFO_STRUCT_LEN);
		memset(tempdata,0,512);                                                                                    //��ʱ������ȫ������
		memset(Sign,0,256);                                                                                        //��ʱ������ȫ������	
		sprintf((char *)tempdata,"clientId%s&%sdeviceName%sproductKey%s",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,AliInfoCB_SUB[2].DeviceNameBuff,AliInfoCB_SUB[2].ProductKeyBuff);     //��������ʱ������   
		utils_hmac_sha1((char *)tempdata,strlen((char *)tempdata),(char *)Sign,(char *)AliInfoCB_SUB[2].DeviceSecretBuff,DEVICESECRET_LEN);//��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������豸ǩ���������浽��������
		memset(tempdata,0,512);
		sprintf((char *)tempdata,"{\"id\": \"1\",\"params\": {\"productKey\": \"%s\",\"deviceName\": \"%s\",\"clientId\": \"%s&%s\",\"signMethod\": \"hmacSha1\",\"sign\": \"%s\",\"cleanSession\": \"true\"}}",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,Sign);  //�������豸1��������
		CAT1_SubDeviceLogin((char *)tempdata);                                                                     //�������豸�������ݱ���	
    }
    /*----------------------------------------------*/
	/*            ���յ����豸1�Ϸ�������           */
	/*----------------------------------------------*/
	if((data[0]==SUB1_DATA)&&(datalen==(ATTRIBUTE_STRUCT_LEN+1))){
		u1_printf("���յ����豸1����\r\n"); 
		memcpy(&AttributeCB[0],&data[1],ATTRIBUTE_STRUCT_LEN);                   //��������
	    u1_printf("����1��%d\r\n",AttributeCB[0].Switch1Sta);                    //�����������
		u1_printf("����2��%d\r\n",AttributeCB[0].Switch2Sta);                    //�����������
		u1_printf("����3��%d\r\n",AttributeCB[0].Switch3Sta);                    //�����������
		u1_printf("����4��%d\r\n",AttributeCB[0].Switch4Sta);                    //�����������
		u1_printf("�¶ȣ�%.2f ��C\r\n",AttributeCB[0].tempdata);                  //�����������
		u1_printf("ʪ�ȣ�%.2f %\r\n",AttributeCB[0].humidata);                   //�����������
		u1_printf("���նȣ�%.2f lx\r\n",AttributeCB[0].lightata);   	         //�������	
	    u1_printf("ADC_CH5:%.2f V\r\n",AttributeCB[0].adcdata[0]);               //�������
		u1_printf("ADC_CH6:%.2f V\r\n",AttributeCB[0].adcdata[1]);               //�������
		u1_printf("ADC_CH7:%.2f V\r\n",AttributeCB[0].adcdata[2]);               //�������		
		memset(tempdata,0,512);                                                  //��ʱ������ȫ������
		sprintf((char *)tempdata,"{\"id\":\"2\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f}}}]}}",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,ATTRIBUTE1,AttributeCB[0].Switch1Sta,ATTRIBUTE2,AttributeCB[0].Switch2Sta,ATTRIBUTE3,AttributeCB[0].Switch3Sta,ATTRIBUTE4,AttributeCB[0].Switch4Sta,ATTRIBUTE5,AttributeCB[0].tempdata,ATTRIBUTE6,AttributeCB[0].humidata,ATTRIBUTE7,AttributeCB[0].lightata,ATTRIBUTE8,AttributeCB[0].adcdata[0],ATTRIBUTE9,AttributeCB[0].adcdata[1],ATTRIBUTE10,AttributeCB[0].adcdata[2]);  //��������
		CAT1_SubDevicePost((char *)tempdata);   			
    }
    /*----------------------------------------------*/
	/*            ���յ����豸2�Ϸ�������           */
	/*----------------------------------------------*/
	if((data[0]==SUB2_DATA)&&(datalen==(ATTRIBUTE_STRUCT_LEN+1))){
		u1_printf("���յ����豸2����\r\n"); 
		memcpy(&AttributeCB[1],&data[1],ATTRIBUTE_STRUCT_LEN);                   //��������
	    u1_printf("����1��%d\r\n",AttributeCB[1].Switch1Sta);                    //�����������
		u1_printf("����2��%d\r\n",AttributeCB[1].Switch2Sta);                    //�����������
		u1_printf("����3��%d\r\n",AttributeCB[1].Switch3Sta);                    //�����������
		u1_printf("����4��%d\r\n",AttributeCB[1].Switch4Sta);                    //�����������
		u1_printf("�¶ȣ�%.2f ��C\r\n",AttributeCB[1].tempdata);                  //�����������
		u1_printf("ʪ�ȣ�%.2f %\r\n",AttributeCB[1].humidata);                   //�����������
		u1_printf("���նȣ�%.2f lx\r\n",AttributeCB[1].lightata);   	         //�������	
	    u1_printf("ADC_CH5:%.2f V\r\n",AttributeCB[1].adcdata[0]);               //�������
		u1_printf("ADC_CH6:%.2f V\r\n",AttributeCB[1].adcdata[1]);               //�������
		u1_printf("ADC_CH7:%.2f V\r\n",AttributeCB[1].adcdata[2]);               //�������		
		memset(tempdata,0,512);                                                  //��ʱ������ȫ������
		sprintf((char *)tempdata,"{\"id\":\"2\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f}}}]}}",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,ATTRIBUTE1,AttributeCB[1].Switch1Sta,ATTRIBUTE2,AttributeCB[1].Switch2Sta,ATTRIBUTE3,AttributeCB[1].Switch3Sta,ATTRIBUTE4,AttributeCB[1].Switch4Sta,ATTRIBUTE5,AttributeCB[1].tempdata,ATTRIBUTE6,AttributeCB[1].humidata,ATTRIBUTE7,AttributeCB[1].lightata,ATTRIBUTE8,AttributeCB[1].adcdata[0],ATTRIBUTE9,AttributeCB[1].adcdata[1],ATTRIBUTE10,AttributeCB[1].adcdata[2]);  //��������
		CAT1_SubDevicePost((char *)tempdata);   			
    }
	/*----------------------------------------------*/
	/*            ���յ����豸3�Ϸ�������           */
	/*----------------------------------------------*/
	if((data[0]==SUB3_DATA)&&(datalen==(ATTRIBUTE_STRUCT_LEN+1))){
		u1_printf("���յ����豸3����\r\n"); 
		memcpy(&AttributeCB[2],&data[1],ATTRIBUTE_STRUCT_LEN);                   //��������
	    u1_printf("����1��%d\r\n",AttributeCB[2].Switch1Sta);                    //�����������
		u1_printf("����2��%d\r\n",AttributeCB[2].Switch2Sta);                    //�����������
		u1_printf("����3��%d\r\n",AttributeCB[2].Switch3Sta);                    //�����������
		u1_printf("����4��%d\r\n",AttributeCB[2].Switch4Sta);                    //�����������
		u1_printf("�¶ȣ�%.2f ��C\r\n",AttributeCB[2].tempdata);                  //�����������
		u1_printf("ʪ�ȣ�%.2f %\r\n",AttributeCB[2].humidata);                   //�����������
		u1_printf("���նȣ�%.2f lx\r\n",AttributeCB[2].lightata);   	         //�������	
	    u1_printf("ADC_CH5:%.2f V\r\n",AttributeCB[2].adcdata[0]);               //�������
		u1_printf("ADC_CH6:%.2f V\r\n",AttributeCB[2].adcdata[1]);               //�������
		u1_printf("ADC_CH7:%.2f V\r\n",AttributeCB[2].adcdata[2]);               //�������		
		memset(tempdata,0,512);                                                  //��ʱ������ȫ������
		sprintf((char *)tempdata,"{\"id\":\"2\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f}}}]}}",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,ATTRIBUTE1,AttributeCB[2].Switch1Sta,ATTRIBUTE2,AttributeCB[2].Switch2Sta,ATTRIBUTE3,AttributeCB[2].Switch3Sta,ATTRIBUTE4,AttributeCB[2].Switch4Sta,ATTRIBUTE5,AttributeCB[2].tempdata,ATTRIBUTE6,AttributeCB[2].humidata,ATTRIBUTE7,AttributeCB[2].lightata,ATTRIBUTE8,AttributeCB[2].adcdata[0],ATTRIBUTE9,AttributeCB[2].adcdata[1],ATTRIBUTE10,AttributeCB[2].adcdata[2]);  //��������
		CAT1_SubDevicePost((char *)tempdata);   			
    }
	/*----------------------------------------------*/
	/*        ���յ����豸1�ظ��Ŀ��ؿ�������       */
	/*----------------------------------------------*/
	if((data[0]==SUB1_SWITCH_DATA)&&(datalen==5)){
		u1_printf("���յ����豸1�ظ��Ŀ��ؿ�������\r\n"); 
		memset(tempdata,0,512);                                                  //��ʱ������ȫ������
		sprintf((char *)tempdata,"{\"id\":\"3\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d}}}]}}",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,ATTRIBUTE1,data[1],ATTRIBUTE2,data[2],ATTRIBUTE3,data[3],ATTRIBUTE4,data[4]);  //��������
		CAT1_SubDevicePost((char *)tempdata);   			
    }
    /*----------------------------------------------*/
	/*        ���յ����豸2�ظ��Ŀ��ؿ�������       */
	/*----------------------------------------------*/
	if((data[0]==SUB2_SWITCH_DATA)&&(datalen==5)){
		u1_printf("���յ����豸2�ظ��Ŀ��ؿ�������\r\n"); 
		memset(tempdata,0,512);                                                  //��ʱ������ȫ������
		sprintf((char *)tempdata,"{\"id\":\"3\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d}}}]}}",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,ATTRIBUTE1,data[1],ATTRIBUTE2,data[2],ATTRIBUTE3,data[3],ATTRIBUTE4,data[4]);  //��������
		CAT1_SubDevicePost((char *)tempdata);   			
    }	
	/*----------------------------------------------*/
	/*        ���յ����豸3�ظ��Ŀ��ؿ�������       */
	/*----------------------------------------------*/
	if((data[0]==SUB3_SWITCH_DATA)&&(datalen==5)){
		u1_printf("���յ����豸3�ظ��Ŀ��ؿ�������\r\n"); 
		memset(tempdata,0,512);                                                  //��ʱ������ȫ������
		sprintf((char *)tempdata,"{\"id\":\"3\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d}}}]}}",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,ATTRIBUTE1,data[1],ATTRIBUTE2,data[2],ATTRIBUTE3,data[3],ATTRIBUTE4,data[4]);  //��������
		CAT1_SubDevicePost((char *)tempdata);   			
    }
	/*----------------------------------------------*/
	/*       ���յ����豸1����һ��OTA���ݵĻظ�     */
	/*----------------------------------------------*/
	if((data[0]==SUB1_OTA_S)&&(datalen==1)){
		AliOTA.OTA_num++;                                                               //AliOTA.OTA_num ��1
		if(AliOTA.OTA_num<=AliOTA.OTA_timers){                                          //��û��������ϣ�����if
			u1_printf("׼��OTA���͹̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//���������ʾ��Ϣ		
			memset(tempdata,0,512);                                                     //��ջ�����
			tempdata[0] = SUB1_HADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[1] = SUB1_LADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[2] = SUB1_CH;                                              	    //���շ� �ŵ�		
			tempdata[3] = 0x5A;                                                         //�̶��ֽ�
			tempdata[4] = 0xA5;                                                         //�̶��ֽ�
			tempdata[5] = AliOTA.OTA_num/256;                                           //���ݰ���Ÿ��ֽ�		
			tempdata[6] = AliOTA.OTA_num%256;                                           //���ݰ���ŵ��ֽ�											
			W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //��ȡ����
			u3_TxDataBuf((uint8_t *)tempdata,135);                                      //������ݵ����ͻ�����
		}else{ 
			u1_printf("���豸1 OTA�̼��������\r\n"); 
			CAT1_PropertyVersion(AliInfoCB_SUB[0].Version_ABuff,"SUB1");		        //ͬ�����豸1��ǰ�̼��汾��
			memset(tempdata,0,512);                                                	 	//��ջ�����			
			tempdata[0] = SUB1_HADR;                                               		//���շ� ��ַ���ֽ�
			tempdata[1] = SUB1_LADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[2] = SUB1_CH;                                              	    //���շ� �ŵ�
			sprintf((char *)&tempdata[3],"AT+VER1=%s",AliInfoCB_SUB[0].Version_ABuff);  //����µİ汾��
			tempdata[3+strlen((char *)&tempdata[3])] = 0;                               //�̶� 0
			tempdata[4+strlen((char *)&tempdata[3])] = AliOTA.streamSize/256;           //�̼���С���ֽ�
			tempdata[5+strlen((char *)&tempdata[3])] = AliOTA.streamSize%256;           //�̼���С���ֽ�
			u3_TxDataBuf((uint8_t *)tempdata,6+strlen((char *)&tempdata[3]));           //������ݵ����ͻ�����
		}
	}
	/*----------------------------------------------*/
	/*       ���յ����豸1�ط�һ��OTA���ݵĻظ�     */
	/*----------------------------------------------*/
	if((data[0]==SUB1_OTA_C)&&(datalen==1)){
		u1_printf("׼��OTA���͹̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	    //���������ʾ��Ϣ		
		memset(tempdata,0,512);                                                         //��ջ�����
		tempdata[0] = SUB1_HADR;                                                        //���շ� ��ַ���ֽ�
		tempdata[1] = SUB1_LADR;                                                        //���շ� ��ַ���ֽ�
		tempdata[2] = SUB1_CH;                                              	        //���շ� �ŵ�		
		tempdata[3] = 0x5A;                                                             //�̶��ֽ�
		tempdata[4] = 0xA5;                                                             //�̶��ֽ�
		tempdata[5] = AliOTA.OTA_num/256;                                               //���ݰ���Ÿ��ֽ�		
		tempdata[6] = AliOTA.OTA_num%256;                                               //�ݰ���ŵ��ֽ�									
		W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //��ȡ����
		u3_TxDataBuf((uint8_t *)tempdata,135);                                          //������ݵ����ͻ�����

	}
    /*----------------------------------------------*/
	/*       ���յ����豸2����һ��OTA���ݵĻظ�     */
	/*----------------------------------------------*/
	if((data[0]==SUB2_OTA_S)&&(datalen==1)){
		AliOTA.OTA_num++;                                                               //AliOTA.OTA_num ��1
		if(AliOTA.OTA_num<=AliOTA.OTA_timers){                                          //��û��������ϣ�����if
			u1_printf("׼��OTA���͹̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//���������ʾ��Ϣ		
			memset(tempdata,0,512);                                                     //��ջ�����
			tempdata[0] = SUB2_HADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[1] = SUB2_LADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[2] = SUB2_CH;                                              	    //���շ� �ŵ�		
			tempdata[3] = 0x5A;                                                         //�̶��ֽ�
			tempdata[4] = 0xA5;                                                         //�̶��ֽ�
			tempdata[5] = AliOTA.OTA_num/256;                                           //���ݰ���Ÿ��ֽ�			
			tempdata[6] = AliOTA.OTA_num%256;                                           //���ݰ���ŵ��ֽ�									
			W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //��ȡ����
			u3_TxDataBuf((uint8_t *)tempdata,135);                                      //������ݵ����ͻ�����
		}else{ 
			u1_printf("���豸2 OTA�̼��������\r\n"); 
			CAT1_PropertyVersion(AliInfoCB_SUB[1].Version_ABuff,"SUB2");		        //ͬ�����豸2��ǰ�̼��汾��
			memset(tempdata,0,512);                                                     //��ջ�����			
			tempdata[0] = SUB2_HADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[1] = SUB2_LADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[2] = SUB2_CH;                                              	    //���շ� �ŵ�
			sprintf((char *)&tempdata[3],"AT+VER2=%s",AliInfoCB_SUB[1].Version_ABuff);  //����µİ汾��
			tempdata[3+strlen((char *)&tempdata[3])] = 0;                               //�̶� 0
			tempdata[4+strlen((char *)&tempdata[3])] = AliOTA.streamSize/256;           //�̼���С���ֽ�
			tempdata[5+strlen((char *)&tempdata[3])] = AliOTA.streamSize%256;           //�̼���С���ֽ�
			u3_TxDataBuf((uint8_t *)tempdata,6+strlen((char *)&tempdata[3]));           //������ݵ����ͻ�����
		}
	}
	/*----------------------------------------------*/
	/*       ���յ����豸2�ط�һ��OTA���ݵĻظ�     */
	/*----------------------------------------------*/
	if((data[0]==SUB2_OTA_C)&&(datalen==1)){
		u1_printf("׼��OTA���͹̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//���������ʾ��Ϣ		
		memset(tempdata,0,512);                                                     //��ջ�����
		tempdata[0] = SUB2_HADR;                                                    //���շ� ��ַ���ֽ�
		tempdata[1] = SUB2_LADR;                                                    //���շ� ��ַ���ֽ�
		tempdata[2] = SUB2_CH;                                              	    //���շ� �ŵ�		
		tempdata[3] = 0x5A;                                                         //�̶��ֽ�
		tempdata[4] = 0xA5;                                                         //�̶��ֽ�
		tempdata[5] = AliOTA.OTA_num/256;                                           //���ݰ���Ÿ��ֽ�		
		tempdata[6] = AliOTA.OTA_num%256;                                           //�ݰ���ŵ��ֽ�										
		W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //��ȡ����
		u3_TxDataBuf((uint8_t *)tempdata,135);                                      //������ݵ����ͻ�����

	}
	/*----------------------------------------------*/
	/*       ���յ����豸3����һ��OTA���ݵĻظ�     */
	/*----------------------------------------------*/
	if((data[0]==SUB3_OTA_S)&&(datalen==1)){
		AliOTA.OTA_num++;                                                               //AliOTA.OTA_num ��1
		if(AliOTA.OTA_num<=AliOTA.OTA_timers){                                          //��û��������ϣ�����if
			u1_printf("׼��OTA���͹̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//���������ʾ��Ϣ		
			memset(tempdata,0,512);                                                     //��ջ�����
			tempdata[0] = SUB3_HADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[1] = SUB3_LADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[2] = SUB3_CH;                                              	    //���շ� �ŵ�		
			tempdata[3] = 0x5A;                                                         //�̶��ֽ�
			tempdata[4] = 0xA5;                                                         //�̶��ֽ�
			tempdata[5] = AliOTA.OTA_num/256;                                           //���ݰ���Ÿ��ֽ�			
			tempdata[6] = AliOTA.OTA_num%256;                                           //���ݰ���ŵ��ֽ�									
			W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //��ȡ����
			u3_TxDataBuf((uint8_t *)tempdata,135);                                      //������ݵ����ͻ�����
		}else{ 
			u1_printf("���豸2 OTA�̼��������\r\n"); 
			CAT1_PropertyVersion(AliInfoCB_SUB[2].Version_ABuff,"SUB3");		        //ͬ�����豸3��ǰ�̼��汾��
			memset(tempdata,0,512);                                                     //��ջ�����			
			tempdata[0] = SUB3_HADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[1] = SUB3_LADR;                                                    //���շ� ��ַ���ֽ�
			tempdata[2] = SUB3_CH;                                              	    //���շ� �ŵ�
			sprintf((char *)&tempdata[3],"AT+VER3=%s",AliInfoCB_SUB[2].Version_ABuff);  //����µİ汾��
			tempdata[3+strlen((char *)&tempdata[3])] = 0;                               //�̶� 0
			tempdata[4+strlen((char *)&tempdata[3])] = AliOTA.streamSize/256;           //�̼���С���ֽ�
			tempdata[5+strlen((char *)&tempdata[3])] = AliOTA.streamSize%256;           //�̼���С���ֽ�
			u3_TxDataBuf((uint8_t *)tempdata,6+strlen((char *)&tempdata[3]));           //������ݵ����ͻ�����
		}
	}
	/*----------------------------------------------*/
	/*       ���յ����豸3�ط�һ��OTA���ݵĻظ�     */
	/*----------------------------------------------*/
	if((data[0]==SUB2_OTA_C)&&(datalen==1)){
		u1_printf("׼��OTA���͹̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//���������ʾ��Ϣ		
		memset(tempdata,0,512);                                                     //��ջ�����
		tempdata[0] = SUB3_HADR;                                                    //���շ� ��ַ���ֽ�
		tempdata[1] = SUB3_LADR;                                                    //���շ� ��ַ���ֽ�
		tempdata[2] = SUB3_CH;                                              	    //���շ� �ŵ�		
		tempdata[3] = 0x5A;                                                         //�̶��ֽ�
		tempdata[4] = 0xA5;                                                         //�̶��ֽ�
		tempdata[5] = AliOTA.OTA_num/256;                                           //���ݰ���Ÿ��ֽ�		
		tempdata[6] = AliOTA.OTA_num%256;                                           //�ݰ���ŵ��ֽ�										
		W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //��ȡ����
		u3_TxDataBuf((uint8_t *)tempdata,135);                                      //������ݵ����ͻ�����
	}
}

