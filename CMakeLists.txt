project(SepiaStream)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")

cmake_minimum_required(VERSION 2.8)
set( HEADER_FILES
    ${CMAKE_SOURCE_DIR}/defs/sepia/reader.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/writer.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/stream.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/databuffer.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/semaphore_interface.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/sharedmemory.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/semaphore.h )
set( HEADER_FILES ${HEADER_FILES}
    ${CMAKE_SOURCE_DIR}/defs/sepia/network/tcpsocket.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/network/client.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/network/server.h )
aux_source_directory(src SRC_LIST)
include_directories(defs)
include_directories(defs/sepia)
include_directories(defs/sepia/network)
add_library( ${PROJECT_NAME} ${SRC_LIST} ${HEADER_FILES} )
target_link_libraries( ${PROJECT_NAME} -lrt )
add_subdirectory(examples)

install( TARGETS ${PROJECT_NAME} DESTINATION lib )
install( FILES ${INCLUDE_FILES} DESTINATION include )
install( FILES ${CMAKE_SOURCE_DIR}/defs/sepia/databuffer.h DESTINATION include/sepia )

