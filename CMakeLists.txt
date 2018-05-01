cmake_minimum_required(VERSION 3.10)
project(SourcemodCompileManager)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
include_directories(src/base)
include_directories(src/formatter)

add_executable(SourcemodCompileManager
        src/base/Compiler.cpp
        src/base/Compiler.h
        src/base/CompileResult.h
        src/base/SCMErrors.cpp
        src/base/SCMErrors.h
        src/formatter/Formatter.cpp
        src/formatter/Formatter.h
        src/SourcemodCompileManager.cpp
        )
