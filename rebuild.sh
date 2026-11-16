#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_TESTS="${1:-OFF}"

echo "==> Cleaning build..."
rm -rf build

echo "==> Configuring (BUILD_TESTS=${BUILD_TESTS})..."
cmake -B build -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DBUILD_TESTS=${BUILD_TESTS}

echo "==> Building..."
cmake --build build --parallel

echo "==> Done. Executable: build/ArcaneJourney.exe"

if [[ "${BUILD_TESTS}" == "ON" ]]; then
    echo "==> Running tests..."
    ./build/ArcaneJourneyTests.exe
fi

echo "==> Clearing save data..."
rm -f save.dat build/save.dat