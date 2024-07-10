#ifndef TCP_CLIENT_TEST_H
#define TCP_CLIENT_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>

void TcpClientTest(const char* host, unsigned short port);

#ifdef __cplusplus
}
#endif

#endif /* TCP_CLIENT_TEST_H */
