#!/usr/bin/env lua
-- MoonSDL2 example: messagebox.lua
local sdl = require("moonsdl2")

sdl.init()

sdl.show_simple_message_box(sdl.MESSAGEBOX_ERROR, "This is the title", "this is the message")

local mboxdata = {
   flags = 0, --sdl.MESSAGEBOX_INFORMATION|sdl.MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT,
   window = nil,
   title = "This is the title",
   message = "This is the message",
   buttons = {
      { flags = sdl.MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, buttonid = 11, text = "button1"  },
      { flags = sdl.MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, buttonid = 12, text = "button2"  },
      { flags = 0, buttonid = 13, text = "button3" },
      { flags = 0, buttonid = 14, text = "exit" },
   },
}

local mbox = sdl.create_message_box(mboxdata)
mbox:set_color_scheme({
   {100, 100, 100}, -- background
   {  0,   0, 255},  --text
   {255,   0,   0}, -- button border
   {255, 255, 255}, -- button background
   {  0, 255,   0}, -- button selected
})

local buttonid

while buttonid ~= 14 do
   buttonid = mbox:show()
   print("buttonid", buttonid)
end

