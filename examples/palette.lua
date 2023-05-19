#!/usr/bin/env lua
-- MoonSDL2 example: palette.lua
local sdl = require("moonsdl2")
local printf = function(...) io.write(string.format(...)) end

sdl.trace_objects(true)
sdl.init()

local palette = sdl.alloc_palette(4)
local colors = palette:get_colors()
print(#colors, colors)
print()
for _, c in ipairs(colors) do printf("%.2x %.2x %.2x %.2x\n", table.unpack(c)) end

palette:set_colors({{ 1, 2, 3, 0xff}, { 4, 5, 6, 0xff}}, 2)
colors = palette:get_colors()
print()
for _, c in ipairs(colors) do printf("%.2x %.2x %.2x %.2x\n", table.unpack(c)) end

palette:set_colors({{ 255, 0, 0, 0xff}, { 0, 255, 0, 0xff}, { 0, 0, 255, 0xff}})
colors = palette:get_colors()
print()
for _, c in ipairs(colors) do printf("%.2x %.2x %.2x %.2x\n", table.unpack(c)) end
