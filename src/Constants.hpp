#pragma once

// ── Window ──────────────────────────────────────────────────────────────────
constexpr unsigned WIN_W = 1024;
constexpr unsigned WIN_H = 576;

// ── Tile system ─────────────────────────────────────────────────────────────
constexpr int TILE = 32;       // pixels per tile
constexpr float TILEF = 32.f;  // float version for maths

// WORLD_H is gone — world height is always derived from the level file:
//   float worldH = TileLoader::load(...);
// Pass worldH wherever you previously used WORLD_H.

// ── Physics (all in pixels/sec) ─────────────────────────────────────────────
constexpr float GRAVITY = 2000.f;
constexpr float MAX_FALL = 1200.f;

// ── Movement ────────────────────────────────────────────────────────────────
constexpr float RUN_ACCEL = 2200.f;
constexpr float AIR_ACCEL = 1600.f;
constexpr float MAX_SPEED = 320.f;
constexpr float FRICTION = 1800.f;
constexpr float AIR_FRIC = 400.f;

// ── Jumping ──────────────────────────────────────────────────────────────────
constexpr float JUMP_VEL = -680.f;
constexpr float JUMP_HOLD_GRAV = 800.f;
constexpr float WALL_JUMP_X = 380.f;
constexpr float WALL_JUMP_Y = -620.f;
constexpr float ROPE_LAUNCH_X = 300.f;      // sideways launch when jumping off a rope holding a direction
constexpr float GRAB_COOLDOWN = 0.2f;       // seconds a rope/ladder can't be re-grabbed after jumping off it
constexpr float DROP_THROUGH_TIME = 0.25f;  // seconds one-way platforms are ignored after Down+Space

// ── Dash ─────────────────────────────────────────────────────────────────────
constexpr float DASH_SPEED = 780.f;
constexpr float DASH_DURATION = 0.13f;
constexpr float DASH_COOLDOWN = 0.22f;

// ── Colors (Consolidated 12-Color System) ───────────────────────────────────────
// Each color has exactly ONE job. No scales. No near-duplicates.
// Use these constants throughout the codebase instead of inline sf::Color values.

constexpr sf::Color VOID      = sf::Color(8,   4,   20);   // Background, overlays — the unknown
constexpr sf::Color PANEL     = sf::Color(30,  16,  56);   // All panels, cards — elevated surface
constexpr sf::Color ARCANE    = sf::Color(120, 80,  200);  // THE brand accent — focus, hover, outline, magic
constexpr sf::Color GOLD      = sf::Color(255, 215, 0);    // Player agency ONLY — coins, goals, equipped, purchase CTAs
constexpr sf::Color SILVER    = sf::Color(200, 200, 224);  // Secondary text, Seer accent, muted UI, Eyes cosmetic
constexpr sf::Color EMERALD   = sf::Color(46,  224, 112);  // Success, owned, Chronicler, Boots slot
constexpr sf::Color AMBER     = sf::Color(255, 184, 0);    // Warning, unaffordable, time pressure, Wings slot
constexpr sf::Color ROSE      = sf::Color(220, 60,  60);   // Danger, Devil's Pact, loss, unequip, Aura slot
constexpr sf::Color COPPER    = sf::Color(184, 112, 48);   // Earth/decay — crumbling, hatch, Hat slot
constexpr sf::Color TEAL      = sf::Color(40,  180, 170);  // Water/fluid — sinking, moving vertical
constexpr sf::Color SKY       = sf::Color(120, 210, 255);  // Air/wind — moving horizontal, wind, fan
constexpr sf::Color INK       = sf::Color(240, 232, 248);  // Primary text — never pure white

// Player state colors (used in Player.cpp)
constexpr sf::Color P_GROUNDED    = sf::Color(100, 180, 255);  // On ground, dash available
constexpr sf::Color P_AIRBORNE    = sf::Color(80,  140, 255);  // In air, dash available
constexpr sf::Color P_DASHING     = sf::Color(255, 120, 50);   // Dash active
constexpr sf::Color P_CLIMBING    = sf::Color(180, 220, 100);  // On rope/ladder
constexpr sf::Color P_DASH_CD     = sf::Color(160, 100, 220);  // Dash on cooldown

