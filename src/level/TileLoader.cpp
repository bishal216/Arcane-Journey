#include "level/TileLoader.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Constants.hpp"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static sf::Color platformColor(char sym) {
    switch (sym) {
        case '=':
            return PLAT_STATIC;
        case '~':
            return PLAT_MOVE_H;
        case '^':
            return PLAT_MOVE_V;
        case '@':
            return PLAT_CRUMBLE;
        case '*':
            return PLAT_BOUNCE;
        case '?':
            return PLAT_VANISH;
        case '-':
            return PLAT_ONEWAY;
        case 'H':
            return PLAT_HATCH;
        case 'R':
            return PLAT_ROPE;
        case 'L':
            return PLAT_LADDER;
        case 'C':
            return PLAT_COIN;
        case 'T':
            return PLAT_TELEPORT;
        case 'S':
            return PLAT_SINK;
        case 'G':
            return PLAT_GOAL;
        case 'W':  // Wind — SKY with transparency
            return sf::Color(SKY.r, SKY.g, SKY.b, 180);
        case 'F':  // Fan — SKY with transparency
            return sf::Color(SKY.r, SKY.g, SKY.b, 160);
        case 'K':  // Key Door — ROSE
            return PLAT_KEY_DOOR;
        case 'U':  // Key Switch — AMBER
            return PLAT_KEY_SWITCH;
        default:
            return sf::Color(128, 128, 128);
    }
}

static constexpr float MOVE_SPEED_H = 80.f;
static constexpr float MOVE_SPEED_V = 80.f;
static constexpr float MOVE_TRAVEL_H = 200.f;
static constexpr float MOVE_TRAVEL_V = 150.f;
static constexpr float DISAP_ON = 1.8f;
static constexpr float DISAP_OFF = 1.4f;
static constexpr float DISAP_FADE = 0.3f;

// New platform constants
static constexpr float WIND_STRENGTH = 400.f;    // px/s² horizontal acceleration
static constexpr float FAN_STRENGTH = 600.f;     // px/s² upward acceleration

struct Span {
    int col, end;
    char sym;
};

static std::vector<Span> rowSpans(const std::string& row) {
    std::vector<Span> spans;
    int i = 0, n = (int)row.size();
    while (i < n) {
        char c = row[i];
        if (c == '.') {
            ++i;
            continue;
        }
        int start = i;
        while (i < n && row[i] == c) ++i;
        spans.push_back({start, i, c});
    }
    return spans;
}

