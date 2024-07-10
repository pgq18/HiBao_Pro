#include "lwip/netifapi.h"
#include "hi_wifi_api.h"
#include "hi_types.h"
#include "hi_timer.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "STA.h"

hi_s32 example_get_match_network(hi_wifi_assoc_request *expected_bss)
{
    hi_s32 ret;
    hi_u32 num = 0; /* 扫描到的Wi-Fi网络数量 */
    hi_char expected_ssid[] = "TP-LINK XDR3010";
    hi_char key[] = "1119zbgpa4.0"; /* 待连接的网络接入密码 */
    hi_bool find_ap = HI_FALSE;
    hi_u8 bss_index;

    /* 获取扫描结果 */
    hi_u32 scan_len = sizeof(hi_wifi_ap_info) * WIFI_SCAN_AP_LIMIT;
    hi_wifi_ap_info *pst_results = malloc(scan_len);
    if (pst_results == HI_NULL)
    {
        return HISI_FAIL;
    }
    memset_s(pst_results, scan_len, 0, scan_len);
    ret = hi_wifi_sta_scan_results(pst_results, &num);
    if (ret != HISI_OK)
    {
        free(pst_results);
        return HISI_FAIL;
    }

    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    for (bss_index = 0; bss_index < num; bss_index++)
    {
        if (strlen(expected_ssid) == strlen(pst_results[bss_index].ssid))
        {
            if (memcmp(expected_ssid, pst_results[bss_index].ssid, strlen(expected_ssid)) == HISI_OK)
            {
                find_ap = HI_TRUE;
                break;
            }
        }
    }

    /* 未找到待连接AP,可以继续尝试扫描或者退出 */
    if (find_ap == HI_FALSE)
    {
        free(pst_results);
        return HISI_FAIL;
    }

    /* 找到网络后复制网络信息和接入密码 */
    if (memcpy_s(expected_bss->ssid, (HI_WIFI_MAX_SSID_LEN + 1), expected_ssid, strlen(expected_ssid)) != HISI_OK)
    {
        free(pst_results);
        return HISI_FAIL;
    }
    expected_bss->auth = pst_results[bss_index].auth;
    // 不需要设置 hidden_ssid，因为结构体中没有定义
    expected_bss->pairwise = 0;
    if (memcpy_s(expected_bss->key, (HI_WIFI_MAX_KEY_LEN + 1), key, strlen(key)) != HISI_OK)
    {
        free(pst_results);
        return HISI_FAIL;
    }

    free(pst_results);
    return HISI_OK;
}

hi_bool example_check_connect_status(hi_void)
{
    hi_u8 index;
    hi_wifi_status wifi_status;
    /* 获取网络连接状态，共查询5次，每次间隔500ms */
    for (index = 0; index < 5; index++)
    {
        hi_udelay(500000); /* 延迟500ms */
        memset_s(&wifi_status, sizeof(hi_wifi_status), 0, sizeof(hi_wifi_status));
        if (hi_wifi_sta_get_connect_info(&wifi_status) != HISI_OK)
        {
            continue;
        }
        if (wifi_status.status == HI_WIFI_CONNECTED)
        {
            return HI_TRUE; /* 连接成功退出循环 */
        }
    }
    return HI_FALSE;
}

hi_s32 example_sta_function(hi_void)
{
    hi_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0}; /* 创建的STA接口名 */
    hi_s32 len = WIFI_IFNAME_MAX_SIZE + 1;          /* STA接口名长度，+1为了存放字符串结束符 */
    hi_wifi_assoc_request expected_bss = {0};       /* 连接请求信息 */
    struct netif *netif_p = HI_NULL;

    /* 创建STA接口 */
    if (hi_wifi_sta_start(ifname, &len) != HISI_OK)
    {
        return HISI_FAIL;
    }

    /* 启动STA扫描 */
    if (hi_wifi_sta_scan() != HISI_OK)
    {
        return HISI_FAIL;
    }
    hi_udelay(1000000); /* 延迟1s等待扫描完成 */

    /* 获取待连接的网络 */
    if (example_get_match_network(&expected_bss) != HISI_OK)
    {
        return HISI_FAIL;
    }

    /* 启动连接 */
    if (hi_wifi_sta_connect(&expected_bss) != HISI_OK)
    {
        return HISI_FAIL;
    }

    /* 检查网络是否连接成功 */
    if (example_check_connect_status() == HI_FALSE)
    {
        return HISI_FAIL;
    }

    /* DHCP获取IP地址 */
    netif_p = netif_find(ifname);
    if (netif_p == HI_NULL)
    {
        return HISI_FAIL;
    }
    if (netifapi_dhcp_start(netif_p) != HISI_OK)
    {
        return HISI_FAIL;
    }

    /* 连接成功 */
    printf("STA connect success.\n");
    return HISI_OK;
}
