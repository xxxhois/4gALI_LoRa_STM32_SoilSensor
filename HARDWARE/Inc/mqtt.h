/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*            实现MQTT协议功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __MQTT_H
#define __MQTT_H
         
#define  MQTT_TxData(x)       u2_TxData(x)           //串口2控制数据
#define  TxDataBuf_Deal       u2_TxDataBuf           //放入串口2缓冲区

#define  PRODUCTKEY           AliInfoCB.ProductKeyBuff       //产品KEY
#define  DEVICENAME  		  AliInfoCB.DeviceNameBuff       //设备名  
#define  DEVICESECRE  		  AliInfoCB.DeviceSecretBuff     //设备秘钥   
#define  TOPIC_NUM    		  7                              //需要订阅的最大Topic数量
#define  TOPIC_SIZE   		  64                             //存放Topic字符串名称缓冲区长度
#define  PACK_SIZE    		  512                            //存放报文数据缓冲区大小
#define  CLIENTID_SIZE        64                             //存放客户端ID的缓冲区大小
#define  USERNAME_SIZE        64                             //存放用户名的缓冲区大小
#define  PASSWARD_SIZE        64                             //存放密码的缓冲区大小
#define  SERVERIP_SIZE        64                             //存放服务器IP或是域名缓冲区大小
#define  CMD_SIZE             512                            //保存推送的PUBLISH报文中的数据缓冲区大小

typedef struct{
	char ClientID[CLIENTID_SIZE];                            //存放客户端ID的缓冲区
	char Username[USERNAME_SIZE];                            //存放用户名的缓冲区
	char Passward[PASSWARD_SIZE];                            //存放密码的缓冲区
	char ServerIP[SERVERIP_SIZE];                            //存放服务器IP或是域名缓冲区
	unsigned char Pack_buff[PACK_SIZE];        				 //存放报文数据缓冲区
	unsigned short int  ServerPort;                  	     //存放服务器的端口号	
	unsigned short int  MessageID;                           //记录报文标识符
	unsigned int  Fixed_len;                       	         //固定报头长度
	unsigned int  Variable_len;                              //可变报头长度
	unsigned int  Payload_len;                               //有效负荷长度
	char Stopic_Buff[TOPIC_NUM][TOPIC_SIZE];                 //包含的是订阅的主题列表
	char cmdbuff[CMD_SIZE];                                  //保存推送的PUBLISH报文中的数据缓冲区
}MQTT_CB;  
#define MQTT_CB_LEN         sizeof(MQTT_CB)                  //结构体长度 

extern MQTT_CB   Aliyun_mqtt;                                //外部变量声明，用于连接阿里云mqtt的结构体

void IoT_Parameter_Init(void);                                                   			 //函数声明，云服务器初始化参数，得到客户端ID，用户名和密码
void MQTT_ConectPack(void);                                                      			 //函数声明，MQTT CONNECT报文       鉴权连接
void MQTT_DISCONNECT(void);                                                       			 //函数声明，MQTT DISCONNECT报文    断开连接
void MQTT_Subscribe(char topicbuff[TOPIC_NUM][TOPIC_SIZE],unsigned char, unsigned char);     //函数声明，MQTT SUBSCRIBE报文     订阅Topic
void MQTT_UNSubscribe(char *);                                                   			 //函数声明，MQTT UNSUBSCRIBE报文   取消订阅Topic
void MQTT_PingREQ(void);                                                          			 //函数声明，MQTT PING报文          保活心跳包
void MQTT_PublishQs0(char *,char *,int);                                          			 //函数声明，MQTT PUBLISH报文 等级0 发布数据
void MQTT_PublishQs1(char *,char *,int);                                         			 //函数声明，MQTT PUBLISH报文 等级1 发布数据
void MQTT_DealPushdatatopic_Qs0(unsigned char *, int);                            		   	 //函数声明，处理服务器发来的等级0的推送数据，附带topic信息

#endif
