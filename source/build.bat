nasm -f win64 runtime.asm -o runtime.obj
nasm -f win64 out.asm -o out.obj
gcc runtime.obj out.obj -o program.exe -nostartfiles -Wl,-e,_start -lkernel32

program.exe > result.txt
