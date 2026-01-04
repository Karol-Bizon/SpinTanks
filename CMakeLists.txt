cmake_minimum_required(VERSION 3.20)
project(TankRotate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(spinTanks
    src/main.cpp
    src/game.cpp
    src/tank.cpp
)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

target_include_directories(spinTanks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(spinTanks PRIVATE sfml-graphics sfml-window sfml-system)
