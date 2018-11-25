cmake_minimum_required(VERSION 3.12.0)
project(FaceyThing)

find_package(dlib CONFIG REQUIRED)

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../Cinder" ABSOLUTE)
get_filename_component( APP_PATH    "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
message( STATUS ${CINDER_PATH} )
message( STATUS ${APP_PATH} )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( USE_AVX_INSTRUCTIONS ON)
set( FACEYTHING_TARGET "FaceyThing" )

set( FACEYTHING_SOURCES
     ${APP_PATH}/src/CinderDlib.h
     ${APP_PATH}/src/CameraCapture.h
     ${APP_PATH}/src/FaceyThingApp.cpp
     ${APP_PATH}/src/CameraCapture.cpp
)

ci_make_app(
    APP_NAME    ${FACEYTHING_TARGET}
    CINDER_PATH ${CINDER_PATH}
    SOURCES     ${FACEYTHING_SOURCES}
    INCLUDES    ${APP_PATH}/include
    #RESOURCES   ${APP_PATH}/../../data/models/8lbs.obj
)

target_link_libraries(${FACEYTHING_TARGET} dlib::dlib)
