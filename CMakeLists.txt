cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(WinAPICommon LANGUAGES CXX)

set(WINAPICOMMON_CPP_VERSION 23 CACHE STRING "C++ language version." FORCE)
set_property(CACHE WINAPICOMMON_CPP_VERSION PROPERTY STRINGS 20 23)
if(WINAPICOMMON_CPP_VERSION LESS 20)
    message(FATAL_ERROR "Minimum allowed language version is C++20.")
endif()
set(WINAPICOMMON_INSTALL_HOME "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE STRING "The installation base directory of WinAPICommon library." FORCE)

file(GLOB_RECURSE WIN_API_COMMON_SOURCE CONFIGURE_DEPENDS "./src/*.hpp" "./src/*.inl" "./src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${WIN_API_COMMON_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME WinAPICommon CXX_STANDARD ${WINAPICOMMON_CPP_VERSION} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME} INTERFACE -lwtsapi32 -lkernel32 -lshlwapi -lcmcfg32 -lpathcch -lshell32)
target_precompile_headers(${PROJECT_NAME} PUBLIC "src/Precompiled.hpp")
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Debug>:_DEBUG>" "$<$<CONFIG:Release>:NDEBUG>" "WINVER=0x0A00;_WIN32_WINNT=0x0A00") # Set to target every version of the Windows 10 operating system.
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Debug>:-m64;-municode;-Wall;-Wextra;-Wformat=2;-Wunused;-Wpedantic;>" "$<$<CONFIG:Release>:-m64;-municode;>")

message("Installing Windows API Common Helper Library Project !!!")
install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${WINAPICOMMON_INSTALL_HOME})
