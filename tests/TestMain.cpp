#include "TestFramework.hpp"

// Include all test files
#include "PlatformTests_MovingPlatform.cpp"
#include "PlatformTests_CrumblingPlatform.cpp"
#include "PlatformTests_BouncePad.cpp"
#include "PlatformTests_DisappearingPlatform.cpp"
#include "PlatformTests_Climbable.cpp"
#include "PlatformTests_TeleportBlock.cpp"
#include "PlatformTests_Trapdoor.cpp"
#include "PlatformTests_SinkingPlatform.cpp"
#include "PlatformTests_Integration.cpp"

int main() {
    return test::TestSuite::instance().runAll();
}