/*-----------------------------------------------------*/
/*              超子说物联网STM32系列开发板            */
/*-----------------------------------------------------*/
/*                                                     */
/*                程序中各种配置信息头文件             */
/*                                                     */
/*-----------------------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H

//---------------------------------------------------------------//
//--------------------------子设备信息---------------------------//
//---------------------------------------------------------------//
#define SUB_DEVICE_NUM       3                                                    //子设备个数

#define VALID_ALICER1_DATA      0xA1                                              //表示 子设备1 证书信息
#define SUB1_DATA               0xB1                                              //表示 子设备1 数据
#define SUB1_SWITCH_DATA        0xC1                                              //表示 子设备1 回复开关下发命令
#define SUB1_OTA_S              0xD1                                              //表示 子设备1 接收了OTA一包数据
#define SUB1_OTA_C              0xE1                                              //表示 子设备1 要求重发OTA一包数据

#define VALID_ALICER2_DATA      0xA2                                              //表示 子设备2 证书信息
#define SUB2_DATA               0xB2                                              //表示 子设备2 数据
#define SUB2_SWITCH_DATA        0xC2                                              //表示 子设备2 回复开关下发命令
#define SUB2_OTA_S              0xD2                                              //表示 子设备2 接收了OTA一包数据
#define SUB2_OTA_C              0xE2                                              //表示 子设备2 要求重发OTA一包数据

#define VALID_ALICER3_DATA      0xA3                                              //表示 子设备3 证书信息
#define SUB3_DATA               0xB3                                              //表示 子设备3 数据
#define SUB3_SWITCH_DATA        0xC3                                              //表示 子设备3 回复开关下发命令
#define SUB3_OTA_S              0xD3                                              //表示 子设备3 接收了OTA一包数据
#define SUB3_OTA_C              0xE3                                              //表示 子设备3 要求重发OTA一包数据

/*---------------------------------------------------------------*/
/*-----------EEPROM内保存的阿里云证书信息有效长度----------------*/
/*---------------------------------------------------------------*/
#define VALID_ALICER_DATA      0xDD                                            //网关板 表示三元组+固件版本有效
#define PRODUCTKEY_LEN         11                                              //三元组 ProductKey     字符串长度        
#define DEVICENAME_LEN         32                                              //三元组 DeviceName     字符串长度        
#define DEVICESECRET_LEN       32                                              //三元组 DeviceSecret   字符串长度                
#define VERSION_LEN            23                                              //A区应用程序固件版本号字符串长度

//---------------------------------------------------------------//
//------------------启动信息字节，表示含义-----------------------//
//---------------------------------------------------------------//
#define BOOT_STA_O             0x0AA0C00C                                      //表示需要OTA更新应用程序

/*---------------------------------------------------------------*/
/*--------------EEPROM内保存阿里云证书信息的结构体---------------*/
/*---------------------------------------------------------------*/
typedef struct{
	uint8_t  valid_cer;                                                         //保存有效标志变量 如果等于 VALID_CER_DATA 说明信息有效 不等于的话 需要重新设置
	uint8_t  ProductKeyBuff[PRODUCTKEY_LEN+1];                                  //保存ProductKey的缓冲区
	uint8_t  DeviceNameBuff[DEVICENAME_LEN+1];                                  //保存DeviceName的缓冲区
	uint8_t  DeviceSecretBuff[DEVICESECRET_LEN+1];                              //保存DeviceSecret的缓冲区
	uint8_t  Version_ABuff[VERSION_LEN+1];                                      //保存应用A区程序固件版本的缓冲区
	uint32_t OTA_firelen;                                                       //OTA固件大小
	uint32_t OTA_flag;                                                          //是否需要OTA的标识
}AliyunInfo_CB;   
#define ALIINFO_STRUCT_LEN        sizeof(AliyunInfo_CB)                         //EEPROM内保存阿里云证书信息的结构体 长度 
/*---------------------------------------------------------------*/
/*-------------------用于各种系统参数的结构体--------------------*/
/*---------------------------------------------------------------*/
typedef struct{
	uint32_t SysEventFlag;                                                       //发生各种事件的标志变量
	uint32_t PingTimer;                                                          //用于记录发送PING数据包的计时器 
	int32_t  OTATimer;                                                           //OTA升级超时计数器
	int32_t  SubONlineTimer;                                                     //查询子设备是上线定时器		
	int32_t  SubdataTimer;            										     //用于子设备上报数据的定时器
}Sys_CB;
#define SYS_STRUCT_LEN         sizeof(Sys_CB)                                   //用于各种系统参数的 Sys_CB结构体 长度 
	
