cmake_minimum_required(VERSION 3.5)
project("Stonehenge")

set(CMAKE_CXX_STANDARD 11)

include_directories("include")


add_library(stonehenge_lib SHARED src/board.cpp src/game.cpp src/location.cpp src/megalith.cpp src/piece.cpp src/player.cpp)
target_link_libraries(stonehenge_lib)


add_executable(stonehenge src/main.cpp)
target_link_libraries(stonehenge stonehenge_lib)




