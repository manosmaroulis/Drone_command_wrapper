cmake_minimum_required(VERSION 3.18)
project(Drone_command)
set (INCLUDE_DIR include/)
# set(HEADER_FILES include/"*.h")
set(MAVLINK_DIR mavlink/include/mavlink/v2.0/ )
set(Drone_command_SOURCES
src/Drone_Command.cpp

src/serial_port.cpp
src/wireless_interface_con.cpp
src/iperf_wrapper.cpp
)


add_library(${PROJECT_NAME} ${Drone_command_SOURCES})
target_link_libraries(${PROJECT_NAME} -lpthread)
add_executable(drone_command_test src/drone_command_test.cpp)
include_directories(drone_command_test ${INCLUDE_DIR} ${MAVLINK_DIR})
target_link_libraries( drone_command_test ${PROJECT_NAME})
