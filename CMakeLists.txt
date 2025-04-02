cmake_minimum_required(VERSION 3.10)
project(trader VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)


add_executable(trader trader.cpp env.cpp)


target_include_directories(trader PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(trader PRIVATE Boost::system Boost::thread OpenSSL::SSL OpenSSL::Crypto)

set_property(TARGET trader PROPERTY CXX_STANDARD 17)
