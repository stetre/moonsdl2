#!/usr/bin/env lua
-- MoonSDL2 example: version.lua
local sdl = require("moonsdl2")

print(_VERSION)
print(sdl._VERSION)
print(sdl._SDL_VERSION)
print(sdl._SDL_IMG_VERSION)
print(sdl._SDL_TTF_VERSION)
print(sdl._SDL_MIX_VERSION)

