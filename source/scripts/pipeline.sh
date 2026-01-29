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
SHARED_DIR="${SHARED_DIR:-}"

"${SCRIPT_DIR}/build_compiler.sh"
(
  cd "${SOURCE_DIR}"
  ./yoxc
)
"${SCRIPT_DIR}/build.sh"

if [[ -n "${SHARED_DIR}" ]]; then
  mkdir -p "${SHARED_DIR}"
  rm -f "${SHARED_DIR}/out.asm"
  cat "${SOURCE_DIR}/out.asm" > "${SHARED_DIR}/out.asm"
fi

status=0
"${SOURCE_DIR}/program" > "${SOURCE_DIR}/result.txt" 2> "${SOURCE_DIR}/stderr.txt" || status=$?
echo "${status}" > "${SOURCE_DIR}/exit_code.txt"

if [[ -n "${SHARED_DIR}" ]]; then
  rm -f "${SHARED_DIR}/result.txt" "${SHARED_DIR}/stderr.txt" "${SHARED_DIR}/exit_code.txt"
  cat "${SOURCE_DIR}/result.txt" > "${SHARED_DIR}/result.txt"
  cat "${SOURCE_DIR}/stderr.txt" > "${SHARED_DIR}/stderr.txt"
  cat "${SOURCE_DIR}/exit_code.txt" > "${SHARED_DIR}/exit_code.txt"
fi
