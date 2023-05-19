#!/usr/bin/env lua
-- MoonSDL2 example: display.lua
local sdl = require("moonsdl2")

local printf = function(...) io.write(string.format(...)) end

sdl.init()
sdl.trace_objects(true)

print("VIDEO DRIVERS")
local numdrivers = sdl.get_num_video_drivers()
printf("numdrivers = %d\n", numdrivers)
for i = 1, numdrivers do
   printf("driver %d: %s\n", i, sdl.get_video_driver(i))
end
printf("current driver: %s\n", sdl.get_current_video_driver() or "none")
--do return end

print()
print("DISPLAYS")
local numdisplays = sdl.get_num_displays()
printf("numdisplays = %d\n", numdisplays)

for i = 1, numdisplays do
   printf("display %d (%s)\n", i, sdl.get_display_name(i))
   local r = sdl.get_display_bounds(i)
   printf("\tbounds: %d %d %d %d\n", r[1], r[2], r[3], r[4])
   local r = sdl.get_display_usable_bounds(i)
   printf("\tusable: %d %d %d %d\n", r[1], r[2], r[3], r[4])
   local ddpi, hdpi, vdpi = sdl.get_display_dpi(i)
   printf("\tddpi %.2f hdpi %.2f vdpi %.2f\n", ddpi, hdpi, vdpi)
   local orientation = sdl.get_display_orientation(i)
   printf("\torientation %s\n", orientation)
   local modes = sdl.get_display_modes(i)
   for n, mode in ipairs(modes) do
      local bpp = sdl.bits_per_pixel(mode.format)
      printf("\tmode %d: %s %dx%d %d bpp %d hz\n", n, mode.format, mode.w, mode.h, bpp, mode.refresh_rate)
   end
   local mode = sdl.get_desktop_display_mode(i)
   local bpp = sdl.bits_per_pixel(mode.format)
   printf("\tdesktop mode: %s %dx%d %d bpp %d hz\n", mode.format, mode.w, mode.h, bpp, mode.refresh_rate)
   local mode = sdl.get_current_display_mode(i)
   local bpp = sdl.bits_per_pixel(mode.format)
   printf("\tcurrent mode: %s %dx%d %d bpp %d hz\n", mode.format, mode.w, mode.h, bpp, mode.refresh_rate)
end

