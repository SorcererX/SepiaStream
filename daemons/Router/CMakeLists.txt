project( router )
include_directories( "${CMAKE_SOURCE_DIR}/defs" )
include_directories( "${CMAKE_SOURCE_DIR}/daemons/Router/defs" )
aux_source_directory( "${CMAKE_SOURCE_DIR}/daemons/Router/src" DAEMON_SRC_LIST )
set( DAEMON_HEADER_FILES
     ${CMAKE_SOURCE_DIR}/daemons/Router/defs/messagerouter.h
     ${CMAKE_SOURCE_DIR}/daemons/Router/defs/processmonitor.h )

add_executable( ${PROJECT_NAME} ${DAEMON_SRC_LIST} ${DAEMON_HEADER_FILES} )
add_dependencies( ${PROJECT_NAME} SepiaStream )
target_link_libraries( ${PROJECT_NAME} SepiaStream )
target_link_libraries( ${PROJECT_NAME} -lrt )
