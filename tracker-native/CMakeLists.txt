cmake_minimum_required(VERSION 3.6)
project(tracker_native CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -Wall -fPIC -ggdb")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--icf=all")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,-z,noexecstack")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now")


# Android system libs begin
include_directories(${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/include)
add_library(
  log-lib
  SHARED
  IMPORTED
)
set_target_properties(
  log-lib
  PROPERTIES IMPORTED_LOCATION
  ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/${NDK_LINK_LIB_ARCH}/${ANDROID_PLATFORM}/liblog.so
)
add_library(
  dl-lib
  SHARED
  IMPORTED
)
set_target_properties(
  dl-lib
  PROPERTIES IMPORTED_LOCATION
  ${ANDROID_TOOLCHAIN_ROOT}/sysroot/usr/lib/${NDK_LINK_LIB_ARCH}/${ANDROID_PLATFORM}/libdl.so
)
# Android system libs end


# Frida begin
add_definitions(-DARCH_ABI=${ANDROID_ABI})
add_library(
  frida-core
  SHARED
  IMPORTED
)
set_target_properties(
  frida-core
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/frida-core-14.0.6/${ANDROID_ABI}/libfrida-core.a
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/frida-core-14.0.6/${ANDROID_ABI})

add_library(
  frida-gum
  SHARED
  IMPORTED
)
set_target_properties(
  frida-gum
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/frida-gum-14.0.6/${ANDROID_ABI}/libfrida-gum.a
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/frida-gum-14.0.6/${ANDROID_ABI})
# Frida end


# RapidJSON begin
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/rapidjson-1.1.0/include)
# RapidJSON end


include_directories(include)
# TCPClientServer begin
add_library(
  tcp-client-server
  STATIC
  src/tcp_client_server/client.cpp
  src/tcp_client_server/server.cpp
  src/tcp_client_server/client_manager.cpp
)
# TCPClientServer end

SET(
  BASE_SRCS
  src/base/hack_action.cpp
  src/base/encoder.cpp
)
SET(
  AGENT_SRCS
  src/agent/types.cpp
  src/agent/trace_thread_handler.cpp
  src/agent/query_thread_handler.cpp
  src/agent/server_main.cpp
)
SET(
  ARCH_SPECIFIC_SRCS
  src/agent/arch/${ANDROID_ABI}/stalker.cpp
)

# Agent begin
add_library(
  agent
  MODULE
  ${BASE_SRCS}
  ${ARCH_SPECIFIC_SRCS}
  ${AGENT_SRCS}
)
target_link_libraries(
  agent
  frida-gum
  tcp-client-server
  dl-lib
  log-lib
)
# Agent end

# Just for debug
add_executable(
  tester
  ${BASE_SRCS}
  ${ARCH_SPECIFIC_SRCS}
  ${AGENT_SRCS}
)
target_link_libraries(
  tester
  frida-gum
  tcp-client-server
  dl-lib
  log-lib
)
# end


# Injector begin
add_executable(
  injector
  ${BASE_SRCS}
  src/injector/inject_agent_handler.cpp
  src/injector/server_main.cpp
)
target_link_libraries(
  injector
  frida-core
  tcp-client-server
  dl-lib
  log-lib
)
# Injector end