/*-------------------------------------------------*/
/*           超子说物联网STM32系列开发板           */
/*-------------------------------------------------*/
/*                                                 */
/*            操作LoRa模块功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LORA_H
#define __LORA_H

#define SUB1_HADR               0x00                                              //表示 子设备1 地址高字节
#define SUB1_LADR               0x01                                              //表示 子设备1 地址低字节
#define SUB1_CH                 LoRa_CH21                                         //表示 子设备1 信道

#define SUB2_HADR               0x00                                              //表示 子设备2 地址高字节
#define SUB2_LADR               0x02                                              //表示 子设备2 地址低字节
#define SUB2_CH                 LoRa_CH22                                         //表示 子设备2 信道

#define SUB3_HADR               0x00                                              //表示 子设备3 地址高字节
#define SUB3_LADR               0x03                                              //表示 子设备3 地址低字节
#define SUB3_CH                 LoRa_CH23                                         //表示 子设备3 信道

typedef struct{  
	uint8_t LoRa_AddrH;         //模块地址高字节	   地址0x00
	uint8_t LoRa_AddrL;         //模块地址低字节	   地址0x01
	uint8_t LoRa_NetID;         //模块网络地址       地址0x02
	
	uint8_t LoRa_Baudrate;      //模块串口波特率     地址0x03
	uint8_t LoRa_UartMode;      //模块串口工作模式   地址0x03
	uint8_t LoRa_airvelocity;   //模块空中速率       地址0x03
	
	uint8_t LoRa_DataLen;       //数据分包大小       地址0x04
	uint8_t LoRa_Rssi;          //环境噪声使能       地址0x04
	uint8_t LoRa_TxPower;       //模块发射功率       地址0x04
	
	uint8_t LoRa_CH;            //模块信道           地址0x05
	
	uint8_t LoRa_RssiByte;      //Rssi字节功能            地址0x06
	uint8_t LoRa_DateMode;      //模块数据传输模式        地址0x06
	uint8_t LoRa_Relay;         //模块中继功能            地址0x06
	uint8_t LoRa_LBT;           //模块LBT监听功能         地址0x06
	uint8_t LoRa_WORmode;       //模块WOR模式下工作方式   地址0x06
	uint8_t LoRa_WORcycle;      //模块WOR模式下周期时间   地址0x06

	uint8_t LoRa_KeyH;          //加密秘钥高字节          地址0x07
	uint8_t LoRa_KeyL;          //加密秘钥低字节          地址0x08
	
}LoRaParameter;

#define LoRa_MODE0  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET) //LoRa模块 模式0
#define LoRa_MODE1  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)   //LoRa模块 模式1
#define LoRa_MODE2  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)   //LoRa模块 模式2
#define LoRa_MODE3  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)     //LoRa模块 模式3

#define LoRa_AUX          HAL_GPIO_ReadPin(GPIOB,  GPIO_PIN_7)                  //读取电平状态,判断模块状态
#define LoRa_PowerON      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET)    //控制供电,高电平ON
#define LoRa_PowerOFF     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET)  //控制供电,低电平OFF
      
#define LoRa_1200         0x00   //模块串口 波特率 1200
#define LoRa_2400         0x20   //模块串口 波特率 2400
#define LoRa_4800         0x40   //模块串口 波特率 4800
#define LoRa_9600         0x60   //模块串口 波特率 9600
#define LoRa_19200        0x80   //模块串口 波特率 19200
#define LoRa_38400        0xA0   //模块串口 波特率 38400
#define LoRa_57600        0xC0   //模块串口 波特率 57600
#define LoRa_115200       0xE0   //模块串口 波特率 115200

#define LoRa_8N1          0x00   //模块串口参数 8数据位 无校验 1停止位
#define LoRa_8O1          0x08   //模块串口参数 8数据位 奇校验 1停止位
#define LoRa_8E1          0x10   //模块串口参数 8数据位 偶校验 1停止位

#define LoRa_0_3          0x00   //模块空中速率 0.3K
#define LoRa_1_2          0x01   //模块空中速率 1.2K
#define LoRa_2_4          0x02   //模块空中速率 2.4K
#define LoRa_4_8          0x03   //模块空中速率 4.8K
#define LoRa_9_6          0x04   //模块空中速率 9.6K
#define LoRa_19_2         0x05   //模块空中速率 19.2K
#define LoRa_38_4         0x06   //模块空中速率 38.4K
#define LoRa_62_5         0x07   //模块空中速率 62.5K

#define LoRa_Data240      0x00   //数据分包大小 240字节
#define LoRa_Data128      0x40   //数据分包大小 128字节
#define LoRa_Data64       0x80   //数据分包大小 64字节
#define LoRa_Data32       0xC0   //数据分包大小 32字节

#define LoRa_RssiEN       0x02   //启用RSSI功能
#define LoRa_RssiDIS      0x00   //禁用RSSI功能

#define LoRa_FEC_22DBM    0x00   //发射功率22dbm
#define LoRa_FEC_17DBM    0x01   //发射功率17dbm
#define LoRa_FEC_13DBM    0x02   //发射功率13dbm
#define LoRa_FEC_10DBM    0x03   //发射功率10dbm

#define LoRa_CH0          0x00   //模块信道 对应频率410MHz
#define LoRa_CH1          0x01   //模块信道 对应频率411MHz
#define LoRa_CH2          0x02   //模块信道 对应频率412MHz
#define LoRa_CH3          0x03   //模块信道 对应频率413MHz
#define LoRa_CH4          0x04   //模块信道 对应频率414MHz
#define LoRa_CH5          0x05   //模块信道 对应频率415MHz
#define LoRa_CH6          0x06   //模块信道 对应频率416MHz
#define LoRa_CH7          0x07   //模块信道 对应频率417MHz
#define LoRa_CH8          0x08   //模块信道 对应频率418MHz
#define LoRa_CH9          0x09   //模块信道 对应频率419MHz
#define LoRa_CH10         0x0A   //模块信道 对应频率420MHz
#define LoRa_CH11         0x0B   //模块信道 对应频率421MHz
#define LoRa_CH12         0x0C   //模块信道 对应频率422MHz
#define LoRa_CH13         0x0D   //模块信道 对应频率423MHz
#define LoRa_CH14         0x0E   //模块信道 对应频率424MHz
#define LoRa_CH15         0x0F   //模块信道 对应频率425MHz
#define LoRa_CH16         0x10   //模块信道 对应频率426MHz
#define LoRa_CH17         0x11   //模块信道 对应频率427MHz
#define LoRa_CH18         0x12   //模块信道 对应频率428MHz
#define LoRa_CH19         0x13   //模块信道 对应频率429MHz
#define LoRa_CH20         0x14   //模块信道 对应频率430MHz
#define LoRa_CH21         0x15   //模块信道 对应频率431MHz
#define LoRa_CH22         0x16   //模块信道 对应频率432MHz
#define LoRa_CH23         0x17   //模块信道 对应频率433MHz
#define LoRa_CH24         0x18   //模块信道 对应频率434MHz
#define LoRa_CH25         0x19   //模块信道 对应频率435MHz
#define LoRa_CH26         0x1A   //模块信道 对应频率436MHz
#define LoRa_CH27         0x1B   //模块信道 对应频率437MHz
#define LoRa_CH28         0x1C   //模块信道 对应频率438MHz
#define LoRa_CH29         0x1D   //模块信道 对应频率439MHz
#define LoRa_CH30         0x1E   //模块信道 对应频率440MHz
#define LoRa_CH31         0x1F   //模块信道 对应频率441MHz
#define LoRa_CH32         0x20   //模块信道 对应频率442MHz
#define LoRa_CH33         0x21   //模块信道 对应频率442MHz
#define LoRa_CH34         0x22   //模块信道 对应频率444MHz
#define LoRa_CH35         0x23   //模块信道 对应频率445MHz
#define LoRa_CH36         0x24   //模块信道 对应频率446MHz
#define LoRa_CH37         0x25   //模块信道 对应频率447MHz
#define LoRa_CH38         0x26   //模块信道 对应频率448MHz
#define LoRa_CH39         0x27   //模块信道 对应频率449MHz
#define LoRa_CH40         0x28   //模块信道 对应频率450MHz
#define LoRa_CH41         0x29   //模块信道 对应频率451MHz
#define LoRa_CH42         0x2A   //模块信道 对应频率452MHz
#define LoRa_CH43         0x2B   //模块信道 对应频率453MHz
#define LoRa_CH44         0x2C   //模块信道 对应频率454MHz
#define LoRa_CH45         0x2D   //模块信道 对应频率455MHz
#define LoRa_CH46         0x2E   //模块信道 对应频率456MHz
#define LoRa_CH47         0x2F   //模块信道 对应频率457MHz
#define LoRa_CH48         0x30   //模块信道 对应频率458MHz
#define LoRa_CH49         0x31   //模块信道 对应频率459MHz
#define LoRa_CH50         0x32   //模块信道 对应频率460MHz
#define LoRa_CH51         0x33   //模块信道 对应频率461MHz
#define LoRa_CH52         0x34   //模块信道 对应频率462MHz
#define LoRa_CH53         0x35   //模块信道 对应频率463MHz
#define LoRa_CH54         0x36   //模块信道 对应频率464MHz
#define LoRa_CH55         0x37   //模块信道 对应频率465MHz
#define LoRa_CH56         0x38   //模块信道 对应频率466MHz
#define LoRa_CH57         0x39   //模块信道 对应频率467MHz
#define LoRa_CH58         0x3A   //模块信道 对应频率468MHz
#define LoRa_CH59         0x3B   //模块信道 对应频率469MHz
#define LoRa_CH60         0x3C   //模块信道 对应频率470MHz
#define LoRa_CH61         0x3D   //模块信道 对应频率471MHz
#define LoRa_CH62         0x3E   //模块信道 对应频率472MHz
#define LoRa_CH63         0x3F   //模块信道 对应频率473MHz
#define LoRa_CH64         0x40   //模块信道 对应频率474MHz
#define LoRa_CH65         0x41   //模块信道 对应频率475MHz
#define LoRa_CH66         0x42   //模块信道 对应频率476MHz
#define LoRa_CH67         0x43   //模块信道 对应频率477MHz
#define LoRa_CH68         0x44   //模块信道 对应频率478MHz
#define LoRa_CH69         0x45   //模块信道 对应频率479MHz
#define LoRa_CH70         0x46   //模块信道 对应频率480MHz
#define LoRa_CH71         0x47   //模块信道 对应频率481MHz
#define LoRa_CH72         0x48   //模块信道 对应频率482MHz
#define LoRa_CH73         0x49   //模块信道 对应频率483MHz
#define LoRa_CH74         0x4A   //模块信道 对应频率484MHz
#define LoRa_CH75         0x4B   //模块信道 对应频率485MHz
#define LoRa_CH76         0x4C   //模块信道 对应频率486MHz
#define LoRa_CH77         0x4D   //模块信道 对应频率487MHz
#define LoRa_CH78         0x4E   //模块信道 对应频率488MHz
#define LoRa_CH79         0x4F   //模块信道 对应频率489MHz
#define LoRa_CH80         0x50   //模块信道 对应频率490MHz
#define LoRa_CH81         0x51   //模块信道 对应频率491MHz
#define LoRa_CH82         0x52   //模块信道 对应频率492MHz
#define LoRa_CH83         0x53   //模块信道 对应频率493MHz

#define LoRa_RssiByteEN   0x80   //启用RSSI字节功能
#define LoRa_RssiByteDIS  0x00   //禁用RSSI字节功能

#define LoRa_ModeTRANS    0x00   //模块透明传输
#define LoRa_ModePOINT    0x40   //模块定点传输

#define LoRa_RelayEN      0x20   //启用中继
#define LoRa_RelayDIS     0x00   //禁用中继

#define LoRa_LBTEN        0x10   //启用LBT
#define LoRa_LBTDIS       0x00   //禁用LBT

#define LoRa_WorTX        0x08   //Wor模式发送
#define LoRa_WorRX        0x00   //Wor模式接收

#define LoRa_Wor500ms     0x00   //Wor周期500毫秒
#define LoRa_Wor1000ms    0x01   //Wor周期1000毫秒
#define LoRa_Wor1500ms    0x02   //Wor周期1500毫秒
#define LoRa_Wor2000ms    0x03   //Wor周期2000毫秒
#define LoRa_Wor2500ms    0x04   //Wor周期2500毫秒
#define LoRa_Wor3000ms    0x05   //Wor周期3000毫秒
#define LoRa_Wor3500ms    0x06   //Wor周期3500毫秒
#define LoRa_Wor4000ms    0x07   //Wor周期4000毫秒

void LoRa_GPIO_Init(void);       				//函数声明，初始化模块的IO 
uint8_t LoRa_Reset(int16_t);     				//函数声明，LoRa复位命令
void LoRa_Init(void);         				    //函数声明，初始化模块
void LoRa_Set(void);                            //函数声明，LoRa设置工作参数
void LoRa_Get(uint8_t *);                       //函数声明，LoRa查询工作参数   
void U3PassiveEvent(uint8_t *, uint16_t);       //函数声明，串口3被动事件  

#endif


