cmake_minimum_required(VERSION 3.10)
project(sqlcpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/**")
add_library(sqlcpp STATIC ${SOURCES})
target_include_directories(sqlcpp PUBLIC "${PROJECT_SOURCE_DIR}/inc")

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

    add_test(NAME SqlcppTest COMMAND sqlcpp_test)
endif()
