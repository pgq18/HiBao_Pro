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
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <memory.h>

#include "status_code.h"
#include "link_service.h"
#include "link_platform.h"
#include "link_agent.h"
#include "hisignalling.h"

#include "myapp1_utils.h"

#define NUM2 2
#define SBuf_LEN 33
#define PBUF_LEN 32
#define SEND_BUFF_LEN 22
#define NUM7 7

static const char *g_device_type = "Pegasus:Hi3861";
static volatile sig_atomic_t g_interrupted = 0;
unsigned int hisignallingPackageLen = 0;
unsigned char hisignallingSendBuf[HISIGNALLING_MSG_BUFF_LEN] = {0};
char send_buff[SEND_BUFF_LEN] = {0};
unsigned char light_on_off_buff[4] = {0, 2, 0, 3};
unsigned int led_flag = 0;

/*
 * @berf hisignalling Э�鷢����Ϣ
 * @param void *buf: �������ݻ���
 * @param unsigned int data_len: �������ݳ���
 *
 * @berf hisignalling protocol send msg
 * @param void *buf: send data buffer
 * @param unsigned int data_len: send data length
 */
unsigned int HisignallingMsgSend__(char *buf, unsigned int dataLen)
{
    unsigned int ret = 0;
    HisignallingProtocalType hisignallingMsg = {0};
    unsigned int writeDataLen = 0;

    hisignallingMsg.frameHeader[0]= 0xAA; /* Protocol head data 1 */
    hisignallingMsg.frameHeader[1]= 0x55; /* Protocol head data 2 */
    (void)memcpy_s(hisignallingMsg.hisignallingMsgBuf, dataLen, buf, dataLen);
    hisignallingMsg.endOfFrame = 0xff; /* Protocol tail data */

    hisignallingPackageLen = HisignallingDataPackage(&hisignallingMsg, dataLen, hisignallingSendBuf);
    if (!hisignallingPackageLen) {
        printf("hisignalling_data_package failed\r\n");
        return -1;
    }
    if (*hisignallingSendBuf == 0) {
        printf("hisignalling send buf is null!\r\n");
        return -1;
    }
    return 0;
}

/*
 * @berf �����źž��
 * @param int sig: �ź�
 *
 * @berf Receive signal handle
 * @param int sig: signal
 */
static void SigIntHandler(int sig)
{
    g_interrupted = 1;
}

/*
 * @berf �ͻ��˻�ȡ������״̬��ֵ
 * @param struct LinkService* ar: �������ṹ
 * @param const char* ���ԣ��ͻ�������
 * @param char* value: �ͻ���ֵ
 * @param int len: �ͻ���ֵ����
 *
 * @berf Client gets server status and value
 * @param struct LinkService* ar: server struct
 * @param const char* property: client property
 * @param char* value: client value
 * @param int len: client value len
 */
static int DoGET(struct LinkService* ar, const char* property, char* value, int len)
{
    printf("Receive property: %s(value=%s)\n", property, value);

    if (strcmp(property, "Status") == 0) {
    }
    /*
     * ���ȷ������ StatusOk,
     * �����κδ��󣬷��� StatusFailure
     *
     * if Ok return StatusOk,
     * Otherwise, any error, return StatusFailure
     */
    return StatusOk;
}

/*
 * @berf Client ��ʾ����״̬��ֵ
 * @param struct LinkService* ar: �������ṹ
 * @param const char* ���ԣ��ͻ�������
 * @param char* value: �ͻ���ֵ
 * @param int len: �ͻ���ֵ����
 *
 * @berf Client puts property status and value
 * @param struct LinkService* ar: server struct
 * @param const char* property: client property
 * @param char* value: client value
 * @param int len: client value len
 */
static int DoPUT(struct LinkService* ar, const char* property, char* value, int len)
{
    printf("Receive property: %s(value=%s)\n", property, value);
    /*
     * ���ȷ������ StatusOk,
     * �����κδ��󣬷��� StatusFailure
     *
     * if Ok return StatusOk,
     * Otherwise, any error, return StatusFailure
     */
    return StatusOk;
}

/*
 * wifi staģʽ�豸������Taurus:Hi3516DV300
 * The device type in wifi sta mode is "Taurus: Hi3516DV300"
 */
static const char* g_wifista_type = "Taurus:Hi3516DV300";
static const char* DoType(struct LinkService* ar)
{
    return g_wifista_type;
}

