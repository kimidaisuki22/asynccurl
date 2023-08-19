cmake_minimum_required (VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

project ("asynccurl_examle" LANGUAGES CXX)


add_executable(asynccurl_example main.cpp)

target_link_libraries(asynccurl_example PRIVATE asynccurl)