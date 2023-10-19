
# Makefile
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -I /usr/include/cppconn -L /usr/lib
TESTCFLAGS = -std=c++14 -Wall -I"Catch2/single_include"
CXXFLAGS = std=c++17
LDLIBS = -lmysqlcppconn
 
# ****************************************************
# Targets needed to bring the executable up to date
all: main test

main: main.o data_management.o utils.o
	$(CC) $(CFLAGS) -pthread -o main main.o data_management.o utils.o $(LDLIBS)

test: utils_test.o utils.o
	$(CC) $(TESTCFLAGS) -o test utils_test.o utils.o

utils_test.o:
	$(CC) $(TESTCFLAGS) -c utils_test.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

data_management.o: data_management.cpp
	$(CC) $(CFLAGS) -c data_management.cpp

utils.o: utils.cpp
	$(CC) $(CFLAGS) -c utils.cpp

clean:
	$(RM) main test main.o data_management.o utils.o utils_test.o
# main: main.cpp data_management.cpp
# 	$(CC) $(CFLAGS) -pthread -o main main.cpp $(LDLIBS)

# clean:
# 	$(RM) main
