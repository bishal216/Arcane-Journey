#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "==> Cleaning build..."
rm -rf build

echo "==> Configuring..."
cmake -B build -G "MinGW Makefiles"

echo "==> Building..."
cmake --build build

echo "==> Done. Executable: build/ArcaneJourney.exe"