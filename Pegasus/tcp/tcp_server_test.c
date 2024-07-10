/*
 * Copyright (C) 2021 HiHope Open Source Organization .
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
 *
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <iconv.h>

#include "net_demo.h"
#include "net_common.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "speaker.h"

#define DELAY_1S (1)
#define LED_INTERVAL_TIME_US 300000
#define LED_TASK_STACK_SIZE 4096
#define LED_TASK_PRIO 25
#define LED_TEST_GPIO 9 // for hispark_pegasus

char request[1024];
osMutexId_t requestMutex;

void LedTask(void)
{
    SpeakerInit();
    // printf("SpeakerInit ok!\n");
    SetVolume(20);
    // printf("SetVolume ok!\n");
    SetReader(Reader_XiaoYan);
    // printf("SetReader ok!\n");

    speech_text(request, GB2312);

    // while (GetChipStatus() != ChipStatus_Idle)
    // {
    usleep(10000);
    // printf("Wait for chip idle...\n");
    // }
    // printf("speech_text ok!\n");
}

void TcpServerTest(unsigned short port)
{
    ssize_t retval = 0;
    int backlog = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (sockfd < 0)
    {
        printf("socket creation failed, %d!\n", errno);
        return;
    }

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);              // 端口号，从主机字节序转为网络字节序
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 允许任意主机接入， 0.0.0.0

    retval = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // 绑定端口
    if (retval < 0)
    {
        printf("bind failed, %ld!\r\n", retval);
        close(sockfd);
        return;
    }
    printf("bind to port %hu success!\r\n", port);

    retval = listen(sockfd, backlog); // 开始监听
    if (retval < 0)
    {
        printf("listen failed!\r\n");
        close(sockfd);
        return;
    }
    printf("listen with %d backlog success!\r\n", backlog);

    while (1)
    {
        int connfd = -1;
        struct sockaddr_in clientAddr = {0};
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Add debug print before accept
        printf("Waiting for client connection...\n");

        connfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        // Add debug print after accept
        printf("After accept call\n");
        if (connfd < 0)
        {
            printf("accept failed, %d, %d\r\n", connfd, errno);
            continue; // Go back to waiting for the next connection
        }
        printf("accept success, connfd = %d!\r\n", connfd);
        printf("client addr info: host = %s, port = %hu\r\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        while (1)
        {
            // Clear request buffer
            memset(request, 0, sizeof(request));

            retval = recv(connfd, request, sizeof(request), 0);
            if (retval < 0)
            {
                printf("recv request failed, %ld!\r\n", retval);
                break;
            }
            else if (retval == 0)
            {
                printf("Client closed the connection.\r\n");
                break;
            }

            // printf("recv request{%s} from client done!\r\n", request);

            // Run LedTask after receiving the request
            LedTask();
            // Here you can add code to send a response if needed.
            // For example:
            // retval = send(connfd, request, strlen(request), 0);
            // if (retval <= 0) {
            //     printf("send response failed, %ld!\r\n", retval);
            //     break;
            // }
            // printf("send response{%s} to client done!\r\n", request);
        }

        close(connfd);
        printf("Connection closed, waiting for new connection...\r\n");
    }

    close(sockfd);
}
SERVER_TEST_DEMO(TcpServerTest);
