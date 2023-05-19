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
 
void pushmousebuttonstate(lua_State *L, uint32_t state)
    {
    lua_newtable(L);
    lua_pushboolean(L, state & SDL_BUTTON_LMASK); lua_setfield(L, -2, "left");
    lua_pushboolean(L, state & SDL_BUTTON_RMASK); lua_setfield(L, -2, "right");
    lua_pushboolean(L, state & SDL_BUTTON_MMASK); lua_setfield(L, -2, "middle");
    lua_pushboolean(L, state & SDL_BUTTON_X1MASK); lua_setfield(L, -2, "x1");
    lua_pushboolean(L, state & SDL_BUTTON_X2MASK); lua_setfield(L, -2, "x2");
    }

static int GetMouseFocus(lua_State *L)
    {
    window_t *window = SDL_GetMouseFocus();
    if(!window) return 0;
    pushwindow(L, window);
    return 1;
    }

#define F(Func, func)                           \
static int Func(lua_State *L)                   \
    {                                           \
    int x, y;                                   \
    uint32_t state = func(&x, &y);              \
    lua_pushinteger(L, x);                      \
    lua_pushinteger(L, y);                      \
    pushmousebuttonstate(L, state);             \
    return 3;                                   \
    }
F(GetMouseState, SDL_GetMouseState)
F(GetGlobalMouseState, SDL_GetGlobalMouseState)
F(GetRelativeMouseState, SDL_GetRelativeMouseState)
#undef F

static int WarpMouseInWindow(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    SDL_WarpMouseInWindow(window, x, y);
    return 0;
    }

static int WarpMouseGlobal(lua_State *L)
    {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    if(SDL_WarpMouseGlobal(x, y) < 0) GetError(L);
    return 0;
    }

static int SetRelativeMouseMode(lua_State *L)
    {
    int enabled = checkboolean(L, 1);
    if(SDL_SetRelativeMouseMode(enabled) < 0) GetError(L);
    return 0;
    }

static int CaptureMouse(lua_State *L)
    {
    int enabled = checkboolean(L, 1);
    if(SDL_CaptureMouse(enabled) < 0) GetError(L);
    return 0;
    }

static int GetRelativeMouseMode(lua_State *L)
    {
    lua_pushboolean(L, SDL_GetRelativeMouseMode());
    return 1;
    }

/* ----------------------------------------------------------------------- */

static const struct luaL_Reg Functions[] = 
    {
        { "get_mouse_focus", GetMouseFocus },
        { "get_mouse_state", GetMouseState },
        { "get_global_mouse_state", GetGlobalMouseState },
        { "get_relative_mouse_state", GetRelativeMouseState },
        { "warp_mouse_in_window", WarpMouseInWindow },
        { "warp_mouse_global", WarpMouseGlobal },
        { "set_relative_mouse_mode", SetRelativeMouseMode },
        { "capture_mouse", CaptureMouse },
        { "get_relative_mouse_mode", GetRelativeMouseMode },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_mouse(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
#define ADD(what)   do { lua_pushinteger(L, SDL_##what); lua_setfield(L, -2, #what); } while(0)
    ADD(TOUCH_MOUSEID);
#undef ADD
    }
