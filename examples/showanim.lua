#!/usr/bin/env lua
-- MoonSDL2 example: showanim.lua
-- Loads an animation file and renders it.
-- This is a simplified port of the showanim.c example that comes with SDL2_image.
local sdl = require("moonsdl2")

--sdl.trace_objects(true)

-- Init SDL2
sdl.init()

local function draw_background(renderer, w, h)
-- Draw a Gimpish background pattern to show transparency in the image
    local col = {
        { 0x66, 0x66, 0x66, 0xff },
        { 0x99, 0x99, 0x99, 0xff },
    }
   local rw, rh = 8, 8
   local rect = { 0, 0, rw, rh }
   for y=0,h-1,rh do
      for x=0,w-1,rw do
      -- use an 8x8 checkerboard pattern
         local i = (((x ~ y) >> 3) & 1)+1
         renderer:set_draw_color(col[i])
         rect[1] = x
         rect[2] = y
         renderer:fill_rect(rect)
      end
   end
end

local file = arg[1] -- animation file (eg. animated gif)
if not file then
   print("\nUsage:   "..arg[0].." <animation-file>")
   print("\nExample: "..arg[0].." images/Horse_gif.gif\n\n")
   return
end

-- Load animation and get frames:
local anim = sdl.load_animation(file)
local w, h = anim:get_size()       -- the dimensions of each frame
local numframes = anim:get_count() -- the number of frames
local frames = anim:get_frames()   -- a table of surfaces, one per frame
local delays = anim:get_delays()   -- a table of delays (in milliseconds), also one per frame
assert(#frames == numframes)
assert(#delays == numframes)
print(string.format("%dx%d, %d frames", w, h, numframes))

-- Create a window and a renderer
local window = sdl.create_window(file, nil, nil, w, h, sdl.WINDOW_SHOWN)
local renderer = sdl.create_renderer(window, nil, sdl.RENDERER_ACCELERATED|sdl.RENDERER_PRESENTVSYNC)

-- Create the textures for the frames
local textures = {}
for i=1,numframes do
   textures[i] = sdl.create_texture(renderer, frames[i])
end

local current_frame = 1
local now, delay
local next_time = 0
local quit = false
while not quit do
   e = sdl.poll_event()
   if e then
      if e.type == 'quit' or (e.type =='keydown' and e.name == 'Escape')  then
         quit = true
      end
   end

   -- Note: The original code uses sdl.delay() instead of the logic used here, but the problem
   -- with that solution is that it makes the application deaf to events between frames (i.e
   -- for the duration of the delay), and also gives an imprecise timing.
   now = sdl.get_ticks() 
   if now > next_time then
      -- Draw a background pattern in case the image has transparency
      draw_background(renderer, w, h)
      -- Display the image
      renderer:copy(textures[current_frame])
      renderer:present()
      -- Compute the time when the next frame should be rendered:
      delay = delays[current_frame]
      delay = delay > 0 and delay or 100 -- in case no delays were stored in anim
      next_time = now+delay
      -- Advance to next frame
      current_frame = current_frame < numframes and current_frame + 1 or 1
   end
end

