cmake_minimum_required(VERSION 3.16)
project(ArcaneJourney LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# SFML via FetchContent
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.1
    GIT_SHALLOW    TRUE
)
set(SFML_BUILD_AUDIO      OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK    OFF CACHE BOOL "" FORCE)
set(SFML_FETCH_DEPENDENCIES ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

# ---------------------------------------------------------------------------
# Sources — mirrors src/ folder structure exactly
# ---------------------------------------------------------------------------
add_executable(ArcaneJourney
    # Root
    src/main.cpp
    src/RunManager.cpp

    # core/
    src/core/Camera.cpp
    src/core/Player.cpp
    src/core/World.cpp

    # level/
    src/level/BouncePad.cpp
    src/level/Climbable.cpp
    src/level/CrumblingPlatform.cpp
    src/level/DisappearingPlatform.cpp
    src/level/LevelData.cpp
    src/level/MovingPlatform.cpp
    src/level/TeleportBlock.cpp
    src/level/TileLoader.cpp
    src/level/Trapdoor.cpp

    # systems/
    src/systems/ArtifactManager.cpp
    src/systems/AchievementManager.cpp
    src/systems/DiscoveryTracker.cpp
    src/systems/Juice.cpp
    src/systems/SaveData.cpp

    # ui/
    src/ui/CoinManager.cpp
    src/ui/HubUI.cpp
    src/ui/Npc.cpp
    src/ui/PlatformDemo.cpp
    src/ui/SectionAnnouncer.cpp
    src/ui/SplashScreen.cpp
)

# ---------------------------------------------------------------------------
# Include paths
# ---------------------------------------------------------------------------
target_include_directories(ArcaneJourney PRIVATE
    src/
)

# ---------------------------------------------------------------------------
# Link SFML
# ---------------------------------------------------------------------------
target_link_libraries(ArcaneJourney PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(ArcaneJourney PRIVATE /W3)
else()
    target_compile_options(ArcaneJourney PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Policy fix for FreeType
# ---------------------------------------------------------------------------
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# ---------------------------------------------------------------------------
# Copy assets to build directory after build
# ---------------------------------------------------------------------------
add_custom_command(TARGET ArcaneJourney POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:ArcaneJourney>/assets"
    COMMENT "Copying assets..."
)