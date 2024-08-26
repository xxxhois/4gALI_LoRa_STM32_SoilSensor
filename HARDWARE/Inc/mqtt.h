/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�ͷ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __MQTT_H
#define __MQTT_H
         
#define  MQTT_TxData(x)       u2_TxData(x)           //����2��������
#define  TxDataBuf_Deal       u2_TxDataBuf           //���봮��2������

#define  PRODUCTKEY           AliInfoCB.ProductKeyBuff       //��ƷKEY
#define  DEVICENAME  		  AliInfoCB.DeviceNameBuff       //�豸��  
#define  DEVICESECRE  		  AliInfoCB.DeviceSecretBuff     //�豸��Կ   
#define  TOPIC_NUM    		  7                              //��Ҫ���ĵ����Topic����
#define  TOPIC_SIZE   		  64                             //���Topic�ַ������ƻ���������
#define  PACK_SIZE    		  512                            //��ű������ݻ�������С
#define  CLIENTID_SIZE        64                             //��ſͻ���ID�Ļ�������С
#define  USERNAME_SIZE        64                             //����û����Ļ�������С
#define  PASSWARD_SIZE        64                             //�������Ļ�������С
#define  SERVERIP_SIZE        64                             //��ŷ�����IP����������������С
#define  CMD_SIZE             512                            //�������͵�PUBLISH�����е����ݻ�������С

typedef struct{
	char ClientID[CLIENTID_SIZE];                            //��ſͻ���ID�Ļ�����
	char Username[USERNAME_SIZE];                            //����û����Ļ�����
	char Passward[PASSWARD_SIZE];                            //�������Ļ�����
	char ServerIP[SERVERIP_SIZE];                            //��ŷ�����IP��������������
	unsigned char Pack_buff[PACK_SIZE];        				 //��ű������ݻ�����
	unsigned short int  ServerPort;                  	     //��ŷ������Ķ˿ں�	
	unsigned short int  MessageID;                           //��¼���ı�ʶ��
	unsigned int  Fixed_len;                       	         //�̶���ͷ����
	unsigned int  Variable_len;                              //�ɱ䱨ͷ����
	unsigned int  Payload_len;                               //��Ч���ɳ���
	char Stopic_Buff[TOPIC_NUM][TOPIC_SIZE];                 //�������Ƕ��ĵ������б�
	char cmdbuff[CMD_SIZE];                                  //�������͵�PUBLISH�����е����ݻ�����
}MQTT_CB;  
#define MQTT_CB_LEN         sizeof(MQTT_CB)                  //�ṹ�峤�� 

extern MQTT_CB   Aliyun_mqtt;                                //�ⲿ�����������������Ӱ�����mqtt�Ľṹ��

void IoT_Parameter_Init(void);                                                   			 //�����������Ʒ�������ʼ���������õ��ͻ���ID���û���������
void MQTT_ConectPack(void);                                                      			 //����������MQTT CONNECT����       ��Ȩ����
void MQTT_DISCONNECT(void);                                                       			 //����������MQTT DISCONNECT����    �Ͽ�����
void MQTT_Subscribe(char topicbuff[TOPIC_NUM][TOPIC_SIZE],unsigned char, unsigned char);     //����������MQTT SUBSCRIBE����     ����Topic
void MQTT_UNSubscribe(char *);                                                   			 //����������MQTT UNSUBSCRIBE����   ȡ������Topic
void MQTT_PingREQ(void);                                                          			 //����������MQTT PING����          ����������
void MQTT_PublishQs0(char *,char *,int);                                          			 //����������MQTT PUBLISH���� �ȼ�0 ��������
void MQTT_PublishQs1(char *,char *,int);                                         			 //����������MQTT PUBLISH���� �ȼ�1 ��������
void MQTT_DealPushdatatopic_Qs0(unsigned char *, int);                            		   	 //������������������������ĵȼ�0���������ݣ�����topic��Ϣ

#endif
