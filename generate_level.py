#!/usr/bin/env python3
"""
Arcane Journey Level Generator — Full Scale
Generates the complete level.txt with proper vertical platformer scale (~2,510 rows).
"""

import random

# Constants
TILE_W = 32
WORLD_W_TILES = 100  # 100 tiles = 3200px wide
AIR = '.'

# Platform symbols
STATIC = '='
MOVE_H = '~'
MOVE_V = '^'
CRUMBLE = '@'
BOUNCE = '*'
DISAPPEAR = '?'
ONE_WAY = '-'
HATCH = 'H'
ROPE = 'R'
LADDER = 'L'
TELEPORT = 'T'
COIN = 'C'
GOAL = 'G'
SINK = 'S'
WIND = 'W'
FAN = 'F'
KEY_DOOR = 'K'
KEY_SWITCH = 'U'

def make_row(pattern_dict, width=WORLD_W_TILES):
    """Create a row from a dict of {col: symbol} or list of (start, end, symbol)."""
    row = [AIR] * width
    for item in pattern_dict:
        if isinstance(item, tuple):
            start, end, sym = item
            for c in range(start, min(end, width)):
                row[c] = sym
        else:
            col, sym = item
            if 0 <= col < width:
                row[col] = sym
    return ''.join(row)

def platform_run(sym, start, length):
    """Helper: (start, end, symbol)"""
    return (start, start + length, sym)

def coin_at(col):
    return (col, col+1, COIN)

def section_header(name):
    return f"\n[{name}]\n"

# ============================================================
# SECTION GENERATORS — Parameterized by row count
# ============================================================

