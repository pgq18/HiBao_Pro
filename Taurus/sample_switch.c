#include <poll.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>

#include "myapp1_utils.h"
#include "hisignalling.h"

#define MSG(args...) printf(args)

int gpio1_fd = -1;
int gpio2_fd = -1;
int ret1     = -1;
int ret2     = -1;
char key1_value[10];
char key2_value[10];

int SWITCH_INIT(void)
{
    /*
     * 按键初始化定义
     * Key initialization definition
     */

    InitGpio1();
    InitGpio2();

    gpio1_fd = open("/sys/class/gpio/gpio1/value", O_RDONLY);
    if (gpio1_fd < 0)
    {
        MSG("Failed to open gpio1 !\n");
    }

    gpio2_fd = open("/sys/class/gpio/gpio2/value", O_RDONLY);
    if (gpio2_fd < 0)
    {
        MSG("Failed to open gpio1 !\n");
    }
}

int SWITCH_TEST(void)
{
    /*
     * 按键初始化定义
     * Key initialization definition
     */
    SWITCH_INIT();

    while(1)
    {
        memset(key1_value, 0, sizeof(key1_value));
        lseek(gpio1_fd, 0, SEEK_SET);
        ret1 = read(gpio1_fd, key1_value, sizeof(key1_value));
        if ((key1_value[0] - '0') == 0) {
            MSG("F2 is pressed\n");
        }

        memset(key2_value, 0, sizeof(key2_value));
        lseek(gpio2_fd, 0, SEEK_SET);
        ret2 = read(gpio2_fd, key2_value, sizeof(key2_value));
        if ((key2_value[0] - '0') == 0) {
            MSG("F1 is pressed\n");
        }
        usleep(5);
    }

    return 0;
}

int CHECK_SWITCH_F1(void) {
    memset(key2_value, 0, sizeof(key2_value));
    lseek(gpio2_fd, 0, SEEK_SET);
    ret2 = read(gpio2_fd, key2_value, sizeof(key2_value));
    if ((key2_value[0] - '0') == 0) {
        MSG("F1 is pressed\n");
        usleep(1*1000000);
        return 1;
    }
    else {
        return 0;
    }
}

int CHECK_SWITCH_F2(void) {
    memset(key1_value, 0, sizeof(key1_value));
    lseek(gpio1_fd, 0, SEEK_SET);
    ret1 = read(gpio1_fd, key1_value, sizeof(key1_value));
    if ((key1_value[0] - '0') == 0) {
        MSG("F2 is pressed\n");
        usleep(1*1000000);
        return 1;
    }
    else {
        return 0;
    }
}