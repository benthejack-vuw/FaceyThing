cmake_minimum_required(VERSION 3.12.0)
project(FaceyThing)

find_package(OpenCV CONFIG REQUIRED)

#get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../Cinder" ABSOLUTE)
get_filename_component( APP_PATH    "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(CINDER_PATH "" CACHE PATH "Location of cinder folder")
set(OPENMESH_PATH "" CACHE PATH "Location of OpenMesh Install Directory")


message(STATUS "Cinder path: ${CINDER_PATH} App path: ${APP_PATH}" )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( USE_AVX_INSTRUCTIONS ON)
set( FACEYTHING_TARGET "FaceyThing" )

set( FACEYTHING_SOURCES

     ${APP_PATH}/src/json.hpp
     ${APP_PATH}/src/FaceyThingApp.cpp
     ${APP_PATH}/include/CinderOpenCV.h

     ${APP_PATH}/include/CameraCapture.h
     ${APP_PATH}/include/FaceTracker.h
     ${APP_PATH}/include/ObjectTracker.h
     ${APP_PATH}/include/FaceDetector.h
     ${APP_PATH}/include/LandmarkDetector.h
     ${APP_PATH}/include/FacialComponents.h
     ${APP_PATH}/include/CollageItem.h
     ${APP_PATH}/include/FaceCollage.h
     ${APP_PATH}/include/FacePainter.h
     ${APP_PATH}/include/FaceyThing.h
     ${APP_PATH}/include/FaceMesh.h
     ${APP_PATH}/include/FaceRenderer.h

     ${APP_PATH}/src/CameraCapture.cpp
     ${APP_PATH}/src/ObjectTracker.cpp
     ${APP_PATH}/src/FaceDetector.cpp
     ${APP_PATH}/src/LandmarkDetector.cpp
     ${APP_PATH}/src/FaceTracker.cpp
     ${APP_PATH}/src/FacialComponents.cpp
     ${APP_PATH}/src/CollageItem.cpp
     ${APP_PATH}/src/FaceCollage.cpp
     ${APP_PATH}/src/FacePainter.cpp
     ${APP_PATH}/src/FaceyThing.cpp
     ${APP_PATH}/src/FaceMesh.cpp
     ${APP_PATH}/src/FaceRenderer.cpp

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
target_link_libraries(${FACEYTHING_TARGET}
  ${OPENMESH_PATH}/lib/OpenMeshCore.lib
  ${OPENMESH_PATH}/lib/OpenMeshTools.lib
)


add_custom_command(TARGET ${FACEYTHING_TARGET} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                 ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${FACEYTHING_TARGET}>/assets)
