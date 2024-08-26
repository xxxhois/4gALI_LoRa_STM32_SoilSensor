/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*            实现IIc总线功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "clock.h"            //包含需要的头文件
#include "iic.h"              //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：初始化IIC接口                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IIC_Init(void)
{			
	GPIO_InitTypeDef GPIO_InitStruct;                 //定义一个设置GPIO的变量
		
	SDA_GROUP_CLK_ENABLE;                             //SDA 对应的IO分组 时钟使能
	SCL_GROUP_CLK_ENABLE;                             //SCL 对应的IO分组 时钟使能
	
	GPIO_InitStruct.Pin = SDA_PIN;                    //设置SDA对应的IO口
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;       //推免输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     //高速
	HAL_GPIO_Init(SDA_GROUP, &GPIO_InitStruct);       //设置
	
	GPIO_InitStruct.Pin = SCL_PIN;                    //设置SCL对应的IO口
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;       //推免输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     //高速
	HAL_GPIO_Init(SCL_GROUP, &GPIO_InitStruct);       //设置
	
	IIC_SCL_H;   //SCL拉高
	IIC_SDA_H;   //SDA拉高
}
/*-------------------------------------------------*/
/*函数名：IIC起始信号                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IIC_Start(void)
{
    IIC_SDA_H;	   //SDA拉高  
	IIC_SCL_H;     //SCL拉高	  
	Delay_Us(4);   //延时
 	IIC_SDA_L;     //SCL高电平的时候，SDA由高到低，起始信号
}
/*-------------------------------------------------*/
/*函数名：IIC停止信号                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IIC_Stop(void)
{
	IIC_SDA_L;     //SDA拉低
	IIC_SCL_H;     //SCL拉高
 	Delay_Us(4);   //延时
	IIC_SDA_H;     //SCL高电平的时候，SDA由低到高，发出停止信号					   	
}
/*-------------------------------------------------*/
/*函数名：等待应答                                 */
/*参  数：无                                       */
/*返回值：0：成功  1：失败                         */
/*-------------------------------------------------*/
char IIC_Wait_Ack(void)
{
	unsigned char timeout=0; //定义一个变量，用于计算超时时间
	
	IIC_SDA_H;	             //SDA拉高
	IIC_SCL_H;	             //SCL拉高
	 
	while(READ_SDA){         //等待SDA变低电平，表示应答到来，不然一直while循环，直到超时	
		timeout++;           //超时计数+1
		if(timeout>250){     //如果大于250	
			IIC_Stop();      //发送停止信号
			return 1;        //返回1，表示失败
		}
		Delay_Us(2);         //延时
	}
	IIC_SCL_L;               //SCL拉低
	Delay_Us(20);            //延时	
	return 0;                //返回0，表示成功
} 	
/*-------------------------------------------------*/
/*函数名：发送一个字节                             */
/*参  数：txd：发送的字节                          */
/*返回值：无                                       */
/*-------------------------------------------------*/ 			  
void IIC_Send_Byte(uint8_t txd)
{                        
    unsigned char t;                   //定义一个变量用于for循环 
	
    IIC_SCL_L;                         //SCL拉低，开始数据传输
    for(t=0;t<8;t++){                  //for循环，一位一位的发送，从最高位 位7开始                 
        IIC_SDA_OUT(((txd&0x80)>>7));  //给SDA数据线
        txd<<=1; 	                   //左移一位，准备下一次发送
		Delay_Us(2);                   //延时
		IIC_SCL_H;                     //SCL拉高
		Delay_Us(2);                   //延时
		IIC_SCL_L;	                   //SCL拉低
    }	 
} 	    
/*-------------------------------------------------*/
/*函数名：读取一个字节                             */
/*参  数：ack：是否发送应答 1:发 0：不发           */
/*返回值：读取的数据                               */
/*-------------------------------------------------*/   
unsigned char IIC_Read_Byte(uint8_t ack)
{
	unsigned char i;           //定义一个变量用于for循环 
	unsigned char receive;     //定义一个变量用于保存接收到的数据
		
	receive = 0;               //清除receive，准备接收数据
    IIC_SCL_L;                 //SCL拉低
    Delay_Us(20);              //延时	
	for(i=0;i<8;i++){          //for循环，一位一位的读取	 
		IIC_SCL_H;             //SCL拉高 
		Delay_Us(20);          //延时
        receive<<=1;           //左移一位，准备下次的读取
        if(READ_SDA)receive++; //读取数据
		Delay_Us(20);          //延时
		IIC_SCL_L;             //SCL拉低  
		Delay_Us(20);          //延时 		
    }					 
    if(!ack){        //不需要发送
		IIC_SDA_H;   //时钟脉冲的高电平期间保持稳定的高电平
		IIC_SCL_H;   //SCL拉高
		Delay_Us(2); //延时
		IIC_SCL_L;   //SCL拉低 
	}else{           //需要发送 
		IIC_SDA_L;   //时钟脉冲的高电平期间保持稳定的低电平
		IIC_SCL_H;   //SCL拉高
		Delay_Us(2); //延时
		IIC_SCL_L;   //SCL拉低
		IIC_SDA_H;   //SDA拉高
	}
    return receive;
}