/*---------------------------------------------------------------*/
/*-----------------------系统事件发生标志定义--------------------*/
/*---------------------------------------------------------------*/
#define CONNECT_EVENT         0x40000000          						        //表示连接上服务器事件发生
#define CONNECT_PACK          0x20000000          						        //表示MQTT CONNECT报文成功事件发生
#define PING_SENT             0x10000000          						        //表示PING保活包发送事件发生
#define OTA_EVENT             0x08000000          						        //表示OTA事件发生
#define SMART_EVENT           0x04000000          						        //wifi配网事件发生
#define SMART_GOING_EVENT     0x02000000                                        //wifi配网启动事件发生
#define SUB_DEVICE1_ON        0x00000001                                        //子设备1在线事件发生
#define SUB_DEVICE2_ON        0x00000002                                        //子设备2在线事件发生
#define SUB_DEVICE3_ON        0x00000004                                        //子设备3在线事件发生
/*---------------------------------------------------------------*/
/*-------------------------  分  割  线  ------------------------*/
/*--------------  模板程序中 分割线以上部分 不修改 --------------*/
/*-------  模板程序中 分割线以下部分 根据不同的功能修改 ---------*/
/*---------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/*----------------------自定义事件发生标志定义-------------------*/
/*---------------------------------------------------------------*/
//根据功能自行添加，注意不要和系统事件发生标志定义的数值一样
//需要多少，自行添加
#define UP_CER_EVENT          0x00000001          //表示查询子设备上线
#define UP_SUB1_EVENT         0x00000002          //表示该子设备1上传数据事件发生
#define UP_SUB2_EVENT         0x00000004          //表示该子设备2上传数据事件发生
#define UP_SUB3_EVENT         0x00000008          //表示该子设备3上传数据事件发生

/*---------------------------------------------------------------*/
/*-----------------------功能属性标识符宏定义--------------------*/
/*---------------------------------------------------------------*/
//所有程序中需要用到的功能属性的标识符在此定义
//标识符名称必须和服务器后台设置的完全一样，大小写也必须一样
#define ATTRIBUTE1            "PowerSwitch_1"       //功能属性1标识符
#define ATTRIBUTE2            "PowerSwitch_2"       //功能属性2标识符
#define ATTRIBUTE3            "PowerSwitch_3"       //功能属性2标识符
#define ATTRIBUTE4            "PowerSwitch_4"       //功能属性2标识符
#define ATTRIBUTE5            "temperature"         //功能属性3标识符
#define ATTRIBUTE6            "humidity"            //功能属性4标识符
#define ATTRIBUTE7            "lightlux"            //功能属性5标识符
#define ATTRIBUTE8            "ADC_CH1"             //功能属性6标识符
#define ATTRIBUTE9            "ADC_CH2"             //功能属性7标识符
#define ATTRIBUTE10           "ADC_CH3"             //功能属性8标识符
//... ... 需要多少，自行添加

//---------------------事件报警标识符宏定义--------------------//
//所有程序中需要用到的事件报警的标识符在此定义
//标识符名称必须和服务器后台设置的完全一样，大小写也必须一样
#define EVENT1                "事件1标识符"     //事件1标识符
#define EVENT2                "事件2标识符"     //事件2标识符
//... ... 需要多少，自行添加

//---------功能属性参数结构体，定义各种变量用于保存属性值---------//              
typedef struct{
//... ... 需要多少，自行添加
	uint8_t Switch1Sta;					         //用于记录开关1状态的变量											
	uint8_t Switch2Sta;					         //用于记录开关2状态的变量
	uint8_t Switch3Sta;					         //用于记录开关2状态的变量
	uint8_t Switch4Sta;					         //用于记录开关2状态的变量
	double tempdata;	                         //用于记录温度的变量
	double humidata;	                         //用于记录湿度的变量	
	double lightata;	                         //用于记录光照度的变量	
	double adcdata[3];	                         //用于记录3个ADC通道值的数组
}Attribute_CB;   
#define ATTRIBUTE_STRUCT_LEN         sizeof(Attribute_CB)   //功能属性参数结构体 长度 
	
//---------------------------OTA升级时结构体----------------------//  
typedef struct{
	int  streamId;                             //OTA升级时，保存streamId数据
	int  streamFileId;                         //OTA升级时，保存streamFileId
	int  streamSize;                           //OTA升级时，固件总的大小
    char OTA_Versionbuff[VERSION_LEN+1];       //OTA升级时，保存新的版本号
	int  OTA_timers;                           //OTA升级时，总共需要下载多少次
	int  OTA_num;                              //OTA升级时，保存当前下载到第几次数据了
	int  OTA_dev;                              //OTA升级时，升级哪个设备
}OTA_CB;  
#define OTA_CB_LEN         sizeof(OTA_CB)      //结构体长度 
	
//------------各种外部变量声明，便于其他源文件调用变量-----------//
extern Sys_CB          SysCB;                                //外部变量声明，用于各种系统参数的结构体
extern Attribute_CB    AttributeCB[SUB_DEVICE_NUM];          //外部变量声明，用于保存各种属性功能的结构体
extern AliyunInfo_CB   AliInfoCB;                            //外部变量声明，EEPROM内保存的阿里云证书信息结构体
extern AliyunInfo_CB   AliInfoCB_SUB[SUB_DEVICE_NUM];        //外部变量声明，用于保存子设备阿里云证书信息结构体
extern OTA_CB          AliOTA;                               //外部变量声明，用于阿里云OTA升级的结构体

#endif
