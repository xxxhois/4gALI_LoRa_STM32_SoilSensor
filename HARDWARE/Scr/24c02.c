/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*             实现24c02功能的源文件               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "clock.h" 	          //包含需要的头文件			 
#include "iic.h"              //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "mqtt.h"             //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：从24c02指定的地址读一字节数据            */
/*参  数：ReadAddr:需要读数据的地址                */
/*返回值：读到的数据                               */
/*-------------------------------------------------*/
uint8_t M24C02_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t receive;               //定义一个变量用于保存接收到的数据
		
	IIC_Start();                   //IIC开始信号
	IIC_Send_Byte(M24C02_WADDR);   //发送24c02器件地址+写操作	   	
	IIC_Wait_Ack();                //等待应答
    IIC_Send_Byte(ReadAddr);       //发送需要读数据的地址 
	IIC_Wait_Ack();	               //等待应答 
	IIC_Start();  	 	           //IIC开始信号
	IIC_Send_Byte(M24C02_RADDR);   //发送24c02器件地址+读操作				   
	IIC_Wait_Ack();	               //等待应答
    receive=IIC_Read_Byte(0);	   //读一字节数据，保存在receive变量中   
    IIC_Stop();                    //停止信号
	HAL_Delay(5);                   //延迟
	return receive;                //返回读取的数据
}
/*-------------------------------------------------*/
/*函数名：指定地址写入一个数据                     */
/*参  数：WriteAddr  :写入数据的目的地址           */
/*参  数：DataToWrite:要写入的数据                 */
/*返回值：无                                       */
/*-------------------------------------------------*/
void M24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();                    //开始信号
	IIC_Send_Byte(M24C02_WADDR);    //发送器件地址+写操作	 
	IIC_Wait_Ack();	                //等待应答
    IIC_Send_Byte(WriteAddr);       //发送写入数据的目的地址
	IIC_Wait_Ack(); 	 	        //等待应答									  		   
	IIC_Send_Byte(DataToWrite);     //发送要写入的数据							   
	IIC_Wait_Ack();  		        //等待应答	   
    IIC_Stop();                     //停止信号  
	HAL_Delay(5);                    //延迟
}
/*-------------------------------------------------*/
/*函数名：从指定的地址读指定数量数据               */
/*参  数：ReadAddr:需要读数据的地址                */
/*参  数：Redata:保存数据的缓冲区                  */
/*参  数：Redatalen:读多少数据                     */
/*返回值：无                                       */
/*-------------------------------------------------*/
void M24C02_ReadSequential(uint8_t ReadAddr,uint8_t *Redata,uint16_t Redatalen)
{				  		
	uint16_t i;                                //定义一个变量用于for循环   
	
	IIC_Start();                               //IIC开始信号
	IIC_Send_Byte(M24C02_WADDR);               //发送器件地址+写操作	   	
	IIC_Wait_Ack();                            //等待应答
    IIC_Send_Byte(ReadAddr);                   //发送需要读数据的地址 
	IIC_Wait_Ack();	                           //等待应答 
	IIC_Start();  	 	                       //IIC开始信号
	IIC_Send_Byte(M24C02_RADDR);               //发送器件地址+读操作				   
	IIC_Wait_Ack();	                           //等待应答
	for(i=0;i<(Redatalen-1);i++){              //循环读取前Redatalen-1个字节	
		Redata[i]=IIC_Read_Byte(1);            //读一字节数据，发送ACK信号 
    }	
	Redata[Redatalen-1]=IIC_Read_Byte(0);      //读最后一字节数据，不发送ACK信号 
    IIC_Stop();                                //停止信号
	HAL_Delay(5);                               //延迟
}
/*-------------------------------------------------*/
/*函数名：从EEprom指定的地址读数据                 */
/*参  数：addr:需要读数据的地址                    */
/*参  数：data:保存数据缓冲区                      */
/*参  数：len:要读取的数据长度                     */
/*返回值：无                                       */
/*-------------------------------------------------*/
void EEprom_ReadData(uint8_t addr,void *data, uint16_t len)
{				    
    uint8_t *ptr;                                 //定义一个指针用于读取数据
	
	ptr = data;                                   //ptr指向需要读取数据的缓冲区
	M24C02_ReadSequential(addr,ptr,len);          //读取数据
}
/*-------------------------------------------------*/
/*函数名：EEprom指定地址写入指定字节的数据         */
/*参  数：addr：地址                               */
/*参  数：data：写入的数据                         */
/*参  数：len：写入多少数据                        */
/*返回值：无                                       */
/*-------------------------------------------------*/
void EEprom_WriteData(uint8_t addr, void *data, uint16_t len)
{
	uint16_t i;                                    //用于for循环
	uint8_t *ptr;                                  //定义一个指针用于写入数据
	  
	ptr = data;                                    //ptr指向需要写入数据的缓冲区
	for(i=0;i<len;i++){                            //循环写入数据
		M24C02_WriteOneByte(addr,ptr[i]);          //写一字节数据
		addr++;                                    //地址+1
	}
}
/*-------------------------------------------------*/
/*函数名：从EEprom读取所有证书参数信息             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void EEprom_ReadInfo(void)
{				 
    memset(&AliInfoCB,0,ALIINFO_STRUCT_LEN);                              //清空结构体	
	EEprom_ReadData(0,&AliInfoCB,ALIINFO_STRUCT_LEN);                     //从地址0开始，读取保存的数据
	if(AliInfoCB.valid_cer != VALID_ALICER_DATA){                         //如果不成立 说明证书信息无效，需要重新设置
		u1_printf("证书信息无效 请在BootLoader中重新设置\r\n");           //串口1输出  提示信息
        NVIC_SystemReset();                                               //重启
	}else{                                                                //反之 说明证书信息有效 进入else
		IoT_Parameter_Init();                                             //阿里云初始化参数，得到客户端ID，用户名和密码
		u1_printf("--------------------------阿里云设备证书信息-------------------------\r\n");          //串口1输出信息
		u1_printf("ProductKey：%s\r\n",AliInfoCB.ProductKeyBuff);                                        //串口1输出信息
		u1_printf("DeviceName：%s\r\n",AliInfoCB.DeviceNameBuff);                                        //串口1输出信息
		u1_printf("DeviceSecret：%s\r\n",AliInfoCB.DeviceSecretBuff);                                    //串口1输出信息
		u1_printf("固件版本：%s\r\n",AliInfoCB.Version_ABuff);                                           //串口1输出信息		
		u1_printf("客户端ID：%s\r\n",Aliyun_mqtt.ClientID);                                              //串口1输出信息
		u1_printf("用 户 名：%s\r\n",Aliyun_mqtt.Username);               							     //串口1输出信息
		u1_printf("密    码：%s\r\n",Aliyun_mqtt.Passward);              							     //串口1输出信息
		u1_printf("服 务 器：%s:%d\r\n",Aliyun_mqtt.ServerIP,Aliyun_mqtt.ServerPort);                    //串口1输出信息
		u1_printf("---------------------------------------------------------------------\r\n\r\n");      //串口1输出信息					
	}
}

