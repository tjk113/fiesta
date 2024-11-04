#pragma once

#include <stdbool.h>
#include <stdint.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#endif

#ifdef _WIN32
typedef enum {
    IP_Protocol_TCP = IPPROTO_TCP,
    IP_Protocol_UDP = IPPROTO_UDP
} IP_Protocol;
#endif

typedef struct {
    SOCKET socket;
    IP_Protocol protocol;
} Socket;

/* net */

uint32_t net_start();
void net_stop();