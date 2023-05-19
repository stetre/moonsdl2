#!/usr/bin/env lua
-- MoonSDL2 example: playmus.lua
-- This is a simplified port of the playmus.c example that comes with SDL2_mixer.
local sdl = require("moonsdl2")

--sdl.trace_objects(true)

local function printf(...) io.write(string.format(...)) end

local function usage()
   printf("Usage: %s [-i] [-l] [-8] [-f32] [-r rate] [-c channels] [-b buffers] [-v N] <musicfile-1>..<musicfile-n>\n\n", arg[0])
   os.exit(false, true)
end

-- Initialize variables 
local audio_volume = sdl.MIX_MAX_VOLUME
local audio_rate = sdl.MIX_DEFAULT_FREQUENCY
local audio_format = sdl.MIX_DEFAULT_FORMAT
local audio_channels = sdl.MIX_DEFAULT_CHANNELS
local audio_buffers = 4096
local looping = false
local interactive = false

-- Parse command-line arguments -----------------------------------------------

local i = 1
while arg[i] and arg[i]:sub(1, 1)=='-' do
   if arg[i]=="-r" then
      i=i+1; audio_rate = math.tointeger(arg[i]) or usage()
   elseif arg[i]=="-m" then audio_channels = 1;
   elseif arg[i]=="-c" then 
      i=i+1; audio_channels = math.tointeger(arg[i]) or usage()
   elseif arg[i]=="-b" then 
      i=i+1; audio_buffers = math.tointeger(arg[i]) or usage()
   elseif arg[i]=="-v" then
      i=i+1; audio_volume = math.tointeger(arg[i]) or usage()
   elseif arg[i]=="-l" then looping = true
   elseif arg[i]=="-i" then interactive = true
   elseif arg[i]=="-8" then audio_format = 'u8'
   elseif arg[i]=="-f32" then audio_format = 'f32'
   else usage()
   end
   i=i+1
end

local files = {} -- sound files to be played
while arg[i] do
   table.insert(files, arg[i])
   i=i+1
end

if #files==0 then usage() end

print(audio_volume, audio_rate, audio_format, audio_channels, audio_buffers, looping, interactive, table.unpack(files))

local function Menu()
   io.write("Available commands: (p)ause (r)esume (h)alt volume(v#) > ")
   local buf = io.read('l')
   local c = buf:sub(1, 1)
   if c == '0' then sdl.music_set_position(0)
   elseif c == '1' then sdl.music_set_position(10)
   elseif c == '2' then sdl.music_set_position(20)
   elseif c == '3' then sdl.music_set_position(30)
   elseif c == '4' then sdl.music_set_position(40)
   elseif c == 'p' or c =='P' then sdl.music_pause()
   elseif c == 'r' or c =='R' then sdl.music_resume()
   elseif c == 'h' or c =='H' then sdl.music_halt()
   elseif c == 'v' or c =='V' then
      local vol = buf:gmatch("%d+")()
      if vol then sdl.music_set_volume(vol) end
   end
   local playing = sdl.music_playing()
   local paused = sdl.music_paused()
   printf("Music playing: %s Paused: %s\n", playing and "yes" or"no", paused and "yes" or "no")
end

-- Init SDL2
sdl.init()

-- Open the audio device 

sdl.open_audio(audio_rate, audio_format, audio_channels, audio_buffers)
local open, audio_rate, audio_format, audio_channels = sdl.query_audio()
if not open then error("@@") end
printf("Opened audio, frequency=%d hz, format=%s, channels=%d buffersize=%d\n", 
                  audio_rate, audio_format, audio_channels, audio_buffers)

-- Set the music volume 
sdl.music_set_volume(audio_volume)

for _, file in ipairs(files) do
   local music = sdl.load_music(file)
   printf("Loaded file %s\n", file)
   printf("\ttype: %s\n", music:get_type())
   printf("\ttitle: %s\n", music:get_title_tag())
   printf("\tartist: %s\n", music:get_artist_tag())
   printf("\talbum: %s\n", music:get_album_tag())
   printf("\tcopyright: %s\n", music:get_copyright_tag())

   local loop_start = music:get_loop_start_time()
   local loop_end = music:get_loop_end_time()
   local loop_length = music:get_loop_length_time()
   
   -- Play and then exit 
   printf("Playing %s, duration %.f s\n", file, music:get_duration())
   if loop_start > 0.0 and loop_end > 0.0 and loop_length > 0.0 then
      printf("Loop points: start %.1f s, end %g s, length %.1f s\n", loop_start, loop_end, loop_length)
   end
   music:fade_in(2000, nil, looping and -1 or 0) -- looping = -1 means 'loop indefinitely'
   while music:playing() or music:paused() do
      if interactive then 
         Menu()
      else
         local current_position = music:get_position()
         if current_position >= 0.0 then
            printf("Position: %.1f seconds\n", current_position);
            io.stdout:flush()
            sdl.delay(1000)
         end
      end
   end
   music = music:free()
end

