/*-------------------------------------------------*/
/*           超子说物联网STM32系列开发板           */
/*-------------------------------------------------*/
/*                                                 */
/*            操作LoRa模块功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "clock.h"            //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "lora.h"	          //包含需要的头文件
#include "clock.h"	          //包含需要的头文件
#include "utils_hmac.h"       //包含需要的头文件
#include "cat1.h"	          //包含需要的头文件
#include "mqtt.h"             //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件

LoRaParameter LoRaSetData = {            //模块工作参数
	0x00,                                //模块地址高字节
	0x00,                                //模块地址低字节
	0x01,                                //模块网络地址
	LoRa_9600,                           //模块串口 波特率 9600
	LoRa_8N1,                            //串口工作模式 8数据位 无校验 1停止位	
	LoRa_38_4,                           //模块空中速率 38.4K
	LoRa_Data240,                        //数据分包大小 240字节
	LoRa_RssiDIS,                        //关闭RSSI功能
	LoRa_FEC_22DBM,                      //发射功率22dbm
	LoRa_CH23,                           //模块信道
	LoRa_RssiByteDIS,                    //禁用RSSI字节功能
	LoRa_ModePOINT,                      //定点模式
	LoRa_RelayDIS,                       //禁用中继模式
	LoRa_LBTEN,                          //使能LBT
	LoRa_WorTX,                          //Wor模式发送        只在模式1才有效
	LoRa_Wor2000ms,                      //Wor周期2000毫秒    只在模式1才有效
	0x22,                                //模块加密秘钥高字节
	0x33,                                //模块加密秘钥低字节	
};

/*-------------------------------------------------*/
/*函数名：初始化LoRa模块的IO                       */
/*参  数：timeout： 超时时间                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LoRa_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_Initure;                //GPIO端口设置变量
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			      //使能GPIOB时钟
	__HAL_RCC_GPIOF_CLK_ENABLE();			      //使能GPIOF时钟
	
	GPIO_Initure.Pin = GPIO_PIN_7;                //准备设置PB7
	GPIO_Initure.Mode = GPIO_MODE_INPUT;          //输入功能
	GPIO_Initure.Pull = GPIO_PULLUP;              //上拉
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);          //设置
	
	GPIO_Initure.Pin = GPIO_PIN_1;                //准备设置PF1
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;      //输出功能
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);          //设置	
	
	GPIO_Initure.Pin = GPIO_PIN_5 | GPIO_PIN_6;   //准备设置PB5 6
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;      //输出功能
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);          //设置	
	
	LoRa_MODE2;                                   //LoRa工作模式2	
	HAL_Delay(200);                               //适当延时
}
/*-------------------------------------------------*/
/*函数名：LoRa上电复位                             */
/*参  数：timeout：超时时间                        */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
uint8_t LoRa_Reset(int16_t timeout)
{
    LoRa_PowerOFF;                  //先关闭电源
	HAL_Delay(200);                 //延时
	LoRa_PowerON;                   //再打开电源
	
	while(timeout--)                //等待复位成功
	{                           
		HAL_Delay(100);             //延时100ms
		if(LoRa_AUX==1)             //需要等到高电平1，才能跳出while，表示复位完毕
			break;       			//主动跳出while循环
		u1_printf("%d ",timeout);   //串口输出现在的超时时间		
	} 
	u1_printf("\r\n");              //串口输出信息
	if(timeout<=0)return 1;         //如果timeout<=0，说明超时时间到了，也没能等到LoRa_AUX高电平，返回1
	return 0;                       //正确，返回0
}
/*-------------------------------------------------*/
/*函数名：初始化模块                               */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LoRa_Init(void)
{
	LoRa_GPIO_Init();                                 //Lora模块引脚初始化
	u1_printf("准备复位Lora模块\r\n");   	          //串口输出信息
	if(LoRa_Reset(100)){                              //复位Lora模块，返回值如果非零，表示错误
		u1_printf("复位Lora模块失败，重启\r\n");   	  //串口输出信息
		NVIC_SystemReset();                           //重启
	}else u1_printf("Lora模块复位成功\r\n");   	      //串口输出信息	
	u1_printf("准备设置Lora模块\r\n");   	          //串口输出信息
	LoRa_Set();                                       //设置Lora模块
	HAL_Delay(200);                                   //延时		
}
/*-------------------------------------------------*/
/*函数名：LoRa设置工作参数                         */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LoRa_Set(void)
{
	uint8_t cmd[12];                                         //保存设置参数的数组缓冲区，命令共计12字节
	 
	cmd[0] = 0xC0;                                           //设置参数指令的起始字节，固定值0xC0
	cmd[1] = 0x00;                                           //设置参数的起始寄存器地址，从寄存器0开始设置，一共9个
	cmd[2] = 0x09;                                           //一共设置9个寄存器
	cmd[3] = LoRaSetData.LoRa_AddrH;                         //设置模块地址高字节
	cmd[4] = LoRaSetData.LoRa_AddrL;                         //设置模块地址低字节
	cmd[5] = LoRaSetData.LoRa_NetID;                         //设置模块网络地址	
	cmd[6] = LoRaSetData.LoRa_Baudrate | LoRaSetData.LoRa_UartMode | LoRaSetData.LoRa_airvelocity;    //设置模块 波特率 串口模式 空中速率
	cmd[7] = LoRaSetData.LoRa_DataLen | LoRaSetData.LoRa_Rssi | LoRaSetData.LoRa_TxPower;             //设置模块 数据分包长度 RSSI噪声是否使能 发射功率
	cmd[8] = LoRaSetData.LoRa_CH;                            //设置模块信道	
	cmd[9] = LoRaSetData.LoRa_RssiByte | LoRaSetData.LoRa_DateMode | LoRaSetData.LoRa_Relay | LoRaSetData.LoRa_LBT | LoRaSetData.LoRa_WORmode | LoRaSetData.LoRa_WORcycle; //设置模块 是否使用RSSI字节功能 传输方式 是否中继 是否使用LBT WOR模式收发控制 WOR周期
	cmd[10] = LoRaSetData.LoRa_KeyH;                         //设置模块加密秘钥高字节
	cmd[11] = LoRaSetData.LoRa_KeyL;                         //设置模块加密秘钥低字节
    u3_TxDataBuf(cmd,12);                                    //发送缓冲区加入数据	
}
/*-------------------------------------------------*/
/*函数名：LoRa查询工作参数                         */
/*参  数：data：工作参数数据                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LoRa_Get(uint8_t *data)
{
	uint8_t ch;                                                          //保存信道的变量
	 		         			           	                         
	u1_printf("模块地址:0x%02X%02X\r\n",data[3],data[4]);                //串口输出信息
	u1_printf("模块网络ID:0x%02X\r\n",data[5]);                          //串口输出信息
	
	switch(data[6]&0xE0){                                                //判断串口波特率
		case LoRa_1200 : u1_printf("波特率 1200\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_2400 : u1_printf("波特率 2400\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_4800 : u1_printf("波特率 4800\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_9600 : u1_printf("波特率 9600\r\n");                   //串口输出信息
						 break;                                          //跳出
		case LoRa_19200 : u1_printf("波特率 19200\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_38400 : u1_printf("波特率 38400\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_57600 : u1_printf("波特率 57600\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_115200: u1_printf("波特率 115200\r\n");                //串口输出信息
						 break;                                          //跳出
	}	
	switch(data[6]&0x18){                                                //判断串口参数
		case LoRa_8N1 :	 u1_printf("8数据位 无校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		case LoRa_8O1 :	 u1_printf("8数据位 奇校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
		case LoRa_8E1 :	 u1_printf("8数据位 偶校验 1停止位\r\n");        //串口输出信息
						 break;                                          //跳出
	}		
	switch(data[6]&0x07){                                                //判断空中速率
		case LoRa_0_3 :  u1_printf("空中速率 0.3K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_1_2 :  u1_printf("空中速率 1.2K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_2_4 :  u1_printf("空中速率 2.4K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_4_8 :  u1_printf("空中速率 4.8K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_9_6 :  u1_printf("空中速率 9.6K\r\n");                 //串口输出信息
						 break;                                          //跳出
		case LoRa_19_2 : u1_printf("空中速率 19.2K\r\n");                //串口输出信息
						 break;                                          //跳出     
		case LoRa_38_4 : u1_printf("空中速率 38.4K\r\n");                //串口输出信息
						 break;  		                                 //跳出
		case LoRa_62_5 : u1_printf("空中速率 62.5K\r\n");                //串口输出信息
						 break;  		                                 //跳出
	}	
	switch(data[7]&0xC0){                                                //判断数据分包大小
		case LoRa_Data240 : u1_printf("数据分包大小：240字节\r\n");      //串口输出信息
							break;                                       //跳出
		case LoRa_Data128 : u1_printf("数据分包大小：128字节\r\n");      //串口输出信息
							break;                                       //跳出
		case LoRa_Data64  : u1_printf("数据分包大小：64字节\r\n");       //串口输出信息
							break;                                       //跳出
		case LoRa_Data32  : u1_printf("数据分包大小：32字节\r\n");       //串口输出信息
							break;                                       //跳出
	}	
	switch(data[7]&0x20){                                                //判断是否启用RSSI功能
		case LoRa_RssiEN  :	u1_printf("启用RSSI功能\r\n");               //串口输出信息
							break;                                       //跳出
		case LoRa_RssiDIS :	u1_printf("禁用RSSI功能\r\n");               //串口输出信息
							break;                                       //跳出
	}		
	switch(data[7]&0x03){                                                //判断发射功率
		case LoRa_FEC_22DBM :  u1_printf("发射功率 22dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_17DBM :  u1_printf("发射功率 17dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_13DBM :  u1_printf("发射功率 13dbm\r\n");          //串口输出信息
							   break;                                    //跳出
		case LoRa_FEC_10DBM :  u1_printf("发射功率 10dbm\r\n");          //串口输出信息
							   break;                                    //跳出
	}
	
	ch = data[8] & 0x7F;                                                 //保存信道
	u1_printf("信道:0x%02X  对应频率%dMHz\r\n",ch,410+ch);               //串口输出信息
	
	switch(data[9]&0x80){                                                //判断RSSI字节功能
		case LoRa_RssiByteEN  : u1_printf("启用RSSI字节功能\r\n");       //串口输出信息
								break;                                   //跳出
		case LoRa_RssiByteDIS : u1_printf("禁用RSSI字节功能\r\n");       //串口输出信息
								break;                                   //跳出
	}
	switch(data[9]&0x40){                                                //判断传输模式
		case LoRa_ModeTRANS :  u1_printf("透明传输\r\n");                //串口输出信息
							   break;                                    //跳出
		case LoRa_ModePOINT :  u1_printf("定点传输\r\n");                //串口输出信息
							   break;                                    //跳出
	}
	switch(data[9]&0x20){                                               //判断中继功能
		case LoRa_RelayEN  :  u1_printf("启用中继\r\n");                //串口输出信息
							  break;                                    //跳出
		case LoRa_RelayDIS :  u1_printf("禁用中继\r\n");                //串口输出信息
							  break;                                    //跳出
	}
	switch(data[9]&0x10){                                               //判断LBT功能
		case LoRa_LBTEN  :  u1_printf("启用LBT\r\n");                   //串口输出信息
							  break;                                    //跳出
		case LoRa_LBTDIS :  u1_printf("禁用LBT\r\n");                   //串口输出信息
							  break;                                    //跳出
	}
//	switch(data[9]&0x08){                                               //判断WOR模式
//		case LoRa_WorTX  :  u1_printf("Wor模式发送\r\n");               //串口输出信息
//							  break;                                    //跳出
//		case LoRa_WorRX :  u1_printf("Wor模式接收\r\n");                //串口输出信息
//							  break;                                    //跳出
//	}
//	switch(data[9]&0x07){                                               //判断WOR周期
//		case LoRa_Wor500ms  : u1_printf("WOR周期时间 500毫秒\r\n");     //串口输出信息
//							  break;                                    //跳出
//		case LoRa_Wor1000ms : u1_printf("WOR周期时间 1000毫秒\r\n");    //串口输出信息
//							  break;                                    //跳出
//		case LoRa_Wor1500ms : u1_printf("WOR周期时间 1500毫秒\r\n");    //串口输出信息
//							  break;                                    //跳出
//		case LoRa_Wor2000ms : u1_printf("WOR周期时间 2000毫秒\r\n");    //串口输出信息
//							  break;                                    //跳出
//		case LoRa_Wor2500ms : u1_printf("WOR周期时间 2500毫秒\r\n");    //串口输出信息
//							  break;                                    //跳出
//		case LoRa_Wor3000ms : u1_printf("WOR周期时间 3000毫秒\r\n");    //串口输出信息
//							  break;                                    //跳出
//		case LoRa_Wor3500ms : u1_printf("WOR周期时间 3500毫秒\r\n");    //串口输出信息
//							  break;                                    //跳出
//		case LoRa_Wor4000ms : u1_printf("WOR周期时间 4000毫秒\r\n");    //串口输出信息
//							  break;                                    //跳出
//	}
}
/*-------------------------------------------------*/
/*函数名：串口3被动事件                            */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
uint8_t tempdata[512];
uint8_t Sign[256];     
void U3PassiveEvent(uint8_t *data, uint16_t datalen)
{
	
	/*----------------------------------------------*/
	/*           接收设置LoRa模块返回数据           */
	/*----------------------------------------------*/
	if((data[0]==0xC1)&&(datalen==12)){                   
		u1_printf("接收设置LoRa模块返回数据\r\n");        //串口输出信息
		LoRa_Get(data);                                   //分析设置的参数
		LoRa_MODE0;                                       //切换到模式0
		HAL_Delay(200);                                   //延时
		CAT1_Reset();                                     //4G模块重启
    }	
	/*----------------------------------------------*/
	/*         接收到子设备1上发的证书信息          */
	/*----------------------------------------------*/
	if((data[0]==VALID_ALICER1_DATA)&&(datalen==(ALIINFO_STRUCT_LEN+1))){
		u1_printf("接收到子设备1证书信息\r\n"); 
		memcpy(&AliInfoCB_SUB[0],&data[1],ALIINFO_STRUCT_LEN);
		memset(tempdata,0,512);                                                                                    //临时缓冲区全部清零
		memset(Sign,0,256);                                                                                        //临时缓冲区全部清零	
		sprintf((char *)tempdata,"clientId%s&%sdeviceName%sproductKey%s",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,AliInfoCB_SUB[0].DeviceNameBuff,AliInfoCB_SUB[0].ProductKeyBuff);     //构建加密时的明文   
		utils_hmac_sha1((char *)tempdata,strlen((char *)tempdata),(char *)Sign,(char *)AliInfoCB_SUB[0].DeviceSecretBuff,DEVICESECRET_LEN);//以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是子设备签名，并保存到缓冲区中
		memset(tempdata,0,512);
		sprintf((char *)tempdata,"{\"id\": \"1\",\"params\": {\"productKey\": \"%s\",\"deviceName\": \"%s\",\"clientId\": \"%s&%s\",\"signMethod\": \"hmacSha1\",\"sign\": \"%s\",\"cleanSession\": \"true\"}}",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,Sign);  //构建子设备1上线数据
		CAT1_SubDeviceLogin((char *)tempdata);                                                                     //插入子设备上线数据报文	
    }	
	/*----------------------------------------------*/
	/*         接收到子设备2上发的证书信息           */
	/*----------------------------------------------*/
	if((data[0]==VALID_ALICER2_DATA)&&(datalen==(ALIINFO_STRUCT_LEN+1))){
		u1_printf("接收到子设备2证书信息\r\n"); 
		memcpy(&AliInfoCB_SUB[1],&data[1],ALIINFO_STRUCT_LEN);
		memset(tempdata,0,512);                                                                                    //临时缓冲区全部清零
		memset(Sign,0,256);                                                                                        //临时缓冲区全部清零	
		sprintf((char *)tempdata,"clientId%s&%sdeviceName%sproductKey%s",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,AliInfoCB_SUB[1].DeviceNameBuff,AliInfoCB_SUB[1].ProductKeyBuff);     //构建加密时的明文   
		utils_hmac_sha1((char *)tempdata,strlen((char *)tempdata),(char *)Sign,(char *)AliInfoCB_SUB[1].DeviceSecretBuff,DEVICESECRET_LEN);//以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是子设备签名，并保存到缓冲区中
		memset(tempdata,0,512);
		sprintf((char *)tempdata,"{\"id\": \"1\",\"params\": {\"productKey\": \"%s\",\"deviceName\": \"%s\",\"clientId\": \"%s&%s\",\"signMethod\": \"hmacSha1\",\"sign\": \"%s\",\"cleanSession\": \"true\"}}",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,Sign);  //构建子设备1上线数据
		CAT1_SubDeviceLogin((char *)tempdata);                                                                     //插入子设备上线数据报文	
    }
	/*----------------------------------------------*/
	/*         接收到子设备3上发的证书信息           */
	/*----------------------------------------------*/
	if((data[0]==VALID_ALICER3_DATA)&&(datalen==(ALIINFO_STRUCT_LEN+1))){
		u1_printf("接收到子设备3证书信息\r\n"); 
		memcpy(&AliInfoCB_SUB[2],&data[1],ALIINFO_STRUCT_LEN);
		memset(tempdata,0,512);                                                                                    //临时缓冲区全部清零
		memset(Sign,0,256);                                                                                        //临时缓冲区全部清零	
		sprintf((char *)tempdata,"clientId%s&%sdeviceName%sproductKey%s",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,AliInfoCB_SUB[2].DeviceNameBuff,AliInfoCB_SUB[2].ProductKeyBuff);     //构建加密时的明文   
		utils_hmac_sha1((char *)tempdata,strlen((char *)tempdata),(char *)Sign,(char *)AliInfoCB_SUB[2].DeviceSecretBuff,DEVICESECRET_LEN);//以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是子设备签名，并保存到缓冲区中
		memset(tempdata,0,512);
		sprintf((char *)tempdata,"{\"id\": \"1\",\"params\": {\"productKey\": \"%s\",\"deviceName\": \"%s\",\"clientId\": \"%s&%s\",\"signMethod\": \"hmacSha1\",\"sign\": \"%s\",\"cleanSession\": \"true\"}}",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,Sign);  //构建子设备1上线数据
		CAT1_SubDeviceLogin((char *)tempdata);                                                                     //插入子设备上线数据报文	
    }
    /*----------------------------------------------*/
	/*            接收到子设备1上发的数据           */
	/*----------------------------------------------*/
	if((data[0]==SUB1_DATA)&&(datalen==(ATTRIBUTE_STRUCT_LEN+1))){
		u1_printf("接收到子设备1数据\r\n"); 
		memcpy(&AttributeCB[0],&data[1],ATTRIBUTE_STRUCT_LEN);                   //拷贝数据
	    u1_printf("开关1：%d\r\n",AttributeCB[0].Switch1Sta);                    //串口输出数据
		u1_printf("开关2：%d\r\n",AttributeCB[0].Switch2Sta);                    //串口输出数据
		u1_printf("开关3：%d\r\n",AttributeCB[0].Switch3Sta);                    //串口输出数据
		u1_printf("开关4：%d\r\n",AttributeCB[0].Switch4Sta);                    //串口输出数据
		u1_printf("温度：%.2f °C\r\n",AttributeCB[0].tempdata);                  //串口输出数据
		u1_printf("湿度：%.2f %\r\n",AttributeCB[0].humidata);                   //串口输出数据
		u1_printf("光照度：%.2f lx\r\n",AttributeCB[0].lightata);   	         //串口输出	
	    u1_printf("ADC_CH5:%.2f V\r\n",AttributeCB[0].adcdata[0]);               //串口输出
		u1_printf("ADC_CH6:%.2f V\r\n",AttributeCB[0].adcdata[1]);               //串口输出
		u1_printf("ADC_CH7:%.2f V\r\n",AttributeCB[0].adcdata[2]);               //串口输出		
		memset(tempdata,0,512);                                                  //临时缓冲区全部清零
		sprintf((char *)tempdata,"{\"id\":\"2\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f}}}]}}",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,ATTRIBUTE1,AttributeCB[0].Switch1Sta,ATTRIBUTE2,AttributeCB[0].Switch2Sta,ATTRIBUTE3,AttributeCB[0].Switch3Sta,ATTRIBUTE4,AttributeCB[0].Switch4Sta,ATTRIBUTE5,AttributeCB[0].tempdata,ATTRIBUTE6,AttributeCB[0].humidata,ATTRIBUTE7,AttributeCB[0].lightata,ATTRIBUTE8,AttributeCB[0].adcdata[0],ATTRIBUTE9,AttributeCB[0].adcdata[1],ATTRIBUTE10,AttributeCB[0].adcdata[2]);  //构建数据
		CAT1_SubDevicePost((char *)tempdata);   			
    }
    /*----------------------------------------------*/
	/*            接收到子设备2上发的数据           */
	/*----------------------------------------------*/
	if((data[0]==SUB2_DATA)&&(datalen==(ATTRIBUTE_STRUCT_LEN+1))){
		u1_printf("接收到子设备2数据\r\n"); 
		memcpy(&AttributeCB[1],&data[1],ATTRIBUTE_STRUCT_LEN);                   //拷贝数据
	    u1_printf("开关1：%d\r\n",AttributeCB[1].Switch1Sta);                    //串口输出数据
		u1_printf("开关2：%d\r\n",AttributeCB[1].Switch2Sta);                    //串口输出数据
		u1_printf("开关3：%d\r\n",AttributeCB[1].Switch3Sta);                    //串口输出数据
		u1_printf("开关4：%d\r\n",AttributeCB[1].Switch4Sta);                    //串口输出数据
		u1_printf("温度：%.2f °C\r\n",AttributeCB[1].tempdata);                  //串口输出数据
		u1_printf("湿度：%.2f %\r\n",AttributeCB[1].humidata);                   //串口输出数据
		u1_printf("光照度：%.2f lx\r\n",AttributeCB[1].lightata);   	         //串口输出	
	    u1_printf("ADC_CH5:%.2f V\r\n",AttributeCB[1].adcdata[0]);               //串口输出
		u1_printf("ADC_CH6:%.2f V\r\n",AttributeCB[1].adcdata[1]);               //串口输出
		u1_printf("ADC_CH7:%.2f V\r\n",AttributeCB[1].adcdata[2]);               //串口输出		
		memset(tempdata,0,512);                                                  //临时缓冲区全部清零
		sprintf((char *)tempdata,"{\"id\":\"2\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f}}}]}}",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,ATTRIBUTE1,AttributeCB[1].Switch1Sta,ATTRIBUTE2,AttributeCB[1].Switch2Sta,ATTRIBUTE3,AttributeCB[1].Switch3Sta,ATTRIBUTE4,AttributeCB[1].Switch4Sta,ATTRIBUTE5,AttributeCB[1].tempdata,ATTRIBUTE6,AttributeCB[1].humidata,ATTRIBUTE7,AttributeCB[1].lightata,ATTRIBUTE8,AttributeCB[1].adcdata[0],ATTRIBUTE9,AttributeCB[1].adcdata[1],ATTRIBUTE10,AttributeCB[1].adcdata[2]);  //构建数据
		CAT1_SubDevicePost((char *)tempdata);   			
    }
	/*----------------------------------------------*/
	/*            接收到子设备3上发的数据           */
	/*----------------------------------------------*/
	if((data[0]==SUB3_DATA)&&(datalen==(ATTRIBUTE_STRUCT_LEN+1))){
		u1_printf("接收到子设备3数据\r\n"); 
		memcpy(&AttributeCB[2],&data[1],ATTRIBUTE_STRUCT_LEN);                   //拷贝数据
	    u1_printf("开关1：%d\r\n",AttributeCB[2].Switch1Sta);                    //串口输出数据
		u1_printf("开关2：%d\r\n",AttributeCB[2].Switch2Sta);                    //串口输出数据
		u1_printf("开关3：%d\r\n",AttributeCB[2].Switch3Sta);                    //串口输出数据
		u1_printf("开关4：%d\r\n",AttributeCB[2].Switch4Sta);                    //串口输出数据
		u1_printf("温度：%.2f °C\r\n",AttributeCB[2].tempdata);                  //串口输出数据
		u1_printf("湿度：%.2f %\r\n",AttributeCB[2].humidata);                   //串口输出数据
		u1_printf("光照度：%.2f lx\r\n",AttributeCB[2].lightata);   	         //串口输出	
	    u1_printf("ADC_CH5:%.2f V\r\n",AttributeCB[2].adcdata[0]);               //串口输出
		u1_printf("ADC_CH6:%.2f V\r\n",AttributeCB[2].adcdata[1]);               //串口输出
		u1_printf("ADC_CH7:%.2f V\r\n",AttributeCB[2].adcdata[2]);               //串口输出		
		memset(tempdata,0,512);                                                  //临时缓冲区全部清零
		sprintf((char *)tempdata,"{\"id\":\"2\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f},\"%s\": {\"value\": %.2f}}}]}}",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,ATTRIBUTE1,AttributeCB[2].Switch1Sta,ATTRIBUTE2,AttributeCB[2].Switch2Sta,ATTRIBUTE3,AttributeCB[2].Switch3Sta,ATTRIBUTE4,AttributeCB[2].Switch4Sta,ATTRIBUTE5,AttributeCB[2].tempdata,ATTRIBUTE6,AttributeCB[2].humidata,ATTRIBUTE7,AttributeCB[2].lightata,ATTRIBUTE8,AttributeCB[2].adcdata[0],ATTRIBUTE9,AttributeCB[2].adcdata[1],ATTRIBUTE10,AttributeCB[2].adcdata[2]);  //构建数据
		CAT1_SubDevicePost((char *)tempdata);   			
    }
	/*----------------------------------------------*/
	/*        接收到子设备1回复的开关控制数据       */
	/*----------------------------------------------*/
	if((data[0]==SUB1_SWITCH_DATA)&&(datalen==5)){
		u1_printf("接收到子设备1回复的开关控制数据\r\n"); 
		memset(tempdata,0,512);                                                  //临时缓冲区全部清零
		sprintf((char *)tempdata,"{\"id\":\"3\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d}}}]}}",AliInfoCB_SUB[0].ProductKeyBuff,AliInfoCB_SUB[0].DeviceNameBuff,ATTRIBUTE1,data[1],ATTRIBUTE2,data[2],ATTRIBUTE3,data[3],ATTRIBUTE4,data[4]);  //构建数据
		CAT1_SubDevicePost((char *)tempdata);   			
    }
    /*----------------------------------------------*/
	/*        接收到子设备2回复的开关控制数据       */
	/*----------------------------------------------*/
	if((data[0]==SUB2_SWITCH_DATA)&&(datalen==5)){
		u1_printf("接收到子设备2回复的开关控制数据\r\n"); 
		memset(tempdata,0,512);                                                  //临时缓冲区全部清零
		sprintf((char *)tempdata,"{\"id\":\"3\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d}}}]}}",AliInfoCB_SUB[1].ProductKeyBuff,AliInfoCB_SUB[1].DeviceNameBuff,ATTRIBUTE1,data[1],ATTRIBUTE2,data[2],ATTRIBUTE3,data[3],ATTRIBUTE4,data[4]);  //构建数据
		CAT1_SubDevicePost((char *)tempdata);   			
    }	
	/*----------------------------------------------*/
	/*        接收到子设备3回复的开关控制数据       */
	/*----------------------------------------------*/
	if((data[0]==SUB3_SWITCH_DATA)&&(datalen==5)){
		u1_printf("接收到子设备3回复的开关控制数据\r\n"); 
		memset(tempdata,0,512);                                                  //临时缓冲区全部清零
		sprintf((char *)tempdata,"{\"id\":\"3\",\"params\":{\"subDevices\": [{\"identity\": {\"productKey\": \"%s\",\"deviceName\": \"%s\"},\"properties\": {\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d},\"%s\": {\"value\": %d}}}]}}",AliInfoCB_SUB[2].ProductKeyBuff,AliInfoCB_SUB[2].DeviceNameBuff,ATTRIBUTE1,data[1],ATTRIBUTE2,data[2],ATTRIBUTE3,data[3],ATTRIBUTE4,data[4]);  //构建数据
		CAT1_SubDevicePost((char *)tempdata);   			
    }
	/*----------------------------------------------*/
	/*       接收到子设备1接收一包OTA数据的回复     */
	/*----------------------------------------------*/
	if((data[0]==SUB1_OTA_S)&&(datalen==1)){
		AliOTA.OTA_num++;                                                               //AliOTA.OTA_num 加1
		if(AliOTA.OTA_num<=AliOTA.OTA_timers){                                          //还没有下载完毕，进入if
			u1_printf("准备OTA发送固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//串口输出提示信息		
			memset(tempdata,0,512);                                                     //清空缓冲区
			tempdata[0] = SUB1_HADR;                                                    //接收方 地址高字节
			tempdata[1] = SUB1_LADR;                                                    //接收方 地址低字节
			tempdata[2] = SUB1_CH;                                              	    //接收方 信道		
			tempdata[3] = 0x5A;                                                         //固定字节
			tempdata[4] = 0xA5;                                                         //固定字节
			tempdata[5] = AliOTA.OTA_num/256;                                           //数据包编号高字节		
			tempdata[6] = AliOTA.OTA_num%256;                                           //数据包编号低字节											
			W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //读取数据
			u3_TxDataBuf((uint8_t *)tempdata,135);                                      //添加数据到发送缓冲区
		}else{ 
			u1_printf("子设备1 OTA固件发送完毕\r\n"); 
			CAT1_PropertyVersion(AliInfoCB_SUB[0].Version_ABuff,"SUB1");		        //同步子设备1当前固件版本号
			memset(tempdata,0,512);                                                	 	//清空缓冲区			
			tempdata[0] = SUB1_HADR;                                               		//接收方 地址高字节
			tempdata[1] = SUB1_LADR;                                                    //接收方 地址低字节
			tempdata[2] = SUB1_CH;                                              	    //接收方 信道
			sprintf((char *)&tempdata[3],"AT+VER1=%s",AliInfoCB_SUB[0].Version_ABuff);  //添加新的版本号
			tempdata[3+strlen((char *)&tempdata[3])] = 0;                               //固定 0
			tempdata[4+strlen((char *)&tempdata[3])] = AliOTA.streamSize/256;           //固件大小高字节
			tempdata[5+strlen((char *)&tempdata[3])] = AliOTA.streamSize%256;           //固件大小低字节
			u3_TxDataBuf((uint8_t *)tempdata,6+strlen((char *)&tempdata[3]));           //添加数据到发送缓冲区
		}
	}
	/*----------------------------------------------*/
	/*       接收到子设备1重发一包OTA数据的回复     */
	/*----------------------------------------------*/
	if((data[0]==SUB1_OTA_C)&&(datalen==1)){
		u1_printf("准备OTA发送固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	    //串口输出提示信息		
		memset(tempdata,0,512);                                                         //清空缓冲区
		tempdata[0] = SUB1_HADR;                                                        //接收方 地址高字节
		tempdata[1] = SUB1_LADR;                                                        //接收方 地址低字节
		tempdata[2] = SUB1_CH;                                              	        //接收方 信道		
		tempdata[3] = 0x5A;                                                             //固定字节
		tempdata[4] = 0xA5;                                                             //固定字节
		tempdata[5] = AliOTA.OTA_num/256;                                               //数据包编号高字节		
		tempdata[6] = AliOTA.OTA_num%256;                                               //据包编号低字节									
		W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //读取数据
		u3_TxDataBuf((uint8_t *)tempdata,135);                                          //添加数据到发送缓冲区

	}
    /*----------------------------------------------*/
	/*       接收到子设备2接收一包OTA数据的回复     */
	/*----------------------------------------------*/
	if((data[0]==SUB2_OTA_S)&&(datalen==1)){
		AliOTA.OTA_num++;                                                               //AliOTA.OTA_num 加1
		if(AliOTA.OTA_num<=AliOTA.OTA_timers){                                          //还没有下载完毕，进入if
			u1_printf("准备OTA发送固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//串口输出提示信息		
			memset(tempdata,0,512);                                                     //清空缓冲区
			tempdata[0] = SUB2_HADR;                                                    //接收方 地址高字节
			tempdata[1] = SUB2_LADR;                                                    //接收方 地址低字节
			tempdata[2] = SUB2_CH;                                              	    //接收方 信道		
			tempdata[3] = 0x5A;                                                         //固定字节
			tempdata[4] = 0xA5;                                                         //固定字节
			tempdata[5] = AliOTA.OTA_num/256;                                           //数据包编号高字节			
			tempdata[6] = AliOTA.OTA_num%256;                                           //数据包编号低字节									
			W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //读取数据
			u3_TxDataBuf((uint8_t *)tempdata,135);                                      //添加数据到发送缓冲区
		}else{ 
			u1_printf("子设备2 OTA固件发送完毕\r\n"); 
			CAT1_PropertyVersion(AliInfoCB_SUB[1].Version_ABuff,"SUB2");		        //同步子设备2当前固件版本号
			memset(tempdata,0,512);                                                     //清空缓冲区			
			tempdata[0] = SUB2_HADR;                                                    //接收方 地址高字节
			tempdata[1] = SUB2_LADR;                                                    //接收方 地址低字节
			tempdata[2] = SUB2_CH;                                              	    //接收方 信道
			sprintf((char *)&tempdata[3],"AT+VER2=%s",AliInfoCB_SUB[1].Version_ABuff);  //添加新的版本号
			tempdata[3+strlen((char *)&tempdata[3])] = 0;                               //固定 0
			tempdata[4+strlen((char *)&tempdata[3])] = AliOTA.streamSize/256;           //固件大小高字节
			tempdata[5+strlen((char *)&tempdata[3])] = AliOTA.streamSize%256;           //固件大小低字节
			u3_TxDataBuf((uint8_t *)tempdata,6+strlen((char *)&tempdata[3]));           //添加数据到发送缓冲区
		}
	}
	/*----------------------------------------------*/
	/*       接收到子设备2重发一包OTA数据的回复     */
	/*----------------------------------------------*/
	if((data[0]==SUB2_OTA_C)&&(datalen==1)){
		u1_printf("准备OTA发送固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//串口输出提示信息		
		memset(tempdata,0,512);                                                     //清空缓冲区
		tempdata[0] = SUB2_HADR;                                                    //接收方 地址高字节
		tempdata[1] = SUB2_LADR;                                                    //接收方 地址低字节
		tempdata[2] = SUB2_CH;                                              	    //接收方 信道		
		tempdata[3] = 0x5A;                                                         //固定字节
		tempdata[4] = 0xA5;                                                         //固定字节
		tempdata[5] = AliOTA.OTA_num/256;                                           //数据包编号高字节		
		tempdata[6] = AliOTA.OTA_num%256;                                           //据包编号低字节										
		W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //读取数据
		u3_TxDataBuf((uint8_t *)tempdata,135);                                      //添加数据到发送缓冲区

	}
	/*----------------------------------------------*/
	/*       接收到子设备3接收一包OTA数据的回复     */
	/*----------------------------------------------*/
	if((data[0]==SUB3_OTA_S)&&(datalen==1)){
		AliOTA.OTA_num++;                                                               //AliOTA.OTA_num 加1
		if(AliOTA.OTA_num<=AliOTA.OTA_timers){                                          //还没有下载完毕，进入if
			u1_printf("准备OTA发送固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//串口输出提示信息		
			memset(tempdata,0,512);                                                     //清空缓冲区
			tempdata[0] = SUB3_HADR;                                                    //接收方 地址高字节
			tempdata[1] = SUB3_LADR;                                                    //接收方 地址低字节
			tempdata[2] = SUB3_CH;                                              	    //接收方 信道		
			tempdata[3] = 0x5A;                                                         //固定字节
			tempdata[4] = 0xA5;                                                         //固定字节
			tempdata[5] = AliOTA.OTA_num/256;                                           //数据包编号高字节			
			tempdata[6] = AliOTA.OTA_num%256;                                           //数据包编号低字节									
			W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //读取数据
			u3_TxDataBuf((uint8_t *)tempdata,135);                                      //添加数据到发送缓冲区
		}else{ 
			u1_printf("子设备2 OTA固件发送完毕\r\n"); 
			CAT1_PropertyVersion(AliInfoCB_SUB[2].Version_ABuff,"SUB3");		        //同步子设备3当前固件版本号
			memset(tempdata,0,512);                                                     //清空缓冲区			
			tempdata[0] = SUB3_HADR;                                                    //接收方 地址高字节
			tempdata[1] = SUB3_LADR;                                                    //接收方 地址低字节
			tempdata[2] = SUB3_CH;                                              	    //接收方 信道
			sprintf((char *)&tempdata[3],"AT+VER3=%s",AliInfoCB_SUB[2].Version_ABuff);  //添加新的版本号
			tempdata[3+strlen((char *)&tempdata[3])] = 0;                               //固定 0
			tempdata[4+strlen((char *)&tempdata[3])] = AliOTA.streamSize/256;           //固件大小高字节
			tempdata[5+strlen((char *)&tempdata[3])] = AliOTA.streamSize%256;           //固件大小低字节
			u3_TxDataBuf((uint8_t *)tempdata,6+strlen((char *)&tempdata[3]));           //添加数据到发送缓冲区
		}
	}
	/*----------------------------------------------*/
	/*       接收到子设备3重发一包OTA数据的回复     */
	/*----------------------------------------------*/
	if((data[0]==SUB2_OTA_C)&&(datalen==1)){
		u1_printf("准备OTA发送固件第%d/%d次\r\n",AliOTA.OTA_num,AliOTA.OTA_timers);	//串口输出提示信息		
		memset(tempdata,0,512);                                                     //清空缓冲区
		tempdata[0] = SUB3_HADR;                                                    //接收方 地址高字节
		tempdata[1] = SUB3_LADR;                                                    //接收方 地址低字节
		tempdata[2] = SUB3_CH;                                              	    //接收方 信道		
		tempdata[3] = 0x5A;                                                         //固定字节
		tempdata[4] = 0xA5;                                                         //固定字节
		tempdata[5] = AliOTA.OTA_num/256;                                           //数据包编号高字节		
		tempdata[6] = AliOTA.OTA_num%256;                                           //据包编号低字节										
		W25Qxx_Read((uint8_t *)&tempdata[7],AliOTA.OTA_dev*64*1024 + (AliOTA.OTA_num-1)*128,128);    //读取数据
		u3_TxDataBuf((uint8_t *)tempdata,135);                                      //添加数据到发送缓冲区
	}
}

