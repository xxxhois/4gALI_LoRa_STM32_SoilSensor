/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*          ʵ�ָ���SPI�ӿڹ��ܵ�Դ�ļ�            */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "spi.h"              //������Ҫ��ͷ�ļ�

SPI_HandleTypeDef SPI2_Handler;  //SPI2���
uint8_t temprxdata;

/*-------------------------------------------------*/
/*����������ʼ��SPI2�ӿ�                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI2_Init(void)
{
    SPI2_Handler.Instance=SPI2;                                  //SPI2
    SPI2_Handler.Init.Mode=SPI_MODE_MASTER;                      //����SPI����ģʽ������Ϊ��ģʽ
    SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;            //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;                //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;             //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;                  //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI2_Handler.Init.NSS=SPI_NSS_SOFT;                          //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2; //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
    SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;                 //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;                 //�ر�TIģʽ
    SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE; //�ر�Ӳ��CRCУ��
    SPI2_Handler.Init.CRCPolynomial=7;                           //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI2_Handler);                                 //��ʼ��
	__HAL_SPI_ENABLE(&SPI2_Handler);                             //ʹ��SPI2
}

/*-------------------------------------------------*/
/*��������SPI�ӿڵĵײ��ʼ��                      */
/*��  ����hspi��SPI�ӿ����þ��                    */
/*����ֵ����                                       */
/*˵  �����˺����ᱻHAL_SPI_Init()�ص�����         */
/*-------------------------------------------------*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
	if(hspi->Instance==SPI2){     
		__HAL_RCC_GPIOB_CLK_ENABLE();                        //�����˿�Dʱ��  
		__HAL_RCC_SPI2_CLK_ENABLE();                         //����SPI2�ӿ�ʱ��
		
		GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;   //����PD1 3 4
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;                   //����ģʽ
		GPIO_Initure.Pull=GPIO_PULLUP;                       //����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;        //VERY_HIGHģʽ
		GPIO_Initure.Alternate = GPIO_AF0_SPI2;	             //����ΪSPI2�ӿ�
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);                  //����	
	}
}
/*-------------------------------------------------*/
/*��������SPI�ӿڶ�д����                          */
/*��  ����TxData�����͵�����                       */
/*����ֵ����ȡ������                               */
/*-------------------------------------------------*/
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_TXE))==0);			  
	*(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR = TxData ;	  					
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_RXNE))==0); 
	return *(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR;  	
}
/*-------------------------------------------------*/
/*��������SPI2�ӿ�д����                           */
/*��  ����TxData�����͵�����                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI2_WriteByte(uint8_t TxData)
{  
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_TXE))==0);			  
	*(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR = TxData ;	  					
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_RXNE))==0); 
	temprxdata = *(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR;  	
}
/*-------------------------------------------------*/
/*��������SPI2�ӿڶ�����                           */
/*��  ������                                       */
/*����ֵ������������                               */
/*-------------------------------------------------*/
uint8_t SPI2_ReadByte(void)
{			 	
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_TXE))==0);			  
	*(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR = 0xFF ;	  					
	while((__HAL_SPI_GET_FLAG(&SPI2_Handler, SPI_FLAG_RXNE))==0); 
	return *(__IO uint8_t *)&(&SPI2_Handler)->Instance->DR;  			    
}
/*-------------------------------------------------*/
/*��������SPI2�ӿڶ�дָ����������                 */
/*��  ����TxData�����͵�����                       */
/*��  ����Rxdata�����͵�����                       */
/*��  ����len�����͵���������                      */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI2_WriteRead(uint8_t *TxData, uint8_t *Rxdata, uint16_t len)
{  
	uint16_t i;
	
	for(i=0;i<len;i++){
		Rxdata[i] = SPI2_ReadWriteByte(TxData[i]);
	}
}
/*-------------------------------------------------*/
/*��������SPI2�ӿ�дָ����������                   */
/*��  ����TxData�����͵�����                       */
/*��  ����len�����͵���������                      */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI2_Write(uint8_t *TxData, uint16_t len)
{  
	uint16_t i;
	
	for(i=0;i<len;i++){
		SPI2_WriteByte(TxData[i]);
	}
}
/*-------------------------------------------------*/
/*��������SPI2�ӿڶ�ָ����������                   */
/*��  ����TxData�����͵�����                       */
/*��  ����len�����͵���������                      */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI2_Read(uint8_t *Rxdata, uint16_t len)
{  
	uint16_t i;
	
	for(i=0;i<len;i++){
		Rxdata[i] = SPI2_ReadWriteByte(0xFF);
	}
}
