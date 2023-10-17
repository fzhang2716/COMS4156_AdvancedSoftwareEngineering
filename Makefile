
# Makefile
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -I /usr/include/cppconn -L /usr/lib
CXXFLAGS = std=c++17
LDLIBS = -lmysqlcppconn
 
# ****************************************************
# Targets needed to bring the executable up to date
all: main

main: main.cpp
	$(CC) $(CFLAGS) -pthread -o main main.cpp $(LDLIBS)

clean:
	$(RM) main