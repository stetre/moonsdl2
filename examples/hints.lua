#!/usr/bin/env lua
-- MoonSDL2 example: hints.lua
local sdl = require("moonsdl2")

-- Initialize SDL
sdl.init()
sdl.trace_objects(true)

-- Register a callback:
local hintcb = sdl.add_hint_callback("SDL_HINT_RENDER_SCALE_QUALITY",
      function(name, oldval, newval) 
         print("callback called", name, oldval, newval)
      end)
print("Callback registered")

-- Should execute the callback:
sdl.set_hint("SDL_HINT_RENDER_SCALE_QUALITY", "1")
sdl.set_hint("SDL_HINT_RENDER_SCALE_QUALITY", "0")
print(sdl.get_hint("SDL_HINT_RENDER_SCALE_QUALITY"))

-- Unregister the callback
hintcb = hintcb:del()
print("Callback unregistered")

-- Should not execute the callback:
sdl.set_hint("SDL_HINT_RENDER_SCALE_QUALITY", "1")


