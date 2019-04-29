#include "serverTCP.h"
#include "argumentParser.h"

int main(int argc, char *argv[]) {
    ServerTCP server;
    parseArgs(argc,argv,server);
    server.run();
    return 0;
}
