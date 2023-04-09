cmake_minimum_required (VERSION 2.8)

project (c_server)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory (src SRC_LIST)

include_directories (include)

add_executable (main ${SRC_LIST})

find_package(Threads)

target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(main mysqlclient)