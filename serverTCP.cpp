#include "serverTCP.h"

ServerTCP::ServerTCP() :
                            masterSocket(-1),
                            newConnectionSocket(-1),
                            workersCount(std::thread::hardware_concurrency()),
                            isRun(true) {

    ::bzero((char *) &serverAddr, sizeof(serverAddr));
    ::bzero((char *) &clientAddr, sizeof(clientAddr));

    serverAddr.sin_family          = AF_INET;
    serverAddr.sin_addr.s_addr     = inet_addr(HOST);
    serverAddr.sin_port            = htons(PORT);
}

ServerTCP::~ServerTCP() {
    ::close(masterSocket);
    logger.log("Server stopped", masterSocket, serverAddr);
}

void ServerTCP::run() {
    ThreadPool pool(workersCount);

    createSocket();
    bindSocket();

    logger.log("Server started", masterSocket, serverAddr);

    readyToStop();

    while (isRun) {
        if (getNewConnection()) {
            auto func = std::bind(&ServerTCP::handleConnection, this, newConnectionSocket);
            pool.addJob(func);
        }
    }
}

void ServerTCP::setPort(size_t port_num) {
    serverAddr.sin_port = htons(port_num);
}

void ServerTCP::setHost(const char *host_name) {
    serverAddr.sin_addr.s_addr = inet_addr(host_name);
}

void ServerTCP::setWorkersCount(size_t workers) {
    workersCount = workers;
}

void ServerTCP::setLoggerStatus(bool activated){
    logger.setStatus(activated);
}

void ServerTCP::createSocket() {
    masterSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (masterSocket < 0)
        logger.logError("ERROR opening socket", true);
    int reuse = 1;
    if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        logger.logError("setsockopt(SO_REUSEADDR) failed");
}

void ServerTCP::bindSocket() {
    if (::bind(masterSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
        logger.logError("ERROR on binding", true);
}

bool ServerTCP::getNewConnection() {
    if (::listen(masterSocket, 1000) != 0) {
        logger.logError("ERROR on listening");
    }
    clientLen              = sizeof(clientAddr);
    newConnectionSocket    = ::accept(masterSocket, (struct sockaddr *) &clientAddr, &clientLen);

    if (newConnectionSocket < 0) {
        logger.logError("ERROR on accept");
        return false;
    }

    logger.log("New client connected", newConnectionSocket, clientAddr);

    return true;
}

void ServerTCP::handleConnection(int fd) {
    struct sockaddr_in      address;
    socklen_t               address_len;

    getpeername(fd, (struct sockaddr *) &address, (socklen_t *) &address_len);

    while (isRun) {
        int     n;
        char    buffer[BUFFER_SIZE];
        int     buffer_len;
        bzero(buffer, BUFFER_SIZE);

        n = ::read(fd, buffer, BUFFER_SIZE);

        if (n < 0) {
            logger.logError("ERROR reading from socket");
            return;
        }

        buffer_len = strlen(buffer);

        logger.log("Received message:<" + logger.deleteLastChar(buffer, buffer_len, '\n') + ">", fd, address );

        if (n == 0) {
            logger.log("Client disconnected", fd, address );
            break;
        }

        n = ::write(fd, buffer, buffer_len);

        if (n < 0) {
            logger.logError("ERROR writing to socket");
            return;
        }

        logger.log("Send message:<" + logger.deleteLastChar(buffer, buffer_len, '\n') + ">", fd, address );

    }

    if (fd > 0) {
        ::close(fd);
    }
}

void ServerTCP::readyToStop() {
    std::thread exiter([this](){
        while(1) {
            std::string s= "";
            std::cin >> s;
            if (s == "q" || s == "quit") {
                isRun = false;
                ::close(masterSocket);
                break;
            }
        }
    });
    exiter.detach();
}
