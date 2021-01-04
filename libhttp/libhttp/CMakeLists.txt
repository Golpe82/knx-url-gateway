#for KNX http class

#replace /home/golpe/ by the path where you have cloned the knxGW project

cmake_minimum_required(VERSION 3.7.2)	
project (urlGateway CXX)

# make this output a static library
add_library (http 
    STATIC 
    /home/golpe/knxGW/project/gateway/libhttp/libhttp/http.cpp 
)

#include header files
target_include_directories (http 
    PUBLIC 
    /home/golpe/knxGW/project/gateway/libhttp/libhttp
    /home/golpe/knxGW/project/gateway/libhttp/thirdparty
)

