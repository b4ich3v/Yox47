nasm -f win64 out.asm -o out.obj
link out.obj /SUBSYSTEM:CONSOLE /ENTRY:_start ^
    /DEFAULTLIB:msvcrt.lib /DEFAULTLIB:legacy_stdio_definitions.lib ^
    /NODEFAULTLIB:libcmt.lib /OUT:prog.exe

prog.exe
pause
