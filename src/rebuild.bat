@echo off

:: Change these to suit your installation
set LUAVER=5.1
set LUA_LIBDIR=C:\Code\Tools\Lua\LuaJIT\lib
set LUA_INCDIR=C:\Code\Tools\Lua\LuaJIT\include
set SDL_LIBDIR=C:\Code\Tools\SDL2\lib
set SDL_INCDIR=C:\Code\Tools\SDL2\include\SDL2
set DLLNAME=moonsdl2.dll

:: Clean up
del /Q *.obj *.dll *.exp *.lib

:: Build
cl.exe /c /O2 /std:c11 /MD /LD /nologo /Zc:preprocessor /DMSVC_BUILDDLL /DLUAVER=%LUAVER% /I. /I%LUA_INCDIR% /I%SDL_INCDIR% *.c
link.exe /DLL /MACHINE:X64 /NOLOGO /OPT:NOREF /LIBPATH:%LUA_LIBDIR% /LIBPATH:%SDL_LIBDIR% /OUT:%DLLNAME% *.obj lua51.lib SDL2.lib SDL2_image.lib SDL2_ttf.lib SDL2_mixer.lib
