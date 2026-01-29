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

INCLUDES=(
  -I"${SOURCE_DIR}/lexer"
  -I"${SOURCE_DIR}/parser"
  -I"${SOURCE_DIR}/semantic"
  -I"${SOURCE_DIR}/code_generator"
  -I"${SOURCE_DIR}/ast_nodes/node"
  -I"${SOURCE_DIR}/ast_nodes/expressions"
  -I"${SOURCE_DIR}/ast_nodes/statements"
  -I"${SOURCE_DIR}/ast_nodes/declarations_and_program"
  -I"${SOURCE_DIR}/ast_nodes/call_expression_and_variables"
)

CPP_FILES=(
  "${SOURCE_DIR}/main.cpp"
  "${SOURCE_DIR}/lexer/Lexer.cpp"
  "${SOURCE_DIR}/parser/Parser.cpp"
  "${SOURCE_DIR}/semantic/Semantic.cpp"
  "${SOURCE_DIR}/code_generator/CodeGenerator.cpp"
  "${SOURCE_DIR}/ast_nodes/node/Node.cpp"
  "${SOURCE_DIR}/ast_nodes/expressions/Expressions.cpp"
  "${SOURCE_DIR}/ast_nodes/statements/Statements.cpp"
  "${SOURCE_DIR}/ast_nodes/declarations_and_program/DeclarationsAndProgram.cpp"
  "${SOURCE_DIR}/ast_nodes/call_expression_and_variables/CallExpressionAndVariables.cpp"
)

CXX="${CXX:-g++}"
CXXFLAGS="${CXXFLAGS:-}"

"${CXX}" ${CXXFLAGS} -O2 "${INCLUDES[@]}" "${CPP_FILES[@]}" -o "${SOURCE_DIR}/yoxc" -std=gnu++17
