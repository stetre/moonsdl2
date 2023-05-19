#!/usr/bin/env lua
-- MoonSDL2 example: hello.lua
-- Draws a triangle using the built-in 2D renderer
local sdl = require("moonsdl2")

sdl.init()

local window = sdl.create_window("Hello, triangle!", nil, nil, 800, 600, sdl.WINDOW_SHOWN)

local renderer = sdl.create_renderer(window, nil, sdl.RENDERER_ACCELERATED|sdl.RENDERER_PRESENTVSYNC)

local vertices = {
 --   position           color         
   {{ 400, 150 }, { 255, 0, 0, 255 }},
   {{ 200, 450 }, { 0, 0, 255, 255 }},
   {{ 600, 450 }, { 0, 255, 0, 255 }},
}

local quit = false
while not quit do
   e = sdl.poll_event()
   if e then
      if e.type == 'quit' then quit = true end
   end
   renderer:set_draw_color({0, 0, 0, 255})
   renderer:clear()
   renderer:render_geometry(nil, vertices, nil)
   renderer:present()
end

