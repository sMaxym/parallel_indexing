cmake_minimum_required(VERSION 3.5)

project(indexing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LibArchive REQUIRED)
find_package(Boost REQUIRED)

file(GLOB SOURCES
    header/*.h
    src/*.cpp
)

add_executable(indexing ${SOURCES})
target_link_libraries(indexing ${LibArchive_LIBRARIES} ${Boost_LIBRARIES} -lboost_system -lboost_filesystem -lboost_locale)
