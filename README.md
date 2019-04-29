# Simple-TCP-echo-server
This is simple TCP multi-threading echo server.

### Building
``` bash
# create working directory
mkdir server

# build
make server

# go to working directory
cd server
```

### Running
``` bash
# run server
./server [options]
# List of options:
-w || --workers count   Number of threads in pool. The default is set with std::thread::hardware_concurrency()\n
-p || --port    port    Set the server port. The default is 8080\n
      --host    host    Set the server host. The default is 127.0.0.1\n
-l || --logs    0       Disable writing logs. The default is enable and writing to logs.txt.\n
-h || --help            Help page.
```

