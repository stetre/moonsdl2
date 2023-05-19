#!/usr/bin/env lua
-- MoonSDL2 example: init.lua
local sdl = require("moonsdl2")

local printf = function(...) io.write(string.format(...)) end

-- init() attempts to initialize all subsystems and extension libraries,
-- and returns flags that we can use to check what was actually initialized
local sdlflags, imgflags, mixflags = sdl.init()
printf("sdlflags=%.8x imgflags=%.8x mixflags=%.8x\n", sdlflags, imgflags, mixflags);
print("enabled sdl subsystems: " .. table.concat({sdl.initflags(sdlflags)}, ', '))
print("supported img types: ".. table.concat({sdl.imginitflags(imgflags)}, ', '))
print("supported mix types: ".. table.concat({sdl.mixinitflags(mixflags)}, ', '))

-- We can also query the flags later, with was_init()
sdlflags, imgflags, mixflags = sdl.was_init()
printf("sdlflags=%.8x imgflags=%.8x mixflags=%.8x\n", sdlflags, imgflags, mixflags);

sdl.quit() -- not really needed (it is executed automatically at exit)
