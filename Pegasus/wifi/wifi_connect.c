#include <hi_types_base.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "wifi_connect.h"

hi_void app_main(hi_void)
{
    hi_u32 ret;
    const hi_uchar wifi_vap_res_num = APP_INIT_VAP_NUM;
    const hi_uchar wifi_user_res_num = APP_INIT_USR_NUM;
    ret = hi_wifi_init(wifi_vap_res_num, wifi_user_res_num);
    if (ret != 0) {
        printf("fail to init wifi\n");
    } else {
       printf("wifi init success\n");
    }
}
