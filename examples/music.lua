#!/usr/bin/env lua
local sdl = require("moonsdl2")

sdl.trace_objects(true)
sdl.init()

sdl.open_audio(44100, nil, 2, 48)

local music = sdl.load_music("sounds/Jingle_Win_00.wav")
print("type: "..music:get_type())
print("duration: "..music:get_duration().." s")

local finished
sdl.set_music_finished_callback(function() print("finished") finished=true end)

print("play()")
music:play()
finished=false
while not finished do end

print("fade_in()")
music:fade_in(2000)
finished=false
while not finished do end

print("fade_out()")
music:play()
finished = not music:fade_out(2000)
while not finished do end

