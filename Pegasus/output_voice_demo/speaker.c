#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "ohos_types.h"
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_errno.h"

#include "speaker.h"

#define I2C_1_BAUDRATE (15 * 1000)
#define I2C_IDX 0
#define I2C_ADDR 0x60 // 默认地址为 0x30, 由于最右边一位要做读写位，所以右移一位为0x60
#define DATAHEAD 0xFD   //帧头
#define OLED_I2C_ADDR 0x78 // 默认地址为 0x78

uint32_t SpeakerInit(void) {

    IoTGpioInit(13); /* 初始化gpio13 */ // I2C0_SDA
    IoSetFunc(13, 6); /* gpio13使用6功能 */
    IoTGpioInit(14); /* 初始化gpio14 */ // I2C0_SCL
    IoSetFunc(14, 6); /* gpio14使用6功能 */

    if (IOT_SUCCESS != IoTI2cInit(I2C_IDX, I2C_1_BAUDRATE)) {
        printf("Falied\n");
    }

    return IOT_SUCCESS;
}

uint32_t I2C_ByteWrite(uint8_t pBuffer) {

    unsigned int id = I2C_IDX;
    uint8_t byte = pBuffer;
    // uint8_t buffer[] = {I2C_ADDR, byte};
    uint8_t buffer[] = {byte};
    IotI2cData i2cData = {0};

    i2cData.sendBuf = buffer;
    i2cData.sendLen = sizeof(buffer) / sizeof(buffer[0]);
    
    return IoTI2cWrite(id, I2C_ADDR, i2cData.sendBuf, i2cData.sendLen);
    // return IoTI2cWrite(id, I2C_ADDR, i2cData.sendBuf, 1);
}

uint32_t I2C_ByteRead() {
    unsigned int id = I2C_IDX;
    uint8_t addr = I2C_ADDR;
    uint8_t buffer[] = {0};
    IotI2cData i2cData = {0};

    // i2cData.sendBuf = &addr;
    // i2cData.sendLen = 1;
    // IoTI2cWrite(id, OLED_I2C_ADDR, i2cData.sendBuf, i2cData.sendLen);
    i2cData.receiveBuf = buffer;
    i2cData.receiveLen = 1;

    if (IOT_SUCCESS != IoTI2cRead(id, I2C_ADDR, i2cData.receiveBuf, i2cData.receiveLen)) {
        printf("Falied\n");
    }

    printf("0: %d\n", i2cData.receiveBuf[0]);

    return i2cData.receiveBuf[0];
}

void I2C_Writes_Bytes(uint8_t *buff,int number) {
	int i;
	for(i = 0;i < number;i++)
	{
		I2C_ByteWrite(buff[i]);
	}
}

void SetBase(uint8_t *str) {

    uint16_t size = strlen(str) + 2;
	
	XFS_Protocol_TypeDef DataPack;

	DataPack.DataHead = DATAHEAD;
	DataPack.Length_HH = size >> 8;
	DataPack.Length_LL = size;
	DataPack.Commond = 0x01;
	DataPack.EncodingFormat = 0x00;
	DataPack.Text = str;
	
	if (IOT_SUCCESS != I2C_ByteWrite(DataPack.DataHead)) {
        printf("failed to write\n");
    }
	I2C_ByteWrite(DataPack.Length_HH);	
	I2C_ByteWrite(DataPack.Length_LL);
	I2C_ByteWrite(DataPack.Commond);	
	I2C_ByteWrite(DataPack.EncodingFormat);

	I2C_Writes_Bytes(DataPack.Text,strlen(DataPack.Text));

}

void TextCtrl(char c, int d) {
    char str[10];
    if (d != -1)
        sprintf(str, "[%c%d]", c, d);
    else
        sprintf(str, "[%c]", c);
    SetBase(str); 
}

uint8_t GetChipStatus(void) {
    uint8_t AskState[4] = {0xFD,0x00,0x01,0x21};
    I2C_Writes_Bytes(AskState, 4);
    usleep(350000);
    uint8_t pBuffer = I2C_ByteRead();
    return pBuffer;
}

uint32_t SetVolume(int volume) {
    TextCtrl('v', volume);
    long g_iState = 0;
    // usleep(10000);
    // printf("%d\n", GetChipStatus());
    while(GetChipStatus() != ChipStatus_Idle)
    {
        usleep(10000);
        printf("asking\n");
        g_iState++;
        if (g_iState == 0xffffffff) {
            g_iState = 0;
            break;
        }
    }
}

void SetReader(Reader_Type reader)
{
    TextCtrl('m', reader);
    long g_iState = 0;
    // usleep(10000);
    // printf("%d\n", GetChipStatus());
	while(GetChipStatus() != ChipStatus_Idle)
	{
        usleep(10000);
        printf("asking\n");
        g_iState++;
        if (g_iState == 0xffffffff) {
            g_iState = 0;
            break;
        }
	}
}

void speech_text(uint8_t *str,uint8_t encoding_format)
{
	 uint16_t size = strlen(str) + 2;
	//  uint16_t size_t = sizeof(str)/sizeof(str[0]);
	//  uint16_t size = size_t + 2;
     printf("ok!!!!!!\n");
	//  uint16_t size = 4 + 2;
    printf("size: %d\n", size);
    for(int i = 0;i < strlen(str);i++) {
        printf("%d\n", str[i]);
    }
    printf("\n");
	 
	 XFS_Protocol_TypeDef DataPack;
	
	 DataPack.DataHead = DATAHEAD;
	 DataPack.Length_HH = size >> 8;
	 DataPack.Length_LL = size;
	 DataPack.Commond = 0x01;
	 DataPack.EncodingFormat = encoding_format;
	 DataPack.Text = str;
	 
	 I2C_ByteWrite(DataPack.DataHead );
	 I2C_ByteWrite(DataPack.Length_HH);	
	 I2C_ByteWrite(DataPack.Length_LL);
	 I2C_ByteWrite(DataPack.Commond);	
	 I2C_ByteWrite(DataPack.EncodingFormat);		
	
	 I2C_Writes_Bytes(DataPack.Text,strlen(DataPack.Text));	 
	//  I2C_Writes_Bytes(DataPack.Text, size_t);
	//  I2C_Writes_Bytes(DataPack.Text, 4);	 
}