static bool isSectionMarker(const std::string& line, std::string& nameOut) {
    if (line.size() >= 2 && line.front() == '[' && line.back() == ']') {
        nameOut = line.substr(1, line.size() - 2);
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// TileLoader::load
// ---------------------------------------------------------------------------

float TileLoader::load(const std::string& path, LevelData& level, float* worldW) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("TileLoader: cannot open " + path);

    struct RawRow {
        bool isSection;
        std::string text;
    };
    std::vector<RawRow> rawRows;

    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::string sectionName;
        if (isSectionMarker(line, sectionName))
            rawRows.push_back({true, sectionName});
        else
            rawRows.push_back({false, line});
    }

    if (rawRows.empty()) throw std::runtime_error("TileLoader: no content in " + path);

    // World width from the first tile row
    int numCols = 0;
    for (const auto& r : rawRows)
        if (!r.isSection) {
            numCols = (int)r.text.size();
            break;
        }

    if (numCols == 0) throw std::runtime_error("TileLoader: no tile rows found");

    int numTileRows = 0;
    for (const auto& r : rawRows)
        if (!r.isSection) ++numTileRows;

    int T = TILE;
    float worldH = (float)(numTileRows * T);

    int tileRow = 0;
    float disappearPhase = 0.f;
    std::vector<std::string> grid;  // normalised tile rows, used to merge ropes/ladders

    for (const auto& raw : rawRows) {
        if (raw.isSection) {
            float sectionY = tileRow * T;  // top-down: row 0 = top of world
            level.addSection(sectionY, raw.text);
            continue;
        }

        // Normalise row width — pad short rows with air, trim long ones
        std::string row = raw.text;
        if ((int)row.size() < numCols)
            row.resize(numCols, '.');
        else if ((int)row.size() > numCols)
            row.resize(numCols);

        grid.push_back(row);
        float tileTop = tileRow * T;

        for (const auto& sp : rowSpans(row)) {
            float x = sp.col * T;
            float w = (sp.end - sp.col) * T;
            float y = tileTop;
            sf::Color col = platformColor(sp.sym);

            switch (sp.sym) {
                case '=':
                    level.addStatic(x, y, w, T, col);
                    break;
                case 'G':
                    level.addGoal(x, y, w, T);
                    break;
                case '@':
                    level.addCrumbling(x, y, w, T, col);
                    break;
                case '-':
                    level.addTrapdoorOneWay(x, y, w, T, col);
                    break;
                case 'H':
                    level.addTrapdoorHatch(x, y, w, T, col);
                    break;
                case 'S':
                    level.addSinking(x, y, w, T, col);
                    break;
                case 'W': {
                    // Wind: direction based on row position (alternating for variety)
                    // Even rows = right, odd rows = left, or use a simple pattern
                    int dir = ((int)(y / T) % 2 == 0) ? 1 : -1;
                    level.addWind(x, y, w, T, dir, WIND_STRENGTH, col);
                    break;
                }
                case 'F': {
                    level.addFan(x, y, w, T, FAN_STRENGTH, col);
                    break;
                }

                case '*':
                    level.addBounce(x, y, w, T, 920.f, col);
                    break;

                case '?':
                    level.addDisappearing(x, y, w, T, col, DISAP_ON, DISAP_OFF, DISAP_FADE,
                                          disappearPhase);
                    disappearPhase += 0.6f;
                    break;

                case '~': {
                    float cx = x + w * 0.5f;
                    level.addMoving(x, y, w, T, {cx - MOVE_TRAVEL_H, y}, {cx + MOVE_TRAVEL_H, y},
                                    MOVE_SPEED_H, col);
                    break;
                }

                case '^': {
                    level.addMoving(x, y, w, T, {x, y - MOVE_TRAVEL_V}, {x, y + MOVE_TRAVEL_V},
                                    MOVE_SPEED_V, col);
                    break;
                }

                case 'R':
                case 'L':
                    // Handled after this loop: vertical runs are merged into one climbable.
                    break;

                case 'T':
                    level.addTeleport(x, y, w, T);
                    break;

                case 'C':
                    for (int ci = sp.col; ci < sp.end; ++ci)
                        level.addCoin(ci * T + T / 2, y + T / 2);
                    break;
                case 'K': {  // Key Door — paired with switches by index
                    static int doorCounter = 0;
                    level.addKeyDoor(x, y, w, T, std::to_string(doorCounter), doorCounter);
                    ++doorCounter;
                    break;
                }
                case 'U': {  // Key Switch (Unlocker) — paired with doors by index
                    static int switchCounter = 0;
                    level.addKeySwitch(x, y, w, T, std::to_string(switchCounter));
                    ++switchCounter;
                    break;
                }

                default:
                    break;
            }
        }

        ++tileRow;
    }

    // Ropes and ladders: merge each vertical run of the same symbol in a column into ONE
    // climbable. Per-row 32px segments can't be climbed past the first seam, because the
    // climb clamp confines the (32px tall) player to a single segment.
    for (int c = 0; c < numCols; ++c) {
        int r = 0;
        while (r < (int)grid.size()) {
            char s = grid[r][c];
            if (s != 'R' && s != 'L') {
                ++r;
                continue;
            }
            int start = r;
            while (r < (int)grid.size() && grid[r][c] == s) ++r;
            float cx = c * T + T * 0.5f;
            float topY = start * T;
            float h = (r - start) * T;
            if (s == 'R')
                level.addRope(cx, topY, h, platformColor('R'));
            else
                level.addLadder(cx, topY, h, platformColor('L'));
        }
    }

    // Guaranteed solid floor at the very bottom
    float floorY = (float)(numTileRows - 1) * T;
    level.addStatic(0.f, floorY, (float)(numCols * T), (float)T, PLAT_STATIC);

    if (worldW) *worldW = (float)(numCols * T);
    level.sortSections();
    level.finalize();
    return worldH;
}