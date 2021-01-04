
#toolchain file to specify which compilers, linkers, and libraries are used to build the project

#for crosscompile, clone first the Raspberry pi tools from here https://github.com/raspberrypi/tools
#replace /home/golpe/knxGW/project/gateway/tools/ by the path where you have the Raspberry tools

#Define your host system
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

#Define cross compiler locations
SET(CMAKE_C_COMPILER /home/golpe/knxGW/project/gateway/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-cc)
SET(CMAKE_CXX_COMPILER /home/golpe/knxGW/project/gateway/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-c++)

# Define the sysroot path for the RaspberryPi distribution in our tools folder
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search for libraries and headers in the target directories only
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_definitions(-Wall -std=c++11)
