/*-------------------------------------------------*/
/*            超子说物联网STM32系列开发板          */
/*-------------------------------------------------*/
/*                                                 */
/*                实现CRC功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32g0xx_hal.h"    //包含需要的头文件
#include "crc.h"              //包含需要的头文件

CRC_HandleTypeDef hcrc;       //crc句柄

/*-------------------------------------------------*/
/*函数名：CRC16_Xmodem模式初始化                   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CRC16_XmodemInit(void)
{
	hcrc.Instance = CRC;                                                        //配置CRC
	hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;                //不使用默认多项式
	hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;                 //不使用默认初始值
	hcrc.Init.GeneratingPolynomial = 0x1021;                                    //设置多项式
	hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;                                   //CRC长度 16
	hcrc.Init.InitValue = 0x0000;                                               //设置初始值
	hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;            //输入数据不反转
	hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;       //输出数据不反转
	hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;                          //输入数据格式长度 1个字节
	HAL_CRC_Init(&hcrc);                                                        //设置
}
/*-------------------------------------------------*/
/*函数名：CRC 的底层初始化                         */
/*参  数：hcrc：CRC配置句柄                        */
/*返回值：无                                       */
/*说  明：此函数会被HAL_CRC_Init()回调调用         */
/*-------------------------------------------------*/
void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
{
	if(hcrc->Instance==CRC){           //如果配置crc
		__HAL_RCC_CRC_CLK_ENABLE();    //开crc时钟
	}
}
