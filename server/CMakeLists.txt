cmake_minimum_required(VERSION 3.22.1)
project(TcpServer)

set(CMAKE_CXX_STANDARD 17)
set(Boost_ADDITIONAL_VERSIONS "1.72" "1.72.0")

#set(Boost_USE_MULTITHREADED OFF)

find_package(Boost COMPONENTS system thread REQUIRED)
message(status "** Boost Incllude: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")


#add_library(Boost::thread SHARED IMPORTED)
add_executable(tcp_server main.cpp server.cpp db_handler.cpp user.cpp session.cpp parser.cpp)

add_subdirectory(./libpqxx build-pqxx)

target_link_libraries(tcp_server PRIVATE pqxx ${Boost_LIBRARIES})