/*
 * @berf histreaming ������Ϣ
 * @param unsigned char *write_buffer: �������ݻ�����
 *
 * @berf histreaming send message
 * @param unsigned char *write_buffer: send data buff
 */
static unsigned int HistreamingMsgSendSwitch(unsigned char *write_buffer)
{
    SwitchTriggerSendMsg(write_buffer);
    return 0;
}

int HiSteramingServer(void)
{
    /*
     * ���������
     * Server start
     */
    if (signal(SIGINT, SigIntHandler) == 0) {
        return 0;
    }
    if (signal(SIGKILL, SigIntHandler) == 0) {
        return 0;
    }
    if (signal(SIGTERM, SigIntHandler) == 0) {
        return 0;
    }
}

char HiStreamingData(char *send_buff)
{
    if (led_flag % NUM2 == 0) {
        char send_buff1[SEND_BUFF_LEN] = "AA5500020003FF8ED2BEDF";
        (void)memcpy_s(send_buff, SEND_BUFF_LEN, send_buff1, SEND_BUFF_LEN);
    } else {
        char send_buff2[SEND_BUFF_LEN] = "AA5500020004FFC1932818";
        (void)memcpy_s(send_buff, SEND_BUFF_LEN, send_buff2, SEND_BUFF_LEN);
    }
    printf("out = %s\n", send_buff);
    if (led_flag >= 0xffffffff) {
        led_flag = 0;
    }
    led_flag++;
    return 0;
}

LinkPlatform *linkp = 0;
LinkAgent *linkAgentp = 0;
LinkService* doorbellp = 0;
int statusp = 0;

/*
 * @berf main����
 * @berf main function
 */
int HISTREAMING_TEST(void)
{
    // LinkPlatform *link = 0;
    // LinkAgent *linkAgent = 0;
    // LinkService* doorbell = 0;
    // int status = 0;

    /*
     * ���������
     * Server start
     */
    HiSteramingServer();

    /*
     * ����doorbell
     * Construct doorbell
     */
    doorbellp = (LinkService*)malloc(sizeof(LinkService));
    if (!doorbellp) {
        return NULL;
    }

    doorbellp->get    = DoGET;
    doorbellp->modify = DoPUT;
    doorbellp->type = DoType;

    linkp = LinkPlatformGet(); // start histreaming server link platform service
    if (!linkp) {
        LinkPlatformFree(linkp); // free histreaming server link platform
    }
    linkp->addLinkService(linkp, doorbellp, HISIGNALLING_MSG_HEADER_LEN); // add histreaming server link service
    linkp->open(linkp);

    /*
     * �����ͻ���
     * Client start
     */
    HiSteramingServer();
    linkp = LinkPlatformGet();
    if (!linkp) { 
        LinkPlatformFree(linkp);
    }

    linkAgentp = LinkAgentGet(linkp);
    if (!linkAgentp) {
        LinkAgentFree(linkAgentp);
    }

    linkp->setDebuglevel(linkp, NUM7); // set histreaming client debug level
    linkp->open(linkp);

    // while (!g_interrupted) {
        QueryResult *qres = linkAgentp->query(linkAgentp, g_device_type); // start histreaming client agent module
        while (qres == NULL && !g_interrupted) {
            qres = linkAgentp->query(linkAgentp, g_device_type); // client Waiting and server connection
        }
        if (qres != 0) { // Connect with the server
            LinkServiceAgent *serviceAgent = qres->at(qres, 0);
            if (serviceAgent != NULL) {
                HiStreamingData(send_buff);
                statusp = serviceAgent->modify(serviceAgent, "status", send_buff, SEND_BUFF_LEN);
            }
            if (StatusDeviceOffline == statusp) {
                linkp->discover(linkp, g_device_type);
            }
            QueryResultFree(qres); // free histreaming agent module
        }

        sleep(HISIGNALLING_MSG_HEADER_LEN);
    // }
    LinkPlatformFree(linkp);
    LinkAgentFree(linkAgentp);
    return 0;
}


