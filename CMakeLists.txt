cmake_minimum_required(VERSION 3.10)

# Set project name
project(SFMLDemo)

# Find SFML
find_package(SFML 2.5 REQUIRED graphics window system)

# Add executable
add_executable(sfml-demo main.cpp)

# Link SFML libraries
target_link_libraries(sfml-demo sfml-graphics sfml-window sfml-system)
