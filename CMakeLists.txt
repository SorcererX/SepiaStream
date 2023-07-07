cmake_minimum_required( VERSION 3.16.3 )
project(SepiaStream)
include( ExternalProject )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O2 -pthread -lrt")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")
set(BUILD_COMM ON CACHE BOOL "Include deprecated Communication library (requires Boost and Protobuf)")
set(BUILD_COMM2 ON CACHE BOOL "Include Communication library (requires ZeroMQ and FlatBuffers)")
set(BUILD_DAEMONS ON CACHE BOOL "Build daemons")
set(WITH_PROTOBUF ON CACHE BOOL "Add Protobuf support (required for comm)" )
set(WITH_FLATBUFFERS ON CACHE BOOL "Add Flatbuffers support" )

if(WITH_PROTOBUF)
    find_package(Protobuf REQUIRED)
    include_directories(${PROTOBUF_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif(WITH_PROTOBUF)

if(BUILD_COMM)
    PROTOBUF_GENERATE_CPP(PROTO_SRCS
                          PROTO_HDRS
                          ${CMAKE_SOURCE_DIR}/src/comm/header.proto
                          ${CMAKE_SOURCE_DIR}/src/comm/internal.proto )
    set( COMM_INCLUDES
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/dispatcher.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/globalreceiver.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/messageclient.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/messagehandler.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/messagesender.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/observer.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/observerbase.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/observerraw.h
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/scom.h )
    aux_source_directory(src/comm SRC_LIST)
    include_directories(defs/sepia/comm)
    install( FILES ${COMM_INCLUDES} DESTINATION include/sepia/comm )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/header.pb.h DESTINATION include/sepia/comm )
endif(BUILD_COMM)

if(WITH_FLATBUFFERS)
    find_package(FlatBuffers REQUIRED)
endif(WITH_FLATBUFFERS)

if(BUILD_COMM2)
    set( COMM2_INCLUDES
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/dispatcher.h
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/observer.h
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/observerbase.h
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/messagesender.h
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/messagereceiver.h
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/globalreceiver.h
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/requestor.h
         ${CMAKE_SOURCE_DIR}/defs/sepia/comm2/internals.h )
     aux_source_directory(src/comm2 SRC_LIST)
     include_directories(defs/sepia/comm2)
     install( FILES ${COMM2_INCLUDES} DESTINATION include/sepia/comm2 )
endif(BUILD_COMM2)


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

set( HEADER_FILES ${NETWORK_INCLUDES} ${SEPIA_INCLUDES} ${COMM_INCLUDES} ${COMM2_INCLUDES} ${UTIL_INCLUDES} ${COMPRESSION_INCLUDES} )
aux_source_directory(src SRC_LIST)
aux_source_directory(src/network SRC_LIST)
aux_source_directory(src/util SRC_LIST)
aux_source_directory(src/compression SRC_LIST)
include_directories(defs)

add_library( ${PROJECT_NAME} ${SRC_LIST} ${HEADER_FILES} ${PROTO_SRCS} ${PROTO_HDRS}  )

if(WITH_PROTOBUF)
    target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})
endif(WITH_PROTOBUF)

if(BUILD_COMM2)
    target_link_libraries( ${PROJECT_NAME} -lzmq )
endif(BUILD_COMM2)

if(WITH_FLATBUFFERS)
    target_link_libraries( ${PROJECT_NAME} ${FLATBUFFERS_LIBRARIES})
endif(WITH_FLATBUFFERS)

target_link_libraries( ${PROJECT_NAME} -lrt )

add_subdirectory(examples)
add_subdirectory(daemons)

install( TARGETS ${PROJECT_NAME} DESTINATION lib )
install( FILES ${SEPIA_INCLUDES}   DESTINATION include/sepia )
install( FILES ${NETWORK_INCLUDES} DESTINATION include/sepia/network )
install( FILES ${UTIL_INCLUDES} DESTINATION include/sepia/util )
install( FILES ${COMPRESSION_INCLUDES} DESTINATION include/sepia/compression )
