#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ENV_FILE="${SCRIPT_DIR}/../.env"
if [[ -f "${ENV_FILE}" ]]; then
  set -a
  # shellcheck disable=SC1090
  source "${ENV_FILE}"
  set +a
fi
SOURCE_DIR="${SOURCE_DIR:-${SCRIPT_DIR}/..}"

nasm -f elf64 "${SOURCE_DIR}/runtime.asm" -o "${SOURCE_DIR}/runtime.o"
nasm -f elf64 "${SOURCE_DIR}/out.asm" -o "${SOURCE_DIR}/out.o"
ld -o "${SOURCE_DIR}/program" "${SOURCE_DIR}/out.o" "${SOURCE_DIR}/runtime.o"
