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

HI_S32 WIFI_CONNECT(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;

    /*
     * Connect to Wifi
     */
    system("cd /etc/Wireless;/etc/Wireless/init_sta.sh");
    usleep(3*1000000);
    printf("Connected to Wifi.\n");

    return s32Ret;
}

HI_S32 NFS_START(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;

    /*
     * Connect to Wifi
     */
    system("mount -o nolock,addr=192.168.215.111 -t nfs 192.168.215.111:/d/nfs /mnt");
    usleep(3*1000000);

    return s32Ret;
}