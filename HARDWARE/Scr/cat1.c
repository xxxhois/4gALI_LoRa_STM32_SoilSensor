/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*              ����4Gģ�鹦�ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "main.h"             //������Ҫ��ͷ�ļ�
#include "cat1.h"	          //������Ҫ��ͷ�ļ�
#include "clock.h"	          //������Ҫ��ͷ�ļ�
#include "usart.h"	          //������Ҫ��ͷ�ļ�
#include "mqtt.h"             //������Ҫ��ͷ�ļ�
#include "24c02.h"            //������Ҫ��ͷ�ļ�
#include "w25qxx.h"           //������Ҫ��ͷ�ļ�
#include "lora.h"	          //������Ҫ��ͷ�ļ�
#include "crc.h"              //������Ҫ��ͷ�ļ�

char databuff[256];           //���������õ���ʱ������
uint8_t online_counter = 1;   //��ѯ���豸���ߵļ�������
uint8_t data_counter = 1;     //��ѯ���豸���ݵļ�������

/*-------------------------------------------------*/
/*����������ʼ��CAT1ģ��                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CAT1_Reset(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();                             //ʹ�ܶ˿�Cʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_4 | GPIO_PIN_15;                //����PA4 15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;                    //�������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);                       //����
	
	GPIO_Initure.Pin=GPIO_PIN_0 | GPIO_PIN_1;                 //����PA0 1
    GPIO_Initure.Mode=GPIO_MODE_INPUT;                        //����ģʽ
	GPIO_Initure.Pull = GPIO_NOPULL;                          //��ʹ��������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);                       //����
	
	if(POWER_STA==1){   							           //���PB1�ǵ͵�ƽ����ʾĿǰ���ڹػ�״̬		
		u1_printf("\r\nĿǰ���ڹػ�״̬��׼������\r\n");       //���������Ϣ
		POWER_KEY(1);                                          //������
		HAL_Delay(1500);									   //��ʱ
		POWER_KEY(0);										   //�����ͣ�����
	}else{                                                     //��֮PB1�Ǹߵ�ƽ����ʾĿǰ���ڿ���״̬
		u1_printf("\r\nĿǰ���ڿ���״̬��׼������\r\n");       //���������Ϣ		
		POWER_KEY(1);                                          //������
		HAL_Delay(2000);								       //��ʱ
		POWER_KEY(0);										   //�����ͣ��ػ�
		HAL_Delay(4000);                                       //���
		POWER_KEY(1);                                          //������
		HAL_Delay(1500);									   //��ʱ
		POWER_KEY(0);										   //�����ͣ��������������
	}
	u1_printf("��ȴ�ע��������\r\n");                         //���������Ϣ		
	while(1){                                                  //�ȴ�ע��������
		HAL_Delay(100);                                        //��ʱ100ms
		u1_printf(".");                                        //���������Ϣ	
		if(NET_STA == 0)                                       //NET_STA ���ű�ɵ͵�ƽ ��ʾע��������
			break;       						               //��������whileѭ��
	}

	u1_printf("\r\n�Ѿ�ע��������\r\n");                       //���������Ϣ
	HAL_Delay(1000);                                           //���
	CAT1_printf("AT+CIPMUX=0\r\n");                            //����ָ��رն�·����
}
/*-------------------------------------------------*/
/*�����������ڸ���ϵͳ�����Ľṹ���ʼ��           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SysInit(void)
{			 
	memset(&SysCB,0,SYS_STRUCT_LEN);                             //���ڸ���ϵͳ�����Ľṹ��,ȫ������
	memset(AliInfoCB_SUB,0,SUB_DEVICE_NUM*ALIINFO_STRUCT_LEN);   //������豸֤����Ϣ�ṹ��
	memset(AttributeCB,0,SUB_DEVICE_NUM*ATTRIBUTE_STRUCT_LEN);   //������豸��Ź����������ݵĽṹ��
	memset(&AliOTA,0,OTA_CB_LEN);                                //OTA�ṹ�����
}
/*-------------------------------------------------*/
/*������������2�����¼�                            */
/*��  ����data ������                              */
/*��  ����datalen �����ݳ���                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U2PassiveEvent(uint8_t *data, uint16_t datalen)
{	
	uint16_t i;                        
	uint16_t tempsize,tempoffset;      //���ڱ��棬������������ƫ�Ƶļ�����
	int  firminfo_len;                 //������صĹ̼�������Ϣռ�õĳ��� 
	char firminfo_buff[128];           //������صĹ̼�������Ϣ������
	int  bOffset,bSize;                //�������ص�OTA�̼���ƫ�����͹̼���С
	char *ptr;
	
	/*----------------------------------------------*/
	/*                �رն�·���ӳɹ�              */
	/*----------------------------------------------*/
	if(strstr((char *)data,"AT+CIPMUX=0")){                                         //���յ�����AT+CIPMUX=0
		u1_printf("�رն�·���ӳɹ�\r\n");                                          //���������Ϣ
		CAT1_printf("AT+CIPMODE=1\r\n");                                            //����͸��ģʽָ��		
	}
	/*----------------------------------------------*/
	/*                 ͸��ģʽ�ɹ�                 */
	/*----------------------------------------------*/
	if(strstr((char *)data,"AT+CIPMODE=1")){                                         //���յ�����AT+CIPMODE=1\r\\r\n\r\nOK\r\n
		u1_printf("͸��ģʽ�ɹ�\r\n");                                               //���������Ϣ
		CAT1_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",Aliyun_mqtt.ServerIP,Aliyun_mqtt.ServerPort); //����ָ����ӷ�����		
	}
	/*----------------------------------------------*/
	/*                 �����Ϸ�����                 */
	/*----------------------------------------------*/
	if(strstr((char *)data,"CONNECT")){                                             //���յ�����CONNECT
		u1_printf("�����Ϸ�����\r\n");                                              //���������Ϣ
		SysInit();                                                                  //���ڸ���ϵͳ�����Ľṹ���ʼ��
		SysCB.SysEventFlag |= CONNECT_EVENT;        	                            //��ʾ�����Ϸ������¼�����	
		MQTT_ConectPack();                           	                            //����Conect���ģ����뷢�ͻ�������׼������			
	}
	/*----------------------------------------------*/
	/*                 �����Ϸ�����                 */
	/*----------------------------------------------*/
	if(strstr((char *)data,"CLOSE\r\n")){                                           //���յ�����CLOSE\r\n
		u1_printf("�������Ͽ�������\r\n");                                          //���������Ϣ
		NVIC_SystemReset();                                                         //����	
	}
	/*----------------------------------------------*/
	/*               ����CONNACK����                */
	/*----------------------------------------------*/	
	if((data[0]==0x20)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //����������ݵĵ�1���ֽ���0x20 �� ��ǰ�������Ϸ������ģ�����if
		u1_printf("���յ�CONNACK����\r\n");                                          //���������Ϣ
		switch(data[3]){	 		                                                 //�������ݵĵ�4���ֽڣ���ʾCONNECT�����Ƿ�ɹ�
			case 0x00 : u1_printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
			            SysCB.SysEventFlag |= CONNECT_PACK;        	                 //��ʾMQTT CONNECT���ĳɹ��¼�����
						MQTT_Subscribe(Aliyun_mqtt.Stopic_Buff,TOPIC_NUM,1);         //����Subscribe���ģ����뷢�ͻ�������׼������
			            SysCB.PingTimer = HAL_GetTick();                             //��¼��ǰʱ��
						SysCB.SubONlineTimer = HAL_GetTick();                        //��¼��ǰʱ��
						SysCB.SubdataTimer = HAL_GetTick();                          //��¼��ǰʱ��
						break;                                                       //������֧case 0x00                                              
			case 0x01 : u1_printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
			            NVIC_SystemReset();                                          //����		
						break;                                                       //������֧case 0x01   
			case 0x02 : u1_printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
						NVIC_SystemReset();                                          //����		
						break;                                                       //������֧case 0x02 
			case 0x03 : u1_printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
						NVIC_SystemReset();                                          //����		
						break;                                                       //������֧case 0x03
			case 0x04 : u1_printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
						NVIC_SystemReset();                                          //����		
						break;                                                       //������֧case 0x04
			case 0x05 : u1_printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
						NVIC_SystemReset();                                          //����		
						break;                                                       //������֧case 0x05 		
			default   : u1_printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
						NVIC_SystemReset();                                          //����		
						break;                                                       //������֧default 								
		}		
	}
	/*----------------------------------------------*/
	/*                ����SUBACK����                */
	/*----------------------------------------------*/
	if((data[0]==0x90)&&(SysCB.SysEventFlag&CONNECT_PACK)){	                         //����������ݵĵ�1���ֽ���0x90 �� CONNECT���ķ��ͳɹ�
		u1_printf("���յ�SUBACK����\r\n");                                           //���������Ϣ
        for(i=0;i<datalen-4;i++){                                                    //ѭ����ѯ���Ľ��
			switch(data[4+i]){		                                                 //�ӵ�5���ֽڣ��Ƕ��Ľ�����ݣ�ÿ��topic��һ������ֽ�			
				case 0x00 :
				case 0x01 : u1_printf("��%d��Topic���ĳɹ�\r\n",i+1);                //���������Ϣ					           
							break;                                                   //������֧                                             
				default   : u1_printf("��%d��Topic����ʧ�ܣ�׼������\r\n");          //���������Ϣ 
							NVIC_SystemReset();                                      //����		
							break;                                                   //������֧ 								
			}
		}	
		u1_printf("ͬ����ǰ�����豸�̼��汾��\r\n");                                 //���������Ϣ 
		CAT1_PropertyVersion(AliInfoCB.Version_ABuff,"GATEWAY");                     //ͬ����ǰ�����豸�̼��汾��		
	}
	/*----------------------------------------------*/
	/*              ����PINGRESP����                */
	/*----------------------------------------------*/
    if((data[0]==0xD0)&&(SysCB.SysEventFlag&CONNECT_PACK)){	                         //����������ݵĵ�1���ֽ���0xD0 �� CONNECT���ķ��ͳɹ�
		u1_printf("���յ�PINGRESP����\r\n");                                         //���������Ϣ
		SysCB.SysEventFlag &=~ PING_SENT;                                            //���PING����������¼�������־			
	}
	/*----------------------------------------------*/
	/*              ����PUBACK����                  */
	/*----------------------------------------------*/
	if((data[0]==0x40)&&(SysCB.SysEventFlag&CONNECT_PACK)){	                         //����������ݵĵ�1���ֽ���0x40 �� CONNECT���ķ��ͳɹ�
		u1_printf("���յ�PUBACK����\r\n");                                           //���������Ϣ
        u1_printf("�ȼ�1 ��ʶ��0x%02x%02x�ı��ģ����ͳɹ�\r\n",data[2],data[3]);     //���������Ϣ 
		if((SysCB.SysEventFlag&OTA_EVENT)){                                          //���if��������ʾOTA�¼������ˣ���ʱ�յ��ľ���ͬ��OTA�汾�ŵĻظ�����
			SysCB.SysEventFlag &=~OTA_EVENT;                                         //����¼���־
			if(AliOTA.OTA_dev == 0){                                                 //���������OTA
				u1_printf("׼��������BootLoader���¹̼�\r\n");                       //���������Ϣ
				NVIC_SystemReset();                                                  //����������flsah
			}
		}
	}
	/*----------------------------------------------*/
	/*        ����������������ĵȼ�0����           */
	/*----------------------------------------------*/
	if((data[0]==0x30)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //����������ݵĵ�1���ֽ���0x30 �� ��ǰ�������Ϸ������ģ�����if
		u1_printf("���յ����������͵ĵȼ�0����\r\n");                                //���������Ϣ
        SysCB.PingTimer = HAL_GetTick();                                             //��¼��ǰʱ��
		MQTT_DealPushdatatopic_Qs0(data,datalen);	                                 //����ȼ�0���������ݣ���ȡ��������
        u1_printf("%s\r\n",Aliyun_mqtt.cmdbuff);                                     //����1�������
		
		/*----------------------------------------------*/
		/*             ��ȡ����OTA������Ϣ              */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"GATEWAY\"")){                    //���������"module":"GATEWAY",˵��ҪOTA���������豸
			//��ȡ3���ؼ�����
			//��OTA�����Ĺ̼����
			//��OTA�����Ĺ̼��ļ���
			//��OTA�����İ汾��
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //��λ����OTA������Ϣ��ʼ��
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"GATEWAY\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA�����Ĺ̼���ţ�%d\r\n",AliOTA.streamId);         //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ļ�����%d\r\n",AliOTA.streamFileId);   //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ܴ�С��%d\r\n",AliOTA.streamSize);     //���������Ϣ
					u1_printf("OTA�����İ汾�ţ�%s\r\n",AliOTA.OTA_Versionbuff);    //���������Ϣ
					AliOTA.OTA_dev = 0;                                             //AliOTA.OTA_dev����0�����������豸
					SysCB.SysEventFlag |= OTA_EVENT;                                //����OTA�¼�����
					SysCB.OTATimer = HAL_GetTick();                                 //��ȡ��ǰʱ��
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //������0�� ����64K�ռ�
					if((AliOTA.streamSize%256)!=0)                                  //�жϹ̼��ǲ���������256�������������ǽ���if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //�������ش���
					else                                                            //��֮������else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //�������ش���
					u1_printf("ÿ������256�ֽ����ݣ�������Ҫ���أ�%d��\r\n",AliOTA.OTA_timers);    //���������Ϣ
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num����1����ʼ��һ������	
					u1_printf("׼��OTA���ع̼���%d��\r\n",AliOTA.OTA_num);		                   //���������ʾ��Ϣ
					CAT1_OTADownload(256,0);                                                       //���ͱ��ģ����ع̼�
				}else u1_printf("��ȡ����OTA���ع̼�������Ϣ����ʧ��\r\n");		                   //���������ʾ��Ϣ
			}else u1_printf("��λ����OTA������Ϣ��ʼ��ʧ��\r\n");		                           //���������ʾ��Ϣ
		}
		/*----------------------------------------------*/
		/*          ��ȡ�����豸1 OTA������Ϣ           */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"SUB1\"")){                      //���������"module":"SUB1",˵��ҪOTA�������豸1�豸
			//��ȡ3���ؼ�����
			//��OTA�����Ĺ̼����
			//��OTA�����Ĺ̼��ļ���
			//��OTA�����İ汾��
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //��λ���豸1 OTA������Ϣ��ʼ��
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"SUB1\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA�����Ĺ̼���ţ�%d\r\n",AliOTA.streamId);         //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ļ�����%d\r\n",AliOTA.streamFileId);   //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ܴ�С��%d\r\n",AliOTA.streamSize);     //���������Ϣ
					u1_printf("OTA�����İ汾�ţ�%s\r\n",AliOTA.OTA_Versionbuff);    //���������Ϣ
					AliOTA.OTA_dev = 1;                                             //AliOTA.OTA_dev����1���������豸1
					SysCB.SysEventFlag |= OTA_EVENT;                                //����OTA�¼�����
					SysCB.OTATimer = HAL_GetTick();                                 //��ȡ��ǰʱ��
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //������1�� ����64K�ռ�
					if((AliOTA.streamSize%256)!=0)                                  //�жϹ̼��ǲ���������256�������������ǽ���if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //�������ش���
					else                                                            //��֮������else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //�������ش���
					u1_printf("ÿ������256�ֽ����ݣ�������Ҫ���أ�%d��\r\n",AliOTA.OTA_timers);    //���������Ϣ
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num����1����ʼ��һ������	
					u1_printf("׼��OTA���ع̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	   //���������ʾ��Ϣ
					CAT1_OTADownload(256,0);                                                       //���ͱ��ģ����ع̼�
				}else u1_printf("��ȡ�豸1 OTA���ع̼�������Ϣ����ʧ��\r\n");		               //���������ʾ��Ϣ
			}else u1_printf("��λ���豸1 OTA������Ϣ��ʼ��ʧ��\r\n");		                       //���������ʾ��Ϣ
		}
		/*----------------------------------------------*/
		/*          ��ȡ�����豸2 OTA������Ϣ           */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"SUB2\"")){                      //���������"module":"SUB2",˵��ҪOTA�������豸2�豸
			//��ȡ3���ؼ�����
			//��OTA�����Ĺ̼����
			//��OTA�����Ĺ̼��ļ���
			//��OTA�����İ汾��
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //��λ���豸2 OTA������Ϣ��ʼ��
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"SUB2\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA�����Ĺ̼���ţ�%d\r\n",AliOTA.streamId);         //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ļ�����%d\r\n",AliOTA.streamFileId);   //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ܴ�С��%d\r\n",AliOTA.streamSize);     //���������Ϣ
					u1_printf("OTA�����İ汾�ţ�%s\r\n",AliOTA.OTA_Versionbuff);    //���������Ϣ
					AliOTA.OTA_dev = 2;                                             //AliOTA.OTA_dev����2���������豸2
					SysCB.SysEventFlag |= OTA_EVENT;                                //����OTA�¼�����
					SysCB.OTATimer = HAL_GetTick();                                 //��ȡ��ǰʱ��
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //������2�� ����64K�ռ�
					if((AliOTA.streamSize%256)!=0)                                  //�жϹ̼��ǲ���������256�������������ǽ���if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //�������ش���
					else                                                            //��֮������else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //�������ش���
					u1_printf("ÿ������256�ֽ����ݣ�������Ҫ���أ�%d��\r\n",AliOTA.OTA_timers);    //���������Ϣ
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num����1����ʼ��һ������	
					u1_printf("׼��OTA���ع̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);    //���������ʾ��Ϣ
					CAT1_OTADownload(256,0);                                                       //���ͱ��ģ����ع̼�
				}else u1_printf("��ȡ���豸2 OTA���ع̼�������Ϣ����ʧ��\r\n");		               //���������ʾ��Ϣ
			}else u1_printf("��λ���豸2 OTA������Ϣ��ʼ��\r\n");		                           //���������ʾ��Ϣ
		}
		/*----------------------------------------------*/
		/*          ��ȡ�����豸3 OTA������Ϣ           */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"SUB3\"")){                      //���������"module":"SUB3",˵��ҪOTA�������豸3�豸
			//��ȡ3���ؼ�����
			//��OTA�����Ĺ̼����
			//��OTA�����Ĺ̼��ļ���
			//��OTA�����İ汾��
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //��λ�����豸3 OTA������Ϣ��ʼ��
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"SUB3\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA�����Ĺ̼���ţ�%d\r\n",AliOTA.streamId);         //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ļ�����%d\r\n",AliOTA.streamFileId);   //���������Ϣ
					u1_printf("OTA�����Ĺ̼��ܴ�С��%d\r\n",AliOTA.streamSize);     //���������Ϣ
					u1_printf("OTA�����İ汾�ţ�%s\r\n",AliOTA.OTA_Versionbuff);    //���������Ϣ
					AliOTA.OTA_dev = 3;                                             //AliOTA.OTA_dev����3���������豸3
					SysCB.SysEventFlag |= OTA_EVENT;                                //����OTA�¼�����
					SysCB.OTATimer = HAL_GetTick();                                 //��ȡ��ǰʱ��
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //������2�� ����64K�ռ�
					if((AliOTA.streamSize%256)!=0)                                  //�жϹ̼��ǲ���������256�������������ǽ���if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //�������ش���
					else                                                            //��֮������else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //�������ش���
					u1_printf("ÿ������256�ֽ����ݣ�������Ҫ���أ�%d��\r\n",AliOTA.OTA_timers);    //���������Ϣ
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num����1����ʼ��һ������	
					u1_printf("׼��OTA���ع̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	   //���������ʾ��Ϣ
					CAT1_OTADownload(256,0);                                                       //���ͱ��ģ����ع̼�
				}else u1_printf("��ȡ���豸3 OTA���ع̼�������Ϣ����ʧ��\r\n");		               //���������ʾ��Ϣ
			}else u1_printf("��λ���豸3 OTA������Ϣ��ʼ��ʧ��\r\n");		                       //���������ʾ��Ϣ
		}
		/*----------------------------------------------*/
		/*              ��ȡÿ�����صĹ̼�              */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"download_reply")){                              //���������download_reply,˵�������صĹ̼�����
			ptr = strstr(Aliyun_mqtt.cmdbuff,"download_reply");                        //��λ����λ��
			if(ptr != NULL){
				u1_printf("���յ�OTA���ع̼�\r\n");		                               //���������ʾ��Ϣ
				firminfo_len = ptr[14] *256 + ptr[15] + 2;                             //���صĹ̼�������Ϣռ�õĳ��� 
				memset(firminfo_buff,0,128);                                           //��ջ�����
				memcpy(firminfo_buff,&ptr[16],firminfo_len - 2);	     	           //�����̼�������Ϣ
				if(sscanf(firminfo_buff,"{\"code\":200,\"data\":{\"bOffset\":%d,\"fileLength\":%*d,\"bSize\":%d},\"id\":\"%*d\",\"message\":\"success\"}",&bOffset,&bSize)==2){ //��ȡ����
					u1_printf("�������ط�Χ��%d ~ %d\r\n",bOffset,bOffset+bSize-1);	   //���������ʾ��Ϣ
					W25Qxx_Write_Page((uint8_t *)&ptr[14+firminfo_len],(bOffset/256) + AliOTA.OTA_dev*256); //д���ⲿflash		
					AliOTA.OTA_num++;                                                  //AliOTA.OTA_num���ش�������1
					if(AliOTA.OTA_num<=AliOTA.OTA_timers){                             //��û��������ϣ�����if
						if(((AliOTA.streamSize-((AliOTA.OTA_num - 1)*256))/256)>0)     //�ǲ��ǹ�256���ǵĻ�����if
							tempsize = 256;                                            //����������256
						else                                                           //����256�Ļ�����else
							tempsize = AliOTA.streamSize-((AliOTA.OTA_num - 1)*256);   //����������
						tempoffset = ((AliOTA.OTA_num - 1)*256);                       //���㵱�����ص�ƫ����		
						u1_printf("׼��OTA���ع̼���%d/%d��\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	 //���������ʾ��Ϣ
						CAT1_OTADownload(tempsize,tempoffset);                         //���ͱ��ģ����ع̼�
					}else{                                                                                                   //���������ϣ�����else
						switch(AliOTA.OTA_dev){                                                                              //�ж������أ��������豸123
							case 0:	u1_printf("���� OTA�̼��������\r\n");	                                                 //���������ʾ��Ϣ   
									memset(AliInfoCB.Version_ABuff,0,VERSION_LEN+1);                                         //����̼��汾�Ż�����
									memcpy(AliInfoCB.Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));   //�����µİ汾��
									AliInfoCB.OTA_firelen = AliOTA.streamSize;                                               //��¼�̼��Ĵ�С
									AliInfoCB.OTA_flag = BOOT_STA_O;                                                         //�����ҪOTA
									EEprom_WriteData(0,&AliInfoCB,ALIINFO_STRUCT_LEN);                                       //������Ϣ��EEprom
									CAT1_PropertyVersion(AliInfoCB.Version_ABuff,"GATEWAY");		                         //ͬ����ǰ�����豸�̼��汾��
									break;
							case 1:	u1_printf("���豸1 OTA�̼��������\r\n");	                                             //���������ʾ��Ϣ   
									memset(AliInfoCB_SUB[0].Version_ABuff,0,VERSION_LEN+1);                                  //����̼��汾�Ż�����
									memcpy(AliInfoCB_SUB[0].Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));  //�����µİ汾��
									if((AliOTA.streamSize%128)!=0)                                                           //�жϹ̼��ǲ���������128�������������ǽ���if
										AliOTA.OTA_timers = AliOTA.streamSize/128 + 1;                                       //���㷢�ʹ���
									else                                                                                     //��֮������else
										AliOTA.OTA_timers = AliOTA.streamSize/128;                                           //���㷢�ʹ���
									u1_printf("ÿ�η���128�ֽ����ݣ�������Ҫ���ͣ�%d��\r\n",AliOTA.OTA_timers);              //���������Ϣ
									u1_printf("׼��OTA���͹̼���%d��\r\n",AliOTA.OTA_num);		                             //���������ʾ��Ϣ
									AliOTA.OTA_num = 1;                                                                      //AliOTA.OTA_num����1����ʼ��һ�η���
									memset(databuff,0,256);                                                                  //��ջ�����
									databuff[0] = SUB1_HADR;                                                                 //���շ� ��ַ���ֽ�
									databuff[1] = SUB1_LADR;                                                                 //���շ� ��ַ���ֽ�
									databuff[2] = SUB1_CH;                                              	                 //���շ� �ŵ�		
									databuff[3] = 0x5A;                                                                      //�̶��ֽ�
									databuff[4] = 0xA5;                                                                      //�̶��ֽ�
									databuff[5] = AliOTA.OTA_num/256;                                                        //���ݰ���Ÿ��ֽ�	
									databuff[6] = AliOTA.OTA_num%256;                                                        //���ݰ���ŵ��ֽ�										
									W25Qxx_Read((uint8_t *)&databuff[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);//��ȡ����
									u3_TxDataBuf((uint8_t *)databuff,135);                                                   //������ݵ����ͻ�����	
									break;
							case 2:	u1_printf("���豸2 OTA�̼��������\r\n");	                                             //���������ʾ��Ϣ   
									memset(AliInfoCB_SUB[1].Version_ABuff,0,VERSION_LEN+1);                                  //����̼��汾�Ż�����
									memcpy(AliInfoCB_SUB[1].Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));  //�����µİ汾��
									if((AliOTA.streamSize%128)!=0)                                                           //�жϹ̼��ǲ���������128�������������ǽ���if
										AliOTA.OTA_timers = AliOTA.streamSize/128 + 1;                                       //���㷢�ʹ���
									else                                                                                     //��֮������else
										AliOTA.OTA_timers = AliOTA.streamSize/128;                                           //���㷢�ʹ���
									u1_printf("ÿ�η���128�ֽ����ݣ�������Ҫ���ͣ�%d��\r\n",AliOTA.OTA_timers);              //���������Ϣ
									u1_printf("׼��OTA���͹̼���%d��\r\n",AliOTA.OTA_num);		                             //���������ʾ��Ϣ
									AliOTA.OTA_num = 1;                                                                      //AliOTA.OTA_num����1����ʼ��һ�η���
									memset(databuff,0,256);                                                                  //��ջ�����
									databuff[0] = SUB2_HADR;                                                                 //���շ� ��ַ���ֽ�
									databuff[1] = SUB2_LADR;                                                                 //���շ� ��ַ���ֽ�
									databuff[2] = SUB2_CH;                                              	                 //���շ� �ŵ�		
									databuff[3] = 0x5A;                                                                      //�̶��ֽ�
									databuff[4] = 0xA5;                                                                      //�̶��ֽ�
									databuff[5] = AliOTA.OTA_num/256;                                                        //���ݰ���Ÿ��ֽ�		
									databuff[6] = AliOTA.OTA_num%256;                                                        //���ݰ���ŵ��ֽ�										
									W25Qxx_Read((uint8_t *)&databuff[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);//��ȡ����
									u3_TxDataBuf((uint8_t *)databuff,135);                                                   //������ݵ����ͻ�����	
									break;
							case 3:	u1_printf("���豸3 OTA�̼��������\r\n");	                                             //���������ʾ��Ϣ   
									memset(AliInfoCB_SUB[2].Version_ABuff,0,VERSION_LEN+1);                                  //����̼��汾�Ż�����
									memcpy(AliInfoCB_SUB[2].Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));  //�����µİ汾��
									if((AliOTA.streamSize%128)!=0)                                                           //�жϹ̼��ǲ���������128�������������ǽ���if
										AliOTA.OTA_timers = AliOTA.streamSize/128 + 1;                                       //���㷢�ʹ���
									else                                                                                     //��֮������else
										AliOTA.OTA_timers = AliOTA.streamSize/128;                                           //���㷢�ʹ���
									u1_printf("ÿ�η���128�ֽ����ݣ�������Ҫ���ͣ�%d��\r\n",AliOTA.OTA_timers);              //���������Ϣ
									u1_printf("׼��OTA���͹̼���%d��\r\n",AliOTA.OTA_num);		                             //���������ʾ��Ϣ
									AliOTA.OTA_num = 1;                                                                      //AliOTA.OTA_num����1����ʼ��һ�η���
									memset(databuff,0,256);                                                                  //��ջ�����
									databuff[0] = SUB3_HADR;                                                                 //���շ� ��ַ���ֽ�
									databuff[1] = SUB3_LADR;                                                                 //���շ� ��ַ���ֽ�
									databuff[2] = SUB3_CH;                                              	                 //���շ� �ŵ�		
									databuff[3] = 0x5A;                                                                      //�̶��ֽ�
									databuff[4] = 0xA5;                                                                      //�̶��ֽ�
									databuff[5] = AliOTA.OTA_num/256;                                                        //���ݰ���Ÿ��ֽ�		
									databuff[6] = AliOTA.OTA_num%256;                                                        //���ݰ���ŵ��ֽ�										
									W25Qxx_Read((uint8_t *)&databuff[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);//��ȡ����
									u3_TxDataBuf((uint8_t *)databuff,135);                                                   //������ݵ����ͻ�����	
									break;
						}
					}
				}else{  
					SysCB.SysEventFlag &=~OTA_EVENT;                 //����¼���־
					u1_printf("��ȡ���ع̼�������Ϣ����ʧ��\r\n");	 //���������ʾ��Ϣ	
				}
			}
		}
		/*----------------------------------------------*/
		/*             ���豸���������               */
		/*----------------------------------------------*/		
		if(strstr(Aliyun_mqtt.cmdbuff,"login_reply{\"code\":200")){	                 //�������������login_reply{"code":200��˵�������豸���߳ɹ�	
			if(strstr(Aliyun_mqtt.cmdbuff,"D001")){	                                 //�������������D001��˵�������豸1���߳ɹ�	
				u1_printf("���豸1���߳ɹ�\r\n");                                    //���������Ϣ
				SysCB.SysEventFlag |= SUB_DEVICE1_ON;                                //���豸1���߳ɹ� ��־��λ
				CAT1_PropertyVersion(AliInfoCB_SUB[0].Version_ABuff,"SUB1");		 //ͬ�����豸1��ǰ�̼��汾��	
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"D002")){	                                 //�������������D002��˵�������豸2���߳ɹ�	
				u1_printf("���豸2���߳ɹ�\r\n");                                    //���������Ϣ
				SysCB.SysEventFlag |= SUB_DEVICE2_ON;                                //���豸2���߳ɹ� ��־��λ
				CAT1_PropertyVersion(AliInfoCB_SUB[1].Version_ABuff,"SUB2");		 //ͬ�����豸2��ǰ�̼��汾��
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"D003")){	                                 //�������������D003��˵�������豸3���߳ɹ�	
				u1_printf("���豸3���߳ɹ�\r\n");                                    //���������Ϣ
				SysCB.SysEventFlag |= SUB_DEVICE3_ON;                                //���豸3���߳ɹ� ��־��λ
				CAT1_PropertyVersion(AliInfoCB_SUB[2].Version_ABuff,"SUB3");		 //ͬ�����豸3��ǰ�̼��汾��	
			}
		}
		/*----------------------------------------------*/
		/*            ���豸1���������               */
		/*----------------------------------------------*/	
		memset(databuff,0,256);                                                      //��ջ�����
		sprintf(databuff,"/sys/%s/%s/thing/service/property/set",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff);
		if(strstr(Aliyun_mqtt.cmdbuff,databuff)){	                                 //�������������databuff�����ݣ�˵�������豸1�����·�
			u1_printf("���豸1�����·�\r\n");                                        //���������Ϣ
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":0")){                   //���������"PowerSwitch_1":0 ˵���������·��رտ�������				
				AttributeCB[0].Switch1Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":1")){                   //���������"PowerSwitch_1":1 ˵���������·��򿪿�������
				AttributeCB[0].Switch1Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":0")){                   //���������"PowerSwitch_2":0 ˵���������·��رտ�������				
				AttributeCB[0].Switch2Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":1")){                   //���������"PowerSwitch_2":1 ˵���������·��򿪿�������
				AttributeCB[0].Switch2Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":0")){                   //���������"PowerSwitch_3":0 ˵���������·��رտ�������				
				AttributeCB[0].Switch3Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":1")){                   //���������"PowerSwitch_3":1 ˵���������·��򿪿�������
				AttributeCB[0].Switch3Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":0")){                   //���������"PowerSwitch_4":0 ˵���������·��رտ�������				
				AttributeCB[0].Switch4Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":1")){                   //���������"PowerSwitch_4":1 ˵���������·��򿪿�������
				AttributeCB[0].Switch4Sta = 1;	                                     //��¼����״̬
			}
			memset(databuff,0,256);                                                  //��ջ�����
			databuff[0] = SUB1_HADR;                                                 //���շ� ��ַ���ֽ�
			databuff[1] = SUB1_LADR;                                                 //���շ� ��ַ���ֽ�
			databuff[2] = SUB1_CH;                                           	     //���շ� �ŵ�		
			sprintf(&databuff[3],"AT+PowerSwitch1=%d%d%d%d\r\n",AttributeCB[0].Switch1Sta,AttributeCB[0].Switch2Sta,AttributeCB[0].Switch3Sta,AttributeCB[0].Switch4Sta);
			u3_TxDataBuf((uint8_t *)databuff,3+strlen(&databuff[3]));                //������ݵ����ͻ�����		
		}
		/*----------------------------------------------*/
		/*            ���豸2���������               */
		/*----------------------------------------------*/	
		memset(databuff,0,64);                                                       //��ջ�����
		sprintf(databuff,"/sys/%s/%s/thing/service/property/set",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff);
		if(strstr(Aliyun_mqtt.cmdbuff,databuff)){	                                 //�������������databuff�����ݣ�˵�������豸2�����·�
			u1_printf("���豸2�����·�\r\n");                                        //���������Ϣ
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":0")){                   //���������"PowerSwitch_1":0 ˵���������·��رտ�������				
				AttributeCB[1].Switch1Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":1")){                   //���������"PowerSwitch_1":1 ˵���������·��򿪿�������
				AttributeCB[1].Switch1Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":0")){                   //���������"PowerSwitch_2":0 ˵���������·��رտ�������				
				AttributeCB[1].Switch2Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":1")){                   //���������"PowerSwitch_2":1 ˵���������·��򿪿�������
				AttributeCB[1].Switch2Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":0")){                   //���������"PowerSwitch_3":0 ˵���������·��رտ�������				
				AttributeCB[1].Switch3Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":1")){                   //���������"PowerSwitch_3":1 ˵���������·��򿪿�������
				AttributeCB[1].Switch3Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":0")){                   //���������"PowerSwitch_4":0 ˵���������·��رտ�������				
				AttributeCB[1].Switch4Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":1")){                   //���������"PowerSwitch_4":1 ˵���������·��򿪿�������
				AttributeCB[1].Switch4Sta = 1;	                                     //��¼����״̬
			}
			memset(databuff,0,256);                                                  //��ջ�����
			databuff[0] = SUB2_HADR;                                                 //���շ� ��ַ���ֽ�
			databuff[1] = SUB2_LADR;                                                 //���շ� ��ַ���ֽ�
			databuff[2] = SUB2_CH;                                           	     //���շ� �ŵ�			
			sprintf(&databuff[3],"AT+PowerSwitch2=%d%d%d%d\r\n",AttributeCB[1].Switch1Sta,AttributeCB[1].Switch2Sta,AttributeCB[1].Switch3Sta,AttributeCB[1].Switch4Sta);
			u3_TxDataBuf((uint8_t *)databuff,3+strlen(&databuff[3]));                //������ݵ����ͻ�����	
		}	
		/*----------------------------------------------*/
		/*            ���豸3���������               */
		/*----------------------------------------------*/	
		memset(databuff,0,64);                                                       //��ջ�����
		sprintf(databuff,"/sys/%s/%s/thing/service/property/set",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff);
		if(strstr(Aliyun_mqtt.cmdbuff,databuff)){	                                 //�������������databuff�����ݣ�˵�������豸3�����·�
			u1_printf("���豸3�����·�\r\n");                                        //���������Ϣ
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":0")){                   //���������"PowerSwitch_1":0 ˵���������·��رտ�������				
				AttributeCB[2].Switch1Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":1")){                   //���������"PowerSwitch_1":1 ˵���������·��򿪿�������
				AttributeCB[2].Switch1Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":0")){                   //���������"PowerSwitch_2":0 ˵���������·��رտ�������				
				AttributeCB[2].Switch2Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":1")){                   //���������"PowerSwitch_2":1 ˵���������·��򿪿�������
				AttributeCB[2].Switch2Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":0")){                   //���������"PowerSwitch_3":0 ˵���������·��رտ�������				
				AttributeCB[2].Switch3Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":1")){                   //���������"PowerSwitch_3":1 ˵���������·��򿪿�������
				AttributeCB[2].Switch3Sta = 1;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":0")){                   //���������"PowerSwitch_4":0 ˵���������·��رտ�������				
				AttributeCB[2].Switch4Sta = 0;	                                     //��¼����״̬
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":1")){                   //���������"PowerSwitch_4":1 ˵���������·��򿪿�������
				AttributeCB[2].Switch4Sta = 1;	                                     //��¼����״̬
			}
			memset(databuff,0,256);                                                  //��ջ�����
			databuff[0] = SUB3_HADR;                                                 //���շ� ��ַ���ֽ�
			databuff[1] = SUB3_LADR;                                                 //���շ� ��ַ���ֽ�
			databuff[2] = SUB3_CH;                                           	     //���շ� �ŵ�		
			sprintf(&databuff[3],"AT+PowerSwitch3=%d%d%d%d\r\n",AttributeCB[2].Switch1Sta,AttributeCB[2].Switch2Sta,AttributeCB[2].Switch3Sta,AttributeCB[2].Switch4Sta);
			u3_TxDataBuf((uint8_t *)databuff,3+strlen(&databuff[3]));                //������ݵ����ͻ�����	
		}	
	}
}
/*-------------------------------------------------*/
/*�������������¼�                                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void ActiveEvent(void)
{			
	/*---------------------------------------------*/
	/*               ��ѯOTA�Ƿ�ʱ               */
	/*---------------------------------------------*/
	if(SysCB.SysEventFlag&OTA_EVENT){                                            
		if((HAL_GetTick() - SysCB.OTATimer) >= 180000){                      //����120s ��û�и������
			SysCB.SysEventFlag &=~OTA_EVENT;                                 //���OTA�¼���־		
		}
	}	
	/*---------------------------------------------*/
	/*             ����PING���ݰ�ʱ�䵽            */
	/*---------------------------------------------*/
	if((HAL_GetTick() - SysCB.PingTimer >=60000)&&(SysCB.SysEventFlag&CONNECT_PACK)){                             
		SysCB.PingTimer = HAL_GetTick();                                     //��¼��ǰʱ��
		if(SysCB.SysEventFlag&PING_SENT){                                    //�ж�PING_SENT��־�������λ������if��˵���ϴ���PING���ݰ����ͺ�û���յ���������PINGRESP�ظ��������ܵ�����
			u1_printf("���ӵ��ߣ�����\r\n");                                 //���������Ϣ
			NVIC_SystemReset();                                              //����             
		}else{				
			MQTT_PingREQ();                                                  //��һ��PING���ݰ����뷢�ͻ�����			
			SysCB.SysEventFlag |= PING_SENT;                                 //��ʾPING����������¼�����				
		}	
	} 
	/*---------------------------------------------*/
	/*           ��ѯ���豸����ʱ�䵽              */
	/*---------------------------------------------*/
	if(((HAL_GetTick() - SysCB.SubONlineTimer) >=27000)&&(!(SysCB.SysEventFlag&OTA_EVENT))&&(SysCB.SysEventFlag&CONNECT_PACK)){                    
		SysCB.SubONlineTimer = HAL_GetTick();                                //��¼��ǰʱ��
		switch(online_counter){                                              //�жϲ�ѯ�ڼ������豸
			case 1: if(!(SysCB.SysEventFlag&SUB_DEVICE1_ON)){                //���豸1�Ƿ�����
						memset(databuff,0,256);                              //��ջ�����
						databuff[0] = SUB1_HADR;                             //���շ� ��ַ���ֽ�
						databuff[1] = SUB1_LADR;                             //���շ� ��ַ���ֽ�
						databuff[2] = SUB1_CH;                               //���շ� �ŵ�
						memcpy(&databuff[3],"AT+CER1\r\n",9);                //�������
						u3_TxDataBuf((uint8_t *)databuff,12);                //������ݵ����ͻ�����
						online_counter = 2;	                                 //�´β�ѯ���豸2
						break;                                               //����
					}
			case 2: if(!(SysCB.SysEventFlag&SUB_DEVICE2_ON)){                //���豸2�Ƿ�����
						memset(databuff,0,256);                              //��ջ�����
						databuff[0] = SUB2_HADR;                             //���շ� ��ַ���ֽ�
						databuff[1] = SUB2_LADR;                             //���շ� ��ַ���ֽ�
						databuff[2] = SUB2_CH;                               //���շ� �ŵ�
						memcpy(&databuff[3],"AT+CER2\r\n",9);                //�������
						u3_TxDataBuf((uint8_t *)databuff,12);                //������ݵ����ͻ�����
						online_counter = 3;	                                 //�´β�ѯ���豸3
						break;                                               //����
					}
			case 3: if(!(SysCB.SysEventFlag&SUB_DEVICE3_ON)){                //���豸3�Ƿ�����
						memset(databuff,0,256);                              //��ջ�����
						databuff[0] = SUB3_HADR;                             //���շ� ��ַ���ֽ�
						databuff[1] = SUB3_LADR;                             //���շ� ��ַ���ֽ�
						databuff[2] = SUB3_CH;                               //���շ� �ŵ�
						memcpy(&databuff[3],"AT+CER3\r\n",9);                //�������
						u3_TxDataBuf((uint8_t *)databuff,12);                //������ݵ����ͻ�����
						online_counter = 1;	                                 //�´β�ѯ���豸1
						break;                                               //����
					}
			default:online_counter = 1;	                                     //�´β�ѯ���豸1
					break;                                                   //����
		}
	}   
	/*---------------------------------------------*/
	/*      ���豸�ϴ�����ʱ�䵽�����β�ѯ��       */
	/*---------------------------------------------*/
	if(((HAL_GetTick() - SysCB.SubdataTimer) >=10000)&&(!(SysCB.SysEventFlag&OTA_EVENT))&&(SysCB.SysEventFlag&CONNECT_PACK)){ 
		SysCB.SubdataTimer = HAL_GetTick();                                  //��¼��ǰʱ��	
		switch(data_counter){                                                //�жϵڼ������豸                          
			case 1: if(SysCB.SysEventFlag&SUB_DEVICE1_ON){                   //���豸1�Ƿ�����
						memset(databuff,0,256);                              //��ջ�����
						databuff[0] = SUB1_HADR;                             //���շ� ��ַ���ֽ�
						databuff[1] = SUB1_LADR;                             //���շ� ��ַ���ֽ�
						databuff[2] = SUB1_CH;                               //���շ� �ŵ�
						memcpy(&databuff[3],"AT+DATA1\r\n",10);              //�������
						u3_TxDataBuf((uint8_t *)databuff,13);                //������ݵ����ͻ�����
						data_counter = 2;	                                 //�´����豸2	
						break;                                               //����
					}
			case 2: if(SysCB.SysEventFlag&SUB_DEVICE2_ON){                   //���豸2�Ƿ�����
						memset(databuff,0,256);                              //��ջ�����
						databuff[0] = SUB2_HADR;                             //���շ� ��ַ���ֽ�
						databuff[1] = SUB2_LADR;                             //���շ� ��ַ���ֽ�
						databuff[2] = SUB2_CH;                               //���շ� �ŵ�
						memcpy(&databuff[3],"AT+DATA2\r\n",10);              //�������
						u3_TxDataBuf((uint8_t *)databuff,13);                //������ݵ����ͻ�����
						data_counter = 3;	                                 //�´����豸3
						break;                                               //����
					}
			case 3: if(SysCB.SysEventFlag&SUB_DEVICE3_ON){                   //���豸3�Ƿ�����
						memset(databuff,0,256);                              //��ջ�����
						databuff[0] = SUB3_HADR;                             //���շ� ��ַ���ֽ�
						databuff[1] = SUB3_LADR;                             //���շ� ��ַ���ֽ�
						databuff[2] = SUB3_CH;                               //���շ� �ŵ�
						memcpy(&databuff[3],"AT+DATA3\r\n",10);              //�������
						u3_TxDataBuf((uint8_t *)databuff,13);                //������ݵ����ͻ�����
						data_counter = 1;	                                 //�´����豸1
						break;                                               //����
					}		
			default:data_counter = 1;	                                     //�´����豸1
					break;
		}
	} 	
}
/*-------------------------------------------------*/
/*����������������ϴ��汾��                       */
/*��  ����ver���汾��                              */
/*��  ����module��ģ����                           */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CAT1_PropertyVersion(uint8_t * ver,uint8_t * module)
{	
	char topicdatabuff[64];                                                                             //���ڹ�������topic�Ļ�����
    char tempdatabuff[128];                                                                             //������ʱ�������ݵĻ�����
	
	memset(topicdatabuff,0,64);                                                                         //�����ʱ������	
	sprintf(topicdatabuff,"/ota/device/inform/%s/%s",PRODUCTKEY,DEVICENAME);                            //��������topic					
	memset(tempdatabuff,0,128);                                                                         //�����ʱ������	
	sprintf(tempdatabuff,"{\"id\": 1,\"params\":{\"version\":\"%s\",\"module\":\"%s\"}}",ver,module);   //��������	
	MQTT_PublishQs1(topicdatabuff,tempdatabuff,strlen(tempdatabuff));                                   //�ȼ�1��PUBLISH���ģ����뷢�ͻ�����   	
}
/*-------------------------------------------------*/
/*��������OTA���ع̼�                              */
/*��  ����size �����ض�������                      */
/*��  ����offset ������ƫ����                      */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CAT1_OTADownload(int size, int offset)
{	
	char topicdatabuff[64];        													    //���ڹ�������topic�Ļ�����
    char tempdatabuff[128];        													    //������ʱ�������ݵĻ�����
	
	memset(topicdatabuff,0,64);                                                         //�����ʱ������	
	sprintf(topicdatabuff,"/sys/%s/%s/thing/file/download",PRODUCTKEY,DEVICENAME);      //��������topic					
	memset(tempdatabuff,0,128);                                                         //�����ʱ������	
	u1_printf("OTA�����Ĺ̼���ţ�%d\r\n",AliOTA.streamId);                             //���������Ϣ
	HAL_Delay(300);
	sprintf(tempdatabuff,"{\"id\": \"1\",\"params\": {\"fileInfo\":{\"streamId\":%d,\"fileId\":%d},\"fileBlock\":{\"size\":%d,\"offset\":%d}}}",AliOTA.streamId,AliOTA.streamFileId,size,offset);   //��������	
	MQTT_PublishQs0(topicdatabuff,tempdatabuff,strlen(tempdatabuff));                   //�ȼ�0��PUBLISH���ģ����뷢�ͻ�����   	
}
/*-------------------------------------------------*/
/*�����������豸����                              */
/*��  ����postdata ���ϴ�������                    */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CAT1_SubDeviceLogin(char * postdata)
{	
	char topicdatabuff[64];                                                              //���ڹ�������topic�Ļ�����

	memset(topicdatabuff,0,64);                                                          //�����ʱ������	
	sprintf(topicdatabuff,"/ext/session/%s/%s/combine/login",PRODUCTKEY,DEVICENAME);     //��������topic					
	MQTT_PublishQs0(topicdatabuff,postdata,strlen(postdata));                            //�ȼ�0��PUBLISH���ģ����뷢�ͻ�����   	
}
/*-------------------------------------------------*/
/*�����������豸�����ϴ�                           */
/*��  ����postdata ���ϴ�������                    */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CAT1_SubDevicePost(char * postdata)
{	
	char topicdatabuff[64];                                                                      //���ڹ�������topic�Ļ�����

	memset(topicdatabuff,0,64);                                                                  //�����ʱ������	
	sprintf(topicdatabuff,"/sys/%s/%s/thing/event/property/pack/post",PRODUCTKEY,DEVICENAME);    //��������topic					
	MQTT_PublishQs0(topicdatabuff,postdata,strlen(postdata));                                    //�ȼ�0��PUBLISH���ģ����뷢�ͻ�����   	
}
