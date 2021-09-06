cmake_minimum_required(VERSION 2.8)
PROJECT(out)

list(APPEND CMAKE_CXX_FLAGS
-pthread)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

set(CMAKE_CXX_STANDARD 11)
LINK_LIBRARIES("/opt/MVS/lib/64/libFormatConversion.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libGCBase_gcc421_v3_0.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libGenApi_gcc421_v3_0.so")
LINK_LIBRARIES("/opt/MVS/lib/64/liblog4cpp_gcc421_v3_0.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libLog_gcc421_v3_0.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libMathParser_gcc421_v3_0.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libMediaProcess.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libMvCameraControl.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libMvCameraControl.so.3.1.3.0")
LINK_LIBRARIES("/opt/MVS/lib/64/libMVGigEVisionSDK.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libMVGigEVisionSDK.so.3.1.3.0")
LINK_LIBRARIES("/opt/MVS/lib/64/libMVRender.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libMvUsb3vTL.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libMvUsb3vTL.so.3.1.3.0")
LINK_LIBRARIES("/opt/MVS/lib/64/libNodeMapData_gcc421_v3_0.so")
LINK_LIBRARIES("/opt/MVS/lib/64/libXmlParser_gcc421_v3_0.so")

find_package(Pangolin REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIRS})

set(PROJ_SOURCES
main.cpp
header.h
camera.cpp
camera.h
pgl.cpp
pgl.h
arrow.cpp
arrow.h
zhencha.cpp
zhencha.h
dianxuan.cpp
dianxuan.h
jiaozheng.cpp
jiaozheng.h
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJ_SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Pangolin_LIBRARIES})