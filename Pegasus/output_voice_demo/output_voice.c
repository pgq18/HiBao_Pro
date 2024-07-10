/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iconv.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "iot_gpio.h"
#include "iot_errno.h"

#include "speaker.h"

char* gb2312_to_utf8(const char* gb2312_str) {
    size_t gb2312_len = strlen(gb2312_str) + 1;
    size_t utf8_len = gb2312_len * 3; // UTF-8 瀛�?�拷?锟戒覆锟�??鑳戒細鏇�?�?
    char* utf8_str = (char*)malloc(utf8_len);
    if (!utf8_str) {
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }

    iconv_t cd = iconv_open("UTF-8", "GB2312");
    if (cd == (iconv_t)-1) {
        perror("iconv_open");
        free(utf8_str);
        exit(EXIT_FAILURE);
    }

    char* inbuf = (char*)gb2312_str;
    char* outbuf = utf8_str;
    size_t inbytesleft = gb2312_len;
    size_t outbytesleft = utf8_len;

    if (iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) == (size_t)-1) {
        perror("iconv");
        iconv_close(cd);
        free(utf8_str);
        exit(EXIT_FAILURE);
    }

    iconv_close(cd);
    *outbuf = '\0'; // 锟�?淇濆瓧绗︿�?�锟�?? null 缁撳�?
    return utf8_str;
}


#define LED_INTERVAL_TIME_US 300000
#define LED_TASK_STACK_SIZE 4096
#define LED_TASK_PRIO 25
#define LED_TEST_GPIO 9 // for hispark_pegasus

static long long g_iState = 0;

enum LedState {
    LED_ON,
    LED_OFF,
    LED_SPARK,
};

enum LedState g_ledState = LED_SPARK;

static void *LedTask(const char *arg)
{
    (void)arg;
    IoTGpioSetOutputVal(LED_TEST_GPIO, 1);
    usleep(LED_INTERVAL_TIME_US);
    SpeakerInit();
    printf("SpeakerInit ok!\n");
    SetVolume(5);
    printf("SetVolume ok!\n");
    SetReader(Reader_XiaoYan);
    printf("SetReader ok!\n");

    // const char* gb2312_str = "浣犲ソ锛屼笘鐣岋�?"; // 绀轰緥 GB2312 瀛�?�涓�
    // char* utf8_str = gb2312_to_utf8(gb2312_str);
    // printf("UTF-8 String: %s\n", utf8_str);
    // //free(utf8_str); // 璁板緱閲婃斁鍐呭�?
    

    char str_in[20];
    sprintf(str_in, "[%c%c%c%c]", 0xBA, 0xEE, 0xB8, 0xE7);
    speech_text(str_in, GB2312);
    while(GetChipStatus() != ChipStatus_Idle)
	{
	  usleep(10000);
	}
    printf("speech_text ok!\n");

    while (1) {
        IoTGpioSetOutputVal(LED_TEST_GPIO, 0);
        usleep(LED_INTERVAL_TIME_US);
        g_iState++;
        // printf("1\n");
        if (g_iState == 0xffffffff) {
            g_iState = 0;
            break;
        }
    }
    printf("2\n");
    return NULL;
}

static void LedExampleEntry(void)
{
    osThreadAttr_t attr;

    IoTGpioInit(LED_TEST_GPIO);
    IoTGpioSetDir(LED_TEST_GPIO, IOT_GPIO_DIR_OUT);

    attr.name = "LedTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = LED_TASK_STACK_SIZE;
    attr.priority = LED_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)LedTask, NULL, &attr) == NULL) {
        printf("[LedExample] Falied to create LedTask!\n");
    }
}

SYS_RUN(LedExampleEntry);
