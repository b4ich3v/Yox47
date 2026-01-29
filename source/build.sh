#!/usr/bin/env bash
set -euo pipefail

nasm -f elf64 runtime.asm -o runtime.o
nasm -f elf64 out.asm -o out.o
ld -o program out.o runtime.o
