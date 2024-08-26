/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ�ָ������ڹ��ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "usart.h"            //������Ҫ��ͷ�ļ�

USART_ControlCB UART1_ControlCB;            //����1���ƽṹ��
uint8_t  U1_TxBuff[U1_TXBUFF_SIZE];         //����һ�����黺����������1���ͻ����� 

USART_ControlCB UART3_ControlCB;            //����3���ƽṹ��
uint8_t  U3_TxBuff[U3_TXBUFF_SIZE];         //����һ�����黺����������3���ͻ����� 
uint8_t  U3_RxBuff[U3_RXBUFF_SIZE];         //����һ�����黺����������3���ջ����� 

USART_ControlCB UART2_ControlCB;            //����2���ƽṹ��
uint8_t  U2_TxBuff[U2_TXBUFF_SIZE];         //����һ�����黺����������2���ͻ����� 
uint8_t  U2_RxBuff[U2_RXBUFF_SIZE];         //����һ�����黺����������2���ջ����� 
uint8_t  U2_CopyBuff[U2_RXMAX_SIZE + 1];    //����һ�����黺����������2������������ 

/*-------------------------------------------------*/
/*����������ʼ������1                              */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U1_Init(uint32_t bound)
{	
	UART1_ControlCB.USART_Handler.Instance=USART1;					        //ָ��ʹ�õڼ�������
	UART1_ControlCB.USART_Handler.Init.BaudRate=bound;				        //���ò�����
	UART1_ControlCB.USART_Handler.Init.WordLength=UART_WORDLENGTH_8B;       //�����ֳ�Ϊ8λ���ݸ�ʽ
	UART1_ControlCB.USART_Handler.Init.StopBits=UART_STOPBITS_1;	        //����һ��ֹͣλ
	UART1_ControlCB.USART_Handler.Init.Parity=UART_PARITY_NONE;		        //��������żУ��λ
	UART1_ControlCB.USART_Handler.Init.Mode=UART_MODE_TX;		            //���÷�ģʽ
	UART1_ControlCB.USART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;       //������Ӳ������
	HAL_UART_Init(&UART1_ControlCB.USART_Handler);			                //���ô���
}
/*-------------------------------------------------*/
/*����������ʼ������3                              */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U3_Init(uint32_t bound)
{	
	UART3_ControlCB.USART_Handler.Instance=USART3;					        //ָ��ʹ�õڼ�������
	UART3_ControlCB.USART_Handler.Init.BaudRate=bound;				        //���ò�����
	UART3_ControlCB.USART_Handler.Init.WordLength=UART_WORDLENGTH_8B;       //�����ֳ�Ϊ8λ���ݸ�ʽ
	UART3_ControlCB.USART_Handler.Init.StopBits=UART_STOPBITS_1;	        //����һ��ֹͣλ
	UART3_ControlCB.USART_Handler.Init.Parity=UART_PARITY_NONE;		        //��������żУ��λ
	UART3_ControlCB.USART_Handler.Init.Mode=UART_MODE_TX_RX;		        //�����շ�ģʽ
	UART3_ControlCB.USART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;       //������Ӳ������
	HAL_UART_Init(&UART3_ControlCB.USART_Handler);			                //���ô���
	u3_BuffInit();                                                          //��ʼ������3�շ��������Լ�����ָ��
}
/*-------------------------------------------------*/
/*����������ʼ������2                              */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void U2_Init(uint32_t bound)
{	
	UART2_ControlCB.USART_Handler.Instance=USART2;					        //ָ��ʹ�õڼ�������
	UART2_ControlCB.USART_Handler.Init.BaudRate=bound;				        //���ò�����
	UART2_ControlCB.USART_Handler.Init.WordLength=UART_WORDLENGTH_8B;       //�����ֳ�Ϊ8λ���ݸ�ʽ
	UART2_ControlCB.USART_Handler.Init.StopBits=UART_STOPBITS_1;	        //����һ��ֹͣλ
	UART2_ControlCB.USART_Handler.Init.Parity=UART_PARITY_NONE;		        //��������żУ��λ
	UART2_ControlCB.USART_Handler.Init.Mode=UART_MODE_TX_RX;		        //�����շ�ģʽ
	UART2_ControlCB.USART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;       //������Ӳ������
	HAL_UART_Init(&UART2_ControlCB.USART_Handler);			                //���ô���
	u2_BuffInit();                                                          //��ʼ������3�շ��������Լ�����ָ��
}
/*-------------------------------------------------*/
/*������������1�ĵײ��ʼ��                        */
/*��  ����huart���������þ��                      */
/*����ֵ����                                       */
/*˵  �����˺����ᱻHAL_UART_Init()�ص�����        */
/*-------------------------------------------------*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
	GPIO_InitTypeDef GPIO_Initure;                 //GPIO�˿����ñ���
	
	if(huart->Instance==USART1){                   //������õ��Ǵ���1��������if��֧�����д���1�ĵײ��ʼ��
		__HAL_RCC_GPIOA_CLK_ENABLE();			   //ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			   //ʹ�ܴ���1ʱ��
		
		GPIO_Initure.Pin = GPIO_PIN_9|GPIO_PIN_10; //׼������PA9 10
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;       //���ù���
		GPIO_Initure.Pull = GPIO_PULLUP;           //����
		GPIO_Initure.Alternate = GPIO_AF1_USART1;  //PA9 10����Ϊ����1��TXD RXD����
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);       //����PA9 10           
	}
		
	if(huart->Instance==USART3){                    //������õ��Ǵ���3��������if��֧�����д���1�ĵײ��ʼ��
		__HAL_RCC_GPIOB_CLK_ENABLE();			    //ʹ��GPIOBʱ��
		__HAL_RCC_USART3_CLK_ENABLE();			    //ʹ�ܴ���3ʱ��
		__HAL_RCC_DMA1_CLK_ENABLE();                //ʹ��DMA1ʱ��
		
		GPIO_Initure.Pin = GPIO_PIN_8|GPIO_PIN_9;   //׼������PB8 9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //���ù���
		GPIO_Initure.Pull = GPIO_PULLUP;            //����
		GPIO_Initure.Alternate = GPIO_AF4_USART3;   //PB8 9����Ϊ����3��TXD RXD����
		HAL_GPIO_Init(GPIOB, &GPIO_Initure);        //����PB8 9
		
		HAL_NVIC_SetPriority(USART3_4_IRQn,0,2);    //���ô���3�жϵ���ռ���ȼ�Ϊ0�������ȼ���2������Ҫע�⣬G0ϵ���ж������ȼ�������Ч
		HAL_NVIC_EnableIRQ(USART3_4_IRQn);          //ʹ�ܴ���3���ж�
		
		UART3_ControlCB.USART_TxDMA.Instance                 = DMA1_Channel1;               //DMA1ͨ��1
		UART3_ControlCB.USART_TxDMA.Init.Request             = DMA_REQUEST_USART3_TX;       //����3����
		UART3_ControlCB.USART_TxDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;        //�洢��������
		UART3_ControlCB.USART_TxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //���費����
		UART3_ControlCB.USART_TxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //�洢������
		UART3_ControlCB.USART_TxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART3_ControlCB.USART_TxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART3_ControlCB.USART_TxDMA.Init.Mode                = DMA_NORMAL;                  //����ģʽ
		UART3_ControlCB.USART_TxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //�����ȼ�
		HAL_DMA_Init(&UART3_ControlCB.USART_TxDMA);                                         //����
		__HAL_LINKDMA(huart, hdmatx, UART3_ControlCB.USART_TxDMA);                          //�ʹ���3 DMA��������
		
		UART3_ControlCB.USART_RxDMA.Instance                 = DMA1_Channel2;               //DMA1ͨ��2
		UART3_ControlCB.USART_RxDMA.Init.Request             = DMA_REQUEST_USART3_RX;       //����3����
		UART3_ControlCB.USART_RxDMA.Init.Direction           = DMA_PERIPH_TO_MEMORY;        //���赽�洢��
		UART3_ControlCB.USART_RxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //���費����
		UART3_ControlCB.USART_RxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //�洢������
		UART3_ControlCB.USART_RxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART3_ControlCB.USART_RxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART3_ControlCB.USART_RxDMA.Init.Mode                = DMA_NORMAL;                  //����ģʽ
		UART3_ControlCB.USART_RxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //�����ȼ�
		HAL_DMA_Init(&UART3_ControlCB.USART_RxDMA);                                         //����
		__HAL_LINKDMA(huart, hdmarx, UART3_ControlCB.USART_RxDMA);                          //�ʹ���3 DMA��������

		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);                                     //����DMA1 ͨ��1���жϣ����ȼ�
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);                                             //ʹ��DMA1 ͨ��1���ж� 

		HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);                                   //����DMA1 ͨ��2-3���жϣ����ȼ�
		HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);                                           //ʹ��DMA1 ͨ��2-3���ж�            
	}	
	
	if(huart->Instance==USART2){                    //������õ��Ǵ���2��������if��֧�����д���2�ĵײ��ʼ��
		__HAL_RCC_GPIOA_CLK_ENABLE();			    //ʹ��GPIOAʱ��
		__HAL_RCC_USART2_CLK_ENABLE();			    //ʹ�ܴ���2ʱ��
		__HAL_RCC_DMA1_CLK_ENABLE();                //ʹ��DMA1ʱ��
		
		GPIO_Initure.Pin = GPIO_PIN_2|GPIO_PIN_3;   //׼������PA2 3
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //���ù���
		GPIO_Initure.Pull = GPIO_PULLUP;            //����
		GPIO_Initure.Alternate = GPIO_AF1_USART2;   //����Ϊ����2��TXD RXD����
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);        //����
		
		HAL_NVIC_SetPriority(USART2_IRQn,0,2);      //���ô���2�жϵ���ռ���ȼ�Ϊ0�������ȼ���2������Ҫע�⣬G0ϵ���ж������ȼ�������Ч
		HAL_NVIC_EnableIRQ(USART2_IRQn);            //ʹ�ܴ���2���ж�
		
		UART2_ControlCB.USART_TxDMA.Instance                 = DMA1_Channel4;               //DMA1ͨ��4
		UART2_ControlCB.USART_TxDMA.Init.Request             = DMA_REQUEST_USART2_TX;       //����2����
		UART2_ControlCB.USART_TxDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;        //�洢��������
		UART2_ControlCB.USART_TxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //���費����
		UART2_ControlCB.USART_TxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //�洢������
		UART2_ControlCB.USART_TxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART2_ControlCB.USART_TxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART2_ControlCB.USART_TxDMA.Init.Mode                = DMA_NORMAL;                  //����ģʽ
		UART2_ControlCB.USART_TxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //�����ȼ�
		HAL_DMA_Init(&UART2_ControlCB.USART_TxDMA);                                         //����
		__HAL_LINKDMA(huart, hdmatx, UART2_ControlCB.USART_TxDMA);                          //�ʹ���2 DMA��������

		UART2_ControlCB.USART_RxDMA.Instance                 = DMA1_Channel5;               //DMA1ͨ��5
		UART2_ControlCB.USART_RxDMA.Init.Request             = DMA_REQUEST_USART2_RX;       //����2����
		UART2_ControlCB.USART_RxDMA.Init.Direction           = DMA_PERIPH_TO_MEMORY;        //���赽�洢��
		UART2_ControlCB.USART_RxDMA.Init.PeriphInc           = DMA_PINC_DISABLE;            //���費����
		UART2_ControlCB.USART_RxDMA.Init.MemInc              = DMA_MINC_ENABLE;             //�洢������
		UART2_ControlCB.USART_RxDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART2_ControlCB.USART_RxDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //���ֽڴ�ȡ
		UART2_ControlCB.USART_RxDMA.Init.Mode                = DMA_NORMAL;                  //����ģʽ
		UART2_ControlCB.USART_RxDMA.Init.Priority            = DMA_PRIORITY_HIGH;           //�����ȼ�
		HAL_DMA_Init(&UART2_ControlCB.USART_RxDMA);                                         //����
		__HAL_LINKDMA(huart, hdmarx, UART2_ControlCB.USART_RxDMA);                          //�ʹ���2 DMA��������
		
		HAL_NVIC_SetPriority(DMA1_Ch4_7_DMAMUX1_OVR_IRQn, 0, 0);                            //����DMA1 ͨ��4-7���жϣ����ȼ�
		HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMAMUX1_OVR_IRQn);                                    //ʹ��DMA1 ͨ��4-7���ж� 
	}
}
/*-------------------------------------------------*/
/*������������1 printf����                         */
/*��  ����fmt,...  ��ʽ������ַ����Ͳ���          */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u1_printf(char* fmt,...) 
{  
	uint16_t i;    	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)U1_TxBuff,fmt,ap);
	va_end(ap);
	
	while(__HAL_UART_GET_FLAG(&UART1_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);	   //�ȴ�����1���Ϳ��У�Ȼ����ܷ�������	
	for(i = 0;i < strlen((const char*)U1_TxBuff);i ++){				                   //����forѭ����һ���ֽڣ�һ���ֽڵķ���
		UART1_ControlCB.USART_Handler.Instance->TDR = U1_TxBuff[i];                    //����Ҫ���͵��ֽڣ���䵽����1����������
		while(__HAL_UART_GET_FLAG(&UART1_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //�ȴ������ֽڷ��ͽ������ſ��Է�����һ���ֽ�	
	}	
}
/*-------------------------------------------------*/
/*������������2 printf����                         */
/*��  ����fmt,...  ��ʽ������ַ����Ͳ���          */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u2_printf(char* fmt,...) 
{  
	uint16_t i;    	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)U2_TxBuff,fmt,ap);
	va_end(ap);
	
	while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);	   //�ȴ�����2���Ϳ��У�Ȼ����ܷ�������	
	for(i = 0;i < strlen((const char*)U2_TxBuff);i ++){				                   //����forѭ����һ���ֽڣ�һ���ֽڵķ���
		UART2_ControlCB.USART_Handler.Instance->TDR = U2_TxBuff[i];                    //����Ҫ���͵��ֽڣ���䵽����2����������
		while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //�ȴ������ֽڷ��ͽ������ſ��Է�����һ���ֽ�	
	}	
}
/*----------------------------------------------------------*/
/*����������ʼ������3�շ��������Լ�����ָ��                 */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void u3_BuffInit(void)
{	
	UART3_ControlCB.UsartTxDataInPtr  = &UART3_ControlCB.Usart_TxLocation[0];           //���ͻ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա    
	UART3_ControlCB.UsartTxDataOutPtr =  UART3_ControlCB.UsartTxDataInPtr;              //���ͻ����� Out��ȡָ���λ�� In����ָ��λ��
    UART3_ControlCB.UsartTxDataEndPtr = &UART3_ControlCB.Usart_TxLocation[TX_NUM-1];    //���ͻ����� End��β���ָ���λ�� λ�ýṹ���������1����Ա 
	UART3_ControlCB.UsartTxDataInPtr->StartPtr = U3_TxBuff;	                            //In����ָ������ݳ�Ա startָ��ָ���ͻ�������ʼλ��
	UART3_ControlCB.Usart_TxCounter = 0;                                                //�ۼƷ��ͻ�������ŵ�������=0
	UART3_ControlCB.Usart_TxCpltflag = 0;                                               //���Ϳ���
	UART3_ControlCB.Usart_TxTimer = HAL_GetTick();                                      //��¼��ǰʱ��
	
	UART3_ControlCB.UsartRxDataInPtr  = &UART3_ControlCB.Usart_RxLocation[0];           //���ջ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա    
	UART3_ControlCB.UsartRxDataOutPtr =  UART3_ControlCB.UsartRxDataInPtr;              //���ջ����� Out��ȡָ���λ�� In����ָ��λ��    
    UART3_ControlCB.UsartRxDataEndPtr = &UART3_ControlCB.Usart_RxLocation[RX_NUM-1];    //���ջ����� End��β���ָ���λ�� λ�ýṹ���������1����Ա 
	UART3_ControlCB.UsartRxDataInPtr->StartPtr = U3_RxBuff;                             //In����ָ������ݳ�Ա startָ��ָ����ջ�������ʼλ��	
	UART3_ControlCB.Usart_RxCounter = 0;                                                //�ۼƽ��ջ�������ŵ�������=0
	
	HAL_UART_Receive_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartRxDataInPtr->StartPtr,U3_RXMAX_SIZE);    //����DMA���� ָ������λ��  
	__HAL_UART_CLEAR_IDLEFLAG(&UART3_ControlCB.USART_Handler);                                                        //��������жϱ�־ 														
	__HAL_UART_ENABLE_IT(&UART3_ControlCB.USART_Handler, UART_IT_IDLE);                                               //�򿪿����ж�
}
/*----------------------------------------------------------*/
/*����������ʼ������2�շ��������Լ�����ָ��                 */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void u2_BuffInit(void)
{	
	UART2_ControlCB.UsartTxDataInPtr  = &UART2_ControlCB.Usart_TxLocation[0];           //���ͻ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա    
	UART2_ControlCB.UsartTxDataOutPtr =  UART2_ControlCB.UsartTxDataInPtr;              //���ͻ����� Out��ȡָ���λ�� In����ָ��λ��
    UART2_ControlCB.UsartTxDataEndPtr = &UART2_ControlCB.Usart_TxLocation[TX_NUM-1];    //���ͻ����� End��β���ָ���λ�� λ�ýṹ���������1����Ա 
	UART2_ControlCB.UsartTxDataInPtr->StartPtr = U2_TxBuff;	                            //In����ָ������ݳ�Ա startָ��ָ���ͻ�������ʼλ��
	UART2_ControlCB.Usart_TxCounter = 0;                                                //�ۼƷ��ͻ�������ŵ�������=0
	UART2_ControlCB.Usart_TxCpltflag = 0;                                               //���Ϳ���
	UART2_ControlCB.Usart_TxTimer = HAL_GetTick();                                      //��¼��ǰʱ��
	
	UART2_ControlCB.UsartRxDataInPtr  = &UART2_ControlCB.Usart_RxLocation[0];           //���ջ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա    
	UART2_ControlCB.UsartRxDataOutPtr =  UART2_ControlCB.UsartRxDataInPtr;              //���ջ����� Out��ȡָ���λ�� In����ָ��λ��    
    UART2_ControlCB.UsartRxDataEndPtr = &UART2_ControlCB.Usart_RxLocation[RX_NUM-1];    //���ջ����� End��β���ָ���λ�� λ�ýṹ���������1����Ա 
	UART2_ControlCB.UsartRxDataInPtr->StartPtr = U2_RxBuff;                             //In����ָ������ݳ�Ա startָ��ָ����ջ�������ʼλ��
	UART2_ControlCB.Usart_RxCounter = 0;                                                //�ۼƽ��ջ�������ŵ�������=0
	
	memset(U2_RxBuff,0,U2_RXBUFF_SIZE);                                                                               //��մ���2 ���ջ�����	
	HAL_UART_Receive_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartRxDataInPtr->StartPtr,U2_RXMAX_SIZE);    //����DMA���� ָ������λ��  
	__HAL_UART_CLEAR_IDLEFLAG(&UART2_ControlCB.USART_Handler);                                                        //��������жϱ�־ 														
	__HAL_UART_ENABLE_IT(&UART2_ControlCB.USART_Handler, UART_IT_IDLE);                                               //�򿪿����ж�
}
/*-------------------------------------------------*/
/*������������3��������                            */
/*��  ����data������                               */
/*��  ����len��������                              */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u3_TxData(uint8_t *data, uint16_t len)
{  
	uint16_t i;                                                               
	
	while(__HAL_UART_GET_FLAG(&UART3_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);      //�ȴ�����3���Ϳ��У�Ȼ����ܷ�������	
	for(i=0;i<len;i++){	                                                               //ѭ��һ��һ���ֽڵķ���	
		UART3_ControlCB.USART_Handler.Instance->TDR = data[i];                         //����Ҫ���͵��ֽڣ���䵽����3����������
		while(__HAL_UART_GET_FLAG(&UART3_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //�ȴ�����3���Ϳ��У�Ȼ����ܷ�������	
	}
	
}
/*-------------------------------------------------*/
/*������������2��������                            */
/*��  ����data������                               */
/*��  ����len��������                              */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u2_TxData(uint8_t *data, uint16_t len)
{  
	uint16_t i;                                                               
	
	while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);      //�ȴ�����2���Ϳ��У�Ȼ����ܷ�������	
	for(i=0;i<len;i++){	                                                               //ѭ��һ��һ���ֽڵķ���	
		UART2_ControlCB.USART_Handler.Instance->TDR = data[i];                         //����Ҫ���͵��ֽڣ���䵽����2����������
		while(__HAL_UART_GET_FLAG(&UART2_ControlCB.USART_Handler,UART_FLAG_TC)!=SET);  //�ȴ�����2���Ϳ��У�Ȼ����ܷ�������	
	}
	
}
/*----------------------------------------------------------*/
/*�����������ͻ������������                              */
/*��  ����databuff������                                    */
/*��  ����datalen�����ݳ���                                 */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void u3_TxDataBuf(uint8_t *databuff, uint32_t datalen)
{
	if(U3_TXBUFF_SIZE - UART3_ControlCB.Usart_TxCounter >= datalen){                                   //���㷢�ͻ�������ʣ��Ŀռ䣬����������ű��ε����ݣ����Ļ�����if
		UART3_ControlCB.UsartTxDataInPtr->StartPtr = &U3_TxBuff[UART3_ControlCB.Usart_TxCounter];	   //��Ǳ��δ�����ݵ���ʼλ��
	}else{                                                                                             //��֮��������ű��ε����ݣ�����else
		UART3_ControlCB.Usart_TxCounter = 0;                                                           //�ۼƷ��ͻ�������ŵ�����������
		UART3_ControlCB.UsartTxDataInPtr->StartPtr = U3_TxBuff;                                        //������ݵ���ʼλ���������õ����ջ���������ʼλ��		
	}
	memcpy(UART3_ControlCB.UsartTxDataInPtr->StartPtr,databuff,datalen);	                           //��������            
    UART3_ControlCB.Usart_TxCounter += datalen;	                                                       //�����ۼƷ��ͻ�������ŵ�������
	UART3_ControlCB.UsartTxDataInPtr->EndPtr = &U3_TxBuff[UART3_ControlCB.Usart_TxCounter];            //��Ƿ��ͻ��������δ�ŵĽ���λ��		
				
	UART3_ControlCB.UsartTxDataInPtr++;                                                                //����INָ������
	if(UART3_ControlCB.UsartTxDataInPtr==UART3_ControlCB.UsartTxDataEndPtr)                            //������Ƶ�Endָ���ǵĽ���λ�ã�����if
		UART3_ControlCB.UsartTxDataInPtr = &UART3_ControlCB.Usart_TxLocation[0];                       //���ͻ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա    
}
/*----------------------------------------------------------*/
/*�����������ͻ������������                              */
/*��  ����databuff������                                    */
/*��  ����datalen�����ݳ���                                 */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void u2_TxDataBuf(uint8_t *databuff, uint32_t datalen)
{
	if(U2_TXBUFF_SIZE - UART2_ControlCB.Usart_TxCounter >= datalen){                                   //���㷢�ͻ�������ʣ��Ŀռ䣬����������ű��ε����ݣ����Ļ�����if
		UART2_ControlCB.UsartTxDataInPtr->StartPtr = &U2_TxBuff[UART2_ControlCB.Usart_TxCounter];	   //��Ǳ��δ�����ݵ���ʼλ��
	}else{                                                                                             //��֮��������ű��ε����ݣ�����else
		UART2_ControlCB.Usart_TxCounter = 0;                                                           //�ۼƷ��ͻ�������ŵ�����������
		UART2_ControlCB.UsartTxDataInPtr->StartPtr = U2_TxBuff;                                        //������ݵ���ʼλ���������õ����ջ���������ʼλ��		
	}
	memcpy(UART2_ControlCB.UsartTxDataInPtr->StartPtr,databuff,datalen);	                           //��������            
    UART2_ControlCB.Usart_TxCounter += datalen;	                                                       //�����ۼƷ��ͻ�������ŵ�������
	UART2_ControlCB.UsartTxDataInPtr->EndPtr = &U2_TxBuff[UART2_ControlCB.Usart_TxCounter];            //��Ƿ��ͻ��������δ�ŵĽ���λ��		
				
	UART2_ControlCB.UsartTxDataInPtr++;                                                                //����INָ������
	if(UART2_ControlCB.UsartTxDataInPtr==UART2_ControlCB.UsartTxDataEndPtr)                            //������Ƶ�Endָ���ǵĽ���λ�ã�����if
		UART2_ControlCB.UsartTxDataInPtr = &UART2_ControlCB.Usart_TxLocation[0];                       //���ͻ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա    
}
/*----------------------------------------------------------*/
/*����������ֹ������ɻص�����                              */
/*��  ����huart���������þ��                               */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{  
	if(huart->Instance==USART3){                                                                                          //�ж��Ǵ��ڼ��Ļص�
		UART3_ControlCB.UsartRxDataInPtr->EndPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];   		                  //��Ǳ��ν��յĽ���λ��		
		UART3_ControlCB.UsartRxDataInPtr->Timecounter = HAL_GetTick();                                                    //��¼���յ�ʱ��
		UART3_ControlCB.UsartRxDataInPtr++;                                                                               //���ջ���������INָ������
		if(UART3_ControlCB.UsartRxDataInPtr==UART3_ControlCB.UsartRxDataEndPtr)                                           //���ָ�����Ƶ���β��־ָ���ʱ�򣬽���if
			UART3_ControlCB.UsartRxDataInPtr = &UART3_ControlCB.Usart_RxLocation[0];                                      //���ջ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա   		
		if(U3_RXBUFF_SIZE - UART3_ControlCB.Usart_RxCounter >= U3_RXMAX_SIZE){                                            //���㣬���ʣ��ռ���� ���ν��յ������� ����if
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];                     //��¼�´ν��յ���ʼλ��	
		}else{                                                                                                            //���ʣ��ռ䲻�����ν����� ����else
			UART3_ControlCB.Usart_RxCounter = 0;                                                                          //�ۼƽ��ջ�������ŵ����������
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = U3_RxBuff;                                                       //����λ�ã����»ص�����������ʼλ��		
		}
		HAL_UART_Receive_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartRxDataInPtr->StartPtr,U3_RXMAX_SIZE);    //������һ�ε� DMA���� ָ������λ��  		
	}	
	
	if(huart->Instance==USART2){                                                                                          //�ж��Ǵ��ڼ��Ļص�
		UART2_ControlCB.UsartRxDataInPtr->EndPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];   		                  //��Ǳ��ν��յĽ���λ��		
		UART2_ControlCB.UsartRxDataInPtr++;                                                                               //���ջ���������INָ������
		if(UART2_ControlCB.UsartRxDataInPtr==UART2_ControlCB.UsartRxDataEndPtr)                                           //���ָ�����Ƶ���β��־ָ���ʱ�򣬽���if
			UART2_ControlCB.UsartRxDataInPtr = &UART2_ControlCB.Usart_RxLocation[0];                                      //���ջ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա   		
		if(U2_RXBUFF_SIZE - UART2_ControlCB.Usart_RxCounter >= U2_RXMAX_SIZE){                                            //���㣬���ʣ��ռ���� ���ν��յ������� ����if
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];                     //��¼�´ν��յ���ʼλ��	
		}else{                                                                                                            //���ʣ��ռ䲻�����ν����� ����else
			UART2_ControlCB.Usart_RxCounter = 0;                                                                          //�ۼƽ��ջ�������ŵ����������
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = U2_RxBuff;                                                       //����λ�ã����»ص�����������ʼλ��		
		}
		HAL_UART_Receive_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartRxDataInPtr->StartPtr,U2_RXMAX_SIZE);    //������һ�ε� DMA���� ָ������λ��  		
	}	
}
/*----------------------------------------------------------*/
/*�����������ݽ�����ɻص�����                              */
/*��  ����huart���������þ��                               */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART3){                                                                                          //�ж��Ǵ��ڼ��Ļص�
		UART3_ControlCB.Usart_RxCounter += U3_RXMAX_SIZE;                                                                 //�ۼƵ�ǰ�������Ѿ���ŵ�������				
		UART3_ControlCB.UsartRxDataInPtr->EndPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];   		                  //��Ǳ��ν��յĽ���λ��		
		UART3_ControlCB.UsartRxDataInPtr->Timecounter = HAL_GetTick();                                                    //��¼���յ�ʱ��
		UART3_ControlCB.UsartRxDataInPtr++;                                                                               //���ջ���������INָ������
		if(UART3_ControlCB.UsartRxDataInPtr==UART3_ControlCB.UsartRxDataEndPtr)                                           //���ָ�����Ƶ���β��־ָ���ʱ�򣬽���if
			UART3_ControlCB.UsartRxDataInPtr = &UART3_ControlCB.Usart_RxLocation[0];                                      //���ջ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա   		
		if(U3_RXBUFF_SIZE - UART3_ControlCB.Usart_RxCounter >= U3_RXMAX_SIZE){                                            //���㣬���ʣ��ռ���� ���ν��յ������� ����if
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = &U3_RxBuff[UART3_ControlCB.Usart_RxCounter];                     //��¼�´ν��յ���ʼλ��	
		}else{                                                                                                            //���ʣ��ռ䲻�����ν����� ����else
			UART3_ControlCB.Usart_RxCounter = 0;                                                                          //�ۼƽ��ջ�������ŵ����������
			UART3_ControlCB.UsartRxDataInPtr->StartPtr = U3_RxBuff;                                                       //����λ�ã����»ص�����������ʼλ��		
		}
		HAL_UART_Receive_DMA(&UART3_ControlCB.USART_Handler,UART3_ControlCB.UsartRxDataInPtr->StartPtr,U3_RXMAX_SIZE);     //������һ�ε� DMA���� ָ������λ��  		
	}

	if(huart->Instance==USART2){                                                                                          //�ж��Ǵ��ڼ��Ļص�
		UART2_ControlCB.Usart_RxCounter += U2_RXMAX_SIZE;                                                                 //�ۼƵ�ǰ�������Ѿ���ŵ�������				
		UART2_ControlCB.UsartRxDataInPtr->EndPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];   		                  //��Ǳ��ν��յĽ���λ��		
		UART2_ControlCB.UsartRxDataInPtr++;                                                                               //���ջ���������INָ������
		if(UART2_ControlCB.UsartRxDataInPtr==UART2_ControlCB.UsartRxDataEndPtr)                                           //���ָ�����Ƶ���β��־ָ���ʱ�򣬽���if
			UART2_ControlCB.UsartRxDataInPtr = &UART2_ControlCB.Usart_RxLocation[0];                                      //���ջ����� In����ָ���λ�� λ�ýṹ������0�ų�Ա   		
		if(U2_RXBUFF_SIZE - UART2_ControlCB.Usart_RxCounter >= U2_RXMAX_SIZE){                                            //���㣬���ʣ��ռ���� ���ν��յ������� ����if
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = &U2_RxBuff[UART2_ControlCB.Usart_RxCounter];                     //��¼�´ν��յ���ʼλ��	
		}else{                                                                                                            //���ʣ��ռ䲻�����ν����� ����else
			UART2_ControlCB.Usart_RxCounter = 0;                                                                          //�ۼƽ��ջ�������ŵ����������
			UART2_ControlCB.UsartRxDataInPtr->StartPtr = U2_RxBuff;                                                       //����λ�ã����»ص�����������ʼλ��		
		}
		HAL_UART_Receive_DMA(&UART2_ControlCB.USART_Handler,UART2_ControlCB.UsartRxDataInPtr->StartPtr,U2_RXMAX_SIZE);     //������һ�ε� DMA���� ָ������λ��  		
	}		
}
/*----------------------------------------------------------*/
/*�����������ݷ�����ɻص�����                              */
/*��  ����huart���������þ��                               */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART3){                         //�ж��Ǵ��ڼ��Ļص�
		UART3_ControlCB.Usart_TxCpltflag = 0;            //���Ϳ���
		UART3_ControlCB.Usart_TxTimer = HAL_GetTick();   //��¼��ǰʱ��		
	}
	if(huart->Instance==USART2){                         //�ж��Ǵ��ڼ��Ļص�
		UART2_ControlCB.Usart_TxCpltflag = 0;            //���Ϳ���
		UART2_ControlCB.Usart_TxTimer = HAL_GetTick();   //��¼��ǰʱ��		
	}
}