def gen_training_grounds(rows=120):
    """Training Grounds — Fundamentals tutorial"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        zone = i // 15  # 8 zones of ~15 rows each

        if zone == 0:  # Rows 0-14: Entry corridor
            if i % 3 == 0:
                for c in range(10, 90, 10):
                    row[c] = COIN
            if i == 5:
                row[15:25] = [ONE_WAY] * 10
                row[40:50] = [ONE_WAY] * 10
                row[70:80] = [ONE_WAY] * 10

        elif zone == 1:  # Rows 15-29: Climb Academy
            row[15] = ROPE; row[16] = ROPE
            row[40] = LADDER; row[41] = LADDER
            row[65] = ROPE; row[66] = ROPE
            row[90] = LADDER; row[91] = LADDER
            for c in [22, 24, 26, 28, 30, 32, 34, 36, 38, 55, 57, 59, 61, 63, 78, 80, 82, 84, 86]:
                row[c] = COIN

        elif zone == 2:  # Rows 30-44: Moving Platforms 101
            if i % 2 == 0:
                row[10:18] = [MOVE_H] * 8
                row[30:38] = [MOVE_H] * 8
            else:
                row[60:68] = [MOVE_V] * 8
                row[80:88] = [MOVE_V] * 8

        elif zone == 3:  # Rows 45-59: Hazard Sampler
            hazards = [
                (BOUNCE, 10, 8), (DISAPPEAR, 25, 8), (CRUMBLE, 40, 8),
                (HATCH, 55, 8), (SINK, 70, 8), (WIND, 85, 2), (FAN, 90, 5)
            ]
            for sym, start, length in hazards:
                row[start:start+length] = [sym] * length

        elif zone == 4:  # Rows 60-74: Teleport + Wind/Fan
            if i % 3 == 0:
                row[20:24] = [TELEPORT] * 4
                row[76:80] = [TELEPORT] * 4
            elif i % 3 == 1:
                for x in range(5, 95, 15):
                    row[x] = WIND
            else:
                for x in [15, 40, 65, 90]:
                    row[x:x+4] = [FAN] * 4

        elif zone == 5:  # Rows 75-89: Key Door + Switch
            if i < 4:
                row[30:36] = [KEY_DOOR] * 6
            if i == 0:
                row[65] = KEY_SWITCH

        elif zone == 6:  # Rows 90-104: Free Play (all types)
            all_types = [STATIC, MOVE_H, MOVE_V, CRUMBLE, BOUNCE, DISAPPEAR,
                         ONE_WAY, HATCH, ROPE, LADDER, TELEPORT, SINK, WIND, FAN]
            for col in range(5, 95, 8):
                sym = random.choice(all_types)
                row[col] = sym
                if col < 90:
                    row[col+1] = sym
            for col in range(3, 97, 4):
                row[col] = COIN

        else:  # Rows 105-119: Practice Arena
            for c in range(2, 98, 3):
                row[c] = COIN
            if i % 4 == 0:
                for x in range(5, 95, 12):
                    row[x:x+3] = [BOUNCE] * 3
            elif i % 4 == 1:
                for x in range(10, 90, 15):
                    row[x:x+8] = [FAN] * 8

        out.append(''.join(row))
    return out

def gen_section_standard(name, rows, theme_platforms, coins_per_row=3, special_rows=None):
    """Generic section generator with theme platforms."""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        # Special row overrides
        if special_rows and i in special_rows:
            for item in special_rows[i]:
                if len(item) == 3:
                    sym, start, length = item
                    row[start:start+length] = [sym] * length
                elif len(item) == 2:
                    sym, pos = item
                    row[pos] = sym
        else:
            # Standard themed placement
            for x in range(5, 95, 12):
                sym = random.choice(theme_platforms)
                row[x:x+4] = [sym] * 4

        # Coins
        for c in range(5, 95, max(1, 100 // coins_per_row)):
            row[c] = COIN

        out.append(''.join(row))
    return out

def gen_the_fool(rows=100):
    """The Fool — Leap of Faith: Teleport chains, blind jumps"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        phase = i // 25  # 4 phases of 25 rows

        if phase == 0:  # Rows 0-24: Teleport chains
            if i % 5 == 0:
                for x in [10, 35, 60, 85]:
                    row[x:x+4] = [TELEPORT] * 4
            elif i % 5 == 1:
                for x in [12, 37, 62, 87]:
                    row[x:x+3] = [BOUNCE] * 3
            elif i % 5 == 2:
                for x in [5, 25, 45, 65, 85]:
                    row[x:x+5] = [DISAPPEAR] * 5
            else:
                for x in range(0, 100, 10):
                    row[x] = WIND

        elif phase == 1:  # Rows 25-49: Blind teleport chains
            if i % 4 == 0:
                for x in [15, 45, 75]:
                    row[x:x+4] = [TELEPORT] * 4
            elif i % 4 == 1:
                for x in [17, 47, 77]:
                    row[x:x+3] = [BOUNCE] * 3
            elif i % 4 == 2:
                for x in range(0, 100, 15):
                    row[x:x+2] = [CRUMBLE] * 2
            else:
                for x in [10, 30, 50, 70, 90]:
                    row[x] = WIND
                    row[x+1] = WIND

        elif phase == 2:  # Rows 50-74: Fan-assisted teleports
            if i % 3 == 0:
                for x in [20, 50, 80]:
                    row[x:x+4] = [TELEPORT] * 4
            elif i % 3 == 1:
                for x in [15, 45, 75]:
                    row[x:x+5] = [FAN] * 5
            else:
                for x in range(0, 100, 12):
                    row[x] = WIND

        else:  # Rows 75-99: Final gauntlet
            if i % 2 == 0:
                for x in [10, 30, 50, 70, 90]:
                    row[x:x+4] = [TELEPORT] * 4
            else:
                for x in [12, 32, 52, 72, 92]:
                    row[x:x+3] = [BOUNCE] * 3
                for x in range(5, 95, 10):
                    row[x] = WIND

        out.append(''.join(row))
    return out

def gen_the_magician(rows=100):
    """The Magician — Toolkit: All 17 types in deliberate combos"""
    out = []
    combos = [
        # Screen 1: Rope + Fan + Moving H
        [ROPE, FAN, MOVE_H],
        # Screen 2: Teleport + Crumbling + Wind
        [TELEPORT, CRUMBLE, WIND],
        # Screen 3: Key Door + Disappearing + Bounce
        [KEY_DOOR, DISAPPEAR, BOUNCE],
        # Screen 4: Sinking + Moving V + Teleport
        [SINK, MOVE_V, TELEPORT],
        # Screen 5: Rope + Fan + Moving H (variation)
        [ROPE, FAN, MOVE_H],
        # Screen 6: All types mixed
        [STATIC, MOVE_H, MOVE_V, CRUMBLE, BOUNCE, DISAPPEAR,
         ONE_WAY, HATCH, ROPE, LADDER, TELEPORT, SINK, WIND, FAN, KEY_DOOR, KEY_SWITCH],
        # Screen 7: Wind + Fan + Bounce + Teleport
        [WIND, FAN, BOUNCE, TELEPORT],
    ]

    rows_per_combo = rows // len(combos)
    remainder = rows % len(combos)
    for combo_idx, combo in enumerate(combos):
        combo_rows = rows_per_combo + (1 if combo_idx < remainder else 0)
        for i in range(combo_rows):
            row = [AIR] * WORLD_W_TILES
            row[0:100] = [STATIC] * 100
            for x in range(5, 95, 10):
                sym = random.choice(combo)
                row[x:x+4] = [sym] * 4
            for c in range(5, 95, 7):
                row[c] = COIN
            out.append(''.join(row))
    return out

def gen_high_priestess(rows=100):
    """High Priestess — Hidden Paths: Key Doors, Disappearing, One-Way"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 25 < 5:
            # Key Doors at different positions
            row[10:16] = [KEY_DOOR] * 6
            row[40:46] = [KEY_DOOR] * 6
            row[70:76] = [KEY_DOOR] * 6
        elif i % 25 < 10:
            # Switch only for first door
            if i % 5 == 0:
                row[13] = KEY_SWITCH
        elif i % 25 < 15:
            # Disappearing platforms hiding paths
            for x in [5, 25, 45, 65, 85]:
                row[x:x+6] = [DISAPPEAR] * 6
        elif i % 25 < 20:
            # One-Way platforms as hidden passages
            row[15:25] = [ONE_WAY] * 10
            row[55:65] = [ONE_WAY] * 10
        else:
            # Teleport hidden behind disappearing
            row[5:9] = [TELEPORT] * 4

        out.append(''.join(row))
    return out

def gen_empress(rows=100):
    """The Empress — Abundance: Coins everywhere, Bounce, Fan, Moving H"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        # Dense coins everywhere
        for c in range(2, 98, 3):
            row[c] = COIN

        if i % 4 == 0:
            for x in range(5, 95, 10):
                row[x:x+4] = [BOUNCE] * 4
        elif i % 4 == 1:
            for x in range(10, 90, 12):
                row[x:x+6] = [FAN] * 6
        elif i % 4 == 2:
            for x in range(0, 90, 15):
                row[x:x+8] = [MOVE_H] * 8

        out.append(''.join(row))
    return out

def gen_emperor(rows=100):
    """The Emperor — Precision: Moving V, Static, Crumbling, Sinking"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        # Precise vertical shaft - only 8 tiles wide in center
        row[46] = STATIC
        row[53] = STATIC

        if i % 5 == 0:
            row[47:53] = [MOVE_V] * 6
        elif i % 5 == 1:
            row[47:53] = [CRUMBLE] * 6
        elif i % 5 == 2:
            row[47:53] = [SINK] * 6
        elif i % 5 == 3:
            row[47:50] = [STATIC] * 3
            row[50:53] = [STATIC] * 3
        else:
            row[47:53] = [ONE_WAY] * 6
        out.append(''.join(row))
    return out

def gen_hierophant(rows=100):
    """Hierophant — Ritual: Rope, Ladder, Sinking, Hatch"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 4 == 0:
            row[15] = ROPE; row[16] = ROPE
            row[40] = LADDER; row[41] = LADDER
        elif i % 4 == 1:
            row[30:40] = [SINK] * 10
        elif i % 4 == 2:
            row[20:30] = [HATCH] * 10
        else:
            for x in [10, 25, 40, 55, 70, 85]:
                if x % 2 == 0:
                    row[x] = ROPE; row[x+1] = ROPE
                else:
                    row[x] = LADDER; row[x+1] = LADDER
        out.append(''.join(row))
    return out

def gen_lovers(rows=100):
    """The Lovers — Dual Paths: Teleport, One-Way split, Key Door"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 2 == 0:
            # Left path - Teleports (easy)
            row[10:14] = [TELEPORT] * 4
            row[14:24] = [ONE_WAY] * 10
            for c in range(10, 20):
                row[c] = COIN
            # Right path - harder climb
            row[76:80] = [TELEPORT] * 4
            row[70:76] = [CRUMBLE] * 6
            for c in range(70, 85):
                row[c] = COIN
        else:
            row[25:35] = [ONE_WAY] * 10
            row[65:75] = [ONE_WAY] * 10
            if i % 10 == 5:
                row[10:16] = [KEY_DOOR] * 6
                row[74:80] = [KEY_DOOR] * 6
                row[40] = KEY_SWITCH
        out.append(''.join(row))
    return out

def gen_chariot(rows=100):
    """The Chariot — Momentum: Moving H, Wind, Bounce, dash-heavy"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 3 == 0:
            for x in range(0, 90, 20):
                row[x:x+15] = [MOVE_H] * 15
            for x in range(5, 95, 10):
                row[x] = WIND
        elif i % 3 == 1:
            for x in range(5, 95, 12):
                row[x:x+3] = [BOUNCE] * 3
            for x in [25, 50, 75]:
                row[x] = WIND; row[x+1] = WIND
        else:
            for x in range(0, 90, 18):
                row[x:x+10] = [MOVE_H] * 10
            for x in range(8, 95, 18):
                row[x:x+2] = [BOUNCE] * 2
        out.append(''.join(row))
    return out

def gen_strength(rows=110):
    """Strength — Endurance: Crumbling, Sinking, Disappearing, Moving V"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 5 == 0:
            for x in range(10, 90, 15):
                row[x:x+8] = [CRUMBLE] * 8
        elif i % 5 == 1:
            for x in range(15, 85, 14):
                row[x:x+6] = [SINK] * 6
        elif i % 5 == 2:
            for x in range(5, 95, 18):
                row[x:x+10] = [DISAPPEAR] * 10
        elif i % 5 == 3:
            for x in range(20, 80, 12):
                row[x:x+4] = [MOVE_V] * 4
        else:
            row[20:80] = [STATIC] * 60
        out.append(''.join(row))
    return out

def gen_hermit(rows=110):
    """The Hermit — Solitude: Ladder, Fan, Teleport, sparse coins"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        for c in range(46, 54):
            row[c] = STATIC

        if i % 4 == 0:
            row[49] = LADDER; row[50] = LADDER
        elif i % 4 == 1:
            row[46:54] = [FAN] * 8
        elif i % 4 == 2:
            row[48:52] = [TELEPORT] * 4
        else:
            row[49] = COIN
        out.append(''.join(row))
    return out

def gen_wheel_of_fortune(rows=110):
    """Wheel of Fortune — Cycles: Moving V, Teleport, Crumbling, Moving H"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        phase = i // 11  # 10 rows per phase, 10 phases
        if phase % 4 == 0:
            for x in range(10, 90, 15):
                row[x:x+5] = [MOVE_V] * 5
        elif phase % 4 == 1:
            for x in [15, 40, 65, 90]:
                row[x:x+4] = [TELEPORT] * 4
        elif phase % 4 == 2:
            for x in range(5, 95, 20):
                row[x:x+8] = [CRUMBLE] * 8
        else:
            for x in range(0, 90, 18):
                row[x:x+12] = [MOVE_H] * 12
        out.append(''.join(row))
    return out

def gen_justice(rows=110):
    """Justice — Balance: Mirrored layouts, One-Way pairs, Key Door"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 4 == 0:
            row[10:20] = [ONE_WAY] * 10
            row[80:90] = [ONE_WAY] * 10
        elif i % 4 == 1:
            row[15:25] = [MOVE_H] * 10
            row[75:85] = [MOVE_H] * 10
        elif i % 4 == 2:
            row[20:28] = [CRUMBLE] * 8
            row[72:80] = [CRUMBLE] * 8
        else:
            row[30:36] = [KEY_DOOR] * 6
            row[64:70] = [KEY_DOOR] * 6
            row[47] = KEY_SWITCH
            row[53] = KEY_SWITCH
        out.append(''.join(row))
    return out

def gen_hanged_man(rows=110):
    """Hanged Man — Surrender: Fan, Disappearing, Teleport, Wind"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 4 == 0:
            for x in [20, 40, 60, 80]:
                row[x:x+5] = [FAN] * 5
        elif i % 4 == 1:
            for x in [15, 35, 55, 75]:
                row[x:x+8] = [DISAPPEAR] * 8
        elif i % 4 == 2:
            row[10:14] = [TELEPORT] * 4
            row[86:90] = [TELEPORT] * 4
        else:
            for x in [18, 38, 58, 78]:
                row[x] = WIND; row[x+1] = WIND
        out.append(''.join(row))
    return out

def gen_death(rows=110):
    """Death — Brutal: Crumbling, Sinking, One-Way punishing"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 3 == 0:
            for x in range(5, 95, 10):
                row[x:x+6] = [CRUMBLE] * 6
        elif i % 3 == 1:
            for x in range(10, 90, 12):
                row[x:x+4] = [SINK] * 4
                row[x+6:x+10] = [CRUMBLE] * 4
        else:
            for x in range(15, 85, 14):
                row[x:x+8] = [ONE_WAY] * 8
        out.append(''.join(row))
    return out

def gen_temperance(rows=120):
    """Temperance — Alchemy: Hybrid combos"""
    out = []
    recipes = [
        [(ROPE, 15, 2), (FAN, 20, 8)],
        [(WIND, 10, 1), (WIND, 11, 1), (BOUNCE, 15, 4)],
        [(SINK, 30, 8), (TELEPORT, 40, 4)],
        [(KEY_DOOR, 50, 6), (DISAPPEAR, 60, 8)],
        [(ROPE, 40, 2), (FAN, 45, 8), (MOVE_H, 10, 10)],
        [(WIND, 5, 1), (WIND, 6, 1), (SINK, 15, 8), (TELEPORT, 25, 4)],
        [(FAN, 20, 8), (MOVE_H, 30, 10), (KEY_DOOR, 50, 6)],
        [(DISAPPEAR, 10, 6), (BOUNCE, 20, 4), (WIND, 30, 2)],
    ]

    rows_per_recipe = rows // len(recipes)
    for recipe in recipes:
        for i in range(rows_per_recipe):
            row = [AIR] * WORLD_W_TILES
            row[0:100] = [STATIC] * 100
            for item in recipe:
                if len(item) == 3:
                    sym, start, length = item
                    row[start:start+length] = [sym] * length
                elif len(item) == 2:
                    sym, pos = item
                    row[pos] = sym
            for c in range(5, 95, 7):
                row[c] = COIN
            out.append(''.join(row))
    return out

def gen_devil(rows=120):
    """The Devil — Traps: False walls, Hatch, Teleport traps, fake Key Doors"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 5 == 0:
            for x in range(10, 90, 15):
                row[x:x+3] = [COIN] * 3
                row[x+3:x+9] = [CRUMBLE] * 6
        elif i % 5 == 1:
            for x in range(20, 80, 15):
                row[x:x+5] = [HATCH] * 5
        elif i % 5 == 2:
            row[10:14] = [TELEPORT] * 4
            row[86:90] = [TELEPORT] * 4
        elif i % 5 == 3:
            row[20:26] = [KEY_DOOR] * 6
            row[74:80] = [KEY_DOOR] * 6
        else:
            for x in range(15, 85, 14):
                row[x:x+6] = [DISAPPEAR] * 6
        out.append(''.join(row))
    return out

def gen_tower(rows=120):
    """The Tower — Upheaval: Moving V, Crumbling, Teleport, Wind (chaotic)"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 4 == 0:
            for x in range(5, 95, 12):
                row[x:x+4] = [MOVE_V] * 4
        elif i % 4 == 1:
            for x in range(10, 90, 10):
                row[x:x+5] = [CRUMBLE] * 5
        elif i % 4 == 2:
            if i % 2 == 0:
                row[10:14] = [TELEPORT] * 4
                row[50:54] = [TELEPORT] * 4
            else:
                row[20:24] = [TELEPORT] * 4
                row[70:74] = [TELEPORT] * 4
        else:
            for x in range(0, 100, 7):
                row[x] = WIND
        out.append(''.join(row))
    return out

def gen_star(rows=120):
    """The Star — Hope: Fan, Bounce, wide Static, generous coins"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[10:90] = [STATIC] * 80

        for c in range(12, 88, 4):
            row[c] = COIN

        if i % 4 == 0:
            for x in range(15, 85, 14):
                row[x:x+6] = [FAN] * 6
        elif i % 4 == 1:
            for x in range(20, 80, 12):
                row[x:x+4] = [BOUNCE] * 4
        elif i % 4 == 2:
            for x in range(10, 80, 20):
                row[x:x+15] = [MOVE_H] * 15
        else:
            for x in [25, 35, 45, 55, 65, 75]:
                row[x] = COIN; row[x+1] = COIN
        out.append(''.join(row))
    return out

def gen_moon(rows=120):
    """The Moon — Illusion: Disappearing, Key Door misleading, Wind misleading, Teleport"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        if i % 4 == 0:
            for x in range(5, 95, 18):
                row[x:x+10] = [DISAPPEAR] * 10
        elif i % 4 == 1:
            row[10:16] = [KEY_DOOR] * 6  # Fake
            row[40:46] = [KEY_DOOR] * 6  # Fake
            row[70:76] = [KEY_DOOR] * 6  # Real
        elif i % 4 == 2:
            for x in [15, 35, 55, 75]:
                row[x] = WIND; row[x+1] = WIND
        else:
            row[10:14] = [TELEPORT] * 4
            row[50:54] = [TELEPORT] * 4
        out.append(''.join(row))
    return out

def gen_sun(rows=120):
    """The Sun — Joy: Bounce, Fan, Moving H, max coins, playground"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES
        row[0:100] = [STATIC] * 100

        for c in range(2, 98, 2):
            row[c] = COIN

        if i % 5 == 0:
            for x in range(5, 95, 10):
                row[x:x+3] = [BOUNCE] * 3
        elif i % 5 == 1:
            for x in range(10, 90, 15):
                row[x:x+8] = [FAN] * 8
        elif i % 5 == 2:
            for x in range(0, 90, 18):
                row[x:x+12] = [MOVE_H] * 12
        elif i % 5 == 3:
            for x in [15, 40, 65, 90]:
                row[x:x+4] = [TELEPORT] * 4
        else:
            for x in range(5, 95, 12):
                row[x:x+2] = [BOUNCE] * 2
                row[x+3:x+8] = [FAN] * 5
        out.append(''.join(row))
    return out

def gen_judgement(rows=120):
    """Judgement — Final Exam: All 17 types at max density"""
    out = []
    all_types = [STATIC, MOVE_H, MOVE_V, CRUMBLE, BOUNCE, DISAPPEAR,
                 ONE_WAY, HATCH, ROPE, LADDER, TELEPORT, SINK, WIND, FAN, KEY_DOOR, KEY_SWITCH]

    screens = [
        [ROPE, LADDER, FAN, MOVE_V, SINK],
        [MOVE_H, WIND, BOUNCE, CRUMBLE],
        [MOVE_V, CRUMBLE, DISAPPEAR, ONE_WAY, HATCH],
        [KEY_DOOR, KEY_SWITCH, TELEPORT, ONE_WAY, DISAPPEAR],
        all_types,
    ]

    rows_per_screen = rows // len(screens)
    for screen_idx, screen_types in enumerate(screens):
        for i in range(rows_per_screen):
            row = [AIR] * WORLD_W_TILES
            row[0:100] = [STATIC] * 100

            for x in range(5, 95, 8):
                sym = random.choice(screen_types)
                if sym == 'DASH':
                    sym = CRUMBLE
                row[x:x+4] = [sym] * 4

            for c in range(10, 90, 15):
                row[c] = COIN

            out.append(''.join(row))
    return out

def gen_the_world(rows=80):
    """The World — Summit: Boss chase (rising floor)"""
    out = []
    for i in range(rows):
        row = [AIR] * WORLD_W_TILES

        if i < 10:
            # Bottom - generous platforms
            row[10:90] = [STATIC] * 80
            for x in range(15, 85, 12):
                row[x:x+4] = [FAN] * 4
            for x in range(20, 80, 15):
                row[x:x+3] = [BOUNCE] * 3
            for x in [25, 50, 75]:
                row[x:x+4] = [TELEPORT] * 4
        elif i < 25:
            # Lower-mid - sparser, faster
            for x in range(10, 90, 18):
                row[x:x+6] = [MOVE_H] * 6
            for x in range(15, 85, 20):
                row[x:x+5] = [FAN] * 5
            for x in [30, 70]:
                row[x:x+4] = [TELEPORT] * 4
        elif i < 45:
            # Upper-mid - aggressive
            for x in range(5, 95, 20):
                row[x:x+4] = [CRUMBLE] * 4
            for x in range(10, 90, 25):
                row[x:x+3] = [FAN] * 3
            for x in [40, 60]:
                row[x:x+4] = [TELEPORT] * 4
        elif i < 65:
            # Upper - minimal
            for x in range(5, 95, 25):
                row[x:x+3] = [CRUMBLE] * 3
            for x in range(15, 85, 30):
                row[x:x+2] = [FAN] * 2
            for x in [50]:
                row[x:x+4] = [TELEPORT] * 4
        else:
            # Top - Goal
            row[45:55] = [GOAL] * 10
            row[40:45] = [STATIC] * 5
            row[55:60] = [STATIC] * 5
            row[38:40] = [FAN] * 2
            row[60:62] = [FAN] * 2
        out.append(''.join(row))
    return out

# ============================================================
# MAIN GENERATION
# ============================================================

def generate_level():
    """Generate complete level.txt with full-scale sections"""
    print("Generating Arcane Journey level.txt (Full Scale)...")

    # Section order: top to bottom in file = The World down to Training Grounds
    sections = [
        ("XXII - The World", gen_the_world(80)),
        ("XXI - Judgement", gen_judgement(120)),
        ("XX - The Sun", gen_sun(120)),
        ("XIX - The Moon", gen_moon(120)),
        ("XVIII - The Star", gen_star(120)),
        ("XVII - The Tower", gen_tower(120)),
        ("XVI - The Devil", gen_devil(120)),
        ("XV - Temperance", gen_temperance(120)),
        ("XIV - Death", gen_death(110)),
        ("XIII - The Hanged Man", gen_hanged_man(110)),
        ("XII - Justice", gen_justice(110)),
        ("XI - Wheel of Fortune", gen_wheel_of_fortune(110)),
        ("X - The Hermit", gen_hermit(110)),
        ("IX - Strength", gen_strength(110)),
        ("VIII - The Chariot", gen_chariot(100)),
        ("VII - The Lovers", gen_lovers(100)),
        ("VI - The Hierophant", gen_hierophant(100)),
        ("V - The Emperor", gen_emperor(100)),
        ("IV - The Empress", gen_empress(100)),
        ("III - The High Priestess", gen_high_priestess(100)),
        ("II - The Magician", gen_the_magician(100)),
        ("I - The Fool", gen_the_fool(100)),
        ("Training Grounds", gen_training_grounds(120)),
    ]

    output = []
    output.append("# Arcane Journey — level.txt (FULL SCALE)")
    output.append("# Each character = 32x32 pixels (1 tile)")
    output.append("# Lines starting with # are comments (ignored)")
    output.append("# [Section Name] = section marker")
    output.append("#")
    output.append("# . = air             = = static platform")
    output.append("# @ = crumbling       ~ = moving horizontal   ^ = moving vertical")
    output.append("# * = bounce pad      ? = disappearing        - = one-way")
    output.append("# S = sinking         H = hatch               R = rope                L = ladder")
    output.append("# T = teleport block  C = coin                G = goal")
    output.append("# W = wind            F = fan                 K = key door            U = key switch")
    output.append("")

    total_rows = 0
    for name, rows in sections:
        output.append(f"[{name}]")
        output.extend(rows)
        total_rows += len(rows)
        print(f"  {name}: {len(rows)} rows")

    # Solid floor at bottom
    output.append("=" * WORLD_W_TILES)
    total_rows += 1

    print(f"\nTotal tile rows: {total_rows}")
    print(f"World height: {total_rows * TILE_W}px")
    print(f"World width: {WORLD_W_TILES * TILE_W}px")

    return "\n".join(output)

if __name__ == "__main__":
    random.seed(42)
    level_text = generate_level()

    with open("assets/levels/level.txt", "w") as f:
        f.write(level_text)

    print("\n✓ Level written to assets/levels/level.txt")