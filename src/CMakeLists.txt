cmake_minimum_required(VERSION 3.11.1)

project(ddclock VERSION 1.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

set(TARGET ddclock_${PROJECT_VERSION})
set(SOURCE ddclock.cpp)
set(RESOURCE_FILE resource.rc)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

message(STATUS "CMAKE_CXX_PLATFORM_ID: "    ${CMAKE_CXX_PLATFORM_ID} )
message(STATUS "CMAKE_CXX_COMPILER_ID: "    ${CMAKE_CXX_COMPILER_ID} )

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-m32")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")

    set(CMAKE_EXE_LINKER_FLAGS  "-static -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-s")
    set(CMAKE_RC_FLAGS "-O coff")
endif()

add_executable(${TARGET} WIN32 ${SOURCE} ${RESOURCE_FILE})
target_link_libraries(${TARGET} gdiplus)
