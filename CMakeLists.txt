project(SepiaStream)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")
set(BUILD_COMM ON CACHE BOOL "Include Communication library (requires Boost and Protobuf)")
set(BUILD_DAEMONS ON CACHE BOOL "Build daemons")

if(BUILD_COMM)
    find_package(Protobuf REQUIRED)
    include_directories(${PROTOBUF_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
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
        ${CMAKE_SOURCE_DIR}/defs/sepia/comm/scom.h )
    aux_source_directory(src/comm SRC_LIST)
    include_directories(defs/sepia/comm)
    install( FILES ${COMM_INCLUDES} DESTINATION include/sepia/comm )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/header.pb.h DESTINATION include/sepia/comm )
endif(BUILD_COMM)

cmake_minimum_required(VERSION 2.8)
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
    ${CMAKE_SOURCE_DIR}/defs/sepia/util/threadobject.h )

set( HEADER_FILES ${NETWORK_INCLUDES} ${SEPIA_INCLUDES} ${COMM_INCLUDES} ${UTIL_INCLUDES} )
aux_source_directory(src SRC_LIST)
aux_source_directory(src/network SRC_LIST)
aux_source_directory(src/util SRC_LIST)
include_directories(defs)
include_directories(defs/sepia)
include_directories(defs/sepia/network)
include_directories(defs/sepia/util)
add_library( ${PROJECT_NAME} ${SRC_LIST} ${HEADER_FILES} ${PROTO_SRCS} ${PROTO_HDRS} )
target_link_libraries( ${PROJECT_NAME} -lrt )

if(BUILD_COMM)
    target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})
endif(BUILD_COMM)

add_subdirectory(examples)
add_subdirectory(daemons)

install( TARGETS ${PROJECT_NAME} DESTINATION lib )
install( FILES ${SEPIA_INCLUDES}   DESTINATION include/sepia )
install( FILES ${NETWORK_INCLUDES} DESTINATION include/sepia/network )
install( FILES ${UTIL_INCLUDES} DESTINATION include/sepia/util )
