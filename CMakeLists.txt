cmake_minimum_required(VERSION 3.16)
project(ArcaneJourney)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# SFML 3 — fetched automatically so CI runners don't need it pre-installed
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.1
    GIT_SHALLOW    TRUE
)
set(SFML_BUILD_AUDIO   OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
add_executable(ArcaneJourney
    src/main.cpp

    src/core/Player.cpp
    src/core/Camera.cpp
    src/core/World.cpp

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
    src/ui/PlatformDemo.cpp
    src/ui/SplashScreen.cpp

    src/systems/DiscoveryTracker.cpp
    src/systems/SaveData.cpp
    src/systems/Juice.cpp
)

target_include_directories(ArcaneJourney PRIVATE src)
target_link_libraries(ArcaneJourney PRIVATE SFML::Graphics SFML::Window SFML::System)

# Warnings
if(MSVC)
    target_compile_options(ArcaneJourney PRIVATE /W3)
else()
    target_compile_options(ArcaneJourney PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Copy assets to build directory after each build
# ---------------------------------------------------------------------------
add_custom_command(TARGET ArcaneJourney POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:ArcaneJourney>/assets
)