int HISTREAMING_INIT(void)
{
    /*
     * ���������
     * Server start
     */
    HiSteramingServer();

    /*
     * ����doorbell
     * Construct doorbell
     */
    doorbellp = (LinkService*)malloc(sizeof(LinkService));
    if (!doorbellp) {
        return NULL;
    }

    doorbellp->get    = DoGET;
    doorbellp->modify = DoPUT;
    doorbellp->type = DoType;

    linkp = LinkPlatformGet(); // start histreaming server link platform service
    if (!linkp) {
        LinkPlatformFree(linkp); // free histreaming server link platform
    }
    linkp->addLinkService(linkp, doorbellp, HISIGNALLING_MSG_HEADER_LEN); // add histreaming server link service
    linkp->open(linkp);

    /*
     * �����ͻ���
     * Client start
     */
    HiSteramingServer();
    linkp = LinkPlatformGet();
    if (!linkp) { 
        LinkPlatformFree(linkp);
    }

    linkAgentp = LinkAgentGet(linkp);
    if (!linkAgentp) {
        LinkAgentFree(linkAgentp);
    }

    linkp->setDebuglevel(linkp, NUM7); // set histreaming client debug level
    linkp->open(linkp);

    QueryResult *qres = linkAgentp->query(linkAgentp, g_device_type); // start histreaming client agent module
    while (qres == NULL && !g_interrupted) {
        qres = linkAgentp->query(linkAgentp, g_device_type); // client Waiting and server connection
    }

    return 0;
}

int HISTREAMING_SEND(void)
{
    QueryResult *qres = linkAgentp->query(linkAgentp, g_device_type); // start histreaming client agent module
    while (qres == NULL && !g_interrupted) {
        qres = linkAgentp->query(linkAgentp, g_device_type); // client Waiting and server connection
    }
    // printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    if (qres != 0) { // Connect with the server
        // printf("====================================================================================");
        LinkServiceAgent *serviceAgent = qres->at(qres, 0);
        if (serviceAgent != NULL) {
            HiStreamingData(send_buff);
            statusp = serviceAgent->modify(serviceAgent, "status", send_buff, SEND_BUFF_LEN);
        }
        if (StatusDeviceOffline == statusp) {
            linkp->discover(linkp, g_device_type);
        }
        QueryResultFree(qres); // free histreaming agent module
    }

    sleep(HISIGNALLING_MSG_HEADER_LEN);

    return 0;
}

int HISTREAMING_CLOSE(void) {
    LinkPlatformFree(linkp);
    LinkAgentFree(linkAgentp);
    return 0;
}


// /*
//  * @berf main����
//  * @berf main function
//  */
// int main(int argc, char** argv)
// {
//     LinkPlatform *link = 0;
//     LinkAgent *linkAgent = 0;
//     LinkService* doorbell = 0;
//     int status = 0;

//     /*
//      * ���������
//      * Server start
//      */
//     HiSteramingServer();

//     /*
//      * ����doorbell
//      * Construct doorbell
//      */
//     doorbell = (LinkService*)malloc(sizeof(LinkService));
//     if (!doorbell) {
//         return NULL;
//     }

//     doorbell->get    = DoGET;
//     doorbell->modify = DoPUT;
//     doorbell->type = DoType;

//     link = LinkPlatformGet(); // start histreaming server link platform service
//     if (!link) {
//         LinkPlatformFree(link); // free histreaming server link platform
//     }
//     link->addLinkService(link, doorbell, HISIGNALLING_MSG_HEADER_LEN); // add histreaming server link service
//     link->open(link);

//     /*
//      * �����ͻ���
//      * Client start
//      */
//     HiSteramingServer();
//     link = LinkPlatformGet();
//     if (!link) {
//         LinkPlatformFree(link);
//     }

//     linkAgent = LinkAgentGet(link);
//     if (!linkAgent) {
//         LinkAgentFree(linkAgent);
//     }

//     link->setDebuglevel(link, NUM7); // set histreaming client debug level
//     link->open(link);

//     while (!g_interrupted) {
//         QueryResult *qres = linkAgent->query(linkAgent, g_device_type); // start histreaming client agent module
//         while (qres == NULL && !g_interrupted) {
//             qres = linkAgent->query(linkAgent, g_device_type); // client Waiting and server connection
//         }
//         if (qres != 0) { // Connect with the server
//             LinkServiceAgent *serviceAgent = qres->at(qres, 0);
//             if (serviceAgent != NULL) {
//                 HiStreamingData(send_buff);
//                 status = serviceAgent->modify(serviceAgent, "status", send_buff, SEND_BUFF_LEN);
//             }
//             if (StatusDeviceOffline == status) {
//                 link->discover(link, g_device_type);
//             }
//             QueryResultFree(qres); // free histreaming agent module
//         }

//         sleep(HISIGNALLING_MSG_HEADER_LEN);
//     }
//     LinkPlatformFree(link);
//     LinkAgentFree(linkAgent);
//     return 0;
// }