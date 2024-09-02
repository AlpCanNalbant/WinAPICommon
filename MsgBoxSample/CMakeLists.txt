cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
project(MsgBoxSample LANGUAGES CXX)

set(WIN_API_COMMON_HOME "../../")
set(RESOURCE_DIR "Resource")
set(COMPILER_NAME "mingw")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_subdirectory("${WIN_API_COMMON_HOME}" "${CMAKE_BINARY_DIR}/deps/WinAPICommon")

set(MSG_BOX_SAMPLE_SOURCE "WinMain.cpp")

add_executable(${PROJECT_NAME} ${MSG_BOX_SAMPLE_SOURCE})
target_sources(${PROJECT_NAME} PRIVATE "${RESOURCE_DIR}/MsgBoxSampleData.rc")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME MsgBox CXX_STANDARD ${WINAPICOMMON_CPP_VERSION} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		mingw32 WinAPICommon
		-static-libgcc -static-libstdc++ -static Threads::Threads)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${INSTALL_HOME})
