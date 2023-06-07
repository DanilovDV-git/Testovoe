cmake_minimum_required(VERSION 3.22.1)
project(TcpClient)

set(CMAKE_CXX_STANDARD 17)
set(Boost_ADDITIONAL_VERSIONS "1.72" "1.72.0")

#set(Boost_USE_MULTITHREADED OFF)

find_package(Boost COMPONENTS system thread REQUIRED)
message(status "** Boost Incllude: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")


#add_library(Boost::thread SHARED IMPORTED)
add_executable(tcp_client tcp_client.cpp)

target_link_libraries(tcp_client ${Boost_LIBRARIES})

