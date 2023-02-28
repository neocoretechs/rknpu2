cmake_minimum_required(VERSION 3.4.1)

project(imageinst VERSION 1.0.1 DESCRIPTION "rknn4j Image Processing bindings")
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

set(RKNN_API_PATH ${CMAKE_SOURCE_DIR}/../../runtime/RK3588/${CMAKE_SYSTEM_NAME}/librknn_api)

set(LIB_ARCH aarch64)
set(RKNN_RT_LIB ${RKNN_API_PATH}/${LIB_ARCH}/librknnrt.so)

include_directories(${RKNN_API_PATH}/include)
include_directories(/usr/share/java/jdk1.8.0_161/include)
include_directories(/usr/share/java/jdk1.8.0_161/include/Linux)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/opencv/opencv-linux-aarch64/include)
# opencv

set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/3rdparty/opencv/opencv-linux-aarch64/share/OpenCV)
 
find_package(OpenCV REQUIRED)

#rga
  set(RGA_PATH ${CMAKE_SOURCE_DIR}/3rdparty/rga/RK3588)
  set(LIB_ARCH aarch64)
  set(RGA_LIB ${RGA_PATH}/lib/Linux//${LIB_ARCH}/librga.so)
include_directories( ${RGA_PATH}/include)

#set(CMAKE_INSTALL_RPATH "lib")

# imageinst
include_directories( ${CMAKE_SOURCE_DIR}/include)

add_library(imageinst SHARED 
        ../src/com_neocoretechs_rknn4j_image_Instance.cc
)

set_target_properties(imageinst PROPERTIES 
        VERSION ${PROJECT_VERSION}
        SOVERSION 1 
        PUBLIC_HEADER libimageinst.h)

target_link_libraries(imageinst PRIVATE 
  ${RGA_LIB}
  ${OpenCV_LIBS} opencv_core opencv_imgproc
)


# install target and libraries
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/imageinst_${CMAKE_SYSTEM_NAME})
install(TARGETS imageinst LIBRARY DESTINATION lib 
        PUBLIC_HEADER DESTINATION ./)

#install(PROGRAMS ${RKNN_RT_LIB} DESTINATION lib)
install(PROGRAMS ${RGA_LIB} DESTINATION lib)
install(DIRECTORY model DESTINATION ./)
