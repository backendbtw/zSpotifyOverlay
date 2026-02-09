cmake_minimum_required(VERSION 4.2)

project(zspotifyOverlay VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets DBus Network REQUIRED)

set(CMAKE_BUILD_TYPE Release)
add_compile_options(-Os)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")

add_executable(
    zspotifyOverlay
    ${SOURCES}
    ${HEADERS}
    ${QRC_SOURCES}
)

target_link_libraries(zspotifyOverlay Qt6::Core Qt6::Widgets Qt6::DBus Qt6::Network)
