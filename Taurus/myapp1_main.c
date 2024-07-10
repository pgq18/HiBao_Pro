// #include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "sdk.h"
#include "sample_comm.h"

#include "myapp1_utils.h"

/*
 * 函数：显示用法
 * function: show usage
 */
static void SAMPLE_Usage(char* pchPrgName)
{
    printf("Usage : %s <index> \n", pchPrgName);
    printf("index:\n");
    printf("\t 0) voice saving demo).\n");
    printf("\t 1) image saving demo).\n");
    printf("\t 2) wifi connect demo).\n");
    printf("\t 3) switch testing demo).\n");
    printf("\t 4) image capturing demo).\n");
    printf("\t 5) NFS starting).\n");
    printf("\t 6) demo 0).\n");
    printf("\t 7) histreaming test).\n");
    printf("\t 8) audio test).\n");
    printf("\t 9) demo 1).\n");
}

/*
 * 函数：sample主函数
 * function : sample main function
 */
int main(int argc, char *argv[])
{
    HI_S32 s32Ret = HI_FAILURE;
    if (argc < 2 || argc > 2) { // 2: argc indicates the number of parameters
        SAMPLE_Usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { // 2: used to compare the first 2 characters
        SAMPLE_Usage(argv[0]);
        return HI_SUCCESS;
    }
    sdk_init();

    /*
     * MIPI为GPIO55，开启液晶屏背光
     * MIPI is GPIO55, Turn on the backlight of the LCD screen
     */
    system("cd /sys/class/gpio/;echo 55 > export;echo out > gpio55/direction;echo 1 > gpio55/value");

    switch (*argv[1]) {
        case '0':
            printf("================================= Audio Save =================================\n");
            SWITCH_INIT();
            AUDIO_SAVE();
            break;
        case '1':
            printf("================================= Image Display =================================\n");
            IMAGE_DISPLAY();
            break;
        case '2':
            printf("================================= WIFI CONNECT =================================\n");
            WIFI_CONNECT();
            break;
        case '3':
            printf("================================= SWITCH TEST =================================\n");
            SWITCH_TEST();
            break;
        case '4':
            printf("================================= IMAGE CAPTURE =================================\n");
            SWITCH_INIT();
            IMAGE_CAPTURE();
            break;
        case '5':
            printf("================================= NFS START =================================\n");
            NFS_START();
            break;
        case '6':
            printf("================================= DEMO 0 =================================\n");
            SWITCH_INIT();
            // AUDIO_INIT();
            HISTREAMING_INIT();
            DEMO_0();
            // AUDIO_STOP();
            break;
        case '7':
            printf("================================= HISTREAMING TEST =================================\n");
            HISTREAMING_TEST();
            break;
        case '8':
            printf("================================= AUDIO TEST =================================\n");
            SWITCH_INIT();
            AUDIO_INIT();
            AUDIO_START();
            AUDIO_STOP();
            break;
        case '9':
            printf("================================= DEMO 1 =================================\n");
            SWITCH_INIT();
            HISTREAMING_INIT();
            DEMO_1();
            break;
        default:
            SAMPLE_Usage(argv[0]);
            break;
    }
    sdk_exit();
    SAMPLE_PRT("\nsdk exit success\n");
    return s32Ret;
}