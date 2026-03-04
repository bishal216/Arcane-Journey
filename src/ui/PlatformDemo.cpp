#include "ui/PlatformDemo.hpp"
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

sf::Color PlatformDemo::lerpColor(sf::Color a, sf::Color b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return sf::Color(
        (uint8_t)(a.r + (b.r - a.r) * t),
        (uint8_t)(a.g + (b.g - a.g) * t),
        (uint8_t)(a.b + (b.b - a.b) * t),
        (uint8_t)(a.a + (b.a - a.a) * t)
    );
}

sf::Vector2f PlatformDemo::lerpVec(sf::Vector2f a, sf::Vector2f b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return a + (b - a) * t;
}

void PlatformDemo::setArea(sf::Vector2f origin, sf::Vector2f size) {
    m_origin = origin;
    m_size   = size;
}

// ---------------------------------------------------------------------------
// Demo definitions
// Demo-local coordinate system: (0,0) = top-left of preview area
// Centre of area ≈ (150, 120)
// ---------------------------------------------------------------------------

void PlatformDemo::buildFrames(PlatType type) {
    m_frames.clear();
    m_extras.clear();

    // Shorthand colours
    const sf::Color CHAR_DEF  = sf::Color(100, 180, 255);
    const sf::Color CHAR_LAND = sf::Color(140, 220, 140);
    const sf::Color PLAT_DEF  = sf::Color( 80, 190,  80);

    // cx/cy = horizontal/vertical centre of preview pane
    float cx = m_size.x * 0.5f;
    float cy = m_size.y * 0.5f;
    float pw = 96.f;   // default platform width
    float ph = 16.f;   // default platform height
    float py = cy + 40.f;  // platform Y
    float floor = cy + 80.f;

    switch (type) {

    case PlatType::Static:
        m_frames = {
            {0.0f, {cx, floor - 80.f}, {cx - pw/2, py}, {pw, ph}, PLAT_DEF,          CHAR_DEF,  "A solid platform."},
            {0.5f, {cx, floor - 80.f}, {cx - pw/2, py}, {pw, ph}, PLAT_DEF,          CHAR_DEF,  ""},
            {0.6f, {cx, py - 22.f},    {cx - pw/2, py}, {pw, ph}, PLAT_DEF,          CHAR_LAND, "Land on it safely."},
            {0.4f, {cx, py - 22.f},    {cx - pw/2, py}, {pw, ph}, PLAT_DEF,          CHAR_LAND, ""},
            {0.5f, {cx, py - 22.f},    {cx - pw/2, py}, {pw, ph}, PLAT_DEF,          CHAR_DEF,  "Won't move or break."},
        };
        break;

    case PlatType::MoveH:
        m_frames = {
            {0.0f, {cx - 60.f, py - 22.f}, {cx - pw/2 - 60.f, py}, {pw, ph}, sf::Color(120,210,255), CHAR_DEF, "Moves side to side."},
            {1.2f, {cx + 60.f, py - 22.f}, {cx - pw/2 + 60.f, py}, {pw, ph}, sf::Color(120,210,255), CHAR_DEF, "Ride it across the gap."},
            {1.2f, {cx - 60.f, py - 22.f}, {cx - pw/2 - 60.f, py}, {pw, ph}, sf::Color(120,210,255), CHAR_DEF, ""},
        };
        break;

    case PlatType::MoveV:
        m_frames = {
            {0.0f, {cx, py - 22.f},        {cx - pw/2, py},        {pw, ph}, sf::Color(230,210,90), CHAR_DEF, "Moves up and down."},
            {1.2f, {cx, py - 80.f},        {cx - pw/2, py - 58.f}, {pw, ph}, sf::Color(230,210,90), CHAR_DEF, "Time your jump from the top."},
            {1.2f, {cx, py - 22.f},        {cx - pw/2, py},        {pw, ph}, sf::Color(230,210,90), CHAR_DEF, ""},
        };
        break;

    case PlatType::Crumbling:
        m_frames = {
            {0.0f, {cx, floor - 80.f},  {cx - pw/2, py}, {pw, ph}, sf::Color(160,100,50),  CHAR_DEF,  "Shakes when you land..."},
            {0.4f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(200,130,60),  CHAR_LAND, ""},
            {0.5f, {cx, py - 22.f},     {cx - pw/2 - 2.f, py}, {pw, ph}, sf::Color(220,160,80),  CHAR_LAND, ""},
            {0.2f, {cx, py - 22.f},     {cx - pw/2 + 2.f, py}, {pw, ph}, sf::Color(240,180,80),  CHAR_LAND, "then crumbles! Don't linger."},
            {0.3f, {cx, py + 40.f},     {cx - pw/2, py + 20.f}, {pw, 2.f}, sf::Color(160,100,50,80), CHAR_DEF, "Jump before it's gone."},
        };
        break;

    case PlatType::Bounce:
        m_frames = {
            {0.0f, {cx, floor - 80.f},  {cx - pw/2, py}, {pw, ph}, sf::Color(80,220,120),  CHAR_DEF,  "A bounce pad."},
            {0.5f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(80,220,120),  CHAR_LAND, ""},
            {0.1f, {cx, py - 22.f},     {cx - pw/2, py + 4.f}, {pw, ph * 0.5f}, sf::Color(120,255,160), CHAR_LAND, "Squash!"},
            {0.6f, {cx, floor - 160.f}, {cx - pw/2, py}, {pw, ph}, sf::Color(80,220,120),  sf::Color(255,120,50), "Launched upward!"},
            {0.6f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(80,220,120),  CHAR_DEF,  "No jump needed."},
        };
        break;

    case PlatType::Disappearing:
        m_frames = {
            {0.0f, {cx, floor - 80.f},  {cx - pw/2, py}, {pw, ph}, sf::Color(140,80,200),   CHAR_DEF,  "Fades in and out."},
            {0.8f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(140,80,200),   CHAR_LAND, "Land while it's solid..."},
            {0.6f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(140,80,200,80),CHAR_LAND, ""},
            {0.4f, {cx, py + 60.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(140,80,200,0), CHAR_DEF,  "...then it vanishes. Watch the rhythm!"},
        };
        break;

    case PlatType::OneWay:
        m_frames = {
            {0.0f, {cx, py + 40.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(120,80,160),  CHAR_DEF,  "A one-way platform."},
            {0.6f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(120,80,160),  CHAR_LAND, "Pass through from below..."},
            {0.6f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(120,80,160),  CHAR_LAND, "...solid from above."},
            {0.5f, {cx, floor - 80.f},  {cx - pw/2, py}, {pw, ph}, sf::Color(120,80,160),  CHAR_DEF,  "Jump up through it freely."},
        };
        break;

    case PlatType::Hatch:
        m_frames = {
            {0.0f, {cx, floor - 80.f},  {cx - pw/2, py}, {pw, ph}, sf::Color(100,70,35),   CHAR_DEF,  "A hatch trapdoor."},
            {0.5f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(100,70,35),   CHAR_LAND, "Land on it..."},
            {0.4f, {cx, py - 22.f},     {cx - pw/2, py}, {pw * 0.3f, ph}, sf::Color(100,70,35), CHAR_LAND, "...it swings open!"},
            {0.5f, {cx, py + 50.f},     {cx - pw/2, py}, {pw * 0.1f, ph}, sf::Color(100,70,35,60), CHAR_DEF, "Step off quickly or fall through."},
        };
        break;

    case PlatType::Rope: {
        float ropeCX = cx;
        float ropeTop = cy - 50.f;
        float ropeBot = cy + 60.f;
        // Build rope visual as extras
        sf::RectangleShape rope({5.f, ropeBot - ropeTop});
        rope.setPosition({ropeCX - 2.5f, ropeTop});
        rope.setFillColor(sf::Color(200, 170, 100));
        m_extras.push_back(rope);
        m_frames = {
            {0.0f, {ropeCX + 40.f, cy},  {ropeCX - 48.f, ropeBot}, {96.f, 8.f}, sf::Color(0,0,0,0), CHAR_DEF, "A rope. Press Up/Down near it."},
            {0.5f, {ropeCX, cy},          {ropeCX - 48.f, ropeBot}, {96.f, 8.f}, sf::Color(0,0,0,0), sf::Color(180,220,100), "Grab and climb!"},
            {0.6f, {ropeCX, ropeTop + 20.f},{ropeCX-48.f,ropeBot},  {96.f, 8.f}, sf::Color(0,0,0,0), sf::Color(180,220,100), "Jump to dismount."},
            {0.4f, {ropeCX + 40.f, ropeTop + 10.f},{ropeCX-48.f,ropeBot},{96.f,8.f},sf::Color(0,0,0,0),CHAR_DEF,""},
        };
        break;
    }

    case PlatType::Ladder: {
        float ladCX = cx;
        float ladTop = cy - 50.f;
        float ladBot = cy + 60.f;
        sf::RectangleShape lad({24.f, ladBot - ladTop});
        lad.setPosition({ladCX - 12.f, ladTop});
        lad.setFillColor(sf::Color(140, 110, 60));
        m_extras.push_back(lad);
        m_frames = {
            {0.0f, {ladCX + 50.f, cy},    {ladCX - 48.f, ladBot}, {96.f,8.f}, sf::Color(0,0,0,0), CHAR_DEF, "A ladder. Same as a rope..."},
            {0.5f, {ladCX, cy},            {ladCX - 48.f, ladBot}, {96.f,8.f}, sf::Color(0,0,0,0), sf::Color(180,220,100), "...but sturdier."},
            {0.6f, {ladCX, ladTop + 20.f}, {ladCX - 48.f, ladBot}, {96.f,8.f}, sf::Color(0,0,0,0), sf::Color(180,220,100), "Up/Down to climb, jump to leave."},
            {0.5f, {ladCX + 50.f, ladTop + 10.f},{ladCX-48.f,ladBot},{96.f,8.f},sf::Color(0,0,0,0),CHAR_DEF,""},
        };
        break;
    }

    case PlatType::Teleport: {
        float t1x = cx - 60.f;
        float t2x = cx + 60.f;
        sf::RectangleShape dest({64.f, 16.f});
        dest.setPosition({t2x - 32.f, py});
        dest.setFillColor(sf::Color(60, 40, 120));
        dest.setOutlineColor(sf::Color(160, 100, 255));
        dest.setOutlineThickness(2.f);
        m_extras.push_back(dest);
        m_frames = {
            {0.0f, {t1x, py - 22.f},    {t1x - 32.f, py}, {64.f,16.f}, sf::Color(60,40,120), CHAR_DEF,  "Stand still on a teleport block..."},
            {0.5f, {t1x, py - 22.f},    {t1x - 32.f, py}, {64.f,16.f}, sf::Color(80,60,180), CHAR_DEF,  "Charging..."},
            {0.5f, {t1x, py - 22.f},    {t1x - 32.f, py}, {64.f,16.f}, sf::Color(140,80,255),sf::Color(200,160,255), ""},
            {0.1f, {t2x, py - 22.f},    {t1x - 32.f, py}, {64.f,16.f}, sf::Color(60,40,120), sf::Color(255,255,255), "Teleported!"},
            {0.6f, {t2x, py - 22.f},    {t1x - 32.f, py}, {64.f,16.f}, sf::Color(60,40,120), CHAR_DEF,  "Step off to avoid bouncing back."},
        };
        break;
    }

    case PlatType::Coin: {
        sf::RectangleShape coin({14.f, 14.f});
        coin.setPosition({cx - 7.f, cy - 30.f});
        coin.setFillColor(sf::Color(255, 210, 40));
        m_extras.push_back(coin);
        m_frames = {
            {0.0f, {cx - 40.f, cy + 10.f}, {cx - 48.f, cy + 32.f}, {96.f,8.f}, PLAT_DEF, CHAR_DEF, "Coins are scattered everywhere."},
            {0.5f, {cx, cy - 10.f},         {cx - 48.f, cy + 32.f}, {96.f,8.f}, PLAT_DEF, sf::Color(255,210,40), "Walk into one to collect it."},
            {0.5f, {cx + 40.f, cy + 10.f},  {cx - 48.f, cy + 32.f}, {96.f,8.f}, PLAT_DEF, CHAR_DEF, "Spend them at the Merchant."},
        };
        break;
    }

    case PlatType::Goal:
        m_frames = {
            {0.0f, {cx, floor - 80.f},  {cx - pw/2, py}, {pw, ph}, sf::Color(255,215,0), CHAR_DEF, "The golden platform."},
            {0.7f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(255,215,0), sf::Color(255,215,0), "Reach it to win the run!"},
            {0.5f, {cx, py - 22.f},     {cx - pw/2, py}, {pw, ph}, sf::Color(255,230,80),sf::Color(255,255,200),"It's at the very top."},
        };
        break;

    default:
        m_frames = {
            {0.5f, {cx, cy}, {cx - 48.f, cy + 30.f}, {96.f, 16.f}, PLAT_DEF, CHAR_DEF, "???"},
        };
        break;
    }
}

// ---------------------------------------------------------------------------
// Playback
// ---------------------------------------------------------------------------

void PlatformDemo::play(PlatType type) {
    buildFrames(type);
    m_frameIdx   = 0;
    m_frameTimer = 0.f;
    m_playing    = true;
    if (!m_frames.empty()) {
        applyFrame(m_frames[0]);
        m_currentLabel = m_frames[0].label;
    }
}

void PlatformDemo::applyFrame(const DemoKeyframe& f) {
    m_charPos   = f.charPos;
    m_platPos   = f.platPos;
    m_platSize  = f.platSize;
    m_platColor = f.platColor;
    m_charColor = f.charColor;
    if (!f.label.empty()) m_currentLabel = f.label;
}

void PlatformDemo::lerpFrame(const DemoKeyframe& from, const DemoKeyframe& to, float t) {
    // ease-in-out quad
    float e = t < 0.5f ? 2*t*t : 1 - (-2*t+2)*(-2*t+2)/2;
    m_charPos   = lerpVec(from.charPos,  to.charPos,  e);
    m_platPos   = lerpVec(from.platPos,  to.platPos,  e);
    m_platSize  = lerpVec(from.platSize, to.platSize, e);
    m_platColor = lerpColor(from.platColor, to.platColor, e);
    m_charColor = lerpColor(from.charColor, to.charColor, e);
    if (!to.label.empty()) m_currentLabel = to.label;
}

void PlatformDemo::update(float dt) {
    if (!m_playing || m_frames.empty()) return;

    m_frameTimer += dt;

    if (m_frameIdx + 1 >= (int)m_frames.size()) {
        m_playing = false;
        return;
    }

    const auto& curr = m_frames[m_frameIdx];
    const auto& next = m_frames[m_frameIdx + 1];

    float dur = next.duration;
    if (dur <= 0.f) dur = 0.001f;

    if (m_frameTimer >= dur) {
        m_frameTimer -= dur;
        m_frameIdx++;
        applyFrame(next);
    } else {
        lerpFrame(curr, next, m_frameTimer / dur);
    }
}

void PlatformDemo::draw(sf::RenderWindow& window, const sf::Font& font) const {
    // Background panel
    sf::RectangleShape bg(m_size);
    bg.setPosition(m_origin);
    bg.setFillColor(sf::Color(20, 12, 40));
    bg.setOutlineColor(sf::Color(80, 50, 140));
    bg.setOutlineThickness(1.f);
    window.draw(bg);

    // Translate all drawing to panel-local coords
    auto toScreen = [&](sf::Vector2f p) { return p + m_origin; };

    // Platform
    if (m_platSize.x > 1.f && m_platSize.y > 1.f) {
        sf::RectangleShape plat(m_platSize);
        plat.setPosition(toScreen(m_platPos));
        plat.setFillColor(m_platColor);
        plat.setOutlineColor(sf::Color(255, 255, 255, 40));
        plat.setOutlineThickness(1.f);
        window.draw(plat);
    }

    // Extras (rope, second platform, etc.)
    for (auto sh : m_extras) {
        sh.setPosition(sh.getPosition() + m_origin);
        window.draw(sh);
    }

    // Character — simple rect with head circle
    if (m_charColor.a > 10) {
        sf::Vector2f cp = toScreen(m_charPos);

        sf::RectangleShape body({20.f, 28.f});
        body.setOrigin({10.f, 14.f});
        body.setPosition(cp);
        body.setFillColor(m_charColor);
        window.draw(body);

        sf::CircleShape head(10.f);
        head.setOrigin({10.f, 10.f});
        head.setPosition({cp.x, cp.y - 22.f});
        head.setFillColor(lerpColor(m_charColor, sf::Color::White, 0.3f));
        window.draw(head);

        // Eyes
        sf::CircleShape eye(2.f);
        eye.setFillColor(sf::Color(20, 10, 30));
        eye.setOrigin({2.f, 2.f});
        eye.setPosition({cp.x - 4.f, cp.y - 24.f});
        window.draw(eye);
        eye.setPosition({cp.x + 4.f, cp.y - 24.f});
        window.draw(eye);
    }

    // Label text
    if (!m_currentLabel.empty()) {
        sf::Text label(font, m_currentLabel, 14);
        label.setFillColor(sf::Color(220, 210, 240));
        label.setOutlineColor(sf::Color::Black);
        label.setOutlineThickness(1.f);
        auto lb = label.getLocalBounds();
        label.setOrigin({lb.position.x + lb.size.x / 2.f, 0.f});
        label.setPosition({m_origin.x + m_size.x / 2.f,
                           m_origin.y + m_size.y - 28.f});
        window.draw(label);
    }

    // "Press E to replay" hint after animation ends
    if (!m_playing) {
        sf::Text hint(font, "[E] Replay", 12);
        hint.setFillColor(sf::Color(140, 120, 180));
        hint.setPosition({m_origin.x + 6.f, m_origin.y + 6.f});
        window.draw(hint);
    }
}