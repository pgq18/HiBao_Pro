#include "net_demo.h"
#include "net_common.h"

#include "tcp.h"

int sockfd;
ssize_t retval;
static char request[] = "Hello";
static char response[128] = "";

void TCPClientConnect(const char* host, unsigned short port)
{
    retval = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;    // AF_INET表示IPv4协议
    serverAddr.sin_port = htons(port);  // 端口号，从主机字节序转为网络字节序
    // 将主机IP地址从“点分十进制”字符串 转化为 标准格式（32位整数）
    if (inet_pton(AF_INET, host, &serverAddr.sin_addr) <= 0) {
        printf("inet_pton failed!\r\n");
        // goto do_cleanup;
    }

    // 尝试和目标主机建立连接，连接成功会返回0 ，失败返回 -1
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("connect failed!\r\n");
        // goto do_cleanup;
    }
    printf("connect to server %s success!\r\n", host);
}

void TCPClientSent(void)
{
    retval = send(sockfd, request, sizeof(request), 0);
    if (retval < 0) {
        printf("send request failed!\r\n");
        // goto do_cleanup;
    }
    printf("send request{%s} %ld to server done!\r\n", request, retval);

    // retval = recv(sockfd, &response, sizeof(response), 0);
    // if (retval <= 0) {
    //     printf("send response from server failed or done, %ld!\r\n", retval);
    //     // goto do_cleanup;
    // }
    // response[retval] = '\0';
    // printf("recv response{%s} %ld from server done!\r\n", response, retval);
}