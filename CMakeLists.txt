cmake_minimum_required(VERSION 3.0)
project(AutoBattler)

set(CMAKE_CXX_STANDARD 17)

# Add all source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Card.cpp
    src/Player.cpp
    src/CardFactory.cpp
    src/DeckManager.cpp
)

# Add all header files
set(HEADERS
    src/Game.h
    src/Card.h
    src/Player.h
    src/CardFactory.h
    src/DeckManager.h
)

# Create executable
add_executable(AutoBattler ${SOURCES} ${HEADERS})
