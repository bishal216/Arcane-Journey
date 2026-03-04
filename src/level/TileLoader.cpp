#include "level/TileLoader.hpp"
#include "Constants.hpp"
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static sf::Color platformColor(char sym) {
    switch (sym) {
        case '=': return sf::Color( 80, 190,  80);
        case '~': return sf::Color(120, 210, 255);
        case '^': return sf::Color(230, 210,  90);
        case '@': return sf::Color(160, 100,  50);
        case '*': return sf::Color( 80, 220, 120);
        case '?': return sf::Color(140,  80, 200);
        case '-': return sf::Color(120,  80, 160);
        case 'H': return sf::Color(100,  70,  35);
        case 'R': return sf::Color(200, 170, 100);
        case 'L': return sf::Color(140, 110,  60);
        case 'C': return sf::Color(255, 210,  40);
        case 'T': return sf::Color( 80,  40, 180);
        case 'G': return sf::Color(255, 215,   0);
        default:  return sf::Color(128, 128, 128);
    }
}

static constexpr float MOVE_SPEED_H  = 80.f;
static constexpr float MOVE_SPEED_V  = 80.f;
static constexpr float MOVE_TRAVEL_H = 200.f;
static constexpr float MOVE_TRAVEL_V = 150.f;
static constexpr float DISAP_ON      = 1.8f;
static constexpr float DISAP_OFF     = 1.4f;
static constexpr float DISAP_FADE    = 0.3f;

struct Span { int col, end; char sym; };

static std::vector<Span> rowSpans(const std::string& row) {
    std::vector<Span> spans;
    int i = 0, n = (int)row.size();
    while (i < n) {
        char c = row[i];
        if (c == '.') { ++i; continue; }
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

float TileLoader::load(const std::string& path, LevelData& level, float* worldW)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("TileLoader: cannot open " + path);

    struct RawRow { bool isSection; std::string text; };
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

    if (rawRows.empty())
        throw std::runtime_error("TileLoader: no content in " + path);

    // World width from the first tile row
    int numCols = 0;
    for (const auto& r : rawRows)
        if (!r.isSection) { numCols = (int)r.text.size(); break; }

    if (numCols == 0)
        throw std::runtime_error("TileLoader: no tile rows found");

    int numTileRows = 0;
    for (const auto& r : rawRows)
        if (!r.isSection) ++numTileRows;

    int   T      = TILE;
    float worldH = (float)(numTileRows * T);

    int   tileRow        = 0;
    float disappearPhase = 0.f;

    for (const auto& raw : rawRows) {
        if (raw.isSection) {
            float sectionY = tileRow * T;   // top-down: row 0 = top of world
            level.addSection(sectionY, raw.text);
            continue;
        }

        // Normalise row width — pad short rows with air, trim long ones
        std::string row = raw.text;
        if ((int)row.size() < numCols) row.resize(numCols, '.');
        else if ((int)row.size() > numCols) row.resize(numCols);

        float tileTop = tileRow * T;

        for (const auto& sp : rowSpans(row)) {
            float x   = sp.col * T;
            float w   = (sp.end - sp.col) * T;
            float y   = tileTop;
            sf::Color col = platformColor(sp.sym);

            switch (sp.sym) {
                case '=': level.addStatic(x, y, w, T, col);         break;
                case 'G': level.addGoal  (x, y, w, T);              break;
                case '@': level.addCrumbling(x, y, w, T, col);      break;
                case '-': level.addTrapdoorOneWay(x, y, w, T, col); break;
                case 'H': level.addTrapdoorHatch (x, y, w, T, col); break;

                case '*':
                    level.addBounce(x, y, w, T, 920.f, col);
                    break;

                case '?':
                    level.addDisappearing(x, y, w, T, col,
                                          DISAP_ON, DISAP_OFF, DISAP_FADE,
                                          disappearPhase);
                    disappearPhase += 0.6f;
                    break;

                case '~': {
                    float cx = x + w * 0.5f;
                    level.addMoving(x, y, w, T,
                                    { cx - MOVE_TRAVEL_H, y },
                                    { cx + MOVE_TRAVEL_H, y },
                                    MOVE_SPEED_H, col);
                    break;
                }

                case '^': {
                    level.addMoving(x, y, w, T,
                                    { x, y - MOVE_TRAVEL_V },
                                    { x, y + MOVE_TRAVEL_V },
                                    MOVE_SPEED_V, col);
                    break;
                }

                case 'R':
                    level.addRope(x + T * 0.5f, y, w, col);
                    break;

                case 'L':
                    level.addLadder(x + T * 0.5f, y, w, col);
                    break;

                case 'T':
                    level.addTeleport(x, y, w, T);
                    break;

                case 'C':
                    for (int ci = sp.col; ci < sp.end; ++ci)
                        level.addCoin(ci * T + T / 2, y + T / 2);
                    break;

                default: break;
            }
        }

        ++tileRow;
    }

    // Guaranteed solid floor at the very bottom
    float floorY = (float)(numTileRows - 1) * T;
    level.addStatic(0.f, floorY, (float)(numCols * T), (float)T, sf::Color(90, 65, 45));

    if (worldW) *worldW = (float)(numCols * T);
    level.sortSections();
    level.finalize();
    return worldH;
}