cmake_minimum_required(VERSION 3.12.0)
project(FaceyThing)

find_package(OpenCV CONFIG REQUIRED)

#get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../Cinder" ABSOLUTE)
get_filename_component( APP_PATH    "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(CINDER_PATH "" CACHE PATH "Location of cinder folder")

message(STATUS "Cinder path: ${CINDER_PATH} App path: ${APP_PATH}" )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( USE_AVX_INSTRUCTIONS ON)
set( FACEYTHING_TARGET "FaceyThing" )

set( FACEYTHING_SOURCES
     ${APP_PATH}/src/CinderDlib.h
     ${APP_PATH}/src/CinderOpenCV.h
     ${APP_PATH}/src/CameraCapture.h
     ${APP_PATH}/src/TextureBillboard.h
     ${APP_PATH}/src/GridLayout.h
     ${APP_PATH}/src/FaceTracker.h

     ${APP_PATH}/src/FaceyThingApp.cpp
     ${APP_PATH}/src/CameraCapture.cpp
     ${APP_PATH}/src/CinderDlib.cpp
     ${APP_PATH}/src/TextureBillboard.cpp
     ${APP_PATH}/src/GridLayout.cpp
     ${APP_PATH}/src/FaceTracker.cpp
)

set ( INCLUDE_PATHS
      ${APP_PATH}/include
)

ci_make_app(
    APP_NAME    ${FACEYTHING_TARGET}
    CINDER_PATH ${CINDER_PATH}
    SOURCES     ${FACEYTHING_SOURCES}
    INCLUDES    ${INCLUDE_PATHS}
    #RESOURCES   ${APP_PATH}/../../data/models/8lbs.obj
)
target_include_directories(${FACEYTHING_TARGET} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${FACEYTHING_TARGET} ${OpenCV_LIBS})
