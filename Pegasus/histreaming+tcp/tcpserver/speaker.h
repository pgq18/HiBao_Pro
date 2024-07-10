#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

typedef enum
{
		ChipStatus_InitSuccessful = 0x4A,//初始化成功回传
		ChipStatus_CorrectCommand = 0x41,//收到正确的命令帧回传
		ChipStatus_ErrorCommand = 0x45,//收到不能识别命令帧回传
		ChipStatus_Busy = 0x4E,//芯片忙碌状态回传
		ChipStatus_Idle = 0x4F//芯片空闲状态回传
} ChipStatus_Type;//芯片回传

typedef struct
{
		uint8_t DataHead;
		uint8_t Length_HH;
		uint8_t Length_LL;
		uint8_t Commond;
		uint8_t EncodingFormat;
		uint8_t* Text;
} XFS_Protocol_TypeDef;

typedef enum
{
		GB2312 = 0x00,
		GBK = 0x01,
		BIG5 = 0x02,
		UNICODE = 0x03
} EncodingFormat_Type;//文本的编码格式

void speech_text(uint8_t *str,uint8_t encoding_format);

typedef struct {
    /** Pointer to the buffer storing data to send */
    unsigned char *sendBuf;
    /** Length of data to send */
    unsigned int  sendLen;
    /** Pointer to the buffer for storing data to receive */
    unsigned char *receiveBuf;
    /** Length of data received */
    unsigned int  receiveLen;
} IotI2cData;

typedef enum
{
		Reader_XiaoYan = 3,//? 为 3，设置发音人为小燕(女声, 推荐发音人)
		Reader_XuJiu = 51,//? 为 51，设置发音人为许久(男声, 推荐发音人)
		Reader_XuDuo = 52,//? 为 52，设置发音人为许多(男声)
		Reader_XiaoPing = 53,//? 为 53，设置发音人为小萍(女声)
		Reader_DonaldDuck = 54,//? 为 54，设置发音人为唐老鸭(效果器)
		Reader_XuXiaoBao = 55//? 为 55，设置发音人为许小宝(女童声)
} Reader_Type;//选择发音人 [m?]

uint32_t SpeakerInit(void);

uint32_t SetVolume(int volume);

void SetReader(Reader_Type reader);