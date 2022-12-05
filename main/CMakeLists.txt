#for KNX HTTP Gateway

#replace /home/golpe/knx/url_gateway/ by the path where you have cloned the knxGW project

cmake_minimum_required(VERSION 3.7.2)	
project (KnxHttpGateway CXX)

# Create variables that holds the path to our libraries
set (HTTP_PATH /home/golpe/knx/url_gateway/iot_knx-gateway/libhttp/libhttp/build)
set (KNX_MNG_PATH /home/golpe/knx/url_gateway/iot_knx-gateway/libknx-manage/libknx/manage/build)
set (KNX_CTRL_PATH /home/golpe/knx/url_gateway/iot_knx-gateway/libknx-control/libknx/control/build)

#add headers
include_directories (
    /home/golpe/knx/url_gateway/iot_knx-gateway/libhttp/libhttp/
    /home/golpe/knx/url_gateway/iot_knx-gateway/libknx-manage/libknx/manage/
    /home/golpe/knx/url_gateway/iot_knx-gateway/libknx-control/libknx/control/
)

# find library objects
find_library(HTTP_LIB http HINTS ${HTTP_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(KNX_MNG_LIB knx-manage HINTS ${KNX_MNG_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(KNX_CTRL_LIB knx-control HINTS ${KNX_CTRL_PATH} NO_CMAKE_FIND_ROOT_PATH)

# Alert the user if we do not find them
if(NOT HTTP_LIB)
    message(FATAL_ERROR “http library not found”)
elseif(NOT KNX_MNG_LIB)
    message(FATAL_ERROR “KnxManage library not found”)
elseif(NOT KNX_CTRL_LIB)
    message(FATAL_ERROR “KnxControl library not found”)
endif()

# Add main file to our executable output
add_executable(KnxHttpGateway
    /home/golpe/knx/url_gateway/iot_knx-gateway/main/main.cpp
)
target_include_directories(KnxHttpGateway
	PUBLIC 
	usr/include
	)

# Link the pre-compiled libraries and pthread
target_link_libraries(KnxHttpGateway
    ${HTTP_LIB}
    ${KNX_MNG_LIB}
    ${KNX_CTRL_LIB}
	pthread	
)