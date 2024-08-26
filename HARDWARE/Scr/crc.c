/*-------------------------------------------------*/
/*            ����˵������STM32ϵ�п�����          */
/*-------------------------------------------------*/
/*                                                 */
/*                ʵ��CRC���ܵ�Դ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //������Ҫ��ͷ�ļ�
#include "crc.h"              //������Ҫ��ͷ�ļ�

CRC_HandleTypeDef hcrc;       //crc���

/*-------------------------------------------------*/
/*��������CRC16_Xmodemģʽ��ʼ��                   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CRC16_XmodemInit(void)
{
	hcrc.Instance = CRC;                                                        //����CRC
	hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;                //��ʹ��Ĭ�϶���ʽ
	hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;                 //��ʹ��Ĭ�ϳ�ʼֵ
	hcrc.Init.GeneratingPolynomial = 0x1021;                                    //���ö���ʽ
	hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;                                   //CRC���� 16
	hcrc.Init.InitValue = 0x0000;                                               //���ó�ʼֵ
	hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;            //�������ݲ���ת
	hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;       //������ݲ���ת
	hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;                          //�������ݸ�ʽ���� 1���ֽ�
	HAL_CRC_Init(&hcrc);                                                        //����
}
/*-------------------------------------------------*/
/*��������CRC �ĵײ��ʼ��                         */
/*��  ����hcrc��CRC���þ��                        */
/*����ֵ����                                       */
/*˵  �����˺����ᱻHAL_CRC_Init()�ص�����         */
/*-------------------------------------------------*/
void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
{
	if(hcrc->Instance==CRC){           //�������crc
		__HAL_RCC_CRC_CLK_ENABLE();    //��crcʱ��
	}
}
