/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*            实现各个串口功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "usart.h"            //包含需要的头文件

USART_ControlCB UART1_ControlCB;            //串口1控制结构体
uint8_t  U1_TxBuff[U1_TXBUFF_SIZE];         //定义一个数组缓冲区，串口1发送缓冲区 

USART_ControlCB UART3_ControlCB;            //串口3控制结构体
uint8_t  U3_TxBuff[U3_TXBUFF_SIZE];         //定义一个数组缓冲区，串口3发送缓冲区 
uint8_t  U3_RxBuff[U3_RXBUFF_SIZE];         //定义一个数组缓冲区，串口3接收缓冲区 

USART_ControlCB UART2_ControlCB;            //串口2控制结构体
uint8_t  U2_TxBuff[U2_TXBUFF_SIZE];         //定义一个数组缓冲区，串口2发送缓冲区 
uint8_t  U2_RxBuff[U2_RXBUFF_SIZE];         //定义一个数组缓冲区，串口2接收缓冲区 
uint8_t  U2_CopyBuff[U2_RXMAX_SIZE + 1];    //定义一个数组缓冲区，串口2拷贝接收数据 

/*-------------------------------------------------*/
/*函数名：初始化串口1                              */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U1_Init(uint32_t bound)
{	
	UART1_ControlCB.USART_Handler.Instance=USART1;					        //指定使用第几个串口
	UART1_ControlCB.USART_Handler.Init.BaudRate=bound;				        //设置波特率
	UART1_ControlCB.USART_Handler.Init.WordLength=UART_WORDLENGTH_8B;       //设置字长为8位数据格式
	UART1_ControlCB.USART_Handler.Init.StopBits=UART_STOPBITS_1;	        //设置一个停止位
	UART1_ControlCB.USART_Handler.Init.Parity=UART_PARITY_NONE;		        //设置无奇偶校验位
	UART1_ControlCB.USART_Handler.Init.Mode=UART_MODE_TX;		            //设置发模式
	UART1_ControlCB.USART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;       //设置无硬件流控
	HAL_UART_Init(&UART1_ControlCB.USART_Handler);			                //设置串口
}
/*-------------------------------------------------*/
/*函数名：初始化串口3                              */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U3_Init(uint32_t bound)
{	
	UART3_ControlCB.USART_Handler.Instance=USART3;					        //指定使用第几个串口
	UART3_ControlCB.USART_Handler.Init.BaudRate=bound;				        //设置波特率
	UART3_ControlCB.USART_Handler.Init.WordLength=UART_WORDLENGTH_8B;       //设置字长为8位数据格式
	UART3_ControlCB.USART_Handler.Init.StopBits=UART_STOPBITS_1;	        //设置一个停止位
	UART3_ControlCB.USART_Handler.Init.Parity=UART_PARITY_NONE;		        //设置无奇偶校验位
	UART3_ControlCB.USART_Handler.Init.Mode=UART_MODE_TX_RX;		        //设置收发模式
	UART3_ControlCB.USART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;       //设置无硬件流控
	HAL_UART_Init(&UART3_ControlCB.USART_Handler);			                //设置串口
	u3_BuffInit();                                                          //初始化串口3收发缓冲区以及各个指针
}
/*-------------------------------------------------*/
/*函数名：初始化串口2                              */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U2_Init(uint32_t bound)
{	
	UART2_ControlCB.USART_Handler.Instance=USART2;					        //指定使用第几个串口
	UART2_ControlCB.USART_Handler.Init.BaudRate=bound;				        //设置波特率
	UART2_ControlCB.USART_Handler.Init.WordLength=UART_WORDLENGTH_8B;       //设置字长为8位数据格式
	UART2_ControlCB.USART_Handler.Init.StopBits=UART_STOPBITS_1;	        //设置一个停止位
	UART2_ControlCB.USART_Handler.Init.Parity=UART_PARITY_NONE;		        //设置无奇偶校验位
	UART2_ControlCB.USART_Handler.Init.Mode=UART_MODE_TX_RX;		        //设置收发模式
	UART2_ControlCB.USART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;       //设置无硬件流控
	HAL_UART_Init(&UART2_ControlCB.USART_Handler);			                //设置串口
	u2_BuffInit();                                                          //初始化串口3收发缓冲区以及各个指针
}
/*-------------------------------------------------*/
/*函数名：串口1的底层初始化                        */
/*参  数：huart：串口配置句柄                      */
/*返回值：无                                       */
/*说  明：此函数会被HAL_UART_Init()回调调用        */
/*-------------------------------------------------*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
	GPIO_InitTypeDef GPIO_Initure;                 //GPIO端口设置变量
	
	if(huart->Instance==USART1){                   //如果配置的是串口1，则进入改if分支，进行串口1的底层初始化
		__HAL_RCC_GPIOA_CLK_ENABLE();			   //使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			   //使能串口1时钟
		
		GPIO_Initure.Pin = GPIO_PIN_9|GPIO_PIN_10; //准备设置PA9 10
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;       //复用功能
		GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
		GPIO_Initure.Alternate = GPIO_AF1_USART1;  //PA9 10复用为串口1的TXD RXD功能
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);       //设置PA9 10           
	}
		
	if(huart->Instance==USART3){                    //如果配置的是串口3，则进入改if分支，进行串口1的底层初始化
		__HAL_RCC_GPIOB_CLK_ENABLE();			    //使能GPIOB时钟
		__HAL_RCC_USART3_CLK_ENABLE();			    //使能串口3时钟
		__HAL_RCC_DMA1_CLK_ENABLE();                //使能DMA1时钟
		
		GPIO_Initure.Pin = GPIO_PIN_8|GPIO_PIN_9;   //准备设置PB8 9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //复用功能
		GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
		GPIO_Initure.Alternate = GPIO_AF4_USART3;   //PB8 9复用为串口3的TXD RXD功能
		HAL_GPIO_Init(GPIOB, &GPIO_Initure);        //设置PB8 9
		
		HAL_NVIC_SetPriority(USART3_4_IRQn,0,2);    //设置串口3中断的抢占优先级为0，子优先级是2，但是要注意，G0系列中断子优先级不会生效
		HAL_NVIC_EnableIRQ(USART3_4_IRQn);          //使能串口3的中断
		
		UART3_ControlCB.USART_TxDMA.Instance                 = DMA1_Channel1;               //DMA1通道1
		UART3_ControlCB.USART_TxDMA.Init.Request             = DMA_REQUEST_USART3_TX;       //串口3发送
		UART3_ControlCB.USART_TxDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;        //存储区到外设
		UART3_ControlCB.USART_TxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //外设不递增
		UART3_ControlCB.USART_TxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //存储区递增
		UART3_ControlCB.USART_TxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //按字节存取
		UART3_ControlCB.USART_TxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //按字节存取
		UART3_ControlCB.USART_TxDMA.Init.Mode                = DMA_NORMAL;                  //正常模式
		UART3_ControlCB.USART_TxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //高优先级
		HAL_DMA_Init(&UART3_ControlCB.USART_TxDMA);                                         //配置
		__HAL_LINKDMA(huart, hdmatx, UART3_ControlCB.USART_TxDMA);                          //和串口3 DMA发送连接
		
		UART3_ControlCB.USART_RxDMA.Instance                 = DMA1_Channel2;               //DMA1通道2
		UART3_ControlCB.USART_RxDMA.Init.Request             = DMA_REQUEST_USART3_RX;       //串口3接收
		UART3_ControlCB.USART_RxDMA.Init.Direction           = DMA_PERIPH_TO_MEMORY;        //外设到存储区
		UART3_ControlCB.USART_RxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //外设不递增
		UART3_ControlCB.USART_RxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //存储区递增
		UART3_ControlCB.USART_RxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //按字节存取
		UART3_ControlCB.USART_RxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //按字节存取
		UART3_ControlCB.USART_RxDMA.Init.Mode                = DMA_NORMAL;                  //正常模式
		UART3_ControlCB.USART_RxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //高优先级
		HAL_DMA_Init(&UART3_ControlCB.USART_RxDMA);                                         //配置
		__HAL_LINKDMA(huart, hdmarx, UART3_ControlCB.USART_RxDMA);                          //和串口3 DMA接收连接

		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);                                     //配置DMA1 通道1的中断，优先级
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);                                             //使能DMA1 通道1的中断 

		HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);                                   //配置DMA1 通道2-3的中断，优先级
		HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);                                           //使能DMA1 通道2-3的中断            
	}	
	
	if(huart->Instance==USART2){                    //如果配置的是串口2，则进入改if分支，进行串口2的底层初始化
		__HAL_RCC_GPIOA_CLK_ENABLE();			    //使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();			    //使能串口2时钟
		__HAL_RCC_DMA1_CLK_ENABLE();                //使能DMA1时钟
		
		GPIO_Initure.Pin = GPIO_PIN_2|GPIO_PIN_3;   //准备设置PA2 3
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //复用功能
		GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
		GPIO_Initure.Alternate = GPIO_AF1_USART2;   //复用为串口2的TXD RXD功能
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);        //设置
		
		HAL_NVIC_SetPriority(USART2_IRQn,0,2);      //设置串口2中断的抢占优先级为0，子优先级是2，但是要注意，G0系列中断子优先级不会生效
		HAL_NVIC_EnableIRQ(USART2_IRQn);            //使能串口2的中断
		
		UART2_ControlCB.USART_TxDMA.Instance                 = DMA1_Channel4;               //DMA1通道4
		UART2_ControlCB.USART_TxDMA.Init.Request             = DMA_REQUEST_USART2_TX;       //串口2发送
		UART2_ControlCB.USART_TxDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;        //存储区到外设
		UART2_ControlCB.USART_TxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //外设不递增
		UART2_ControlCB.USART_TxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //存储区递增
		UART2_ControlCB.USART_TxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //按字节存取
		UART2_ControlCB.USART_TxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //按字节存取
		UART2_ControlCB.USART_TxDMA.Init.Mode                = DMA_NORMAL;                  //正常模式
		UART2_ControlCB.USART_TxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //高优先级
		HAL_DMA_Init(&UART2_ControlCB.USART_TxDMA);                                         //配置
		__HAL_LINKDMA(huart, hdmatx, UART2_ControlCB.USART_TxDMA);                          //和串口2 DMA发送连接

		UART2_ControlCB.USART_RxDMA.Instance                 = DMA1_Channel5;               //DMA1通道5
		UART2_ControlCB.USART_RxDMA.Init.Request             = DMA_REQUEST_USART2_RX;       //串口2接收
		UART2_ControlCB.USART_RxDMA.Init.Direction           = DMA_PERIPH_TO_MEMORY;        //外设到存储区
		UART2_ControlCB.USART_RxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //外设不递增
		UART2_ControlCB.USART_RxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //存储区递增
		UART2_ControlCB.USART_RxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //按字节存取
		UART2_ControlCB.USART_RxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //按字节存取
		UART2_ControlCB.USART_RxDMA.Init.Mode                = DMA_NORMAL;                  //正常模式
		UART2_ControlCB.USART_RxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //高优先级
		HAL_DMA_Init(&UART2_ControlCB.USART_RxDMA);                                         //配置
		__HAL_LINKDMA(huart, hdmarx, UART2_ControlCB.USART_RxDMA);                          //和串口2 DMA接收连接
		
		HAL_NVIC_SetPriority(DMA1_Ch4_7_DMAMUX1_OVR_IRQn, 0, 0);                            //配置DMA1 通道4-7的中断，优先级
		HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMAMUX1_OVR_IRQn);                                    //使能DMA1 通道4-7的中断 
	}
}
/*-------------------------------------------------*/
/*函数名：串口1 printf函数                         */
/*参  数：fmt,...  格式化输出字符串和参数          */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u1_printf(char* fmt,...) 
{  
	uint16_t i;    	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)U1_TxBuff,fmt,ap);
	va_end(ap);
	
	while(__HAL_UART_GET_FLAG(&UART1_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);	   //等待串口1发送空闲，然后才能发送数据	
	for(i = 0;i < strlen((const char*)U1_TxBuff);i ++){				                   //利用for循环，一个字节，一个字节的发送
		UART1_ControlCB.USART_Handler.Instance->TDR = U1_TxBuff[i];                    //把需要发送的字节，填充到串口1，启动发送
		while(__HAL_UART_GET_FLAG(&UART1_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //等待本次字节发送结束，才可以发送下一个字节	
	}	
}
/*-------------------------------------------------*/
/*函数名：串口2 printf函数                         */
/*参  数：fmt,...  格式化输出字符串和参数          */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u2_printf(char* fmt,...) 
{  
	uint16_t i;    	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)U2_TxBuff,fmt,ap);
	va_end(ap);
	
	while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);	   //等待串口2发送空闲，然后才能发送数据	
	for(i = 0;i < strlen((const char*)U2_TxBuff);i ++){				                   //利用for循环，一个字节，一个字节的发送
		UART2_ControlCB.USART_Handler.Instance->TDR = U2_TxBuff[i];                    //把需要发送的字节，填充到串口2，启动发送
		while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //等待本次字节发送结束，才可以发送下一个字节	
	}	
}
/*----------------------------------------------------------*/
/*函数名：初始化串口3收发缓冲区以及各个指针                 */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void u3_BuffInit(void)
{	
	UART3_ControlCB.UsartTxDataInPtr  = &UART3_ControlCB.Usart_TxLocation[0];           //发送缓冲区 In插入指针归位到 位置结构体数组0号成员    
	UART3_ControlCB.UsartTxDataOutPtr =  UART3_ControlCB.UsartTxDataInPtr;              //发送缓冲区 Out提取指针归位到 In插入指针位置
    UART3_ControlCB.UsartTxDataEndPtr = &UART3_ControlCB.Usart_TxLocation[TX_NUM-1];    //发送缓冲区 End结尾标记指针归位到 位置结构体数组最后1个成员 
	UART3_ControlCB.UsartTxDataInPtr->StartPtr = U3_TxBuff;	                            //In插入指向的数据成员 start指针指向发送缓冲区起始位置
	UART3_ControlCB.Usart_TxCounter = 0;                                                //累计发送缓冲区存放的数据量=0
	UART3_ControlCB.Usart_TxCpltflag = 0;                                               //发送空闲
	UART3_ControlCB.Usart_TxTimer = HAL_GetTick();                                      //记录当前时间
	
	UART3_ControlCB.UsartRxDataInPtr  = &UART3_ControlCB.Usart_RxLocation[0];           //接收缓冲区 In插入指针归位到 位置结构体数组0号成员    
	UART3_ControlCB.UsartRxDataOutPtr =  UART3_ControlCB.UsartRxDataInPtr;              //接收缓冲区 Out提取指针归位到 In插入指针位置    
    UART3_ControlCB.UsartRxDataEndPtr = &UART3_ControlCB.Usart_RxLocation[RX_NUM-1];    //接收缓冲区 End结尾标记指针归位到 位置结构体数组最后1个成员 
	UART3_ControlCB.UsartRxDataInPtr->StartPtr = U3_RxBuff;                             //In插入指向的数据成员 start指针指向接收缓冲区起始位置	
	UART3_ControlCB.Usart_RxCounter = 0;                                                //累计接收缓冲区存放的数据量=0
	
	HAL_UART_Receive_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartRxDataInPtr->StartPtr,U3_RXMAX_SIZE);    //设置DMA接收 指定接收位置  
	__HAL_UART_CLEAR_IDLEFLAG(&UART3_ControlCB.USART_Handler);                                                        //清除空闲中断标志 														
	__HAL_UART_ENABLE_IT(&UART3_ControlCB.USART_Handler, UART_IT_IDLE);                                               //打开空闲中断
}
/*----------------------------------------------------------*/
/*函数名：初始化串口2收发缓冲区以及各个指针                 */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void u2_BuffInit(void)
{	
	UART2_ControlCB.UsartTxDataInPtr  = &UART2_ControlCB.Usart_TxLocation[0];           //发送缓冲区 In插入指针归位到 位置结构体数组0号成员    
	UART2_ControlCB.UsartTxDataOutPtr =  UART2_ControlCB.UsartTxDataInPtr;              //发送缓冲区 Out提取指针归位到 In插入指针位置
    UART2_ControlCB.UsartTxDataEndPtr = &UART2_ControlCB.Usart_TxLocation[TX_NUM-1];    //发送缓冲区 End结尾标记指针归位到 位置结构体数组最后1个成员 
	UART2_ControlCB.UsartTxDataInPtr->StartPtr = U2_TxBuff;	                            //In插入指向的数据成员 start指针指向发送缓冲区起始位置
	UART2_ControlCB.Usart_TxCounter = 0;                                                //累计发送缓冲区存放的数据量=0
	UART2_ControlCB.Usart_TxCpltflag = 0;                                               //发送空闲
	UART2_ControlCB.Usart_TxTimer = HAL_GetTick();                                      //记录当前时间
	
	UART2_ControlCB.UsartRxDataInPtr  = &UART2_ControlCB.Usart_RxLocation[0];           //接收缓冲区 In插入指针归位到 位置结构体数组0号成员    
	UART2_ControlCB.UsartRxDataOutPtr =  UART2_ControlCB.UsartRxDataInPtr;              //接收缓冲区 Out提取指针归位到 In插入指针位置    
    UART2_ControlCB.UsartRxDataEndPtr = &UART2_ControlCB.Usart_RxLocation[RX_NUM-1];    //接收缓冲区 End结尾标记指针归位到 位置结构体数组最后1个成员 
	UART2_ControlCB.UsartRxDataInPtr->StartPtr = U2_RxBuff;                             //In插入指向的数据成员 start指针指向接收缓冲区起始位置
	UART2_ControlCB.Usart_RxCounter = 0;                                                //累计接收缓冲区存放的数据量=0
	
	memset(U2_RxBuff,0,U2_RXBUFF_SIZE);                                                                               //清空串口2 接收缓冲区	
	HAL_UART_Receive_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartRxDataInPtr->StartPtr,U2_RXMAX_SIZE);    //设置DMA接收 指定接收位置  
	__HAL_UART_CLEAR_IDLEFLAG(&UART2_ControlCB.USART_Handler);                                                        //清除空闲中断标志 														
	__HAL_UART_ENABLE_IT(&UART2_ControlCB.USART_Handler, UART_IT_IDLE);                                               //打开空闲中断
}
/*-------------------------------------------------*/
/*函数名：串口3发送数据                            */
/*参  数：data：数据                               */
/*参  数：len：数据量                              */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u3_TxData(uint8_t *data, uint16_t len)
{  
	uint16_t i;                                                               
	
	while(__HAL_UART_GET_FLAG(&UART3_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);      //等待串口3发送空闲，然后才能发送数据	
	for(i=0;i<len;i++){	                                                               //循环一个一个字节的发送	
		UART3_ControlCB.USART_Handler.Instance->TDR = data[i];                         //把需要发送的字节，填充到串口3，启动发送
		while(__HAL_UART_GET_FLAG(&UART3_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //等待串口3发送空闲，然后才能发送数据	
	}
	
}
/*-------------------------------------------------*/
/*函数名：串口2发送数据                            */
/*参  数：data：数据                               */
/*参  数：len：数据量                              */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u2_TxData(uint8_t *data, uint16_t len)
{  
	uint16_t i;                                                               
	
	while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);      //等待串口2发送空闲，然后才能发送数据	
	for(i=0;i<len;i++){	                                                               //循环一个一个字节的发送	
		UART2_ControlCB.USART_Handler.Instance->TDR = data[i];                         //把需要发送的字节，填充到串口2，启动发送
		while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //等待串口2发送空闲，然后才能发送数据	
	}
	
}
/*----------------------------------------------------------*/
/*函数名：向发送缓冲区添加数据                              */
/*参  数：databuff：数据                                    */
/*参  数：datalen：数据长度                                 */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void u3_TxDataBuf(uint8_t *databuff, uint32_t datalen)
{
	if(U3_TXBUFF_SIZE - UART3_ControlCB.Usart_TxCounter >= datalen){                                   //计算发送缓冲区内剩余的空间，还够不够存放本次的数据，够的话进入if
		UART3_ControlCB.UsartTxDataInPtr->StartPtr = &U3_TxBuff[UART3_ControlCB.Usart_TxCounter];	   //标记本次存放数据的起始位置
	}else{                                                                                             //反之，不够存放本次的数据，进入else
		UART3_ControlCB.Usart_TxCounter = 0;                                                           //累计发送缓冲区存放的数据量清零
		UART3_ControlCB.UsartTxDataInPtr->StartPtr = U3_TxBuff;                                        //存放数据的起始位置重新设置到接收缓冲区的起始位置		
	}
	memcpy(UART3_ControlCB.UsartTxDataInPtr->StartPtr,databuff,datalen);	                           //拷贝数据            
    UART3_ControlCB.Usart_TxCounter += datalen;	                                                       //计算累计发送缓冲区存放的数据量
	UART3_ControlCB.UsartTxDataInPtr->EndPtr = &U3_TxBuff[UART3_ControlCB.Usart_TxCounter];            //标记发送缓冲区本次存放的结束位置		
				
	UART3_ControlCB.UsartTxDataInPtr++;                                                                //数据IN指针下移
	if(UART3_ControlCB.UsartTxDataInPtr==UART3_ControlCB.UsartTxDataEndPtr)                            //如果下移到End指针标记的结束位置，进入if
		UART3_ControlCB.UsartTxDataInPtr = &UART3_ControlCB.Usart_TxLocation[0];                       //发送缓冲区 In插入指针归位到 位置结构体数组0号成员    
}
/*----------------------------------------------------------*/
/*函数名：向发送缓冲区添加数据                              */
/*参  数：databuff：数据                                    */
/*参  数：datalen：数据长度                                 */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void u2_TxDataBuf(uint8_t *databuff, uint32_t datalen)
{
	if(U2_TXBUFF_SIZE - UART2_ControlCB.Usart_TxCounter >= datalen){                                   //计算发送缓冲区内剩余的空间，还够不够存放本次的数据，够的话进入if
		UART2_ControlCB.UsartTxDataInPtr->StartPtr = &U2_TxBuff[UART2_ControlCB.Usart_TxCounter];	   //标记本次存放数据的起始位置
	}else{                                                                                             //反之，不够存放本次的数据，进入else
		UART2_ControlCB.Usart_TxCounter = 0;                                                           //累计发送缓冲区存放的数据量清零
		UART2_ControlCB.UsartTxDataInPtr->StartPtr = U2_TxBuff;                                        //存放数据的起始位置重新设置到接收缓冲区的起始位置		
	}
	memcpy(UART2_ControlCB.UsartTxDataInPtr->StartPtr,databuff,datalen);	                           //拷贝数据            
    UART2_ControlCB.Usart_TxCounter += datalen;	                                                       //计算累计发送缓冲区存放的数据量
	UART2_ControlCB.UsartTxDataInPtr->EndPtr = &U2_TxBuff[UART2_ControlCB.Usart_TxCounter];            //标记发送缓冲区本次存放的结束位置		
				
	UART2_ControlCB.UsartTxDataInPtr++;                                                                //数据IN指针下移
	if(UART2_ControlCB.UsartTxDataInPtr==UART2_ControlCB.UsartTxDataEndPtr)                            //如果下移到End指针标记的结束位置，进入if
		UART2_ControlCB.UsartTxDataInPtr = &UART2_ControlCB.Usart_TxLocation[0];                       //发送缓冲区 In插入指针归位到 位置结构体数组0号成员    
}
/*----------------------------------------------------------*/
/*函数名：中止接收完成回调函数                              */
/*参  数：huart：串口配置句柄                               */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{  
	if(huart->Instance==USART3){                                                                                          //判断是串口几的回调
		UART3_ControlCB.UsartRxDataInPtr->EndPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];   		                  //标记本次接收的结束位置		
		UART3_ControlCB.UsartRxDataInPtr->Timecounter = HAL_GetTick();                                                    //记录接收的时基
		UART3_ControlCB.UsartRxDataInPtr++;                                                                               //接收缓冲区数据IN指针下移
		if(UART3_ControlCB.UsartRxDataInPtr==UART3_ControlCB.UsartRxDataEndPtr)                                           //如果指针下移到结尾标志指针的时候，进入if
			UART3_ControlCB.UsartRxDataInPtr = &UART3_ControlCB.Usart_RxLocation[0];                                      //接收缓冲区 In插入指针归位到 位置结构体数组0号成员   		
		if(U3_RXBUFF_SIZE - UART3_ControlCB.Usart_RxCounter >= U3_RXMAX_SIZE){                                            //计算，如果剩余空间大于 单次接收的数据量 进入if
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];                     //记录下次接收的起始位置	
		}else{                                                                                                            //如果剩余空间不够单次接收量 进入else
			UART3_ControlCB.Usart_RxCounter = 0;                                                                          //累计接收缓冲区存放的数据量清空
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = U3_RxBuff;                                                       //接收位置，重新回到缓冲区的起始位置		
		}
		HAL_UART_Receive_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartRxDataInPtr->StartPtr,U3_RXMAX_SIZE);    //设置新一次的 DMA接收 指定接收位置  		
	}	
	
	if(huart->Instance==USART2){                                                                                          //判断是串口几的回调
		UART2_ControlCB.UsartRxDataInPtr->EndPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];   		                  //标记本次接收的结束位置		
		UART2_ControlCB.UsartRxDataInPtr++;                                                                               //接收缓冲区数据IN指针下移
		if(UART2_ControlCB.UsartRxDataInPtr==UART2_ControlCB.UsartRxDataEndPtr)                                           //如果指针下移到结尾标志指针的时候，进入if
			UART2_ControlCB.UsartRxDataInPtr = &UART2_ControlCB.Usart_RxLocation[0];                                      //接收缓冲区 In插入指针归位到 位置结构体数组0号成员   		
		if(U2_RXBUFF_SIZE - UART2_ControlCB.Usart_RxCounter >= U2_RXMAX_SIZE){                                            //计算，如果剩余空间大于 单次接收的数据量 进入if
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];                     //记录下次接收的起始位置	
		}else{                                                                                                            //如果剩余空间不够单次接收量 进入else
			UART2_ControlCB.Usart_RxCounter = 0;                                                                          //累计接收缓冲区存放的数据量清空
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = U2_RxBuff;                                                       //接收位置，重新回到缓冲区的起始位置		
		}
		HAL_UART_Receive_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartRxDataInPtr->StartPtr,U2_RXMAX_SIZE);    //设置新一次的 DMA接收 指定接收位置  		
	}	
}
/*----------------------------------------------------------*/
/*函数名：数据接收完成回调函数                              */
/*参  数：huart：串口配置句柄                               */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART3){                                                                                          //判断是串口几的回调
		UART3_ControlCB.Usart_RxCounter += U3_RXMAX_SIZE;                                                                 //累计当前缓冲区已经存放的数据量				
		UART3_ControlCB.UsartRxDataInPtr->EndPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];   		                  //标记本次接收的结束位置		
		UART3_ControlCB.UsartRxDataInPtr->Timecounter = HAL_GetTick();                                                    //记录接收的时基
		UART3_ControlCB.UsartRxDataInPtr++;                                                                               //接收缓冲区数据IN指针下移
		if(UART3_ControlCB.UsartRxDataInPtr==UART3_ControlCB.UsartRxDataEndPtr)                                           //如果指针下移到结尾标志指针的时候，进入if
			UART3_ControlCB.UsartRxDataInPtr = &UART3_ControlCB.Usart_RxLocation[0];                                      //接收缓冲区 In插入指针归位到 位置结构体数组0号成员   		
		if(U3_RXBUFF_SIZE - UART3_ControlCB.Usart_RxCounter >= U3_RXMAX_SIZE){                                            //计算，如果剩余空间大于 单次接收的数据量 进入if
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];                     //记录下次接收的起始位置	
		}else{                                                                                                            //如果剩余空间不够单次接收量 进入else
			UART3_ControlCB.Usart_RxCounter = 0;                                                                          //累计接收缓冲区存放的数据量清空
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = U3_RxBuff;                                                       //接收位置，重新回到缓冲区的起始位置		
		}
		HAL_UART_Receive_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartRxDataInPtr->StartPtr,U3_RXMAX_SIZE);     //设置新一次的 DMA接收 指定接收位置  		
	}

	if(huart->Instance==USART2){                                                                                          //判断是串口几的回调
		UART2_ControlCB.Usart_RxCounter += U2_RXMAX_SIZE;                                                                 //累计当前缓冲区已经存放的数据量				
		UART2_ControlCB.UsartRxDataInPtr->EndPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];   		                  //标记本次接收的结束位置		
		UART2_ControlCB.UsartRxDataInPtr++;                                                                               //接收缓冲区数据IN指针下移
		if(UART2_ControlCB.UsartRxDataInPtr==UART2_ControlCB.UsartRxDataEndPtr)                                           //如果指针下移到结尾标志指针的时候，进入if
			UART2_ControlCB.UsartRxDataInPtr = &UART2_ControlCB.Usart_RxLocation[0];                                      //接收缓冲区 In插入指针归位到 位置结构体数组0号成员   		
		if(U2_RXBUFF_SIZE - UART2_ControlCB.Usart_RxCounter >= U2_RXMAX_SIZE){                                            //计算，如果剩余空间大于 单次接收的数据量 进入if
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];                     //记录下次接收的起始位置	
		}else{                                                                                                            //如果剩余空间不够单次接收量 进入else
			UART2_ControlCB.Usart_RxCounter = 0;                                                                          //累计接收缓冲区存放的数据量清空
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = U2_RxBuff;                                                       //接收位置，重新回到缓冲区的起始位置		
		}
		HAL_UART_Receive_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartRxDataInPtr->StartPtr,U2_RXMAX_SIZE);     //设置新一次的 DMA接收 指定接收位置  		
	}		
}
/*----------------------------------------------------------*/
/*函数名：数据发送完成回调函数                              */
/*参  数：huart：串口配置句柄                               */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART3){                         //判断是串口几的回调
		UART3_ControlCB.Usart_TxCpltflag = 0;            //发送空闲
		UART3_ControlCB.Usart_TxTimer = HAL_GetTick();   //记录当前时间		
	}
	if(huart->Instance==USART2){                         //判断是串口几的回调
		UART2_ControlCB.Usart_TxCpltflag = 0;            //发送空闲
		UART2_ControlCB.Usart_TxTimer = HAL_GetTick();   //记录当前时间		
	}
}

