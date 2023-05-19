#!/usr/bin/env lua
-- MoonSDL2 example: blit.lua
local sdl = require("moonsdl2")

local W, H = 300, 300

local function surfaceproperties(s)
   return string.format("format='%s', w=%d, h=%d, pich=%d", s:get_format(), s:get_size())
end

-- Init SDL2
sdl.init()
--sdl.trace_objects(true)

-- Create a window and get an handle to its surface
local window = sdl.create_window("Blit", nil, nil, W, H, sdl.WINDOW_SHOWN)
local winsurface = window:get_surface()
print("winsurface: "..surfaceproperties(winsurface))

-- Load an image
local imgsurface0 = sdl.load_image('../doc/powered-by-lua.gif')
print("imgsurface0: "..surfaceproperties(imgsurface0))

-- Convert the image surface to the window surface format
local imgsurface = imgsurface0:convert(winsurface:get_format())
imgsurface0 = imgsurface0:free() -- don't need it any more
print("imgsurface: "..surfaceproperties(imgsurface))

local quit = false
while not quit do
   e = sdl.poll_event()
   if e then
      if e.type == 'quit' then quit = true end
   end
   -- Blit the image to the window surface, and update:
   imgsurface:soft_stretch_linear(winsurface)
   --[[ alt:
   imgsurface:blit(winsurface)
   imgsurface:blit_scaled(winsurface)
   imgsurface:soft_stretch(winsurface)
   --]]
   window:update_surface()
end