// NPC identity colors (used in Npc.cpp)
constexpr sf::Color NPC_MERCHANT_BODY = sf::Color(200, 160, 64);
constexpr sf::Color NPC_MERCHANT_HEAD = sf::Color(230, 190, 128);
constexpr sf::Color NPC_TAILOR_BODY   = sf::Color(160, 96,  200);
constexpr sf::Color NPC_TAILOR_HEAD   = sf::Color(200, 120, 220);
constexpr sf::Color NPC_CHRONICLER_BODY = sf::Color(40,  168, 200);
constexpr sf::Color NPC_CHRONICLER_HEAD = sf::Color(80,  200, 220);
constexpr sf::Color NPC_SEER_BODY     = sf::Color(180, 60,  60);
constexpr sf::Color NPC_SEER_HEAD     = sf::Color(220, 100, 100);
constexpr sf::Color NPC_SAGE_BODY     = sf::Color(60,  168, 80);
constexpr sf::Color NPC_SAGE_HEAD     = sf::Color(96,  220, 128);

// NPC shared
constexpr sf::Color NPC_EYE         = sf::Color(20, 10, 30);
constexpr sf::Color NPC_OUTLINE     = sf::Color(255, 255, 255, 60);
constexpr sf::Color NPC_HEAD_OUTLINE = sf::Color(255, 255, 255, 80);

// UI shared
constexpr sf::Color UI_OUTLINE      = ARCANE;   // 2px outline for all interactive panels
constexpr sf::Color UI_DIVIDER      = sf::Color(80, 60, 120);  // 1px divider
constexpr sf::Color UI_HINT         = sf::Color(100, 90, 140);
constexpr sf::Color UI_OVERLAY      = sf::Color(0, 0, 0, 180);
constexpr sf::Color UI_PANEL_BG     = PANEL;

// Platform material tints (shape does the work; color is subtle hint)
constexpr sf::Color PLAT_STATIC      = sf::Color(80, 180, 80);   // Green — safe/solid
constexpr sf::Color PLAT_MOVE_H      = SKY;                       // Sky — horizontal motion
constexpr sf::Color PLAT_MOVE_V      = TEAL;                      // Teal — vertical motion
constexpr sf::Color PLAT_CRUMBLE     = COPPER;                    // Copper — decay/earth
constexpr sf::Color PLAT_BOUNCE      = EMERALD;                   // Emerald — spring/life
constexpr sf::Color PLAT_VANISH      = ARCANE;                    // Arcane — magic
constexpr sf::Color PLAT_ONEWAY      = SILVER;                    // Silver — subtle
constexpr sf::Color PLAT_HATCH       = COPPER;                    // Copper — earth/mechanical
constexpr sf::Color PLAT_ROPE        = COPPER;                    // Copper — hemp/rope
constexpr sf::Color PLAT_LADDER      = COPPER;                    // Copper — wood/ladder
constexpr sf::Color PLAT_TELEPORT    = ARCANE;                    // Arcane — magic
constexpr sf::Color PLAT_COIN        = GOLD;                      // Gold — value
constexpr sf::Color PLAT_GOAL        = GOLD;                      // Gold — achievement
constexpr sf::Color PLAT_SINK        = TEAL;                      // Teal — water
constexpr sf::Color PLAT_KEY_DOOR    = ROSE;                      // Rose — locked/danger
constexpr sf::Color PLAT_KEY_SWITCH  = AMBER;                     // Amber — key/warning
constexpr sf::Color PLAT_WIND        = SKY;                       // Sky — air
constexpr sf::Color PLAT_FAN         = SKY;                       // Sky — air

// Cosmetic slot colors (used in ArtifactManager.cpp)
constexpr sf::Color SLOT_HAT    = COPPER;
constexpr sf::Color SLOT_BOOTS  = EMERALD;
constexpr sf::Color SLOT_CAPE   = ARCANE;
constexpr sf::Color SLOT_AURA   = ROSE;
constexpr sf::Color SLOT_WINGS  = AMBER;
constexpr sf::Color SLOT_EYES   = SILVER;

// Outline colors
constexpr sf::Color WHITE_OUTLINE_60  = sf::Color(255, 255, 255, 60);
constexpr sf::Color WHITE_OUTLINE_80  = sf::Color(255, 255, 255, 80);
constexpr sf::Color WHITE_OUTLINE_40  = sf::Color(255, 255, 255, 40);
