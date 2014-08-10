project(SepiaStream)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")

cmake_minimum_required(VERSION 2.8)
set( INCLUDE_FILES
    ${CMAKE_SOURCE_DIR}/defs/imagereader.h
    ${CMAKE_SOURCE_DIR}/defs/imagewriter.h
    ${CMAKE_SOURCE_DIR}/defs/imagestream.h )
set( HEADER_FILES ${INCLUDE_FILES}
    ${CMAKE_SOURCE_DIR}/defs/sepia/databuffer.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/semaphore_interface.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/sharedmemory.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/semaphore.h )
aux_source_directory(src SRC_LIST)
include_directories(defs)
include_directories(defs/sepia)
add_library( ${PROJECT_NAME} ${SRC_LIST} ${HEADER_FILES} )
target_link_libraries( ${PROJECT_NAME} -lrt )
add_subdirectory(examples)

install( TARGETS ${PROJECT_NAME} DESTINATION lib )
install( FILES ${INCLUDE_FILES} DESTINATION include )
install( FILES ${CMAKE_SOURCE_DIR}/defs/sepia/databuffer.h DESTINATION include/sepia )

