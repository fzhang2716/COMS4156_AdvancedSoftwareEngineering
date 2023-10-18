
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

main: main.o data_management.o
	$(CC) $(CFLAGS) -pthread -o main main.o data_management.o $(LDLIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

data_management.o: data_management.cpp
	$(CC) $(CFLAGS) -c data_management.cpp

clean:
	$(RM) main main.o data_management.o
# main: main.cpp data_management.cpp
# 	$(CC) $(CFLAGS) -pthread -o main main.cpp $(LDLIBS)

# clean:
# 	$(RM) main