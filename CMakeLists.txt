cmake_minimum_required(VERSION 3.21)
project(console_engine VERSION 0.1.0 LANGUAGES CXX )

set (CMAKE_CXX_STANDARD 17)
add_definitions(-DUNICODE -D_UNICODE)

if(WIN32)     
    file(COPY ${CMAKE_SOURCE_DIR}/lib/SDL2-2.26.5-mingw64/x86_64-w64-mingw32/bin/SDL2.dll
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set(sdl_path ${CMAKE_SOURCE_DIR}/lib/SDL2-2.26.5-mingw64/cmake)
    find_package(SDL2 CONFIG REQUIRED PATHS ${sdl_path})
endif(WIN32)
if(UNIX)
    find_package(SDL2 CONFIG REQUIRED)
endif(UNIX)

file(COPY ${CMAKE_SOURCE_DIR}/resource
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE all_source_file ${CMAKE_SOURCE_DIR}/src/*.c  ${CMAKE_SOURCE_DIR}/src/*.cpp )
file(GLOB_RECURSE all_hpp_file ${CMAKE_SOURCE_DIR}/src/*.hpp)
set(include_dir ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} main.cpp ${all_source_file} )
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} ${include_dir})
