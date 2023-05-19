/* The MIT License (MIT)
 *
 * Copyright (c) 2023 Stefano Trettel
 *
 * Software repository: MoonSDL2, https://github.com/stetre/moonsdl2
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

lua_State *moonsdl2_L;

static int Quit(lua_State *L)
    {
    moonsdl2_close_audio(L);
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
    }

static void AtExit(void)
    {
    if(moonsdl2_L)
        {
        Quit(moonsdl2_L);
        enums_free_all(moonsdl2_L);
        moonsdl2_L = NULL;
        }
    }
 
static int AddVersions(lua_State *L)
    {
    lua_pushstring(L, "_VERSION");
    lua_pushstring(L, "MoonSDL2 "MOONSDL2_VERSION);
    lua_settable(L, -3);


    do {
        SDL_version ver;
        SDL_GetVersion(&ver);
        if(!((ver.major == 2) && (ver.minor >= 26)))
            return luaL_error(L, "MoonSDL requires SDL2 >= 2.26.0 (found %d.%d.%d)",
                    ver.major, ver.minor, ver.patch);
        lua_pushstring(L, "_SDL_VERSION");
        lua_pushfstring(L, "SDL %d.%d.%d", ver.major, ver.minor, ver.patch);
        lua_settable(L, -3);
    } while(0);

    do {
        const SDL_version *ver = IMG_Linked_Version();
        if(!((ver->major == 2) && (ver->minor >= 6)))
            return luaL_error(L, "MoonSDL requires SDL_image >= 2.6.0 (found %d.%d.%d)",
                    ver->major, ver->minor, ver->patch);
        lua_pushstring(L, "_SDL_IMG_VERSION");
        lua_pushfstring(L, "SDL_image %d.%d.%d", ver->major, ver->minor, ver->patch);
        lua_settable(L, -3);
    } while(0);

    do {
        const SDL_version *ver = TTF_Linked_Version();
        if(!((ver->major == 2) && (ver->minor >= 20)))
            return luaL_error(L, "MoonSDL requires SDL_ttf >= 2.20.0 (found %d.%d.%d)",
                    ver->major, ver->minor, ver->patch);
        lua_pushstring(L, "_SDL_TTF_VERSION");
        lua_pushfstring(L, "SDL_ttf %d.%d.%d", ver->major, ver->minor, ver->patch);
        lua_settable(L, -3);
    } while(0);

    do {
        const SDL_version *ver = Mix_Linked_Version();
        if(!((ver->major == 2) && (ver->minor >= 6)))
            return luaL_error(L, "MoonSDL requires SDL_mixer >= 2.6.0 (found %d.%d.%d)",
                    ver->major, ver->minor, ver->patch);
        lua_pushstring(L, "_SDL_MIX_VERSION");
        lua_pushfstring(L, "SDL_mixer %d.%d.%d", ver->major, ver->minor, ver->patch);
        lua_settable(L, -3);
    } while(0);

    return 0;
    }

#define EVERYTHING (uint32_t)-1
static uint32_t sdl_flags = 0;
static uint32_t img_flags = 0;
static uint32_t mix_flags = 0;

static int Init(lua_State *L)
/* You're lazy, you just init all */
    {
    int ec;
    ec = SDL_Init(EVERYTHING); CheckError(L, ec);
    sdl_flags = SDL_WasInit(0);
    img_flags = IMG_Init(EVERYTHING);
    ec = TTF_Init(); CheckError(L, ec);
    mix_flags = Mix_Init(EVERYTHING);
    moonsdl2_reserve_event_codes(L);
    // flags indicating what has actually been initialized (ttf has no flags)
    lua_pushinteger(L, sdl_flags);
    lua_pushinteger(L, img_flags);
    lua_pushinteger(L, mix_flags);
    return 3;
    }

static int WasInit(lua_State *L)
    {
    lua_pushinteger(L, sdl_flags);
    lua_pushinteger(L, img_flags);
    lua_pushinteger(L, mix_flags);
    return 3;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "init", Init },
        { "was_init", WasInit },
        { "quit", Quit },
        { NULL, NULL } /* sentinel */
    };

int luaopen_moonsdl2(lua_State *L)
/* Lua calls this function to load the module */
    {
    moonsdl2_L = L;
    moonsdl2_utils_init(L);
    atexit(AtExit);

    lua_newtable(L); /* the module table */
    AddVersions(L);
    moonsdl2_open_tracing(L);
    moonsdl2_open_enums(L);
    moonsdl2_open_flags(L);
    luaL_setfuncs(L, Functions, 0);
    moonsdl2_open_log(L);
    moonsdl2_open_hints(L);
    moonsdl2_open_window(L);
    moonsdl2_open_surface(L);
    moonsdl2_open_pxlfmt(L);
    moonsdl2_open_palette(L);
    moonsdl2_open_events(L);
    moonsdl2_open_renderer(L);
    moonsdl2_open_texture(L);
    moonsdl2_open_keyboard(L);
    moonsdl2_open_mouse(L);
    moonsdl2_open_cursor(L);
    moonsdl2_open_joystick(L);
    moonsdl2_open_gamecontroller(L);
    moonsdl2_open_misc(L);
    moonsdl2_open_timer(L);
    moonsdl2_open_glcontext(L);
    moonsdl2_open_display(L);
    moonsdl2_open_sensor(L);
    moonsdl2_open_haptic(L);
    moonsdl2_open_touch(L);
    moonsdl2_open_messagebox(L);
    // SDL_image
    moonsdl2_open_animation(L);
    // SDL_ttf
    moonsdl2_open_font(L);
    // SDL_mixer
    moonsdl2_open_chunk(L);
    moonsdl2_open_music(L);
    moonsdl2_open_mixer(L);

#if 0 //@@
    /* Add functions implemented in Lua */
    lua_pushvalue(L, -1); lua_setglobal(L, "moonsdl2");
    if(luaL_dostring(L, "require('moonsdl2.random')") != 0) lua_error(L);
    lua_pushnil(L);  lua_setglobal(L, "moonsdl2");
#endif
    return 1;
    }

