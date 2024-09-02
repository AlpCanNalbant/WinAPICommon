cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
project(WinAPICommon LANGUAGES CXX)

get_property(WinAPICommonLanguageVersion GLOBAL PROPERTY WINAPICOMMON_LANGUAGE_VERSION)
if(NOT "${WinAPICommonLanguageVersion}" STREQUAL "")
    set(WINAPICOMMON_CPP_VERSION ${WinAPICommonLanguageVersion} CACHE STRING "C++ language version." FORCE)
else()
    set(WINAPICOMMON_CPP_VERSION 20 CACHE STRING "C++ language version." FORCE)
endif()

set_property(CACHE WINAPICOMMON_CPP_VERSION PROPERTY STRINGS 20 23)
if(WINAPICOMMON_CPP_VERSION LESS 20)
    message(FATAL_ERROR "Minimum allowed language version is C++20.")
endif()
message("Desired WinAPICommon library language version is C++${WINAPICOMMON_CPP_VERSION}.")
set(WINAPICOMMON_INSTALL_HOME "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE STRING "The installation base directory of WinAPICommon library." FORCE)
set(WINAPICOMMON_PRECOMPILED_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/Precompiled.hpp")

file(GLOB_RECURSE WIN_API_COMMON_HEADERS CONFIGURE_DEPENDS "src/*.hpp" "src/*.inl")
list(REMOVE_ITEM WIN_API_COMMON_HEADERS ${WINAPICOMMON_PRECOMPILED_HEADER})
file(GLOB_RECURSE WIN_API_COMMON_SOURCE CONFIGURE_DEPENDS "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${WIN_API_COMMON_HEADERS} ${WIN_API_COMMON_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME WinAPICommon CXX_STANDARD ${WINAPICOMMON_CPP_VERSION} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME} INTERFACE -lwtsapi32 -lkernel32 -lshlwapi -lcmcfg32 -lpathcch -lshell32 -lcomctl32 -lUser32 -lUxTheme -lDwmapi -lGdi32)
target_precompile_headers(${PROJECT_NAME} PUBLIC ${WINAPICOMMON_PRECOMPILED_HEADER})
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Debug>:_DEBUG>" "$<$<CONFIG:Release>:NDEBUG>" "WINVER=0x0A00;_WIN32_WINNT=0x0A00") # Set to target every version of the Windows 10 operating system.
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Debug>:-m64;-municode;-Wall;-Wextra;-Wformat=2;-Wunused;-Wpedantic;>" "$<$<CONFIG:Release>:-m64;-municode;>")

if(NOT WINAPICOMMON_SKIP_HEADERS)
    # install(
    #     FILES
    #         ${CMAKE_CURRENT_SOURCE_DIR}/src/WinAPICommon.hpp
    #         ${CMAKE_CURRENT_SOURCE_DIR}/src/StringCounter.hpp
    #         ${CMAKE_CURRENT_SOURCE_DIR}/src/IsNotNullTerminator.hpp
    #     DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/)
    file(COPY ${WIN_API_COMMON_HEADERS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include)

    target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    set(WINAPICOMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE STRING "Include directory of the WinAPICommon library." FORCE)
else()
    target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(WINAPICOMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE STRING "Include directory of the WinAPICommon library." FORCE)
endif()

message("Installing Windows API Common Helper Library Project !!!")
install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${WINAPICOMMON_INSTALL_HOME})
