if(BUILD_COMM2)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set( FLATC_SCHEMA_ARGS --gen-mutable --gen-name-strings )
find_package(FlatBuffers REQUIRED)
FLATBUFFERS_GENERATE_C_HEADERS( TESTMSGS ${CMAKE_SOURCE_DIR}/examples/comm2tester/comm2tester_msgs.fbs )

aux_source_directory( src COMM2TESTER_SRC_LIST )
set( HEADER_FILES
     ${CMAKE_SOURCE_DIR}/examples/comm2tester/defs/receivetester.hpp )
include_directories(defs)
add_executable( comm2tester ${COMM2TESTER_SRC_LIST} ${HEADER_FILES} ${TESTMSGS_OUTPUTS} )
add_dependencies( comm2tester SepiaStream )
target_link_libraries( comm2tester -pthread )
target_link_libraries( comm2tester SepiaStream )
target_link_libraries( comm2tester ${FLATBUFFERS_LIBRARIES} )

endif(BUILD_COMM2)
