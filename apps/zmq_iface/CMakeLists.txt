cmake_minimum_required(VERSION 3.2)
project(ZMQ_IFACE C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(MODULES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules")

include_directories(../inc)
include_directories(../Icarouslib/fsw/platform_inc)
include_directories(../traffic/fsw/platform_inc)
include_directories(../trajectory/fsw/platform_inc)
include_directories(${MODULES_ROOT}/Core/Interfaces)
include_directories(fsw/src)
aux_source_directory(fsw/src APP_SRC_FILES)

add_cfe_app(zmq_iface ${APP_SRC_FILES})
target_include_directories(zmq_iface PRIVATE unit-test)
target_compile_options(zmq_iface PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(zmq_iface zmq json-c)

if (ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)
