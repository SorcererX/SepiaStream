if(BUILD_DAEMONS)
    project( daemons )
    add_subdirectory( Router )

    add_executable( client client.cpp )
    target_link_libraries( client SepiaStream )
    add_executable( server server.cpp )
    target_link_libraries( server SepiaStream )
endif(BUILD_DAEMONS)
