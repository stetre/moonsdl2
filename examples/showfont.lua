#!/usr/bin/env lua
-- MoonSDL2 example: showanim.lua
-- A simple program to test the text rendering feature of the TTF library
-- This is port of the showfont.c example that comes with SDL2_ttf.
local sdl = require("moonsdl2")

--sdl.trace_objects(true)

local DEFAULT_PTSIZE = 18
local DEFAULT_TEXT = "The quick brown fox jumped over the lazy dog"
local WIDTH, HEIGHT = 640, 480
local BG_COLOR = { 255, 255, 255, 255 }
local WHITE = { 255, 255, 255, 0 }
local BLACK = { 0, 0, 0, 0 }

local function usage() -- print usage and exit
   io.write(string.format("\nUsage: %s [-solid] [-shaded] [-blended] [-utf8] [-b] [-i] [-u] [-s] [-outline size] [-ptsize size] [-hintlight|-hintmono|-hintnone] [-nokerning] [-fgcol r,g,b,a] [-bgcol r,g,b,a] <font>.ttf [ptsize] [text]\n\n\n", arg[0]))
   os.exit(false, true)
end

local rendermethod -- 'solid', 'shaded', or 'blended'

local scene = {
   caption = nil,     -- texture
   captionRect = nil, -- rect
   message = nil,     -- texture
   messageRect = nil, -- rect
}

local function draw_scene(renderer, scene)
   renderer:set_draw_color(BG_COLOR)
   renderer:clear()
   renderer:copy(scene.caption, nil, scene.captionRect)
   renderer:copy(scene.message, nil, scene.messageRect)
   renderer:present()
end

-- Look for special execution mode
local dump = false
-- Look for special rendering types
local rendermethod = 'shaded'
local renderstyle = sdl.TTF_STYLE_NORMAL -- ttfstyleflags
local rendertype = 'latin1' -- 'latin1' or 'utf8'
local outline = 0
local hinting = 'normal' -- fonthinting
local kerning = true
local ptsize = DEFAULT_PTSIZE
-- Default is black and white
local fgcol = BLACK
local bgcol = WHITE

-- Parse command-line arguments ------------------------------------------------
local i=1
while arg[i] and arg[i]:sub(1, 1)=='-' do
   if arg[i]=="-solid" then rendermethod = 'solid'
   elseif arg[i]=="-shaded" then rendermethod = 'shaded'
   elseif arg[i]=="-blended" then rendermethod = 'blended'
   elseif arg[i]=="-utf8" then rendertype = 'utf8'
   elseif arg[i]=="-b" then renderstyle = renderstyle|sdl.TTF_STYLE_BOLD
   elseif arg[i]=="-i" then renderstyle = renderstyle|sdl.TTF_STYLE_ITALIC
   elseif arg[i]=="-u" then renderstyle = renderstyle|sdl.TTF_STYLE_UNDERLINE
   elseif arg[i]=="-s" then renderstyle = renderstyle|sdl.TTF_STYLE_STRIKETHROUGH
   elseif arg[i]=="-outline" then
      i=i+1
      outline = math.tointeger(arg[i]) or usage()
   elseif arg[i]=="-ptsize" then
      i=i+1
      ptsize = math.tointeger(arg[i]) or usage()
   elseif arg[i]=="-hintlight" then hinting = 'light'
   elseif arg[i]=="-hintmono" then hinting = 'mono'
   elseif arg[i]=="-hintnone" then hinting = 'none'
   elseif arg[i]=="-nokerning" then kerning = false
   elseif arg[i]=="-dump" then dump = true
   elseif arg[i]=="-fgcol" then
      i=i+1
      local r, g, b, a = 255, 255, 255, 255
      local f = string.gmatch(arg[i], "%d+")
      r, g, b, a = f(), f(), f(), f()
      fgcol = { r, g, b, a }
   elseif arg[i]== "-bgcol" then
      i=i+1
      local r, g, b, a = 255, 255, 255, 255
      local f = string.gmatch(arg[i], "%d+")
      r, g, b, a = f(), f(), f(), f()
      bgcol = { r, g, b, a }
   else
      usage()
   end
   i=i+1
end
local file = arg[i]
if not file then usage() end
local message = arg[i+1] or DEFAULT_TEXT
--print(rendermethod, renderstyle, rendertype, outline, hinting, kerning, ptsize, file, message)

--------------------------------------------------------------------------------

-- Init SDL2 (this also inits SDL2_ttf)
sdl.init()

-- Open the font file with the requested point size
local font = sdl.open_font(file, ptsize)
-- Set the requested properties
font:set_style(renderstyle)
font:set_outline(outline)
font:set_kerning(kerning)
font:set_hinting(hinting)

if dump then
   for i=48, 122 do
      local glyph = font:render_glyph_shaded(i, fgcol, bgcol)
      glyph:save_bmp(string.format("glyph-%d.bmp", i))
   end
   os.exit()
end

-- Create a window
local window = sdl.create_window("showfont", nil, nil, WIDTH, HEIGHT, nil, 0)
local renderer = sdl.create_renderer(window)

-- Show which font file we're looking at
local str = string.format("Font file: %s", file)

local text --> surface
if rendermethod == 'solid' then
   text = font:render_text_solid(str, fgcol)
elseif rendermethod == 'shaded' then
   text = font:render_text_shaded(str, fgcol, bgcol)
elseif rendermethod == 'blended' then
   text = font:render_text_blended(str, fgcol)
end

if text then
   local w, h = text:get_size()
   scene.captionRect = { 4, 4, w, h }
   scene.caption = sdl.create_texture(renderer, text)
   text = text:free()
end

-- Render and center the message
if rendertype == 'latin1' then
   if rendermethod == 'solid' then
      text = font:render_text_solid(message, fgcol)
   elseif rendermethod == 'shaded' then
      text = font:render_text_shaded(message, fgcol, bgcol)
   elseif rendermethod == 'blended' then
      text = font:render_text_blended(message, fgcol)
   end
elseif rendertype == 'utf8' then
   if rendermethod == 'solid' then
      text = font:render_utf8_solid(message, fgcol)
   elseif rendermethod == 'shaded' then
      text = font:render_utf8_shaded(message, fgcol, bgcol)
   elseif rendermethod == 'blended' then
      text = font:render_utf8_blended(message, fgcol)
   end
end

local w, h = text:get_size()
scene.messageRect = { (WIDTH-w)/2, (HEIGHT-h)/2, w, h }
scene.message = sdl.create_texture(renderer, text)
io.write(string.format("Font is generally %d big, and string is %d big\n", font:height(font), h))
draw_scene(renderer, scene)

-- Wait for a keystroke, and blit text on mouse press
local done = false
while not done do
   local event = sdl.wait_event()
   if event.type == 'mousebuttondown' then
      scene.messageRect = { event.x-w/2, event.y-h/2, w, h }
      draw_scene(renderer, scene)
   elseif event.type == 'keydown' or event.type == 'quit' then
      done = true
   end
end
