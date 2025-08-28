cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2")

add_executable(obfuscator obfuscator.cpp)
add_executable(de-obfuscator de-obfuscator.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Compiler is GNU/GCC. Linking with experimental C++ library.")
    target_link_libraries(obfuscator PRIVATE -static -lstdc++exp)
    target_link_libraries(de-obfuscator PRIVATE -static -lstdc++exp)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Compiler is Clang. Using standard static linking.")
    target_link_libraries(obfuscator PRIVATE -static)
    target_link_libraries(de-obfuscator PRIVATE -static)

else()
    message(STATUS "Unknown compiler. Using standard static linking.")
    target_link_libraries(obfuscator PRIVATE -static)
    target_link_libraries(de-obfuscator PRIVATE -static)

endif()
