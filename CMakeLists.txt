cmake_minimum_required(VERSION 3.10)

set(SQLCPP_MAJOR_VERSION "1")
set(SQLCPP_MINOR_VERSION "0")
set(SQLCPP_PATCH_VERSION "0")
set(SQLCPP_VERSION_SPECIAL "")
set(SQLCPP_VERSION_STRING "${SQLCPP_MAJOR_VERSION}.${SQLCPP_MINOR_VERSION}.${SQLCPP_PATCH_VERSION}")

if(NOT "${SQLCPP_VERSION_SPECIAL}" STREQUAL "")
    set(SQLCPP_VERSION_STRING "${SQLCPP_VERSION_STRING}-${SQLCPP_VERSION_SPECIAL}")
endif()

project(sqlcpp VERSION ${SQLCPP_VERSION_STRING} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/**")
add_library(sqlcpp STATIC ${SOURCES})
target_include_directories(sqlcpp PUBLIC "${PROJECT_SOURCE_DIR}/inc")

target_compile_definitions(sqlcpp PUBLIC "SQLCPP_VERSION_MAJOR=${SQLCPP_MAJOR_VERSION}")
target_compile_definitions(sqlcpp PUBLIC "SQLCPP_VERSION_MINOR=${SQLCPP_MINOR_VERSION}")
target_compile_definitions(sqlcpp PUBLIC "SQLCPP_VERSION_PATCH=${SQLCPP_PATCH_VERSION}")
target_compile_definitions(sqlcpp PUBLIC "SQLCPP_VERSION_SPECIAL=\"${SQLCPP_VERSION_SPECIAL}\"")
target_compile_definitions(sqlcpp PUBLIC "SQLCPP_VERSION_STRING=\"${SQLCPP_VERSION_STRING}\"")

if(MSVC)
    target_compile_options(sqlcpp PRIVATE /W4 /WX
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Release>:/O2>)
else()
    target_compile_options(sqlcpp PRIVATE -Wall -Werror
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:Release>:-O3>)
endif()

option(SQLCPP_TESTS "Build the sqlcpp test executable" OFF)

if(SQLCPP_TESTS)
    find_package(SQLite3 REQUIRED)
    add_executable(sqlcpp_test "main.cpp")
    target_include_directories(sqlcpp_test PRIVATE "${PROJECT_SOURCE_DIR}/inc")
    target_link_libraries(sqlcpp_test PRIVATE sqlcpp SQLite::SQLite3)

    target_compile_options(sqlcpp_test PRIVATE
        $<$<CONFIG:Debug>:-O0 -g>
    )

    enable_testing()
    add_test(NAME SqlcppTest COMMAND sqlcpp_test)
    message(STATUS "Enabled sqlcpp tests")
endif()
