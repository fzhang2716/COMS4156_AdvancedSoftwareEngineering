
# Makefile
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -I /usr/include/cppconn -L /usr/lib
CXXFLAGS = std=c++17
LDLIBS = -lmysqlcppconn
 
# ****************************************************
# Targets needed to bring the executable up to date
all: helloworld

helloworld: helloworld.cpp
	$(CC) $(CFLAGS) -pthread -o helloworld helloworld.cpp $(LDLIBS)

clean:
	$(RM) helloworld