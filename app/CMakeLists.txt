# APP SUBDIR

file (GLOB_RECURSE SOURCES "*.cpp")
file (GLOB_RECURSE HEADERS "*.h")

add_executable (McPorts ${SOURCES} ${HEADERS} "mcports_main.cpp" "header.h")

set_target_properties(McPorts PROPERTIES CXX_STANDARD 20)
target_link_libraries(McPorts PRIVATE asio)