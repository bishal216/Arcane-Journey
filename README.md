# Arcane Journey

A vertical platformer built with C++ and SFML 3. Climb through 22 tarot-themed sections from the Training Grounds to The World, mastering movement mechanics along the way.

## Controls

| Input | Action |
|-------|--------|
| Arrow keys / WASD | Move |
| Space / W / Up | Jump (double jump available) |
| Shift | Dash |
| Up / Down near rope or ladder | Climb (ladders are fast; ropes are slower but launch you sideways if you jump off holding a direction) |
| Down + Space on a one-way platform | Drop through it |
| E | Interact with NPC |
| Escape | Close panel |
| R | Restart run |

## Platform Types

| Symbol | Type | Behaviour |
|--------|------|-----------|
| `=` | Static | Solid platform — won't move or break |
| `~` | Moving horizontal | Rides left and right, carries player |
| `^` | Moving vertical | Rises and falls, carries player |
| `@` | Crumbling | Shakes then breaks on contact, respawns |
| `*` | Bounce pad | Launches player upward on contact |
| `?` | Disappearing | Fades in and out on a timer |
| `-` | One-way | Passable from below, solid from above (Down + Space to drop through) |
| `H` | Hatch | Swings open on landing, snaps shut |
| `R` | Rope | Climbable vertical rope — slower, sideways launch when jumping off |
| `L` | Ladder | Climbable vertical ladder — fast. Vertical runs of `R`/`L` form one rope/ladder |
| `T` | Teleport | Stand still for 0.5s to teleport to paired block |
| `C` | Coin | Collectible — spend at the Merchant |
| `G` | Goal | Reach this to win the run |

## Hub NPCs

The hub at the bottom of the world has five NPCs, each opened with **E**:

| NPC | Panel |
|-----|-------|
| **Merchant** | Buy cosmetics with coins |
| **Tailor** | Equip unlocked cosmetics |
| **Chronicler** | View your best run time |
| **Seer** | Read lore for each tarot section |
| **Sage** | Animated guide to every platform type (unlocks as you discover them) |

## Level Editing

The level is defined in `assets/levels/level.txt`. Each character is one **32×32 pixel tile**. The file is read top-to-bottom (first row = top of world). Contiguous runs of the same symbol on a row are merged into one platform. A solid floor is auto-generated at the bottom.

```
# Lines starting with # are comments
[Section Name]        ← section marker (no tile row consumed)
..........====........  ← static platforms
.....C................  ← coins
...--------...........  ← one-way platforms
```

Edit and save — no recompile needed. The build copies `assets/` automatically.

### Teleport pairing

Teleport blocks (`T`) are paired sequentially by order of appearance in the file: first `T` ↔ second `T`, third ↔ fourth, and so on. If the number of `T` spans is odd, the last one has no partner: it is drawn with a red outline and a warning is printed to the console at startup.

## Project Structure

```
ArcaneJourney/
├── assets/
│   └── levels/
│       └── level.txt           # tile map — edit to design levels
├── src/
│   ├── main.cpp
│   ├── Constants.hpp           # TILE size, window dimensions, physics tuning
│   ├── core/
│   │   ├── Player.hpp/cpp      # movement, dash, wall jump, climb, double jump
│   │   ├── Camera.hpp/cpp      # world-clamped scrolling camera
│   │   └── World.hpp/cpp       # static platforms, sections, coin spawns
│   ├── level/
│   │   ├── LevelData.hpp/cpp   # manager hub — owns all platform managers
│   │   ├── TileLoader.hpp/cpp  # parses level.txt → LevelData
│   │   ├── MovingPlatform.hpp/cpp
│   │   ├── CrumblingPlatform.hpp/cpp
│   │   ├── BouncePad.hpp/cpp
│   │   ├── DisappearingPlatform.hpp/cpp
│   │   ├── TeleportBlock.hpp/cpp
│   │   ├── Climbable.hpp/cpp
│   │   └── Trapdoor.hpp/cpp
│   ├── ui/
│   │   ├── HubUI.hpp/cpp       # all NPC panels (shop, cosmetics, lore, sage, time)
│   │   ├── PlatformDemo.hpp/cpp # animated platform previews in Sage panel
│   │   ├── Npc.hpp/cpp         # hub NPC rendering and proximity detection
│   │   ├── CoinManager.hpp/cpp
│   │   ├── CosmeticsManager.hpp/cpp
│   │   ├── SectionAnnouncer.hpp/cpp
│   │   └── Coin.hpp
│   ├── systems/
│   │   ├── DiscoveryTracker.hpp/cpp  # tracks which platform types player has touched
│   │   └── SaveData.hpp/cpp          # binary save file (coins, best time, cosmetics, discoveries)
│   └── panels/                 # reserved for future HubUI panel split-out
└── CMakeLists.txt
```

## Dependencies

- [SFML 3](https://www.sfml-dev.org/) — graphics, window, input
- [MinGW-w64 / MSYS2 UCRT64](https://www.msys2.org/) — compiler toolchain (Windows)

## Building

```bash
./rebuild.sh
```

Or manually:

```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

The build copies `assets/` into the build directory automatically. The executable is at `build/ArcaneJourney.exe`.

## Save File

Progress is saved to `save.dat` next to the executable. The following persists across restarts:

- Total coins collected
- Best run time
- Unlocked and equipped cosmetics
- Discovered platform types (Sage panel)