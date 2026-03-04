# Arcane Journey

A vertical platformer built with C++ and SFML 3. Climb through 22 tarot-themed sections from the ground to the goal, mastering movement mechanics along the way.

## Gameplay

- **Arrow keys / WASD** — move
- **Space / W / Up** — jump (double jump available)
- **Shift** — dash
- **Up / Down while near a rope or ladder** — climb

## Platform Types

| Symbol | Type | Behaviour |
|--------|------|-----------|
| `#` | Static | Normal solid platform |
| `~` | Moving horizontal | Bounces left and right, carries player |
| `^` | Moving vertical | Bounces up and down, carries player |
| `@` | Crumbling | Shakes then breaks on contact, respawns |
| `*` | Bounce pad | Launches player upward |
| `?` | Disappearing | Blinks in and out on a timer |
| `-` | One-way | Passable from below, solid from above |
| `H` | Hatch | Swings open on landing, snaps shut |
| `R` | Rope | Climbable vertical rope |
| `L` | Ladder | Climbable vertical ladder |
| `W` | Wind zone | Applies directional force to player |
| `C` | Coin | Collectible |
| `G` | Goal | Reach this to win |

## Level Editing

The level is defined in `assets/levels/level.txt`. Each character is a 16×16 pixel tile. The file is read top-to-bottom (first row = top of world, last row = floor). Contiguous runs of the same symbol on the same row are merged into one platform.

Example snippet:
```
..........#########..........
.....C................C......
...--------....--------......
.....................................
##############.......############
```

Edit and save the file — no recompile needed.

## Project Structure

```
ArcaneJourney/
├── assets/
│   └── levels/
│       └── level.txt       # tile map — edit this to design levels
├── src/
│   ├── main.cpp
│   ├── Constants.hpp
│   ├── core/
│   │   ├── Player.hpp/cpp  # movement, dash, climb, double jump
│   │   ├── Camera.hpp/cpp  # scrolling camera
│   │   └── World.hpp/cpp   # static platforms and section definitions
│   ├── level/
│   │   ├── LevelData.hpp/cpp        # runtime manager hub
│   │   ├── TileLoader.hpp/cpp       # parses level.txt → LevelData
│   │   ├── MovingPlatform.hpp/cpp
│   │   ├── CrumblingPlatform.hpp/cpp
│   │   ├── BouncePad.hpp/cpp
│   │   ├── DisappearingPlatform.hpp/cpp
│   │   ├── WindZone.hpp/cpp
│   │   ├── Climbable.hpp/cpp
│   │   └── Trapdoor.hpp/cpp
│   └── ui/
│       ├── SectionAnnouncer.hpp/cpp
│       ├── CoinManager.hpp/cpp
│       ├── CosmeticsManager.hpp/cpp
│       ├── HubZone.hpp/cpp
│       └── HubUI.hpp/cpp
└── CMakeLists.txt
```

## Dependencies

- [SFML 3](https://www.sfml-dev.org/) — graphics, window, input

## Building

```bash
cmake -B build
cmake --build build
```

The build copies `assets/` into the build directory automatically. The executable is at `build/ArcaneJourney.exe` (Windows) or `build/ArcaneJourney` (Linux/macOS).