#for KNX control class

#replace /home/golpe/ by the path where you have cloned the knxGW project

cmake_minimum_required(VERSION 3.7.2)	
project (knx-control CXX)

# make this output a static library 
add_library (knx-control
    STATIC 
    /usr/local/iot_knx-gateway/libknx-control/libknx/control/KnxControl.cpp 
)

# be sure to include the current source directory for header files
target_include_directories (knx-control
    PUBLIC 
    /usr/local/iot_knx-gateway/libknx-control/libknx/control/
)