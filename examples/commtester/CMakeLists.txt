if(BUILD_COMM)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS
                      PROTO_HDRS
                      ${CMAKE_SOURCE_DIR}/examples/commtester/commtester_messages.proto)

aux_source_directory( src COMMTESTER_SRC_LIST )
set( HEADER_FILES
     ${CMAKE_SOURCE_DIR}/examples/commtester/defs/receivetester.hpp )
include_directories(defs)
add_executable( commtester ${COMMTESTER_SRC_LIST} ${HEADER_FILES} ${PROTO_SRCS} ${PROTO_HDRS} )
add_dependencies( commtester SepiaStream )
target_link_libraries( commtester SepiaStream )
target_link_libraries( commtester -lboost_system -lboost_thread )
target_link_libraries( commtester ${PROTOBUF_LIBRARIES} )
endif(BUILD_COMM)
