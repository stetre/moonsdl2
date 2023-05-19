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

static int freecursor(lua_State *L, ud_t *ud)
    {
    cursor_t *cursor = (cursor_t*)ud->handle;
    int isborrowed = IsBorrowed(ud);
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "cursor")) return 0;
    if(!isborrowed)
        SDL_FreeCursor(cursor);
    return 0;
    }

static int newcursor(lua_State *L, cursor_t *cursor, ud_t *parent_ud, int borrowed)
    {
    ud_t *ud;
    ud = newuserdata(L, cursor, CURSOR_MT, "cursor");
    ud->parent_ud = parent_ud;
    ud->destructor = freecursor;
    if(borrowed) MarkBorrowed(ud);
    return 1;
    }

static int Create(lua_State *L)
    {
    uint8_t *data = checklightuserdata(L, 1);
    uint8_t *mask = checklightuserdata(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int hot_x = luaL_checkinteger(L, 5);
    int hot_y = luaL_checkinteger(L, 6);
    cursor_t *cursor = SDL_CreateCursor(data, mask, w, h, hot_x, hot_y);
    if(cursor==NULL) GetError(L);
    return newcursor(L, cursor, NULL, 0);
    }

static int CreateColorCursor(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    int hot_x = luaL_checkinteger(L, 2);
    int hot_y = luaL_checkinteger(L, 3);
    cursor_t *cursor = SDL_CreateColorCursor(surface, hot_x, hot_y);
    if(cursor==NULL) GetError(L);
    return newcursor(L, cursor, UD(surface), 0);
    }

static int CreateSystemCursor(lua_State *L)
    {
    SDL_SystemCursor id = checksystemcursor(L, 1);
    cursor_t *cursor = SDL_CreateSystemCursor(id);
    if(cursor==NULL) GetError(L);
    return newcursor(L, cursor, NULL, 0);
    }

static int SetCursor(lua_State *L)
    {
    cursor_t *cursor = optcursor(L, 1, NULL);
    SDL_SetCursor(cursor);
    return 0;
    }

#define F(Func, func)                   \
static int Func(lua_State *L)           \
    {                                   \
    cursor_t *cursor = func();          \
    if(!cursor) return 0;               \
    if(UD(cursor))                      \
        pushcursor(L, cursor);          \
    else                                \
        newcursor(L, cursor, NULL, 1);  \
    return 1;                           \
    }
F(GetCursor, SDL_GetCursor)
F(GetDefaultCursor, SDL_GetDefaultCursor)
#undef F

static int ShowCursor(lua_State *L)
    {
    if(SDL_ShowCursor(SDL_ENABLE) < 0) GetError(L);
    return 0;
    }

static int HideCursor(lua_State *L)
    {
    if(SDL_ShowCursor(SDL_DISABLE) < 0) GetError(L);
    return 0;
    }

static int CursorState(lua_State *L)
    {
    int rc = SDL_ShowCursor(SDL_QUERY);
    switch(rc)
        {
        case SDL_ENABLE: lua_pushstring(L, "shown"); return 1;
        case SDL_DISABLE: lua_pushstring(L, "hidden"); return 1;
        default: GetError(L);
        }
    return 0;
    }

DESTROY_FUNC(cursor)

static const struct luaL_Reg Methods[] = 
    {
        { "free", Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_cursor", Create },
        { "create_color_cursor", CreateColorCursor },
        { "create_system_cursor", CreateSystemCursor },
        { "set_cursor", SetCursor },
        { "get_cursor", GetCursor },
        { "get_default_cursor", GetDefaultCursor },
        { "show_cursor", ShowCursor },
        { "hide_cursor", HideCursor },
        { "cursor_state", CursorState },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_cursor(lua_State *L)
    {
    udata_define(L, CURSOR_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }
