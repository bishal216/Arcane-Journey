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

// ── Dash ─────────────────────────────────────────────────────────────────────
constexpr float DASH_SPEED = 780.f;
constexpr float DASH_DURATION = 0.13f;
constexpr float DASH_COOLDOWN = 0.22f;
