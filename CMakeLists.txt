cmake_minimum_required(VERSION 3.16)
project(ArcaneJourney)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(ArcaneJourney
    src/main.cpp

    src/core/Player.cpp
    src/core/Camera.cpp
    src/core/World.cpp
    src/core/DiscoveryTracker.cpp

    src/level/LevelData.cpp
    src/level/TileLoader.cpp
    src/level/MovingPlatform.cpp
    src/level/CrumblingPlatform.cpp
    src/level/BouncePad.cpp
    src/level/DisappearingPlatform.cpp
    src/level/TeleportBlock.cpp
    src/level/Climbable.cpp
    src/level/Trapdoor.cpp

    src/ui/SectionAnnouncer.cpp
    src/ui/CoinManager.cpp
    src/ui/CosmeticsManager.cpp
    src/ui/Npc.cpp
    src/ui/HubUI.cpp
    src/ui/SaveData.cpp
)

target_include_directories(ArcaneJourney PRIVATE src)
target_link_libraries(ArcaneJourney SFML::Graphics SFML::Window SFML::System)

# Copy assets to build directory after each build
add_custom_command(TARGET ArcaneJourney POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:ArcaneJourney>/assets
)