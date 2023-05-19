#!/usr/bin/env lua
-- MoonSDL2 example: timer.lua
local sdl = require("moonsdl2")

local printf = function(...) print(string.format(...)) end

sdl.init()
sdl.trace_objects(true)

local function callback1(t)
   print("timer T1 expired", t)
   return false
end

local n = 10
local function callback2(t)
   print("timer T2 expired", t, n)
   n = n - 1
   return n > 0
end

sdl.start_timer(3000, callback1)
sdl.start_timer(1000, callback2)

while true do
   if n <= 0 then break end
   local e = sdl.poll_event()
   if e then
      print(e.type)
      if e.type == 'quit' then break end
   end
end

