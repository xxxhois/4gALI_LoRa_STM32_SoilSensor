/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*          实现各个SPI接口功能的源文件            */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "spi.h"              //包含需要的头文件

SPI_HandleTypeDef SPI2_Handler;  //SPI2句柄
uint8_t temprxdata;

/*-------------------------------------------------*/
/*函数名：初始化SPI2接口                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI2_Init(void)
{
    SPI2_Handler.Instance=SPI2;                                  //SPI2
    SPI2_Handler.Init.Mode=SPI_MODE_MASTER;                      //设置SPI工作模式，设置为主模式
    SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;            //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;                //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;             //串行同步时钟的空闲状态为高电平
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;                  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI2_Handler.Init.NSS=SPI_NSS_SOFT;                          //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2; //定义波特率预分频的值:波特率预分频值为2
    SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;                 //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;                 //关闭TI模式
    SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE; //关闭硬件CRC校验
    SPI2_Handler.Init.CRCPolynomial=7;                           //CRC值计算的多项式
    HAL_SPI_Init(&SPI2_Handler);                                 //初始化
	__HAL_SPI_ENABLE(&SPI2_Handler);                             //使能SPI2
}

/*-------------------------------------------------*/
/*函数名：SPI接口的底层初始化                      */
/*参  数：hspi：SPI接口配置句柄                    */
/*返回值：无                                       */
/*说  明：此函数会被HAL_SPI_Init()回调调用         */
/*-------------------------------------------------*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
	if(hspi->Instance==SPI2){     
		__HAL_RCC_GPIOB_CLK_ENABLE();                        //开启端口D时钟  
		__HAL_RCC_SPI2_CLK_ENABLE();                         //开启SPI2接口时钟
		
		GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;   //配置PD1 3 4
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;                   //复用模式
		GPIO_Initure.Pull=GPIO_PULLUP;                       //上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;        //VERY_HIGH模式
		GPIO_Initure.Alternate = GPIO_AF0_SPI2;	             //复用为SPI2接口
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);                  //配置	
	}
}
/*-------------------------------------------------*/
/*函数名：SPI接口读写数据                          */
/*参  数：TxData：发送的数据                       */
/*返回值：读取的数据                               */
/*-------------------------------------------------*/
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_TXE))==0);			  
	*(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR = TxData ;	  					
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_RXNE))==0); 
	return *(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR;  	
}
/*-------------------------------------------------*/
/*函数名：SPI2接口写数据                           */
/*参  数：TxData：发送的数据                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI2_WriteByte(uint8_t TxData)
{  
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_TXE))==0);			  
	*(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR = TxData ;	  					
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_RXNE))==0); 
	temprxdata = *(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR;  	
}
/*-------------------------------------------------*/
/*函数名：SPI2接口读数据                           */
/*参  数：无                                       */
/*返回值：读到的数据                               */
/*-------------------------------------------------*/
uint8_t SPI2_ReadByte(void)
{			 	
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_TXE))==0);			  
	*(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR = 0xFF ;	  					
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_RXNE))==0); 
	return *(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR;  			    
}
/*-------------------------------------------------*/
/*函数名：SPI2接口读写指定数量数据                 */
/*参  数：TxData：发送的数据                       */
/*参  数：Rxdata：发送的数据                       */
/*参  数：len：发送的数据数量                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI2_WriteRead(uint8_t *TxData, uint8_t *Rxdata, uint16_t len)
{  
	uint16_t i;
	
	for(i=0;i<len;i++){
		Rxdata[i] = SPI2_ReadWriteByte(TxData[i]);
	}
}
/*-------------------------------------------------*/
/*函数名：SPI2接口写指定数量数据                   */
/*参  数：TxData：发送的数据                       */
/*参  数：len：发送的数据数量                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI2_Write(uint8_t *TxData, uint16_t len)
{  
	uint16_t i;
	
	for(i=0;i<len;i++){
		SPI2_WriteByte(TxData[i]);
	}
}
/*-------------------------------------------------*/
/*函数名：SPI2接口读指定数量数据                   */
/*参  数：TxData：发送的数据                       */
/*参  数：len：发送的数据数量                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI2_Read(uint8_t *Rxdata, uint16_t len)
{  
	uint16_t i;
	
	for(i=0;i<len;i++){
		Rxdata[i] = SPI2_ReadWriteByte(0xFF);
	}
}
