#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <chrono>
#include <iomanip>



class Logger {
public:
    Logger(): isActivated(true) {
        out.open("logs.txt", std::ios::out);
    }

    ~Logger() {
        out.close();
    }

    void            log(const std::string &msg, int fd, const struct ::sockaddr_in &serverAddr);
    void            logError(const char *msg, bool needExit = false);
    void            setStatus(bool activated);
    std::string     deleteLastChar(const std::string &buffer, int buffer_len, char c);
private:
    bool            isActivated;
    std::ofstream   out;
};
