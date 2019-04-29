#include "logger.h"

void Logger::setStatus(bool activated) {
    isActivated = activated;
}

void Logger::log(const std::string &msg, int fd, const struct ::sockaddr_in &address) {
    if(isActivated) {
        auto now    = std::chrono::system_clock::now();
        auto now_c  = std::chrono::system_clock::to_time_t(now);

        out << std::put_time(std::localtime(&now_c), "%c")
            << ": "
            << msg
            << ". Socket fd: "
            << fd
            << ", ip: "
            << inet_ntoa(address.sin_addr)
            << ", port: "
            << ntohs(address.sin_port)
            << std::endl;
    }
}

void Logger::logError(const char *msg, bool needExit) {
    ::perror(msg);

    if (isActivated) {
        auto now    = std::chrono::system_clock::now();
        auto now_c  = std::chrono::system_clock::to_time_t(now);
        out << std::put_time(std::localtime(&now_c), "%c")
            << ": "
            << msg
            << ": "
            << strerror(errno)
            << std::endl;
    }

    if (needExit) {
        exit(1);
    }
}

std::string Logger::deleteLastChar(const std::string &buffer, int buffer_len, char c) {
    return ((buffer[buffer_len-1] == c)?std::string(buffer).erase(buffer_len - 1):std::string(buffer));
}


