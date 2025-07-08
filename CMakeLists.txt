cmake_minimum_required( VERSION 3.16.3 )
project(SepiaStream C CXX ASM)
include( ExternalProject )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O2 -pthread")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")
set(BUILD_DAEMONS ON CACHE BOOL "Build daemons")

set( NETWORK_INCLUDES
    ${CMAKE_SOURCE_DIR}/defs/sepia/network/tcpsocket.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/network/client.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/network/server.h )
set( SEPIA_INCLUDES
    ${CMAKE_SOURCE_DIR}/defs/sepia/reader.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/writer.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/stream.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/databuffer.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/semaphore_interface.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/sharedmemory.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/semaphore.h )
set( UTIL_INCLUDES
    ${CMAKE_SOURCE_DIR}/defs/sepia/util/threadobject.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/util/helper.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/util/progargs.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/util/threadbarrier.h )
set( COMPRESSION_INCLUDES
    ${CMAKE_SOURCE_DIR}/defs/sepia/compression/common.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/compression/decompress.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/compression/compress.h
    ${CMAKE_SOURCE_DIR}/defs/sepia/compression/enc3b11b.h )

set( HEADER_FILES ${NETWORK_INCLUDES} ${SEPIA_INCLUDES} ${UTIL_INCLUDES} ${COMPRESSION_INCLUDES} )
aux_source_directory(src SRC_LIST)
aux_source_directory(src/network SRC_LIST)
aux_source_directory(src/util SRC_LIST)
aux_source_directory(src/compression SRC_LIST)
include_directories(defs)

add_library( ${PROJECT_NAME} ${SRC_LIST} ${HEADER_FILES} )

target_link_libraries( ${PROJECT_NAME} -lrt )

add_subdirectory(examples)
add_subdirectory(daemons)

install( TARGETS ${PROJECT_NAME} DESTINATION lib )
install( FILES ${SEPIA_INCLUDES}   DESTINATION include/sepia )
install( FILES ${NETWORK_INCLUDES} DESTINATION include/sepia/network )
install( FILES ${UTIL_INCLUDES} DESTINATION include/sepia/util )
install( FILES ${COMPRESSION_INCLUDES} DESTINATION include/sepia/compression )
