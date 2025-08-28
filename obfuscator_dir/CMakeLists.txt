cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2")

add_executable(obfuscator obfuscator.cpp)
add_executable(de-obfuscator de-obfuscator.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(obfuscator PRIVATE -static)
    target_link_libraries(de-obfuscator PRIVATE -static)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(obfuscator PRIVATE -static -lstdc++exp)
    target_link_libraries(de-obfuscator PRIVATE -static -lstdc++exp)
endif()
