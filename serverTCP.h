#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "threadPool.h"
#include "logger.h"

#define PORT                8080
#define HOST                "127.0.0.1"
#define BUFFER_SIZE         1024

class ServerTCP {
public:
    ServerTCP(int argc, char **argv);

    ~ServerTCP();

    void                    run();
    void                    setPort(size_t port_num);
    void                    setHost(const char *host_name);
    void                    setWorkersCount(size_t workers);
    void                    setLoggerStatus(bool activated);

private:
    int                     masterSocket, newConnectionSocket;
    std::size_t             workersCount;
    struct ::sockaddr_in    serverAddr, clientAddr;
    ::socklen_t             clientLen;
    Logger                  logger;
    bool                    isRun;

    void                    createSocket();
    void                    bindSocket();
    bool                    getNewConnection();
    void                    handleConnection(int fd);
    std::thread             runStopper();
    void                    parseArgs(int argc, char *argv[]);
};
