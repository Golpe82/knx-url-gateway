#for KNX manage class

#replace /home/golpe/ by the path where you have cloned the knxGW project

cmake_minimum_required(VERSION 3.7.2)	
project (knx-manage CXX)

# make this output a static library 
add_library (knx-manage 
    STATIC 
    /usr/local/iot_knx-gateway/libknx-manage/libknx/manage/KnxManage.cpp 
)

#include header files
target_include_directories (knx-manage 
    PUBLIC 
    /usr/local/iot_knx-gateway/libknx-manage/libknx/manage/
)