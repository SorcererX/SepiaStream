if(BUILD_EXAMPLES)
    project( examples )
    include_directories( "${CMAKE_SOURCE_DIR}/defs" )

    add_subdirectory( commtester )
    add_subdirectory( comm2tester )

    add_executable( reader reader.cpp )
    add_dependencies( reader SepiaStream )
    target_link_libraries( reader SepiaStream )

    add_executable( writer writer.cpp )
    add_dependencies( writer SepiaStream )
    target_link_libraries( writer SepiaStream )

endif(BUILD_EXAMPLES)
