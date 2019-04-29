#include "serverTCP.h"

int main(int argc, char *argv[]) {
    ServerTCP server(argc,argv);
    server.run();
    return 0;
}