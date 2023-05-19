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


static void Callback(void *usrdata, const char *name, const char *oldval, const char *newval)
    {
    int top, rc;
    lua_State *L = moonsdl2_L;
    hintcb_t *hintcb = (hintcb_t*)(usrdata);
    if(!UD(hintcb)) return;
    top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, hintcb->ref);
    lua_pushstring(L, name);
    lua_pushstring(L, oldval);
    lua_pushstring(L, newval);
    rc = lua_pcall(L, 3, 0, 0);
    if(rc!=LUA_OK) lua_error(L);
    lua_settop(L, top);
    }

static int freehintcb(lua_State *L, ud_t *ud)
    {
    hintcb_t *hintcb = (hintcb_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "hintcb")) return 0;
    luaL_unref(L, LUA_REGISTRYINDEX, hintcb->ref);
    SDL_DelHintCallback(hintcb->name, Callback, hintcb);
    Free(L, hintcb->name);
    Free(L, hintcb);
    return 0;
    }

static int newhintcb(lua_State *L, hintcb_t *hintcb)
    {
    ud_t *ud;
    ud = newuserdata(L, hintcb, HINTCB_MT, "hintcb");
    ud->parent_ud = NULL;
    ud->destructor = freehintcb;
    return 1;
    }

static int Create(lua_State *L)
    {
    hintcb_t *hintcb = NULL;
    const char *name = luaL_checkstring(L, 1);
    if(!lua_isfunction(L, 2)) return argerror(L, 2, ERR_FUNCTION);
    hintcb = Malloc(L, sizeof(hintcb_t));
    lua_pushvalue(L, 2);
    hintcb->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    hintcb->name = Strdup(L, name);
    SDL_AddHintCallback(hintcb->name, Callback, hintcb);
    return newhintcb(L, hintcb);
    }
 
static int SetHint(lua_State *L)
    {
    int rc;
    const char *name = luaL_checkstring(L, 1);
    const char *value = luaL_checkstring(L, 2);
    if(lua_isnoneornil(L, 3))
        rc = SDL_SetHint(name, value);
    else
        {
        int priority = checkhintpriority(L, 3);
        rc = SDL_SetHintWithPriority(name, value, priority);
        }
    lua_pushboolean(L, rc);
    return 1;
    }

static int GetHint(lua_State *L)
    {
    const char *name = luaL_checkstring(L, 1);
    const char *value = SDL_GetHint(name);
    if(!value) return 0;
    lua_pushstring(L, value);
    return 1;
    }

static int ResetHint(lua_State *L)
    {
    const char *name = luaL_checkstring(L, 1);
    lua_pushboolean(L, SDL_ResetHint(name));
    return 1;
    }

static int ResetHints(lua_State *L) /* execute callbacks */
    {
    (void)L;
    SDL_ResetHints();
    return 0;
    }

DESTROY_FUNC(hintcb)

static const struct luaL_Reg Methods[] = 
    {
        { "del", Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "add_hint_callback", Create },
        { "set_hint", SetHint },
        { "get_hint", GetHint },
        { "reset_hint", ResetHint },
        { "reset_hints", ResetHints },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_hints(lua_State *L)
    {
    udata_define(L, HINTCB_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
//SDL_bool SDL_GetHintBoolean(const char *name, SDL_bool default_value); -> confusing
#endif

