CC = g++
CFLAGS = -O3 -Wall

SeaBattle: Main.o SBGame.o ClientServerConfig.o IPPortConfig.o
	$(CC) $(CFLAGS) -o SeaBattle Main.o SBGame.o ClientServerConfig.o IPPortConfig.o

SBGame.o: ./game/SBGame.hpp ./game/SBGame.cpp
	$(CC) $(CFLAGS) -c ./game/SBGame.cpp

ClientServerConfig.o: ./helpers/ClientServerConfig.hpp ./helpers/ClientServerConfig.cpp
	$(CC) $(CFLAGS) -c ./helpers/ClientServerConfig.cpp

IPPortConfig.o: ./helpers/IPPortConfig.hpp ./helpers/IPPortConfig.cpp
	$(CC) $(CFLAGS) -c ./helpers/IPPortConfig.cpp

clean:
	rm -r *.o SeaBattle
