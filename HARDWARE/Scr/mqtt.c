/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "main.h"             //������Ҫ��ͷ�ļ�
#include "mqtt.h"             //������Ҫ��ͷ�ļ�
#include "utils_hmac.h"       //������Ҫ��ͷ�ļ�
#include "clock.h"            //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�
#include "math.h"             //������Ҫ��ͷ�ļ�

MQTT_CB   Aliyun_mqtt;        //����һ���������Ӱ�����mqtt�Ľṹ��

/*----------------------------------------------------------*/
/*����������ʼ������                                        */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void IoT_Parameter_Init(void)
{	
	char temp[64];                                                                					       //������ܵ�ʱ����ʱʹ�õĻ�����
    
	memset(&Aliyun_mqtt,0,MQTT_CB_LEN);                                                                    //���Ӱ�����mqtt�Ľṹ������ȫ������
	sprintf(Aliyun_mqtt.ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);                       //�����ͻ���ID�������뻺����
	sprintf(Aliyun_mqtt.Username,"%s&%s",DEVICENAME,PRODUCTKEY);                                           //�����û����������뻺����	
	memset(temp,0,64);                                                                                     //��ʱ������ȫ������
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);                   //��������ʱ������   
	utils_hmac_sha1(temp,strlen(temp),Aliyun_mqtt.Passward,(char *)DEVICESECRE,DEVICESECRET_LEN);          //��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������룬�����浽��������
	sprintf(Aliyun_mqtt.ServerIP,"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",PRODUCTKEY);                    //��������������
	Aliyun_mqtt.ServerPort = 1883;                                                                         //�������˿ں�1883
    sprintf(Aliyun_mqtt.Stopic_Buff[0],"/sys/%s/%s/thing/service/property/set",PRODUCTKEY,DEVICENAME);     //������1����Ҫ���ĵ�Topic�����շ������·�������
	sprintf(Aliyun_mqtt.Stopic_Buff[1],"/ota/device/upgrade/%s/%s",PRODUCTKEY,DEVICENAME);                 //������2����Ҫ���ĵ�Topic������OTA����֪ͨ
	sprintf(Aliyun_mqtt.Stopic_Buff[2],"/sys/%s/%s/thing/file/download_reply",PRODUCTKEY,DEVICENAME);      //������3��Ҫ���ĵ�Topic������OTA����ʱ�Ĺ̼�
	sprintf(Aliyun_mqtt.Stopic_Buff[3],"/ext/session/%s/%s/combine/login_reply",PRODUCTKEY,DEVICENAME);    //������4����Ҫ���ĵ�Topic�����豸����״̬�ظ�
	sprintf(Aliyun_mqtt.Stopic_Buff[4],"/ext/session/%s/%s/combine/logout_reply",PRODUCTKEY,DEVICENAME);              //������5����Ҫ���ĵ�Topic�����豸����״̬�ظ�
	sprintf(Aliyun_mqtt.Stopic_Buff[5],"/sys/%s/%s/thing/event/property/pack/post_reply",PRODUCTKEY,DEVICENAME);      //������6����Ҫ���ĵ�Topic�������ϱ�����״̬�ظ�
	sprintf(Aliyun_mqtt.Stopic_Buff[6],"/sys/%s/%s/thing/ota/firmware/get_reply",PRODUCTKEY,DEVICENAME);              //������7����Ҫ���ĵ�Topic������OTA��������
}
/*----------------------------------------------------------*/
/*��������MQTT CONNECT���� ��Ȩ����                         */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp;              //���㱨��ʣ�೤��ʱ��ʹ�õ���ʱ����              
	int Remaining_len;     //���汨��ʣ�೤���ֽ� 
	
	Aliyun_mqtt.MessageID = 0;  //���ı�ʶ�����㣬CONNECT������Ȼ����Ҫ��ӱ��ı�ʶ��������CONNECT�����ǵ�һ�����͵ı��ģ��ڴ����㱨�ı�ʶ����Ϊ����������׼��
	Aliyun_mqtt.Fixed_len = 1;                                                                                                        //CONNECT���ģ��̶���ͷ�����ݶ�Ϊ1
	Aliyun_mqtt.Variable_len = 10;                                                                                                    //CONNECT���ģ��ɱ䱨ͷ����=10
	Aliyun_mqtt.Payload_len = 2 + strlen(Aliyun_mqtt.ClientID) + 2 + strlen(Aliyun_mqtt.Username) + 2 + strlen(Aliyun_mqtt.Passward); //CONNECT���ģ����㸺�س���      
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;                                                               //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	Aliyun_mqtt.Pack_buff[0]=0x10;                                   //CONNECT���� �̶���ͷ��1���ֽ� ��0x10	
	do{                                                              //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;                                    //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;                           //ʣ�೤��ȡ��128
		if(Remaining_len>0) temp |= 0x80;                            //���Remaining_len���ڵ���128�� ��Э��Ҫ��λ7��λ                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;         //ʣ�೤���ֽڼ�¼һ������
		Aliyun_mqtt.Fixed_len++;	                                 //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                                         //���Remaining_len>0�Ļ����ٴν���ѭ��
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0]=0x00;             //CONNECT���ģ��ɱ䱨ͷ��1���ֽ� ���̶�0x00	            
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1]=0x04;             //CONNECT���ģ��ɱ䱨ͷ��2���ֽ� ���̶�0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2]=0x4D;	         //CONNECT���ģ��ɱ䱨ͷ��3���ֽ� ���̶�0x4D
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3]=0x51;	         //CONNECT���ģ��ɱ䱨ͷ��4���ֽ� ���̶�0x51
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4]=0x54;	         //CONNECT���ģ��ɱ䱨ͷ��5���ֽ� ���̶�0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+5]=0x54;	         //CONNECT���ģ��ɱ䱨ͷ��6���ֽ� ���̶�0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+6]=0x04;	         //CONNECT���ģ��ɱ䱨ͷ��7���ֽ� ���̶�0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+7]=0xC2;	         //CONNECT���ģ��ɱ䱨ͷ��8���ֽ� ��ʹ���û���������У�飬��ʹ���������ܣ��������Ự����
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+8]=0x00; 	         //CONNECT���ģ��ɱ䱨ͷ��9���ֽ� ������ʱ����ֽ� 0x00
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+9]=0x64;	         //CONNECT���ģ��ɱ䱨ͷ��10���ֽڣ�����ʱ����ֽ� 0x64   ����ֵ=100s
	
	/*     CLIENT_ID      */
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+10] = strlen(Aliyun_mqtt.ClientID)/256;                			  		                                                   //�ͻ���ID���ȸ��ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+11] = strlen(Aliyun_mqtt.ClientID)%256;               			  		                                                   //�ͻ���ID���ȵ��ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12],Aliyun_mqtt.ClientID,strlen(Aliyun_mqtt.ClientID));                                                            //���ƹ����ͻ���ID�ִ�	
	/*     �û���        */ 
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12+strlen(Aliyun_mqtt.ClientID)] = strlen(Aliyun_mqtt.Username)/256; 	                                                    //�û������ȸ��ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+13+strlen(Aliyun_mqtt.ClientID)] = strlen(Aliyun_mqtt.Username)%256; 		                                                //�û������ȵ��ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(Aliyun_mqtt.ClientID)],Aliyun_mqtt.Username,strlen(Aliyun_mqtt.Username));                                //���ƹ����û����ִ�	
	/*      ����        */
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(Aliyun_mqtt.ClientID)+strlen(Aliyun_mqtt.Username)] = strlen(Aliyun_mqtt.Passward)/256;	                        //���볤�ȸ��ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+15+strlen(Aliyun_mqtt.ClientID)+strlen(Aliyun_mqtt.Username)] = strlen(Aliyun_mqtt.Passward)%256;	                        //���볤�ȵ��ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+16+strlen(Aliyun_mqtt.ClientID)+strlen(Aliyun_mqtt.Username)],Aliyun_mqtt.Passward,strlen(Aliyun_mqtt.Passward));   //���ƹ��������ִ�
   
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);                                                      //�����������ݣ����뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������MQTT DISCONNECT���� �Ͽ�����                      */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_DISCONNECT(void)
{
	Aliyun_mqtt.Pack_buff[0]=0xE0;              //��1���ֽ� ���̶�0xE0                      
	Aliyun_mqtt.Pack_buff[1]=0x00;              //��2���ֽ� ���̶�0x00 
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, 2);   //�����������ݣ����뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������MQTT SUBSCRIBE���� ����Topic                      */
/*��  ����topicbuff������topic���ĵĻ�����                  */
/*��  ����topicnum�����ļ���topic����                       */
/*��  ����Qs�����ĵȼ�                                      */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char topicbuff[TOPIC_NUM][TOPIC_SIZE], unsigned char topicnum, unsigned char Qs)
{	
	int i;                            //����forѭ��
	int temp;                         //��������ʱ��ʹ�õ���ʱ����              
	int Remaining_len;                //���汨��ʣ�೤���ֽ� 
	
	Aliyun_mqtt.Fixed_len = 1;                                          //SUBSCRIBE���ģ��̶���ͷ�����ݶ�Ϊ1
	Aliyun_mqtt.Variable_len = 2;                                       //SUBSCRIBE���ģ��ɱ䱨ͷ����=2     2�ֽڱ��ı�ʶ��
	Aliyun_mqtt.Payload_len = 0;                                        //SUBSCRIBE���ģ��������ݳ����ݶ�Ϊ0
	
	for(i=0;i<topicnum;i++)                                             //ѭ��ͳ��topic�ַ������ȣ�����ͳ�Ƹ������ݳ���
		Aliyun_mqtt.Payload_len += strlen(topicbuff[i]);                //ÿ���ۼ�1��topic����
	Aliyun_mqtt.Payload_len += 3*topicnum;                              //ÿ����Ҫ���ĵ�topic���˱�����ַ������ȣ���������ʾtopic�ַ������ȵ�2�ֽڣ��Լ����ĵȼ�1�ֽڣ��������ճ��Ȼ�Ҫ�ټ��� 3*topicnum
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //����ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	Aliyun_mqtt.Pack_buff[0]=0x82;                                      //SUBSCRIBE���� �̶���ͷ��1���ֽ� ��0x82	
	do{                                                                 //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;                                       //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;                              //ʣ�೤��ȡ��128
		if(Remaining_len>0) temp |= 0x80;                               //���Remaining_len���ڵ���128�� ��Э��Ҫ��λ7��λ                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //ʣ�೤���ֽڼ�¼һ������
		Aliyun_mqtt.Fixed_len++;	                                    //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                                            //���Remaining_len>0�Ļ����ٴν���ѭ��
		
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = Aliyun_mqtt.MessageID/256;       //���ı�ʶ�����ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = Aliyun_mqtt.MessageID%256;		  //���ı�ʶ�����ֽ�
	Aliyun_mqtt.MessageID++;                                                          //ÿ��һ�μ�1
	
	temp = 0;
	for(i=0;i<topicnum;i++){                                                                               //ѭ�����Ƹ���topic����		
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+temp] = strlen(topicbuff[i])/256;                    //topic�ַ��� ���ȸ��ֽ� ��ʶ�ֽ�
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3+temp] = strlen(topicbuff[i])%256;		               //topic�ַ��� ���ȵ��ֽ� ��ʶ�ֽ�
		memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+temp],topicbuff[i],strlen(topicbuff[i]));    //����topic�ִ�		
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topicbuff[i])+temp] = Qs;                     //���ĵȼ�0	
		temp += strlen(topicbuff[i]) + 3;                                                                  //len���ڱ���ѭ������ӵ������� ���� topic�ִ������� + 2���ֽڳ��ȱ�ʶ + 1���ֽڶ��ĵȼ�
	}
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //�����������ݣ����뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������MQTT UNSUBSCRIBE���� ȡ������Topic                */
/*��  ����topicbuff��ȡ������topic���ƻ�����                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_UNSubscribe(char *topicbuff)
{	
	int temp;                         //��������ʱ��ʹ�õ���ʱ����              
	int Remaining_len;                //���汨��ʣ�೤���ֽ� 
	
	Aliyun_mqtt.Fixed_len = 1;                                          //UNSUBSCRIBE���ģ��̶���ͷ�����ݶ�Ϊ1
	Aliyun_mqtt.Variable_len = 2;                                       //UNSUBSCRIBE���ģ��ɱ䱨ͷ����=2     2�ֽڱ��ı�ʶ��
	Aliyun_mqtt.Payload_len = strlen(topicbuff) + 2;                    //UNSUBSCRIBE���ģ��������ݳ��� = topic���Ƴ��� + 2�ֽڳ��ȱ�ʶ
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //����ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	Aliyun_mqtt.Pack_buff[0]=0xA0;                                      //UNSUBSCRIBE���� �̶���ͷ��1���ֽ� ��0xA0
	do{                                                                 //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;                                       //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;                              //ʣ�೤��ȡ��128
		if(Remaining_len>0) temp |= 0x80;                               //���Remaining_len���ڵ���128�� ��Э��Ҫ��λ7��λ                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //ʣ�೤���ֽڼ�¼һ������
		Aliyun_mqtt.Fixed_len++;	                                    //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                                            //���Remaining_len>0�Ļ����ٴν���ѭ��
		
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = Aliyun_mqtt.MessageID/256;       //���ı�ʶ�����ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = Aliyun_mqtt.MessageID%256;		  //���ı�ʶ�����ֽ�
	Aliyun_mqtt.MessageID++;                                                          //ÿ��һ�μ�1
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2] = strlen(topicbuff)/256;                 //topic�ַ��� ���ȸ��ֽ� ��ʶ�ֽ�
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3] = strlen(topicbuff)%256;		            //topic�ַ��� ���ȵ��ֽ� ��ʶ�ֽ�
    memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4],topicbuff,strlen(topicbuff));    //����topic�ִ�		
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //�����������ݣ����뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������MQTT PING���� ����������                          */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	Aliyun_mqtt.Pack_buff[0]=0xC0;              //��1���ֽ� ���̶�0xC0                      
	Aliyun_mqtt.Pack_buff[1]=0x00;              //��2���ֽ� ���̶�0x00 
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, 2);   //�����������ݣ����뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������MQTT PUBLISH���� �ȼ�0 ��������                   */
/*��  ����topic_name���������ݵ�topic����                   */
/*��  ����data������                                        */
/*��  ����data_len�����ݳ���                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
		
	Aliyun_mqtt.Fixed_len = 1;                            //PUBLISH�ȼ�0���ģ��̶���ͷ�����ݶ�Ϊ1
	Aliyun_mqtt.Variable_len = 2 + strlen(topic);         //PUBLISH�ȼ�0���ģ��ɱ䱨ͷ����=2�ֽ�(topic����)��ʶ�ֽ�+ topic�ַ����ĳ���
	Aliyun_mqtt.Payload_len = data_len;                   //PUBLISH�ȼ�0���ģ��������ݳ��� = data_len
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //����ʣ�೤��=�ɱ䱨ͷ����+���س���	
			
	Aliyun_mqtt.Pack_buff[0]=0x30;                                      //PUBLISH�ȼ�0���� �̶���ͷ��1���ֽ� ��0x0x30
	do{                                                                 //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;                                       //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;                              //ʣ�೤��ȡ��128
		if(Remaining_len>0) temp |= 0x80;                               //���Remaining_len���ڵ���128�� ��Э��Ҫ��λ7��λ                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //ʣ�೤���ֽڼ�¼һ������
		Aliyun_mqtt.Fixed_len++;	                                    //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                                            //���Remaining_len>0�Ļ����ٴν���ѭ��
			             
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0]=strlen(topic)/256;                      //�ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1]=strlen(topic)%256;		               //�ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));           //�ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���	
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)],data,data_len);   //��Ч���ɣ�����data����
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //�����������ݣ����뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������MQTT PUBLISH���� �ȼ�1 ��������                   */
/*��  ����topic_name���������ݵ�topic����                   */
/*��  ����data������                                        */
/*��  ����data_len�����ݳ���                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs1(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
		
	Aliyun_mqtt.Fixed_len = 1;                            //PUBLISH�ȼ�1���ģ��̶���ͷ�����ݶ�Ϊ1
	Aliyun_mqtt.Variable_len = 2 + 2 + strlen(topic);      //PUBLISH�ȼ�1���ģ��ɱ䱨ͷ����=2�ֽڱ�ʶ�� + 2�ֽ�(topic����)��ʶ�ֽ� + topic�ַ����ĳ���
	Aliyun_mqtt.Payload_len = data_len;                   //PUBLISH�ȼ�1���ģ��������ݳ��� = data_len
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //����ʣ�೤��=�ɱ䱨ͷ����+���س���	
			
	Aliyun_mqtt.Pack_buff[0]=0x32;                                      //PUBLISH�ȼ�1���� �̶���ͷ��1���ֽ� ��0x0x32
	do{                                                                 //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;                                       //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;                              //ʣ�೤��ȡ��128
		if(Remaining_len>0) temp |= 0x80;                               //���Remaining_len���ڵ���128�� ��Э��Ҫ��λ7��λ                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //ʣ�೤���ֽڼ�¼һ������
		Aliyun_mqtt.Fixed_len++;	                                    //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                                            //���Remaining_len>0�Ļ����ٴν���ѭ��
			             	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0]=strlen(topic)/256;                                    //�ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1]=strlen(topic)%256;		                             //�ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));                         //�ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���
   	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)] = Aliyun_mqtt.MessageID/256;            //���ı�ʶ�����ֽ�
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3+strlen(topic)] = Aliyun_mqtt.MessageID%256;		     //���ı�ʶ�����ֽ�
	Aliyun_mqtt.MessageID++;                                                                             //ÿ��һ�μ�1	
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topic)],data,data_len);                 //��Ч���ɣ�����data����
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //�����������ݣ����뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*����������������������ĵȼ�0����������,����topic��Ϣ     */
/*��  ����redata�����յ�����                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_DealPushdatatopic_Qs0(unsigned char *redata, int data_len)
{
	int  i;                 //����forѭ��
	int  Remaining_len;     //���汨��ʣ�೤��
	char Remaining_size;    //���汨��ʣ�೤��ռ�ü����ֽ�
		
	for(i=1;i<5;i++){                    //ѭ���鿴����ʣ�೤��ռ�ü����ֽ� ���4���ֽ�
		if((redata[i]&0x80)==0){         //λ7����1�Ļ���˵�����˱���ʣ�೤������1���ֽ�
			Remaining_size = i;          //��¼i�����Ǳ���ʣ�೤��ռ�õ��ֽ�����
			break;                       //����for
		}
	}
	Remaining_len = 0;                   //ʣ�೤������
	for(i=Remaining_size;i>0;i--){       //����ʣ�೤��ռ�ü����ֽڣ���ѭ���ƴμ��㳤��
		Remaining_len += (redata[i]&0x7f)*pow(128,i-1);  //����ʣ�೤��
	}
    memset(Aliyun_mqtt.cmdbuff,0,CMD_SIZE);                                             //��ջ�����
	memcpy(Aliyun_mqtt.cmdbuff,&redata[1+Remaining_size+2],Remaining_len - 2);	     	//�����������ݲ���+topic��Ϣ�� Aliyun_mqtt.cmdbuff ����������������	
}
