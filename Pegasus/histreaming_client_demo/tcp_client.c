#include <stdio.h>
// #include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "net_demo.h"
#include "net_common.h"

static char request[] = "Hello";
static char response[128] = "你好";

void TcpClientTest(const char* host, unsigned short port)
{
    printf("1\n");
    ssize_t retval = 0;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (sockfd < 0) {
        printf("socket creation failed!\r\n");
        return;
    }
    printf("2\n");

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;    // AF_INET表示IPv4协议
    serverAddr.sin_port = htons(port);  // 端口号，从主机字节序转为网络字节序
    // 将主机IP地址从“点分十进制”字符串 转化为 标准格式（32位整数）
    printf("3\n");

    if (inet_pton(AF_INET, host, &serverAddr.sin_addr) <= 0) {
        printf("inet_pton failed!\r\n");
        goto do_cleanup;
    }
    printf("4\n");

    // 尝试和目标主机建立连接，连接成功会返回0 ，失败返回 -1
    int connect_result = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    printf("number: %d\n", connect_result);
    if (connect_result < 0) {
        perror("connect");
        printf("connect failed!\r\n");
        goto do_cleanup;
    }
    printf("connect to server %s success!\r\n", host);
    printf("5\n");

    // 建立连接成功之后，这个TCP socket描述符 —— sockfd 就具有了 “连接状态”，
    // 发送、接收 对端都是 connect 参数指定的目标主机和端口
    retval = send(sockfd, request, sizeof(request), 0);
    if (retval < 0) {
        perror("send");
        printf("send request failed!\r\n");
        goto do_cleanup;
    }
    printf("send request{%s} %ld to server done!\r\n", request, retval);
    printf("6\n");

    retval = recv(sockfd, response, sizeof(response) - 1, 0); // Leave space for null terminator
    if (retval <= 0) {
        perror("recv");
        printf("recv response from server failed or done, %ld!\r\n", retval);
        goto do_cleanup;
    }
    printf("7\n");

    response[retval] = '\0';
    printf("recv response{%s} %ld from server done!\r\n", response, retval);

do_cleanup:
    printf("do_cleanup...\r\n");
    close(sockfd);
}
