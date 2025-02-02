/*
 * Copyright (c) 2020, HiHope Community.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_device.h"

#include "lwip/netifapi.h"
#include "lwip/api_shell.h"

#define STACK_SIZE     10240
// #define AP_SSID        "TP-LINK XDR3010"
// #define AP_SKEY        "1119zbgpa4.0"
#define AP_SSID        "HONOR"
#define AP_SKEY        "gyf30105381"

#define IDX_0          0
#define IDX_1          1
#define IDX_2          2
#define IDX_3          3
#define IDX_4          4
#define IDX_5          5

#define DELAY_TICKS_10     (10)
#define DELAY_TICKS_50     (50)
#define DELAY_TICKS_1000    (1000)
#define DELAY_TICKS_200    (200)

#define COUNTER            (60)

static void PrintLinkedInfo(WifiLinkedInfo* info)
{
    if (!info) return;

    static char macAddress[32] = {0};
    unsigned char* mac = info->bssid;
    // int ret = snprintf(macAddress, sizeof(macAddress), "%02X:%02X:%02X:%02X:%02X:%02X",
    int ret = snprintf_s(macAddress, sizeof(macAddress), sizeof(macAddress) - 1, "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[IDX_0], mac[IDX_1], mac[IDX_2], mac[IDX_3], mac[IDX_4], mac[IDX_5]);
    if (ret < 0) {
        return;
    }
    printf("bssid: %s, rssi: %d, connState: %d, reason: %d, ssid: %s\r\n",
        macAddress, info->rssi, info->connState, info->disconnectedReason, info->ssid);
}

static int g_connected = 0;
static void OnWifiConnectionChanged(int state, WifiLinkedInfo* info)
{
    if (!info) return;

    printf("%s %d, state = %d, info = \r\n", __FUNCTION__, __LINE__, state);
    PrintLinkedInfo(info);

    if (state == WIFI_STATE_AVALIABLE) {
        g_connected = 1;
    } else {
        g_connected = 0;
    }
}

static void OnWifiScanStateChanged(int state, int size)
{
    printf("%s %d, state = %X, size = %d\r\n", __FUNCTION__, __LINE__, state, size);
}

static void WifiConnectTask(void)
{
    WifiErrorCode errCode;
    WifiEvent eventListener = {
        .OnWifiConnectionChanged = OnWifiConnectionChanged,
        .OnWifiScanStateChanged = OnWifiScanStateChanged
    };
    WifiDeviceConfig apConfig = {};
    int netId = -1;

    osDelay(DELAY_TICKS_10);
    errCode = RegisterWifiEvent(&eventListener);
    printf("RegisterWifiEvent: %d\r\n", errCode);

    // setup your AP params
    // strcpy(apConfig.ssid, "ABCD");
    // strcpy(apConfig.preSharedKey, "147258369");
    strcpy_s(apConfig.ssid, WIFI_MAX_SSID_LEN, AP_SSID);
    strcpy_s(apConfig.preSharedKey, WIFI_MAX_KEY_LEN, AP_SKEY);
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    int timeoutCounter = COUNTER * COUNTER;
    while (timeoutCounter--) {
        errCode = EnableWifi();
        printf("EnableWifi: %d\r\n", errCode);
        osDelay(DELAY_TICKS_10);

        errCode = AddDeviceConfig(&apConfig, &netId);
        printf("AddDeviceConfig: %d\r\n", errCode);

        g_connected = 0;
        errCode = ConnectTo(netId);
        printf("ConnectTo(%d): %d\r\n", netId, errCode);

        while (!g_connected) {
            osDelay(DELAY_TICKS_10);
        }
        printf("g_connected: %d\r\n", g_connected);
        osDelay(DELAY_TICKS_50);

        // 联网业务开始
        struct netif* iface = netifapi_netif_find("wlan0");
        if (iface) {
            err_t ret = netifapi_dhcp_start(iface);
            printf("netifapi_dhcp_start: %d\r\n", ret);

            osDelay(DELAY_TICKS_200); // wait DHCP server give me IP
            ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
            printf("netifapi_netif_common: %d\r\n", ret);
        }
        // 模拟一段时间的联网业务
        int timeout = 0;
        while (1) {
            osDelay(DELAY_TICKS_1000);
            timeout++;
            if (timeout == 0xffffffff) {
            timeout = 0;
            break;
        }
        }

        // // 联网业务结束
        // err_t ret = netifapi_dhcp_stop(iface);
        // printf("netifapi_dhcp_stop: %d\r\n", ret);

        // Disconnect(); // disconnect with your AP

        // RemoveDevice(netId); // remove AP config

        // errCode = DisableWifi();
        // printf("DisableWifi: %d\r\n", errCode);
        // osDelay(DELAY_TICKS_200);
    }
}

static void WifiConnectDemo(void)
{
    osThreadAttr_t attr;

    attr.name = "WifiConnectTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew(WifiConnectTask, NULL, &attr) == NULL) {
        printf("[WifiConnectDemo] Falied to create WifiConnectTask!\n");
    }
}

APP_FEATURE_INIT(WifiConnectDemo);
