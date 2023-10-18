# COMS4156_AdvancedSoftwareEngineering
Fakeit:
1. "git clone https://github.com/Microsoft/vcpkg.git"
2. "cd vcpkg"
3. "./bootstrap-vcpkg.sh"
4. "./vcpkg integrate install"
5. "./vcpkg install fakeit"
(Add '-I"vcpkg/packages/fakeit_x64-linux/include/standalone"' when compiling)

Install crow:
1. "git clone https://github.com/CrowCpp/Crow.git"
2. navigate to Crow && "mkdir build"
3. navigate to build
4. "sudo apt install libasio-dev"
5. "cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF"
6. make install


Install MySQL Connector for C++:
1. "sudo apt update"
2. "sudo apt-get install libmysqlcppconn-dev"


Run Makefile:
"make"
