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
 
int checkdisplaymode(lua_State *L, int arg, SDL_DisplayMode *dst)
    {
    if(lua_type(L, arg)!=LUA_TTABLE)
        return argerror(L, arg, ERR_TABLE);
    lua_getfield(L, arg, "format"); dst->format = checkpixelformat(L, -1); lua_pop(L, 1);
    lua_getfield(L, arg, "w"); dst->w = luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_getfield(L, arg, "h"); dst->h = luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_getfield(L, arg, "refresh_rate"); dst->refresh_rate = luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_getfield(L, arg, "driverdata"); dst->driverdata = checklightuserdata(L, -1); lua_pop(L, 1);
    return ERR_SUCCESS;
    }

int pushdisplaymode(lua_State *L, SDL_DisplayMode *mode)
    {
    lua_newtable(L);
    pushpixelformat(L, mode->format); lua_setfield(L, -2, "format");
    lua_pushinteger(L, mode->w); lua_setfield(L, -2, "w");
    lua_pushinteger(L, mode->h); lua_setfield(L, -2, "h");
    lua_pushinteger(L, mode->refresh_rate); lua_setfield(L, -2, "refresh_rate");
    if(mode->driverdata)
        { lua_pushlightuserdata(L, mode->driverdata); lua_setfield(L, -2, "driverdata"); }
    return 1;
    }

static int IsScreenSaverEnabled(lua_State *L)
    {
    lua_pushboolean(L, SDL_IsScreenSaverEnabled());
    return 1;
    }

static int EnableScreenSaver(lua_State *L)
    {
    (void)L;
    SDL_EnableScreenSaver();
    return 0;
    }

static int DisableScreenSaver(lua_State *L)
    {
    (void)L;
    SDL_DisableScreenSaver();
    return 0;
    }

#if 0
static int VideoInit(lua_State *L)
    {
    const char *driver_name = luaL_optstring(L, 1, NULL);
    int ec = SDL_VideoInit(driver_name);
    CheckError(L, ec);
    return 0;
    }

static int VideoQuit(lua_State *L)
    {
    (void)L;
    SDL_VideoQuit();
    return 0;
    }
#endif

static int GetCurrentVideoDriver(lua_State *L)
    {
    const char *name = SDL_GetCurrentVideoDriver();
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

static int GetNumVideoDrivers(lua_State *L)
    {
    int count = SDL_GetNumVideoDrivers();
    if(count < 0) GetError(L);
    lua_pushinteger(L, count);
    return 1;
    }
static int GetVideoDriver(lua_State *L)
    {
    int driver_index = checkindex(L, 1);
    const char* name = SDL_GetVideoDriver(driver_index);
    if(!name) GetError(L);
    lua_pushstring(L, name);
    return 1;
    }

static int GetNumDisplays(lua_State *L)
    {
    int count = SDL_GetNumVideoDisplays();
    if(count < 0) GetError(L);
    lua_pushinteger(L, count);
    return 1;
    }

static int GetDisplayName(lua_State *L)
    {
    int display_index = checkindex(L, 1);
    const char* name = SDL_GetDisplayName(display_index);
    lua_pushstring(L, name);
    return 1;
    }

static int GetDisplayBounds(lua_State *L)
    {
    rect_t rect;
    int display_index = checkindex(L, 1);
    int ec = SDL_GetDisplayBounds(display_index, &rect);
    CheckError(L, ec);
    pushrect(L, &rect);
    return 1;
    }

static int GetDisplayUsableBounds(lua_State *L)
    {
    rect_t rect;
    int display_index = checkindex(L, 1);
    int ec = SDL_GetDisplayUsableBounds(display_index, &rect);
    CheckError(L, ec);
    pushrect(L, &rect);
    return 1;
    }

static int GetDisplayDPI(lua_State *L)
    {
    float ddpi, hdpi, vdpi;
    int display_index = checkindex(L, 1);
    int ec = SDL_GetDisplayDPI(display_index, &ddpi, &hdpi, &vdpi);
    CheckError(L, ec);
    lua_pushnumber(L, ddpi);
    lua_pushnumber(L, hdpi);
    lua_pushnumber(L, vdpi);
    return 3;
    }

static int GetDisplayOrientation(lua_State *L)
    {
    int display_index = checkindex(L, 1);
    SDL_DisplayOrientation value = SDL_GetDisplayOrientation(display_index);
    pushorientation(L, value);
    return 1;
    }

static int GetDisplayModes(lua_State *L)
    {
    SDL_DisplayMode mode;
    int i, ec;
    int display_index = checkindex(L, 1);
    int count = SDL_GetNumDisplayModes(display_index);
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        ec = SDL_GetDisplayMode(display_index, i, &mode);
        CheckError(L, ec);
        pushdisplaymode(L, &mode);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }

static int GetDesktopDisplayMode(lua_State *L)
    {
    SDL_DisplayMode mode;
    int display_index = checkindex(L, 1);
    int ec = SDL_GetDesktopDisplayMode(display_index, &mode);
    CheckError(L, ec);
    pushdisplaymode(L, &mode);
    return 1;
    }

static int GetCurrentDisplayMode(lua_State *L)
    {
    SDL_DisplayMode mode;
    int display_index = checkindex(L, 1);
    int ec = SDL_GetCurrentDisplayMode(display_index, &mode);
    CheckError(L, ec);
    pushdisplaymode(L, &mode);
    return 1;
    }

static int GetClosestDisplayMode(lua_State *L)
    {
    SDL_DisplayMode mode, closest;
    int display_index = checkindex(L, 1);
    checkdisplaymode(L, 2, &mode);
    if(SDL_GetClosestDisplayMode(display_index, &mode, &closest)==NULL)
        return 0;
    pushdisplaymode(L, &closest);
    return 1;
    }

static int GetPointDisplayIndex(lua_State *L)
    {
    int display_index;
    point_t point;
    checkpoint(L, 1, &point);
    display_index = SDL_GetPointDisplayIndex(&point);
    if(display_index < 0) GetError(L);
    pushindex(L, display_index);
    return 1;
    }

static int GetRectDisplayIndex(lua_State *L)
    {
    int display_index;
    rect_t rect;
    checkrect(L, 1, &rect);
    display_index = SDL_GetRectDisplayIndex(&rect);
    if(display_index < 0) GetError(L);
    pushindex(L, display_index);
    return 1;
    }

/* ----------------------------------------------------------------------- */

static const struct luaL_Reg Functions[] = 
    {
        { "is_screen_saver_enabled", IsScreenSaverEnabled },
        { "enable_screen_saver", EnableScreenSaver },
        { "disable_screen_saver", DisableScreenSaver },
        { "get_num_video_drivers", GetNumVideoDrivers },
        { "get_video_driver", GetVideoDriver },
//      { "video_init", VideoInit },
//      { "video_quit", VideoQuit },
        { "get_current_video_driver", GetCurrentVideoDriver },
        { "get_num_displays", GetNumDisplays },
        { "get_display_name", GetDisplayName },
        { "get_display_bounds", GetDisplayBounds },
        { "get_display_usable_bounds", GetDisplayUsableBounds },
        { "get_display_dpi", GetDisplayDPI },
        { "get_display_orientation", GetDisplayOrientation },
        { "get_display_modes", GetDisplayModes },
        { "get_desktop_display_mode", GetDesktopDisplayMode },
        { "get_current_display_mode", GetCurrentDisplayMode },
        { "get_closest_display_mode", GetClosestDisplayMode },
        { "get_point_display_index", GetPointDisplayIndex },
        { "get_rect_display_index", GetRectDisplayIndex },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_display(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

