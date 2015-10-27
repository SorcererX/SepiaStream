project( proxy )
include_directories( "${CMAKE_SOURCE_DIR}/defs" )
include_directories( "${CMAKE_SOURCE_DIR}/daemons/Proxy/defs" )
aux_source_directory( "${CMAKE_SOURCE_DIR}/daemons/Proxy/src" PROXY_SRC_LIST )
set( PROXY_HEADER_FILES
     ${CMAKE_SOURCE_DIR}/daemons/Proxy/defs/msgsocket.h
     ${CMAKE_SOURCE_DIR}/daemons/Proxy/defs/session.h
     ${CMAKE_SOURCE_DIR}/daemons/Proxy/defs/proxyclient.h
     ${CMAKE_SOURCE_DIR}/daemons/Proxy/defs/proxyserver.h )


add_executable( ${PROJECT_NAME} ${PROXY_SRC_LIST} ${PROXY_HEADER_FILES} )
add_dependencies( ${PROJECT_NAME} SepiaStream )
target_link_libraries( ${PROJECT_NAME} SepiaStream )
target_link_libraries( ${PROJECT_NAME} -lrt )
