cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2")

add_executable(obfuscator obfuscator.cpp)

add_executable(de-obfuscator de-obfuscator.cpp)

target_link_libraries(obfuscator -static)

target_link_libraries(de-obfuscator -static)
