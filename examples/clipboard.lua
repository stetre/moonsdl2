#!/usr/bin/env lua
-- MoonSDL2 example: clipboard.lua
local sdl = require("moonsdl2")

sdl.init()
print(sdl.has_clipboard_text())
print("Clipboard text:", sdl.get_clipboard_text())
sdl.set_clipboard_text("Hello, clipboard!")
print("Clipboard text:", sdl.get_clipboard_text())
sdl.set_clipboard_text("Good morning!")
print("Clipboard text:", sdl.get_clipboard_text())
sdl.set_clipboard_text() -- this should write an empty string ("")
print("Clipboard text:", sdl.get_clipboard_text())
sdl.set_clipboard_text("Bye!")
print("Clipboard text:", sdl.get_clipboard_text())
sdl.pump_events()
