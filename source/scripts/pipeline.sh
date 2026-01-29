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
ARTIFACTS_DIR="${ARTIFACTS_DIR:-${SOURCE_DIR}/.artifacts}"
SHARED_DIR="${SHARED_DIR:-}"
mkdir -p "${ARTIFACTS_DIR}"
rm -f "${SOURCE_DIR}/out.asm" \
  "${SOURCE_DIR}/out.o" \
  "${SOURCE_DIR}/runtime.o" \
  "${SOURCE_DIR}/program" \
  "${SOURCE_DIR}/result.txt" \
  "${SOURCE_DIR}/stderr.txt" \
  "${SOURCE_DIR}/exit_code.txt" \
  "${SOURCE_DIR}/yoxc"

bash "${SCRIPT_DIR}/build_compiler.sh"
(
  cp "${SOURCE_DIR}/test.txt" "${ARTIFACTS_DIR}/test.txt"
  cd "${ARTIFACTS_DIR}"
  ./yoxc
)
bash "${SCRIPT_DIR}/build.sh"

if [[ -n "${SHARED_DIR}" ]]; then
  mkdir -p "${SHARED_DIR}"
  rm -f "${SHARED_DIR}/out.asm"
  cat "${ARTIFACTS_DIR}/out.asm" > "${SHARED_DIR}/out.asm"
fi

status=0
"${ARTIFACTS_DIR}/program" > "${ARTIFACTS_DIR}/result.txt" 2> "${ARTIFACTS_DIR}/stderr.txt" || status=$?
echo "${status}" > "${ARTIFACTS_DIR}/exit_code.txt"

if [[ -n "${SHARED_DIR}" ]]; then
  rm -f "${SHARED_DIR}/result.txt" "${SHARED_DIR}/stderr.txt" "${SHARED_DIR}/exit_code.txt"
  cat "${ARTIFACTS_DIR}/result.txt" > "${SHARED_DIR}/result.txt"
  cat "${ARTIFACTS_DIR}/stderr.txt" > "${SHARED_DIR}/stderr.txt"
  cat "${ARTIFACTS_DIR}/exit_code.txt" > "${SHARED_DIR}/exit_code.txt"
fi
