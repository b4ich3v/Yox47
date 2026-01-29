#!/usr/bin/env bash
set -euo pipefail

./build.sh
./program > result.txt
