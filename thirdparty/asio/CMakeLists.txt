# THIRDPARTY/ASIO SUBDIR

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE include)
target_compile_definitions(asio INTERFACE
	ASIO_STANDALONE 
	ASIO_NO_DEPRECATED
)

if (WIN32)
	target_link_libraries(asio INTERFACE ws2_32 crypt32)
else()
	target_link_libraries(asio INTERFACE pthread)
endif()