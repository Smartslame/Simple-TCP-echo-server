#pragma once

void parseArgs(int argc, char *argv[], ServerTCP &server) {
    if( argc == 1) {
        return;
    }
    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

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
            if(i + 1 < argc) {
                server.setWorkersCount(std::stoi(argv[++i]));
                continue;
            }
        }

        if (arg == "-p" || arg == "--port") {
            if(i + 1 < argc) {
                server.setPort(std::stoi(argv[++i]));
                ++i;
                continue;
            }
        }

        if (arg == "--host") {
            if(i + 1 < argc) {
                server.setHost(argv[++i]);
                ++i;
                continue;
            }
        }

        if (arg == "-l" || arg == "--logs") {
            if(i + 1 < argc) {
                if(std::stoi(argv[++i]) == 0 || argv[++i]){
                    server.setLoggerStatus(false);
                }
                ++i;
                continue;
            }
        }

        std::cout << "Incorrect arguments, please type -h or --help to see help page." << std::endl;
        exit(1);

    }
}