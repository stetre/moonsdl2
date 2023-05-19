#!/usr/bin/env lua
-- MoonSDL2 example: gltriangle.lua
-- Draws a triangle using OpenGL
local sdl = require "moonsdl2"
local gl = require "moongl" -- OpenGL bindings

local width, height = 800, 600

local loc_pos = 0 -- location for position attribute
local loc_col = 1 -- location for color attribute

-- Vertex shader:
local vshSource = string.format([[
#version 330 core
layout(location=%d) in vec4 position;
layout(location=%d) in vec4 color;
out vec4 Color;

void main() {
   gl_Position = position;
   Color = color;
}
]], loc_pos, loc_col)

-- Fragment shader:
local fshSource = [[
#version 330 core
in vec4 Color;
out vec4 out_Color;

void main() {
   out_Color = Color;
}
]]

-- Initialize SDL2
sdl.init()

-- Create a window with SDL2 and initialize the GL context:
sdl.gl_set_context_version(3, 3, 'core')
local window = sdl.create_window("Hello, OpenGL!", nil, nil, 800, 600, 
               sdl.WINDOW_OPENGL|sdl.WINDOW_RESIZABLE|sdl.WINDOW_SHOWN)
local glcontext = sdl.gl_create_context(window)
glcontext:make_current(window)
gl.init() -- this is actually glewInit()

local screen_width, screen_height = window:get_size()
gl.viewport(0, 0, screen_width, screen_height)

-- Compile shaders and link them to create a shading program:
local prog, vsh, fsh = gl.make_program_s('vertex', vshSource, 'fragment', fshSource)
gl.delete_shaders(vsh, fsh)

-- Positions and colors for the triangle's vertices:
local positions = {
   -0.5, -0.5,  0.0, 1.0, -- bottom left
    0.5, -0.5,  0.0, 1.0, -- bottom right
    0.0,  0.5,  0.0, 1.0, -- middle top
}

local colors = {
    1.0, 0.0, 0.0, 1.0, -- red
    0.0, 1.0, 0.0, 1.0, -- green
    0.0, 0.0, 1.0, 1.0, -- blue
}

-- Create a vertex buffer array, load the vertex data on the GPU and define the attributes:
local vao = gl.new_vertex_array()
-- position attribute:
local vbo_pos = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', positions), 'static draw')
gl.vertex_attrib_pointer(loc_pos, 4, 'float', false, 0, 0)
gl.enable_vertex_attrib_array(loc_pos)
gl.unbind_buffer('array')
-- color attribute:
local vbo_col = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', colors), 'static draw')
gl.vertex_attrib_pointer(loc_col, 4, 'float', false, 0, 0)
gl.enable_vertex_attrib_array(loc_col)
gl.unbind_buffer('array')
-- Unbind the vao (will be bound again in the loop):
gl.unbind_vertex_array()

-- Event loop:
local quit = false
while not quit do
   e = sdl.poll_event()
   if e then
      if e.type == 'quit' or (e.type =='keydown' and e.name == 'Escape')  then
         quit = true
      elseif e.type == 'windowevent' and e.event == 'resized' then
         local w, h = e.data1, e.data2
         print("window reshaped to "..w.."x"..h)
         gl.viewport(0, 0, w, h)
      end
   end
   -- Clear the screen:
   gl.clear_color(0, 0, 0, 1) -- black
   gl.clear('color')
   -- Draw the vertices:
   gl.use_program(prog)
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   gl.unbind_vertex_array()
   sdl.gl_swap_window(window)
end

-- cleanup:
gl.delete_buffers(vbo_pos)
gl.delete_buffers(vbo_col)
gl.delete_vertex_arrays(vao)
gl.clean_program(prog)

