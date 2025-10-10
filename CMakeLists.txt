cmake_minimum_required(VERSION 3.4.1)

project(rknn4j VERSION 1.0.1 DESCRIPTION "rknn4j NPU bindings")
set(CMAKE_SOURCE_DIR "/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SYSTEM_NAME "Linux")

# skip 3rd-party lib dependencies
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-shlib-undefined")

# install target and libraries
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/rknn4j_${CMAKE_SYSTEM_NAME})

set(CMAKE_SKIP_INSTALL_RPATH FALSE)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

set(RKNN_API_PATH ${CMAKE_SOURCE_DIR}/../runtime/RK3588/${CMAKE_SYSTEM_NAME}/librknn_api)

set(LIB_ARCH aarch64)
set(RKNN_RT_LIB ${RKNN_API_PATH}/${LIB_ARCH}/librknnrt.so)

include_directories(${RKNN_API_PATH}/include)
include_directories(/usr/share/java/graalvm-jdk-25+20.1/include)
include_directories(/usr/share/java/graalvm-jdk-25+20.1/include/Linux)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)

# opencv

set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/3rdparty/opencv/opencv-linux-aarch64/share/OpenCV)
  
# Find JNI headers
find_package(JNI REQUIRED)

# Find the RKNN runtime library
# Adjust the HINTS path if librknnrt.so lives elsewhere
find_library(RKNNRT_LIB rknnrt
    HINTS /usr/lib /usr/lib/aarch64-linux-gnu /usr/lib/jni)

if(NOT RKNNRT_LIB)
    message(FATAL_ERROR "Could not find librknnrt.so")
endif()

find_package(OpenCV REQUIRED)

#rga
  set(RGA_PATH ${CMAKE_SOURCE_DIR}/3rdparty/rga/RK3588)
  set(LIB_ARCH aarch64)
  set(RGA_LIB ${RGA_PATH}/lib/Linux//${LIB_ARCH}/librga.so)
include_directories( ${RGA_PATH}/include)

#set(CMAKE_INSTALL_RPATH "lib")

# rknn4j
include_directories( ${CMAKE_SOURCE_DIR}/include)

add_library(rknn4j SHARED 
        src/com_neocoretechs_rknn4j_rknpu2.cc
        src/postprocess.cc
)

target_include_directories(rknn4j PRIVATE ${JNI_INCLUDE_DIRS})
set_target_properties(rknn4j PROPERTIES 
        VERSION ${PROJECT_VERSION}
        SOVERSION 1 
        PUBLIC_HEADER librknn4j.h)

#target_link_libraries(rknn4j
#  ${RKNN_RT_LIB}
#  ${RGA_LIB}
#  ${OpenCV_LIBS}
#)

target_link_libraries(rknn4j PRIVATE ${RKNNRT_LIB})

# install target and libraries
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/rknn4j_${CMAKE_SYSTEM_NAME})
install(TARGETS rknn4j LIBRARY DESTINATION lib 
        PUBLIC_HEADER DESTINATION ./)

install(PROGRAMS ${RKNN_RT_LIB} DESTINATION lib)
install(PROGRAMS ${RGA_LIB} DESTINATION lib)
install(DIRECTORY model DESTINATION ./)
