nasm -f win64 runtime.asm -o runtime.o
nasm -f win64 out.asm     -o out.o
gcc runtime.o out.o -o prog.exe -nostartfiles -Wl,--subsystem,console
prog.exe
pause
