# Core
git mv src/core/player.cpp src/core/Player.cpp
git mv src/core/player.hpp src/core/Player.hpp
git mv src/core/camera.cpp src/core/Camera.cpp
git mv src/core/camera.hpp src/core/Camera.hpp
git mv src/core/world.cpp  src/core/World.cpp
git mv src/core/world.hpp  src/core/World.hpp

# Level
git mv src/level/Bouncepad.cpp   src/level/BouncePad.cpp
git mv src/level/Bouncepad.hpp   src/level/BouncePad.hpp
git mv src/level/Crumblingplatform.cpp  src/level/CrumblingPlatform.cpp
git mv src/level/Crumblingplatform.hpp  src/level/CrumblingPlatform.hpp
git mv src/level/Disappearingplatform.cpp src/level/DisappearingPlatform.cpp
git mv src/level/Disappearingplatform.hpp src/level/DisappearingPlatform.hpp
git mv src/level/Leveldata.cpp   src/level/LevelData.cpp
git mv src/level/Leveldata.hpp   src/level/LevelData.hpp
git mv src/level/Movingplatform.cpp src/level/MovingPlatform.cpp
git mv src/level/Movingplatform.hpp src/level/MovingPlatform.hpp
git mv src/level/Teleportblock.cpp  src/level/TeleportBlock.cpp
git mv src/level/Teleportblock.hpp  src/level/TeleportBlock.hpp

# UI
git mv src/ui/coinManager.cpp    src/ui/CoinManager.cpp
git mv src/ui/coinManager.hpp    src/ui/CoinManager.hpp
git mv src/ui/coin.hpp           src/ui/Coin.hpp
git mv src/ui/cosmeticsManager.cpp src/ui/CosmeticsManager.cpp
git mv src/ui/cosmeticsManager.hpp src/ui/CosmeticsManager.hpp
git mv src/ui/npc.cpp            src/ui/Npc.cpp
git mv src/ui/npc.hpp            src/ui/Npc.hpp
git mv src/ui/sectionAnnouncer.cpp src/ui/SectionAnnouncer.cpp
git mv src/ui/sectionAnnouncer.hpp src/ui/SectionAnnouncer.hpp

git add -A
git commit -m "Fix file casing for Linux CI"
git push