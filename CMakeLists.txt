cmake_minimum_required(VERSION 3.24)

project(nslcc LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(NSL_BUILD_COMPILER ON CACHE BOOL "Build the Compiler")

add_library(nsl
    source/ast.cpp
    source/codegen_neo.cpp
    source/codegen_shbin.cpp
    source/compiler.cpp
    source/lexer.cpp
    source/parser.cpp
    source/preprocessor.cpp
    source/symbol.cpp
)
target_include_directories(nsl PUBLIC include)

install(TARGETS nsl)
install(DIRECTORY include DESTINATION .)

if(${NSL_BUILD_COMPILER})
    if(NOT WIN32)
        set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -Wl,--no-whole-archive -Wl,--no-as-needed -ldl")
    endif()
    if(MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
    endif()
    add_executable(nslcc
        compiler/main.cpp
    )
    target_link_libraries(nslcc PUBLIC nsl)

    if(WIN32)
        install(TARGETS nslcc DESTINATION $ENV{DEVKITPRO}/tools/bin)
    else()
        install(TARGETS nslcc DESTINATION /bin)
    endif()
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT nslcc)
    endif()

endif()
