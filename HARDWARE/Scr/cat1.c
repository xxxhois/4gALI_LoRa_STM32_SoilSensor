/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*              操作4G模块功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "cat1.h"	          //包含需要的头文件
#include "clock.h"	          //包含需要的头文件
#include "usart.h"	          //包含需要的头文件
#include "mqtt.h"             //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件
#include "lora.h"	          //包含需要的头文件
#include "crc.h"              //包含需要的头文件

char databuff[256];           //构建数据用的临时缓冲区
uint8_t online_counter = 1;   //查询子设备上线的计数变量
uint8_t data_counter = 1;     //查询子设备数据的计数变量

/*-------------------------------------------------*/
/*函数名：初始化CAT1模块                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CAT1_Reset(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();                             //使能端口C时钟
	
	GPIO_Initure.Pin=GPIO_PIN_4 | GPIO_PIN_15;                //设置PA4 15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;                    //推免输出
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);                       //设置
	
	GPIO_Initure.Pin=GPIO_PIN_0 | GPIO_PIN_1;                 //设置PA0 1
    GPIO_Initure.Mode=GPIO_MODE_INPUT;                        //输入模式
	GPIO_Initure.Pull = GPIO_NOPULL;                          //不使能上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);                       //设置
	
	if(POWER_STA==1){   							           //如果PB1是低电平，表示目前处于关机状态		
		u1_printf("\r\n目前处于关机状态，准备开机\r\n");       //串口输出信息
		POWER_KEY(1);                                          //先拉高
		HAL_Delay(1500);									   //延时
		POWER_KEY(0);										   //再拉低，开机
	}else{                                                     //反之PB1是高电平，表示目前处于开机状态
		u1_printf("\r\n目前处于开机状态，准备重启\r\n");       //串口输出信息		
		POWER_KEY(1);                                          //先拉高
		HAL_Delay(2000);								       //延时
		POWER_KEY(0);										   //再拉低，关机
		HAL_Delay(4000);                                       //间隔
		POWER_KEY(1);                                          //先拉高
		HAL_Delay(1500);									   //延时
		POWER_KEY(0);										   //再拉低，开机，完成重启
	}
	u1_printf("请等待注册上网络\r\n");                         //串口输出信息		
	while(1){                                                  //等待注册上网络
		HAL_Delay(100);                                        //延时100ms
		u1_printf(".");                                        //串口输出信息	
		if(NET_STA == 0)                                       //NET_STA 引脚变成低电平 表示注册上网络
			break;       						               //主动跳出while循环
	}

	u1_printf("\r\n已经注册上网络\r\n");                       //串口输出信息
	HAL_Delay(1000);                                           //间隔
	CAT1_printf("AT+CIPMUX=0\r\n");                            //发送指令关闭多路连接
}
/*-------------------------------------------------*/
/*函数名：用于各种系统参数的结构体初始化           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SysInit(void)
{			 
	memset(&SysCB,0,SYS_STRUCT_LEN);                             //用于各种系统参数的结构体,全部清零
	memset(AliInfoCB_SUB,0,SUB_DEVICE_NUM*ALIINFO_STRUCT_LEN);   //清空子设备证书信息结构体
	memset(AttributeCB,0,SUB_DEVICE_NUM*ATTRIBUTE_STRUCT_LEN);   //清空子设备存放功能数据数据的结构体
	memset(&AliOTA,0,OTA_CB_LEN);                                //OTA结构体清空
}
/*-------------------------------------------------*/
/*函数名：串口2被动事件                            */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U2PassiveEvent(uint8_t *data, uint16_t datalen)
{	
	uint16_t i;                        
	uint16_t tempsize,tempoffset;      //用于保存，下载量和下载偏移的计算结果
	int  firminfo_len;                 //存放下载的固件描述信息占用的长度 
	char firminfo_buff[128];           //存放下载的固件描述信息缓冲区
	int  bOffset,bSize;                //当次下载的OTA固件的偏移量和固件大小
	char *ptr;
	
	/*----------------------------------------------*/
	/*                关闭多路连接成功              */
	/*----------------------------------------------*/
	if(strstr((char *)data,"AT+CIPMUX=0")){                                         //接收到数据AT+CIPMUX=0
		u1_printf("关闭多路连接成功\r\n");                                          //串口输出信息
		CAT1_printf("AT+CIPMODE=1\r\n");                                            //发送透传模式指令		
	}
	/*----------------------------------------------*/
	/*                 透传模式成功                 */
	/*----------------------------------------------*/
	if(strstr((char *)data,"AT+CIPMODE=1")){                                         //接收到数据AT+CIPMODE=1\r\\r\n\r\nOK\r\n
		u1_printf("透传模式成功\r\n");                                               //串口输出信息
		CAT1_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",Aliyun_mqtt.ServerIP,Aliyun_mqtt.ServerPort); //发送指令，连接服务器		
	}
	/*----------------------------------------------*/
	/*                 连接上服务器                 */
	/*----------------------------------------------*/
	if(strstr((char *)data,"CONNECT")){                                             //接收到数据CONNECT
		u1_printf("连接上服务器\r\n");                                              //串口输出信息
		SysInit();                                                                  //用于各种系统参数的结构体初始化
		SysCB.SysEventFlag |= CONNECT_EVENT;        	                            //表示连接上服务器事件发生	
		MQTT_ConectPack();                           	                            //构建Conect报文，加入发送缓冲区，准备发送			
	}
	/*----------------------------------------------*/
	/*                 连接上服务器                 */
	/*----------------------------------------------*/
	if(strstr((char *)data,"CLOSE\r\n")){                                           //接收到数据CLOSE\r\n
		u1_printf("服务器断开，重启\r\n");                                          //串口输出信息
		NVIC_SystemReset();                                                         //重启	
	}
	/*----------------------------------------------*/
	/*               处理CONNACK报文                */
	/*----------------------------------------------*/	
	if((data[0]==0x20)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //如果接收数据的第1个字节是0x20 且 当前是连接上服务器的，进入if
		u1_printf("接收到CONNACK报文\r\n");                                          //串口输出信息
		switch(data[3]){	 		                                                 //接收数据的第4个字节，表示CONNECT报文是否成功
			case 0x00 : u1_printf("CONNECT报文成功\r\n");                            //串口输出信息	
			            SysCB.SysEventFlag |= CONNECT_PACK;        	                 //表示MQTT CONNECT报文成功事件发生
						MQTT_Subscribe(Aliyun_mqtt.Stopic_Buff,TOPIC_NUM,1);         //构建Subscribe报文，加入发送缓冲区，准备发送
			            SysCB.PingTimer = HAL_GetTick();                             //记录当前时间
						SysCB.SubONlineTimer = HAL_GetTick();                        //记录当前时间
						SysCB.SubdataTimer = HAL_GetTick();                          //记录当前时间
						break;                                                       //跳出分支case 0x00                                              
			case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
			            NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x01   
			case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x02 
			case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x03
			case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x04
			case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x05 		
			default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支default 								
		}		
	}
	/*----------------------------------------------*/
	/*                处理SUBACK报文                */
	/*----------------------------------------------*/
	if((data[0]==0x90)&&(SysCB.SysEventFlag&CONNECT_PACK)){	                         //如果接收数据的第1个字节是0x90 且 CONNECT报文发送成功
		u1_printf("接收到SUBACK报文\r\n");                                           //串口输出信息
        for(i=0;i<datalen-4;i++){                                                    //循环查询订阅结果
			switch(data[4+i]){		                                                 //从第5个字节，是订阅结果数据，每个topic有一个结果字节			
				case 0x00 :
				case 0x01 : u1_printf("第%d个Topic订阅成功\r\n",i+1);                //串口输出信息					           
							break;                                                   //跳出分支                                             
				default   : u1_printf("第%d个Topic订阅失败，准备重启\r\n");          //串口输出信息 
							NVIC_SystemReset();                                      //重启		
							break;                                                   //跳出分支 								
			}
		}	
		u1_printf("同步当前网关设备固件版本号\r\n");                                 //串口输出信息 
		CAT1_PropertyVersion(AliInfoCB.Version_ABuff,"GATEWAY");                     //同步当前网关设备固件版本号		
	}
	/*----------------------------------------------*/
	/*              处理PINGRESP报文                */
	/*----------------------------------------------*/
    if((data[0]==0xD0)&&(SysCB.SysEventFlag&CONNECT_PACK)){	                         //如果接收数据的第1个字节是0xD0 且 CONNECT报文发送成功
		u1_printf("接收到PINGRESP报文\r\n");                                         //串口输出信息
		SysCB.SysEventFlag &=~ PING_SENT;                                            //清除PING保活包发送事件发生标志			
	}
	/*----------------------------------------------*/
	/*              处理PUBACK报文                  */
	/*----------------------------------------------*/
	if((data[0]==0x40)&&(SysCB.SysEventFlag&CONNECT_PACK)){	                         //如果接收数据的第1个字节是0x40 且 CONNECT报文发送成功
		u1_printf("接收到PUBACK报文\r\n");                                           //串口输出信息
        u1_printf("等级1 标识符0x%02x%02x的报文，发送成功\r\n",data[2],data[3]);     //串口输出信息 
		if((SysCB.SysEventFlag&OTA_EVENT)){                                          //如果if成立，表示OTA事件发生了，此时收到的就是同步OTA版本号的回复报文
			SysCB.SysEventFlag &=~OTA_EVENT;                                         //清除事件标志
			if(AliOTA.OTA_dev == 0){                                                 //如果是网关OTA
				u1_printf("准备重启由BootLoader更新固件\r\n");                       //串口输出信息
				NVIC_SystemReset();                                                  //重启，更新flsah
			}
		}
	}
	/*----------------------------------------------*/
	/*        处理服务器推送来的等级0报文           */
	/*----------------------------------------------*/
	if((data[0]==0x30)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //如果接收数据的第1个字节是0x30 且 当前是连接上服务器的，进入if
		u1_printf("接收到服务器推送的等级0报文\r\n");                                //串口输出信息
        SysCB.PingTimer = HAL_GetTick();                                             //记录当前时间
		MQTT_DealPushdatatopic_Qs0(data,datalen);	                                 //处理等级0的推送数据，提取命令数据
        u1_printf("%s\r\n",Aliyun_mqtt.cmdbuff);                                     //串口1输出数据
		
		/*----------------------------------------------*/
		/*             提取网关OTA命令信息              */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"GATEWAY\"")){                    //如果搜索到"module":"GATEWAY",说明要OTA更新网关设备
			//提取3个关键数据
			//待OTA升级的固件编号
			//待OTA升级的固件文件数
			//待OTA升级的版本号
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //定位网关OTA命令信息起始处
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"GATEWAY\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA升级的固件编号：%d\r\n",AliOTA.streamId);         //串口输出信息
					u1_printf("OTA升级的固件文件数：%d\r\n",AliOTA.streamFileId);   //串口输出信息
					u1_printf("OTA升级的固件总大小：%d\r\n",AliOTA.streamSize);     //串口输出信息
					u1_printf("OTA升级的版本号：%s\r\n",AliOTA.OTA_Versionbuff);    //串口输出信息
					AliOTA.OTA_dev = 0;                                             //AliOTA.OTA_dev等于0，更新网关设备
					SysCB.SysEventFlag |= OTA_EVENT;                                //网关OTA事件发生
					SysCB.OTATimer = HAL_GetTick();                                 //获取当前时间
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //擦除第0块 共计64K空间
					if((AliOTA.streamSize%256)!=0)                                  //判断固件是不是正好是256的整数倍，不是进入if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //计算下载次数
					else                                                            //反之，进入else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //计算下载次数
					u1_printf("每次下载256字节数据，共计需要下载：%d次\r\n",AliOTA.OTA_timers);    //串口输出信息
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num等于1，开始第一次下载	
					u1_printf("准备OTA下载固件第%d次\r\n",AliOTA.OTA_num);		                   //串口输出提示信息
					CAT1_OTADownload(256,0);                                                       //发送报文，下载固件
				}else u1_printf("提取网关OTA下载固件描述信息数据失败\r\n");		                   //串口输出提示信息
			}else u1_printf("定位网关OTA命令信息起始处失败\r\n");		                           //串口输出提示信息
		}
		/*----------------------------------------------*/
		/*          提取做子设备1 OTA命令信息           */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"SUB1\"")){                      //如果搜索到"module":"SUB1",说明要OTA更新子设备1设备
			//提取3个关键数据
			//待OTA升级的固件编号
			//待OTA升级的固件文件数
			//待OTA升级的版本号
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //定位子设备1 OTA命令信息起始处
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"SUB1\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA升级的固件编号：%d\r\n",AliOTA.streamId);         //串口输出信息
					u1_printf("OTA升级的固件文件数：%d\r\n",AliOTA.streamFileId);   //串口输出信息
					u1_printf("OTA升级的固件总大小：%d\r\n",AliOTA.streamSize);     //串口输出信息
					u1_printf("OTA升级的版本号：%s\r\n",AliOTA.OTA_Versionbuff);    //串口输出信息
					AliOTA.OTA_dev = 1;                                             //AliOTA.OTA_dev等于1，更新子设备1
					SysCB.SysEventFlag |= OTA_EVENT;                                //网关OTA事件发生
					SysCB.OTATimer = HAL_GetTick();                                 //获取当前时间
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //擦除第1块 共计64K空间
					if((AliOTA.streamSize%256)!=0)                                  //判断固件是不是正好是256的整数倍，不是进入if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //计算下载次数
					else                                                            //反之，进入else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //计算下载次数
					u1_printf("每次下载256字节数据，共计需要下载：%d次\r\n",AliOTA.OTA_timers);    //串口输出信息
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num等于1，开始第一次下载	
					u1_printf("准备OTA下载固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	   //串口输出提示信息
					CAT1_OTADownload(256,0);                                                       //发送报文，下载固件
				}else u1_printf("提取设备1 OTA下载固件描述信息数据失败\r\n");		               //串口输出提示信息
			}else u1_printf("定位子设备1 OTA命令信息起始处失败\r\n");		                       //串口输出提示信息
		}
		/*----------------------------------------------*/
		/*          提取做子设备2 OTA命令信息           */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"SUB2\"")){                      //如果搜索到"module":"SUB2",说明要OTA更新子设备2设备
			//提取3个关键数据
			//待OTA升级的固件编号
			//待OTA升级的固件文件数
			//待OTA升级的版本号
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //定位子设备2 OTA命令信息起始处
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"SUB2\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA升级的固件编号：%d\r\n",AliOTA.streamId);         //串口输出信息
					u1_printf("OTA升级的固件文件数：%d\r\n",AliOTA.streamFileId);   //串口输出信息
					u1_printf("OTA升级的固件总大小：%d\r\n",AliOTA.streamSize);     //串口输出信息
					u1_printf("OTA升级的版本号：%s\r\n",AliOTA.OTA_Versionbuff);    //串口输出信息
					AliOTA.OTA_dev = 2;                                             //AliOTA.OTA_dev等于2，更新子设备2
					SysCB.SysEventFlag |= OTA_EVENT;                                //网关OTA事件发生
					SysCB.OTATimer = HAL_GetTick();                                 //获取当前时间
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //擦除第2块 共计64K空间
					if((AliOTA.streamSize%256)!=0)                                  //判断固件是不是正好是256的整数倍，不是进入if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //计算下载次数
					else                                                            //反之，进入else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //计算下载次数
					u1_printf("每次下载256字节数据，共计需要下载：%d次\r\n",AliOTA.OTA_timers);    //串口输出信息
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num等于1，开始第一次下载	
					u1_printf("准备OTA下载固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);    //串口输出提示信息
					CAT1_OTADownload(256,0);                                                       //发送报文，下载固件
				}else u1_printf("提取子设备2 OTA下载固件描述信息数据失败\r\n");		               //串口输出提示信息
			}else u1_printf("定位子设备2 OTA命令信息起始处\r\n");		                           //串口输出提示信息
		}
		/*----------------------------------------------*/
		/*          提取做子设备3 OTA命令信息           */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"\"module\":\"SUB3\"")){                      //如果搜索到"module":"SUB3",说明要OTA更新子设备3设备
			//提取3个关键数据
			//待OTA升级的固件编号
			//待OTA升级的固件文件数
			//待OTA升级的版本号
			ptr = strstr(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\"");                 //定位子子设备3 OTA命令信息起始处
			if(ptr != NULL){
				if(sscanf(ptr,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"module\":\"SUB3\",\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&AliOTA.streamSize,&AliOTA.streamId,AliOTA.OTA_Versionbuff,&AliOTA.streamFileId)==4){
					u1_printf("OTA升级的固件编号：%d\r\n",AliOTA.streamId);         //串口输出信息
					u1_printf("OTA升级的固件文件数：%d\r\n",AliOTA.streamFileId);   //串口输出信息
					u1_printf("OTA升级的固件总大小：%d\r\n",AliOTA.streamSize);     //串口输出信息
					u1_printf("OTA升级的版本号：%s\r\n",AliOTA.OTA_Versionbuff);    //串口输出信息
					AliOTA.OTA_dev = 3;                                             //AliOTA.OTA_dev等于3，更新子设备3
					SysCB.SysEventFlag |= OTA_EVENT;                                //网关OTA事件发生
					SysCB.OTATimer = HAL_GetTick();                                 //获取当前时间
					W25Qxx_Erase_Block(AliOTA.OTA_dev);                             //擦除第2块 共计64K空间
					if((AliOTA.streamSize%256)!=0)                                  //判断固件是不是正好是256的整数倍，不是进入if
						AliOTA.OTA_timers = AliOTA.streamSize/256 + 1;              //计算下载次数
					else                                                            //反之，进入else
						AliOTA.OTA_timers = AliOTA.streamSize/256;                  //计算下载次数
					u1_printf("每次下载256字节数据，共计需要下载：%d次\r\n",AliOTA.OTA_timers);    //串口输出信息
					AliOTA.OTA_num = 1;                                                            //AliOTA.OTA_num等于1，开始第一次下载	
					u1_printf("准备OTA下载固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	   //串口输出提示信息
					CAT1_OTADownload(256,0);                                                       //发送报文，下载固件
				}else u1_printf("提取子设备3 OTA下载固件描述信息数据失败\r\n");		               //串口输出提示信息
			}else u1_printf("定位子设备3 OTA命令信息起始处失败\r\n");		                       //串口输出提示信息
		}
		/*----------------------------------------------*/
		/*              提取每次下载的固件              */
		/*----------------------------------------------*/	
		if(strstr(Aliyun_mqtt.cmdbuff,"download_reply")){                              //如果搜索到download_reply,说明是下载的固件数据
			ptr = strstr(Aliyun_mqtt.cmdbuff,"download_reply");                        //定位数据位置
			if(ptr != NULL){
				u1_printf("接收到OTA下载固件\r\n");		                               //串口输出提示信息
				firminfo_len = ptr[14] *256 + ptr[15] + 2;                             //下载的固件描述信息占用的长度 
				memset(firminfo_buff,0,128);                                           //清空缓冲区
				memcpy(firminfo_buff,&ptr[16],firminfo_len - 2);	     	           //拷贝固件描述信息
				if(sscanf(firminfo_buff,"{\"code\":200,\"data\":{\"bOffset\":%d,\"fileLength\":%*d,\"bSize\":%d},\"id\":\"%*d\",\"message\":\"success\"}",&bOffset,&bSize)==2){ //提取数据
					u1_printf("本次下载范围：%d ~ %d\r\n",bOffset,bOffset+bSize-1);	   //串口输出提示信息
					W25Qxx_Write_Page((uint8_t *)&ptr[14+firminfo_len],(bOffset/256) + AliOTA.OTA_dev*256); //写入外部flash		
					AliOTA.OTA_num++;                                                  //AliOTA.OTA_num下载次数增加1
					if(AliOTA.OTA_num<=AliOTA.OTA_timers){                             //还没有下载完毕，进入if
						if(((AliOTA.streamSize-((AliOTA.OTA_num - 1)*256))/256)>0)     //是不是够256，是的话进入if
							tempsize = 256;                                            //下载量就是256
						else                                                           //不足256的话进入else
							tempsize = AliOTA.streamSize-((AliOTA.OTA_num - 1)*256);   //计算下载量
						tempoffset = ((AliOTA.OTA_num - 1)*256);                       //计算当次下载的偏移量		
						u1_printf("准备OTA下载固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	 //串口输出提示信息
						CAT1_OTADownload(tempsize,tempoffset);                         //发送报文，下载固件
					}else{                                                                                                   //如果下载完毕，进入else
						switch(AliOTA.OTA_dev){                                                                              //判断是网关，还是子设备123
							case 0:	u1_printf("网关 OTA固件下载完毕\r\n");	                                                 //串口输出提示信息   
									memset(AliInfoCB.Version_ABuff,0,VERSION_LEN+1);                                         //清除固件版本号缓冲区
									memcpy(AliInfoCB.Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));   //拷贝新的版本号
									AliInfoCB.OTA_firelen = AliOTA.streamSize;                                               //记录固件的大小
									AliInfoCB.OTA_flag = BOOT_STA_O;                                                         //标记需要OTA
									EEprom_WriteData(0,&AliInfoCB,ALIINFO_STRUCT_LEN);                                       //保存信息到EEprom
									CAT1_PropertyVersion(AliInfoCB.Version_ABuff,"GATEWAY");		                         //同步当前网关设备固件版本号
									break;
							case 1:	u1_printf("子设备1 OTA固件下载完毕\r\n");	                                             //串口输出提示信息   
									memset(AliInfoCB_SUB[0].Version_ABuff,0,VERSION_LEN+1);                                  //清除固件版本号缓冲区
									memcpy(AliInfoCB_SUB[0].Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));  //拷贝新的版本号
									if((AliOTA.streamSize%128)!=0)                                                           //判断固件是不是正好是128的整数倍，不是进入if
										AliOTA.OTA_timers = AliOTA.streamSize/128 + 1;                                       //计算发送次数
									else                                                                                     //反之，进入else
										AliOTA.OTA_timers = AliOTA.streamSize/128;                                           //计算发送次数
									u1_printf("每次发送128字节数据，共计需要发送：%d次\r\n",AliOTA.OTA_timers);              //串口输出信息
									u1_printf("准备OTA发送固件第%d次\r\n",AliOTA.OTA_num);		                             //串口输出提示信息
									AliOTA.OTA_num = 1;                                                                      //AliOTA.OTA_num等于1，开始第一次发送
									memset(databuff,0,256);                                                                  //清空缓冲区
									databuff[0] = SUB1_HADR;                                                                 //接收方 地址高字节
									databuff[1] = SUB1_LADR;                                                                 //接收方 地址低字节
									databuff[2] = SUB1_CH;                                              	                 //接收方 信道		
									databuff[3] = 0x5A;                                                                      //固定字节
									databuff[4] = 0xA5;                                                                      //固定字节
									databuff[5] = AliOTA.OTA_num/256;                                                        //数据包编号高字节	
									databuff[6] = AliOTA.OTA_num%256;                                                        //数据包编号低字节										
									W25Qxx_Read((uint8_t *)&databuff[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);//读取数据
									u3_TxDataBuf((uint8_t *)databuff,135);                                                   //添加数据到发送缓冲区	
									break;
							case 2:	u1_printf("子设备2 OTA固件下载完毕\r\n");	                                             //串口输出提示信息   
									memset(AliInfoCB_SUB[1].Version_ABuff,0,VERSION_LEN+1);                                  //清除固件版本号缓冲区
									memcpy(AliInfoCB_SUB[1].Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));  //拷贝新的版本号
									if((AliOTA.streamSize%128)!=0)                                                           //判断固件是不是正好是128的整数倍，不是进入if
										AliOTA.OTA_timers = AliOTA.streamSize/128 + 1;                                       //计算发送次数
									else                                                                                     //反之，进入else
										AliOTA.OTA_timers = AliOTA.streamSize/128;                                           //计算发送次数
									u1_printf("每次发送128字节数据，共计需要发送：%d次\r\n",AliOTA.OTA_timers);              //串口输出信息
									u1_printf("准备OTA发送固件第%d次\r\n",AliOTA.OTA_num);		                             //串口输出提示信息
									AliOTA.OTA_num = 1;                                                                      //AliOTA.OTA_num等于1，开始第一次发送
									memset(databuff,0,256);                                                                  //清空缓冲区
									databuff[0] = SUB2_HADR;                                                                 //接收方 地址高字节
									databuff[1] = SUB2_LADR;                                                                 //接收方 地址低字节
									databuff[2] = SUB2_CH;                                              	                 //接收方 信道		
									databuff[3] = 0x5A;                                                                      //固定字节
									databuff[4] = 0xA5;                                                                      //固定字节
									databuff[5] = AliOTA.OTA_num/256;                                                        //数据包编号高字节		
									databuff[6] = AliOTA.OTA_num%256;                                                        //数据包编号低字节										
									W25Qxx_Read((uint8_t *)&databuff[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);//读取数据
									u3_TxDataBuf((uint8_t *)databuff,135);                                                   //添加数据到发送缓冲区	
									break;
							case 3:	u1_printf("子设备3 OTA固件下载完毕\r\n");	                                             //串口输出提示信息   
									memset(AliInfoCB_SUB[2].Version_ABuff,0,VERSION_LEN+1);                                  //清除固件版本号缓冲区
									memcpy(AliInfoCB_SUB[2].Version_ABuff,AliOTA.OTA_Versionbuff,strlen(AliOTA.OTA_Versionbuff));  //拷贝新的版本号
									if((AliOTA.streamSize%128)!=0)                                                           //判断固件是不是正好是128的整数倍，不是进入if
										AliOTA.OTA_timers = AliOTA.streamSize/128 + 1;                                       //计算发送次数
									else                                                                                     //反之，进入else
										AliOTA.OTA_timers = AliOTA.streamSize/128;                                           //计算发送次数
									u1_printf("每次发送128字节数据，共计需要发送：%d次\r\n",AliOTA.OTA_timers);              //串口输出信息
									u1_printf("准备OTA发送固件第%d次\r\n",AliOTA.OTA_num);		                             //串口输出提示信息
									AliOTA.OTA_num = 1;                                                                      //AliOTA.OTA_num等于1，开始第一次发送
									memset(databuff,0,256);                                                                  //清空缓冲区
									databuff[0] = SUB3_HADR;                                                                 //接收方 地址高字节
									databuff[1] = SUB3_LADR;                                                                 //接收方 地址低字节
									databuff[2] = SUB3_CH;                                              	                 //接收方 信道		
									databuff[3] = 0x5A;                                                                      //固定字节
									databuff[4] = 0xA5;                                                                      //固定字节
									databuff[5] = AliOTA.OTA_num/256;                                                        //数据包编号高字节		
									databuff[6] = AliOTA.OTA_num%256;                                                        //数据包编号低字节										
									W25Qxx_Read((uint8_t *)&databuff[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);//读取数据
									u3_TxDataBuf((uint8_t *)databuff,135);                                                   //添加数据到发送缓冲区	
									break;
						}
					}
				}else{  
					SysCB.SysEventFlag &=~OTA_EVENT;                 //清除事件标志
					u1_printf("提取下载固件描述信息数据失败\r\n");	 //串口输出提示信息	
				}
			}
		}
		/*----------------------------------------------*/
		/*             子设备上线命令处理               */
		/*----------------------------------------------*/		
		if(strstr(Aliyun_mqtt.cmdbuff,"login_reply{\"code\":200")){	                 //如果搜索到数据login_reply{"code":200，说明是子设备上线成功	
			if(strstr(Aliyun_mqtt.cmdbuff,"D001")){	                                 //如果搜索到数据D001，说明是子设备1上线成功	
				u1_printf("子设备1上线成功\r\n");                                    //串口输出信息
				SysCB.SysEventFlag |= SUB_DEVICE1_ON;                                //子设备1上线成功 标志置位
				CAT1_PropertyVersion(AliInfoCB_SUB[0].Version_ABuff,"SUB1");		 //同步子设备1当前固件版本号	
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"D002")){	                                 //如果搜索到数据D002，说明是子设备2上线成功	
				u1_printf("子设备2上线成功\r\n");                                    //串口输出信息
				SysCB.SysEventFlag |= SUB_DEVICE2_ON;                                //子设备2上线成功 标志置位
				CAT1_PropertyVersion(AliInfoCB_SUB[1].Version_ABuff,"SUB2");		 //同步子设备2当前固件版本号
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"D003")){	                                 //如果搜索到数据D003，说明是子设备3上线成功	
				u1_printf("子设备3上线成功\r\n");                                    //串口输出信息
				SysCB.SysEventFlag |= SUB_DEVICE3_ON;                                //子设备3上线成功 标志置位
				CAT1_PropertyVersion(AliInfoCB_SUB[2].Version_ABuff,"SUB3");		 //同步子设备3当前固件版本号	
			}
		}
		/*----------------------------------------------*/
		/*            子设备1开关命令处理               */
		/*----------------------------------------------*/	
		memset(databuff,0,256);                                                      //清空缓冲区
		sprintf(databuff,"/sys/%s/%s/thing/service/property/set",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff);
		if(strstr(Aliyun_mqtt.cmdbuff,databuff)){	                                 //如果搜索到数据databuff内数据，说明是子设备1命令下发
			u1_printf("子设备1命令下发\r\n");                                        //串口输出信息
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":0")){                   //如果搜索到"PowerSwitch_1":0 说明服务器下发关闭开关命令				
				AttributeCB[0].Switch1Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":1")){                   //如果搜索到"PowerSwitch_1":1 说明服务器下发打开开关命令
				AttributeCB[0].Switch1Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":0")){                   //如果搜索到"PowerSwitch_2":0 说明服务器下发关闭开关命令				
				AttributeCB[0].Switch2Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":1")){                   //如果搜索到"PowerSwitch_2":1 说明服务器下发打开开关命令
				AttributeCB[0].Switch2Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":0")){                   //如果搜索到"PowerSwitch_3":0 说明服务器下发关闭开关命令				
				AttributeCB[0].Switch3Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":1")){                   //如果搜索到"PowerSwitch_3":1 说明服务器下发打开开关命令
				AttributeCB[0].Switch3Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":0")){                   //如果搜索到"PowerSwitch_4":0 说明服务器下发关闭开关命令				
				AttributeCB[0].Switch4Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":1")){                   //如果搜索到"PowerSwitch_4":1 说明服务器下发打开开关命令
				AttributeCB[0].Switch4Sta = 1;	                                     //记录开关状态
			}
			memset(databuff,0,256);                                                  //清空缓冲区
			databuff[0] = SUB1_HADR;                                                 //接收方 地址高字节
			databuff[1] = SUB1_LADR;                                                 //接收方 地址低字节
			databuff[2] = SUB1_CH;                                           	     //接收方 信道		
			sprintf(&databuff[3],"AT+PowerSwitch1=%d%d%d%d\r\n",AttributeCB[0].Switch1Sta,AttributeCB[0].Switch2Sta,AttributeCB[0].Switch3Sta,AttributeCB[0].Switch4Sta);
			u3_TxDataBuf((uint8_t *)databuff,3+strlen(&databuff[3]));                //添加数据到发送缓冲区		
		}
		/*----------------------------------------------*/
		/*            子设备2开关命令处理               */
		/*----------------------------------------------*/	
		memset(databuff,0,64);                                                       //清空缓冲区
		sprintf(databuff,"/sys/%s/%s/thing/service/property/set",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff);
		if(strstr(Aliyun_mqtt.cmdbuff,databuff)){	                                 //如果搜索到数据databuff内数据，说明是子设备2命令下发
			u1_printf("子设备2命令下发\r\n");                                        //串口输出信息
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":0")){                   //如果搜索到"PowerSwitch_1":0 说明服务器下发关闭开关命令				
				AttributeCB[1].Switch1Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":1")){                   //如果搜索到"PowerSwitch_1":1 说明服务器下发打开开关命令
				AttributeCB[1].Switch1Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":0")){                   //如果搜索到"PowerSwitch_2":0 说明服务器下发关闭开关命令				
				AttributeCB[1].Switch2Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":1")){                   //如果搜索到"PowerSwitch_2":1 说明服务器下发打开开关命令
				AttributeCB[1].Switch2Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":0")){                   //如果搜索到"PowerSwitch_3":0 说明服务器下发关闭开关命令				
				AttributeCB[1].Switch3Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":1")){                   //如果搜索到"PowerSwitch_3":1 说明服务器下发打开开关命令
				AttributeCB[1].Switch3Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":0")){                   //如果搜索到"PowerSwitch_4":0 说明服务器下发关闭开关命令				
				AttributeCB[1].Switch4Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":1")){                   //如果搜索到"PowerSwitch_4":1 说明服务器下发打开开关命令
				AttributeCB[1].Switch4Sta = 1;	                                     //记录开关状态
			}
			memset(databuff,0,256);                                                  //清空缓冲区
			databuff[0] = SUB2_HADR;                                                 //接收方 地址高字节
			databuff[1] = SUB2_LADR;                                                 //接收方 地址低字节
			databuff[2] = SUB2_CH;                                           	     //接收方 信道			
			sprintf(&databuff[3],"AT+PowerSwitch2=%d%d%d%d\r\n",AttributeCB[1].Switch1Sta,AttributeCB[1].Switch2Sta,AttributeCB[1].Switch3Sta,AttributeCB[1].Switch4Sta);
			u3_TxDataBuf((uint8_t *)databuff,3+strlen(&databuff[3]));                //添加数据到发送缓冲区	
		}	
		/*----------------------------------------------*/
		/*            子设备3开关命令处理               */
		/*----------------------------------------------*/	
		memset(databuff,0,64);                                                       //清空缓冲区
		sprintf(databuff,"/sys/%s/%s/thing/service/property/set",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff);
		if(strstr(Aliyun_mqtt.cmdbuff,databuff)){	                                 //如果搜索到数据databuff内数据，说明是子设备3命令下发
			u1_printf("子设备3命令下发\r\n");                                        //串口输出信息
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":0")){                   //如果搜索到"PowerSwitch_1":0 说明服务器下发关闭开关命令				
				AttributeCB[2].Switch1Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":1")){                   //如果搜索到"PowerSwitch_1":1 说明服务器下发打开开关命令
				AttributeCB[2].Switch1Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":0")){                   //如果搜索到"PowerSwitch_2":0 说明服务器下发关闭开关命令				
				AttributeCB[2].Switch2Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":1")){                   //如果搜索到"PowerSwitch_2":1 说明服务器下发打开开关命令
				AttributeCB[2].Switch2Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":0")){                   //如果搜索到"PowerSwitch_3":0 说明服务器下发关闭开关命令				
				AttributeCB[2].Switch3Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_3\":1")){                   //如果搜索到"PowerSwitch_3":1 说明服务器下发打开开关命令
				AttributeCB[2].Switch3Sta = 1;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":0")){                   //如果搜索到"PowerSwitch_4":0 说明服务器下发关闭开关命令				
				AttributeCB[2].Switch4Sta = 0;	                                     //记录开关状态
			}
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_4\":1")){                   //如果搜索到"PowerSwitch_4":1 说明服务器下发打开开关命令
				AttributeCB[2].Switch4Sta = 1;	                                     //记录开关状态
			}
			memset(databuff,0,256);                                                  //清空缓冲区
			databuff[0] = SUB3_HADR;                                                 //接收方 地址高字节
			databuff[1] = SUB3_LADR;                                                 //接收方 地址低字节
			databuff[2] = SUB3_CH;                                           	     //接收方 信道		
			sprintf(&databuff[3],"AT+PowerSwitch3=%d%d%d%d\r\n",AttributeCB[2].Switch1Sta,AttributeCB[2].Switch2Sta,AttributeCB[2].Switch3Sta,AttributeCB[2].Switch4Sta);
			u3_TxDataBuf((uint8_t *)databuff,3+strlen(&databuff[3]));                //添加数据到发送缓冲区	
		}	
	}
}
/*-------------------------------------------------*/
/*函数名：主动事件                                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void ActiveEvent(void)
{			
	/*---------------------------------------------*/
	/*               查询OTA是否超时               */
	/*---------------------------------------------*/
	if(SysCB.SysEventFlag&OTA_EVENT){                                            
		if((HAL_GetTick() - SysCB.OTATimer) >= 180000){                      //超过120s 还没有更新完毕
			SysCB.SysEventFlag &=~OTA_EVENT;                                 //清除OTA事件标志		
		}
	}	
	/*---------------------------------------------*/
	/*             发送PING数据包时间到            */
	/*---------------------------------------------*/
	if((HAL_GetTick() - SysCB.PingTimer >=60000)&&(SysCB.SysEventFlag&CONNECT_PACK)){                             
		SysCB.PingTimer = HAL_GetTick();                                     //记录当前时间
		if(SysCB.SysEventFlag&PING_SENT){                                    //判断PING_SENT标志，如果置位，进入if，说明上传的PING数据包发送后，没有收到服务器的PINGRESP回复包，可能掉线了
			u1_printf("连接掉线，重启\r\n");                                 //串口输出信息
			NVIC_SystemReset();                                              //重启             
		}else{				
			MQTT_PingREQ();                                                  //将一个PING数据包加入发送缓冲区			
			SysCB.SysEventFlag |= PING_SENT;                                 //表示PING保活包发送事件发生				
		}	
	} 
	/*---------------------------------------------*/
	/*           查询子设备上线时间到              */
	/*---------------------------------------------*/
	if(((HAL_GetTick() - SysCB.SubONlineTimer) >=27000)&&(!(SysCB.SysEventFlag&OTA_EVENT))&&(SysCB.SysEventFlag&CONNECT_PACK)){                    
		SysCB.SubONlineTimer = HAL_GetTick();                                //记录当前时间
		switch(online_counter){                                              //判断查询第几个子设备
			case 1: if(!(SysCB.SysEventFlag&SUB_DEVICE1_ON)){                //子设备1是否在线
						memset(databuff,0,256);                              //清空缓冲区
						databuff[0] = SUB1_HADR;                             //接收方 地址高字节
						databuff[1] = SUB1_LADR;                             //接收方 地址低字节
						databuff[2] = SUB1_CH;                               //接收方 信道
						memcpy(&databuff[3],"AT+CER1\r\n",9);                //添加数据
						u3_TxDataBuf((uint8_t *)databuff,12);                //添加数据到发送缓冲区
						online_counter = 2;	                                 //下次查询子设备2
						break;                                               //跳出
					}
			case 2: if(!(SysCB.SysEventFlag&SUB_DEVICE2_ON)){                //子设备2是否在线
						memset(databuff,0,256);                              //清空缓冲区
						databuff[0] = SUB2_HADR;                             //接收方 地址高字节
						databuff[1] = SUB2_LADR;                             //接收方 地址低字节
						databuff[2] = SUB2_CH;                               //接收方 信道
						memcpy(&databuff[3],"AT+CER2\r\n",9);                //添加数据
						u3_TxDataBuf((uint8_t *)databuff,12);                //添加数据到发送缓冲区
						online_counter = 3;	                                 //下次查询子设备3
						break;                                               //跳出
					}
			case 3: if(!(SysCB.SysEventFlag&SUB_DEVICE3_ON)){                //子设备3是否在线
						memset(databuff,0,256);                              //清空缓冲区
						databuff[0] = SUB3_HADR;                             //接收方 地址高字节
						databuff[1] = SUB3_LADR;                             //接收方 地址低字节
						databuff[2] = SUB3_CH;                               //接收方 信道
						memcpy(&databuff[3],"AT+CER3\r\n",9);                //添加数据
						u3_TxDataBuf((uint8_t *)databuff,12);                //添加数据到发送缓冲区
						online_counter = 1;	                                 //下次查询子设备1
						break;                                               //跳出
					}
			default:online_counter = 1;	                                     //下次查询子设备1
					break;                                                   //跳出
		}
	}   
	/*---------------------------------------------*/
	/*      子设备上传数据时间到（依次查询）       */
	/*---------------------------------------------*/
	if(((HAL_GetTick() - SysCB.SubdataTimer) >=10000)&&(!(SysCB.SysEventFlag&OTA_EVENT))&&(SysCB.SysEventFlag&CONNECT_PACK)){ 
		SysCB.SubdataTimer = HAL_GetTick();                                  //记录当前时间	
		switch(data_counter){                                                //判断第几个子设备                          
			case 1: if(SysCB.SysEventFlag&SUB_DEVICE1_ON){                   //子设备1是否在线
						memset(databuff,0,256);                              //清空缓冲区
						databuff[0] = SUB1_HADR;                             //接收方 地址高字节
						databuff[1] = SUB1_LADR;                             //接收方 地址低字节
						databuff[2] = SUB1_CH;                               //接收方 信道
						memcpy(&databuff[3],"AT+DATA1\r\n",10);              //添加数据
						u3_TxDataBuf((uint8_t *)databuff,13);                //添加数据到发送缓冲区
						data_counter = 2;	                                 //下次子设备2	
						break;                                               //跳出
					}
			case 2: if(SysCB.SysEventFlag&SUB_DEVICE2_ON){                   //子设备2是否在线
						memset(databuff,0,256);                              //清空缓冲区
						databuff[0] = SUB2_HADR;                             //接收方 地址高字节
						databuff[1] = SUB2_LADR;                             //接收方 地址低字节
						databuff[2] = SUB2_CH;                               //接收方 信道
						memcpy(&databuff[3],"AT+DATA2\r\n",10);              //添加数据
						u3_TxDataBuf((uint8_t *)databuff,13);                //添加数据到发送缓冲区
						data_counter = 3;	                                 //下次子设备3
						break;                                               //跳出
					}
			case 3: if(SysCB.SysEventFlag&SUB_DEVICE3_ON){                   //子设备3是否在线
						memset(databuff,0,256);                              //清空缓冲区
						databuff[0] = SUB3_HADR;                             //接收方 地址高字节
						databuff[1] = SUB3_LADR;                             //接收方 地址低字节
						databuff[2] = SUB3_CH;                               //接收方 信道
						memcpy(&databuff[3],"AT+DATA3\r\n",10);              //添加数据
						u3_TxDataBuf((uint8_t *)databuff,13);                //添加数据到发送缓冲区
						data_counter = 1;	                                 //下次子设备1
						break;                                               //跳出
					}		
			default:data_counter = 1;	                                     //下次子设备1
					break;
		}
	} 	
}
/*-------------------------------------------------*/
/*函数名：向服务器上传版本号                       */
/*参  数：ver：版本号                              */
/*参  数：module：模块名                           */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CAT1_PropertyVersion(uint8_t * ver,uint8_t * module)
{	
	char topicdatabuff[64];                                                                             //用于构建发送topic的缓冲区
    char tempdatabuff[128];                                                                             //用于临时构建数据的缓冲区
	
	memset(topicdatabuff,0,64);                                                                         //清空临时缓冲区	
	sprintf(topicdatabuff,"/ota/device/inform/%s/%s",PRODUCTKEY,DEVICENAME);                            //构建发送topic					
	memset(tempdatabuff,0,128);                                                                         //清空临时缓冲区	
	sprintf(tempdatabuff,"{\"id\": 1,\"params\":{\"version\":\"%s\",\"module\":\"%s\"}}",ver,module);   //构建数据	
	MQTT_PublishQs1(topicdatabuff,tempdatabuff,strlen(tempdatabuff));                                   //等级1的PUBLISH报文，加入发送缓冲区   	
}
/*-------------------------------------------------*/
/*函数名：OTA下载固件                              */
/*参  数：size ：下载多少数据                      */
/*参  数：offset ：下载偏移量                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CAT1_OTADownload(int size, int offset)
{	
	char topicdatabuff[64];        													    //用于构建发送topic的缓冲区
    char tempdatabuff[128];        													    //用于临时构建数据的缓冲区
	
	memset(topicdatabuff,0,64);                                                         //清空临时缓冲区	
	sprintf(topicdatabuff,"/sys/%s/%s/thing/file/download",PRODUCTKEY,DEVICENAME);      //构建发送topic					
	memset(tempdatabuff,0,128);                                                         //清空临时缓冲区	
	u1_printf("OTA升级的固件编号：%d\r\n",AliOTA.streamId);                             //串口输出信息
	HAL_Delay(300);
	sprintf(tempdatabuff,"{\"id\": \"1\",\"params\": {\"fileInfo\":{\"streamId\":%d,\"fileId\":%d},\"fileBlock\":{\"size\":%d,\"offset\":%d}}}",AliOTA.streamId,AliOTA.streamFileId,size,offset);   //构建数据	
	MQTT_PublishQs0(topicdatabuff,tempdatabuff,strlen(tempdatabuff));                   //等级0的PUBLISH报文，加入发送缓冲区   	
}
/*-------------------------------------------------*/
/*函数名：子设备上线                              */
/*参  数：postdata ：上传的数据                    */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CAT1_SubDeviceLogin(char * postdata)
{	
	char topicdatabuff[64];                                                              //用于构建发送topic的缓冲区

	memset(topicdatabuff,0,64);                                                          //清空临时缓冲区	
	sprintf(topicdatabuff,"/ext/session/%s/%s/combine/login",PRODUCTKEY,DEVICENAME);     //构建发送topic					
	MQTT_PublishQs0(topicdatabuff,postdata,strlen(postdata));                            //等级0的PUBLISH报文，加入发送缓冲区   	
}
/*-------------------------------------------------*/
/*函数名：子设备数据上传                           */
/*参  数：postdata ：上传的数据                    */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CAT1_SubDevicePost(char * postdata)
{	
	char topicdatabuff[64];                                                                      //用于构建发送topic的缓冲区

	memset(topicdatabuff,0,64);                                                                  //清空临时缓冲区	
	sprintf(topicdatabuff,"/sys/%s/%s/thing/event/property/pack/post",PRODUCTKEY,DEVICENAME);    //构建发送topic					
	MQTT_PublishQs0(topicdatabuff,postdata,strlen(postdata));                                    //等级0的PUBLISH报文，加入发送缓冲区   	
}
