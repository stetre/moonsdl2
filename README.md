## MoonSDL2: Lua bindings for SDL2

MoonSDL2 is a Lua binding library for the [Simple DirectMedia Layer (SDL2)](https://www.libsdl.org).

It runs on GNU/Linux and on Windows (MSYS2/MinGW or MSVC) and requires
[Lua](http://www.lua.org/) (>=5.3),
[SDL2](https://github.com/libsdl-org/SDL/releases/) (>= 2.26.0),
[SDL_image](https://github.com/libsdl-org/SDL_image/releases/) (>= 2.6.3),
[SDL_mixer](https://github.com/libsdl-org/SDL_mixer/releases/) (>= 2.6.0), and
[SDL_ttf](https://github.com/libsdl-org/SDL_ttf/releases/) (>= 2.20.0).

_Author:_ _[Stefano Trettel](https://www.linkedin.com/in/stetre)_

[![Lua logo](./doc/powered-by-lua.gif)](http://www.lua.org/)

#### License

MIT/X11 license (same as Lua). See [LICENSE](./LICENSE).

#### Documentation

See the [Reference Manual](https://stetre.github.io/moonsdl2/doc/index.html).

#### Getting and installing

Setup the build environment as described [here](https://github.com/stetre/moonlibs), then:

```sh
$ git clone https://github.com/stetre/moonsdl2/
$ cd moonsdl2
moonsdl2$ make
moonsdl2$ sudo make install
```

**On Windows Using Visual Studio**

```
> git clone https://github.com/stetre/moonsdl2/
> cd moonsdl2\src
> mingw32-make -f Makefile.msvc ^
    LUAVER=<lua version> ^
    LUA_INCDIR=<path to directory containing lua.h> ^
    LUA_LIBDIR=<path to directory containing lua5X.dll> ^
    SDL_INCDIR=<path to directory containing SDL2.h, SDL2_image.h, etc.> ^
    SDL_LIBDIR=<path to directory containing SDL2.lib, SDL2_image.lib, etc.>
```

`<lua version>` is in dotted notation, like "5.1" (no quotes).

Ensure all the other directories don't have spaces in their paths.

If you don't have access to `mingw32-make`, you can edit `src\rebuild.bat` to
suit your Lua version and paths, and then run it (from the `src` directory) to
perform a full rebuild.

#### Example

The example below creates a window and draws a triangle using SDL2's 2D renderer.

Other examples can be found in the **examples/** directory contained in the release package.

```lua
-- MoonSDL2 example: hello.lua
local sdl = require("moonsdl2")

sdl.init()

local window = sdl.create_window("Hello, triangle!", nil, nil, 800, 600, sdl.WINDOW_SHOWN)

local renderer = sdl.create_renderer(window, nil, sdl.RENDERER_ACCELERATED|sdl.RENDERER_PRESENTVSYNC)

local vertices =
    { --   position           color
        { { 400, 150 }, { 255, 0, 0, 255 } },
        { { 200, 450 }, { 0, 0, 255, 255 } },
        { { 600, 450 }, { 0, 255, 0, 255 } },
    }

local quit = false
while not quit do
   e = sdl.poll_event()
   if e then
      if e.type == 'quit' then quit = true end
   end
   renderer:set_draw_color({0, 0, 0, 255})
   renderer:clear()
   renderer:render_geometry(nil, vertices, nil)
   renderer:present()
end
```

The script can be executed at the shell prompt with the standard Lua interpreter:

```shell
$ lua hello.lua
```

#### See also

* [MoonLibs - Graphics and Audio Lua Libraries](https://github.com/stetre/moonlibs).
