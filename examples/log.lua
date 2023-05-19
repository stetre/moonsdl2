#!/usr/bin/env lua
-- MoonSDL2 example: log.lua
local sdl = require("moonsdl2")

sdl.init()
sdl.trace_objects(true)

sdl.log_set_all_priority('verbose')

sdl.log("you're lazy, you just stay in bed")
sdl.log_warn(1, "you don't want no money")
sdl.log_debug('system', "you don't want no bread")


sdl.log_set_output_function(function(category, priority, message)
   print(category, priority, message)
end)

sdl.log("you're lazy, you just stay in bed")
sdl.log_warn(1, "you don't want no money")
sdl.log_debug('system', "you don't want no bread")

