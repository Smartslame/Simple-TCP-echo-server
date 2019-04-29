DEST = server/

main.o: main.cpp 
		g++ -std=c++11 -c -o $(DEST)main.o main.cpp
threadPool.o: $(PREF)threadPool.cpp
		g++ -std=c++11 -c -o $(DEST)threadPool.o threadPool.cpp
logger.o: $(PREF)logger.cpp
		g++ -std=c++11 -c -o $(DEST)logger.o logger.cpp
serverTCP.o: $(PREF)serverTCP.cpp
		g++ -std=c++11 -c -o $(DEST)serverTCP.o serverTCP.cpp


server:	main.o logger.o serverTCP.o threadPool.o
		g++ -std=c++11 -o $(DEST)server $(DEST)main.o $(DEST)logger.o $(DEST)serverTCP.o $(DEST)threadPool.o

clean:
	rm -rf $(DEST)*.o $(DEST)server