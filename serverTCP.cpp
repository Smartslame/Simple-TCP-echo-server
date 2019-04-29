#include "serverTCP.h"

ServerTCP::ServerTCP(int argc, char **argv) :
        masterSocket(-1),
        newConnectionSocket(-1),
        workersCount(std::thread::hardware_concurrency()),
        isRun(true) {

    ::bzero((char *) &serverAddr, sizeof(serverAddr));
    ::bzero((char *) &clientAddr, sizeof(clientAddr));

    serverAddr.sin_family          = AF_INET;
    serverAddr.sin_addr.s_addr     = inet_addr(HOST);
    serverAddr.sin_port            = htons(PORT);

    parseArgs(argc, argv);
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

    std::thread stopper = runStopper();

    while (isRun) {
        if (getNewConnection()) {
            auto func = std::bind(&ServerTCP::handleConnection, this, newConnectionSocket);
            pool.addJob(func);
        }
    }

    stopper.join();
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
    socklen_t               addressLen;

    getpeername(fd, (struct sockaddr *) &address, (socklen_t *) &addressLen);

    while (isRun) {
        int     returnCode;
        char    buffer[BUFFER_SIZE];
        int     bufferLen;

        bzero(buffer, BUFFER_SIZE);

        returnCode = ::read(fd, buffer, BUFFER_SIZE);

        if (returnCode < 0) {
            logger.logError("ERROR reading from socket");
            return;
        }

        bufferLen = strlen(buffer);

        logger.log("Received message:<" + logger.deleteLastChar(buffer, bufferLen, '\n') + ">", fd, address );

        if (returnCode == 0) {
            logger.log("Client disconnected", fd, address );
            break;
        }

        returnCode = ::write(fd, buffer, bufferLen);

        if (returnCode < 0) {
            logger.logError("ERROR writing to socket");
            return;
        }

        logger.log("Send message:<" + logger.deleteLastChar(buffer, bufferLen, '\n') + ">", fd, address );

    }

    if (fd > 0) {
        ::close(fd);
    }
}

std::thread ServerTCP::runStopper() {
    return std::thread ([this](){
        while(1) {
            std::string s = "";
            std::cin >> s;
            if (s == "q" || s == "quit") {
                isRun = false;
                ::close(masterSocket);
                break;
            }
        }
    });
}

void ServerTCP::parseArgs(int argc, char **argv) {
    if( argc == 1) {
        return;
    }
    for(int argNum = 1; argNum < argc; ++argNum) {
        std::string arg = argv[argNum];

        if (arg == "-h" || arg == "--help") {

            std::cout   << "This is simple echo TCP server using thread pool.\n"
                        << "Usage: ./serverTCP [options]\n"
                        << "Options are:\n"
                        << "-w || --workers count   Number of threads in pool. The default is set with std::thread::hardware_concurrency()\n"
                        << "-p || --port    port    Set the server port. The default is 8080\n"
                        << "      --host    host    Set the server host. The default is 127.0.0.1\n"
                        << "-l || --logs    0       Disable writing logs. The default is enable and writing to logs.txt.\n"
                        << "-h || --help            This help page.\n";
            exit(0);
        }

        if (arg == "-w" || arg == "--workers") {
            if(argNum + 1 < argc) {
                setWorkersCount(std::stoi(argv[++argNum]));
                continue;
            }
        }

        if (arg == "-p" || arg == "--port") {
            if(argNum + 1 < argc) {
                setPort(std::stoi(argv[++argNum]));
                continue;
            }
        }

        if (arg == "--host") {
            if(argNum + 1 < argc) {
                setHost(argv[++argNum]);
                continue;
            }
        }

        if (arg == "-l" || arg == "--logs") {
            if(argNum + 1 < argc) {
                if(std::stoi(argv[++argNum]) == 0 || argv[++argNum]){
                    setLoggerStatus(false);
                }
                continue;
            }
        }

        std::cout << "Incorrect arguments, please type -h or --help to see help page." << std::endl;
        exit(1);

    }
}
