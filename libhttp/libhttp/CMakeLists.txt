#for KNX http class

#replace /home/golpe/ by the path where you have cloned the knxGW project

cmake_minimum_required(VERSION 3.7.2)	
project (http CXX)

# make this output a static library
add_library (http 
    STATIC 
    /usr/local/iot_knx-gateway/libhttp/libhttp/http.cpp 
)

#include header files
target_include_directories (http 
    PUBLIC 
    /usr/local/iot_knx-gateway/libhttp/libhttp
    /usr/local/iot_knx-gateway/libhttp/thirdparty
)

