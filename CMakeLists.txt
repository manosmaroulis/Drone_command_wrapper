cmake_minimum_required(VERSION 3.21)
project(Drone_command)
set (INCLUDE_DIR include/)
# set(HEADER_FILES include/"*.h")
set(MAVLINK_DIR mavlink/include/mavlink/v2.0/ )
set(Drone_command_SOURCES
src/Drone_Command.cpp
src/drone_command_test.cpp
src/serial_port.cpp
src/wireless_interface_con.cpp
)

add_executable(drone_command_test ${Drone_command_SOURCES})
include_directories(drone_command_test ${INCLUDE_DIR} ${MAVLINK_DIR})
target_link_libraries( drone_command_test -lpthread)