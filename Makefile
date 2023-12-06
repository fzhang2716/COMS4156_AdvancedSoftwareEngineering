
# Makefile
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -I /usr/include/cppconn -I jwt-cpp/include -I /usr/include/jsoncpp -I/usr/include/python3.8
TESTCFLAGS = -std=c++14 -Wall -I"Catch2/single_include" -I /usr/include/jsoncpp -I jwt-cpp/include
ANALYZETESTFLAGS = -std=c++14 -Wall -I"Catch2/single_include" -I/usr/include/python3.8
LDFLAGS = -L /usr/lib -L jwt-cpp/build -L /usr/include/jsoncpp
CXXFLAGS = std=c++17
LDLIBS = -lmysqlcppconn -lssl -lcrypto -lcurl -ljsoncpp -lpthread -lpython3.8
ANALYZELDLIBS = -lpython3.8 -lcurl -lssl -lcrypto
 
# ****************************************************
# Targets needed to bring the executable up to date
all: main utils_test integration_test analyze_data_test

main: main.o data_management.o utils.o
	$(CC) $(CFLAGS) -pthread -o main main.o data_management.o utils.o $(LDFLAGS) $(LDLIBS)


utils_test: utils_test.o utils.o
	$(CC) $(TESTCFLAGS) -o utils_test utils_test.o utils.o

integration_test: utils.o integration_test.o data_management.o
	$(CC) $(TESTCFLAGS) -o integration_test utils.o integration_test.o data_management.o $(LDFLAGS) $(LDLIBS)

analyze_data_test: analyze_data_test.o
	$(CC) $(ANALYZETESTFLAGS) -o analyze_data_test analyze_data_test.o $(ANALYZELDLIBS)

utils_test.o:
	$(CC) $(TESTCFLAGS) -c utils_test.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

data_management.o: data_management.cpp
	$(CC) $(CFLAGS) -c data_management.cpp

utils.o: utils.cpp
	$(CC) $(CFLAGS) -c utils.cpp

integration_test.o: integration_test.cpp
	$(CC) $(TESTCFLAGS) -c integration_test.cpp

analyze_data.o:
	$(CC) $(CFLAGS) -c analyze_data.cpp

analyze_data_test.o:
	$(CC) $(ANALYZETESTFLAGS) -c analyze_data_test.cpp

clean:
	$(RM) main test integration_test utils_test analyze_data_test main.o data_management.o utils.o utils_test.o integration_test.o analyze_data.o analyze_data_test.o
# main: main.cpp data_management.cpp
# 	$(CC) $(CFLAGS) -pthread -o main main.cpp $(LDLIBS)

# clean:
# 	$(RM) main
