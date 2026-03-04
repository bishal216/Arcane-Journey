#pragma once
#include <string>
#include "level/LevelData.hpp"

// Parses a .txt tile map and populates LevelData.
//
// Format:
//   - Lines starting with #          → comments, ignored
//   - Lines of the form [Name]       → section marker
//   - Everything else                → tile row (each char = TILE x TILE px)
//
// Tile size is fixed at TILE (32px) from Constants.hpp.
// World width  = first tile row character count * TILE
// World height = number of tile rows * TILE
// A solid floor is always auto-generated at the bottom.
//
// Usage:
//   LevelData level;
//   float worldH = TileLoader::load("assets/levels/level.txt", level);

class TileLoader {
public:
    static float load(const std::string& path, LevelData& level, float* worldW = nullptr);
};