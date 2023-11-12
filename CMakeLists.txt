cmake_minimum_required(VERSION 3.16)
project(lab10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)


file(GLOB_RECURSE sources src/*.cpp src/*.c)
# file(GLOB_RECURSE includes include/*.h)

add_executable(lab10 ${sources})

target_include_directories(lab10 PUBLIC include)
target_include_directories(lab10 PUBLIC src)

target_link_libraries(lab10 PRIVATE sfml-graphics)
target_compile_features(lab10 PRIVATE cxx_std_17)


if(WIN32)
    add_custom_command(
        TARGET lab10
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
            ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll
            $<TARGET_FILE_DIR:lab10>
        VERBATIM)
    
    add_custom_command(TARGET lab10
        COMMENT "Copy /resources"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources/
            $<TARGET_FILE_DIR:lab10>)
endif()

install(TARGETS lab10)
