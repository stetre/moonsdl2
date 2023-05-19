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

static int freeanimation(lua_State *L, ud_t *ud)
    {
    animation_t *animation = (animation_t*)ud->handle;
    freechildren(L, SURFACE_MT, ud);
    if(!freeuserdata(L, ud, "animation")) return 0;
    IMG_FreeAnimation(animation);
    return 0;
    }

static int newanimation(lua_State *L, animation_t *animation)
    {
    ud_t *ud;
    ud = newuserdata(L, animation, ANIMATION_MT, "animation");
    ud->parent_ud = NULL;
    ud->destructor = freeanimation;
    return 1;
    }

static int Create(lua_State *L)
    {
    const char *file = luaL_checkstring(L, 1);
    animation_t *animation = IMG_LoadAnimation(file);
    if(animation==NULL) GetError(L);
    return newanimation(L, animation);
    }

static int GetSize(lua_State *L)
    {
    animation_t *animation = checkanimation(L, 1, NULL);
    lua_pushinteger(L, animation->w);
    lua_pushinteger(L, animation->h);
    return 2;
    }

static int GetCount(lua_State *L)
    {
    animation_t *animation = checkanimation(L, 1, NULL);
    lua_pushinteger(L, animation->count);
    return 1;
    }

static int GetFrames(lua_State *L)
    {
    animation_t *animation = checkanimation(L, 1, NULL);
    lua_newtable(L);
    for(int i=0; i<animation->count; i++)
        {
        surface_t *surface = animation->frames[i];
        if(UD(surface))
            pushsurface(L, surface);
        else
            newsurface(L, surface, UD(animation));
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }

static int GetDelays(lua_State *L)
    {
    animation_t *animation = checkanimation(L, 1, NULL);
    lua_newtable(L);
    for(int i=0; i<animation->count; i++)
        {
        pushmilliseconds(L, animation->delays[i]);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }

DESTROY_FUNC(animation)

static const struct luaL_Reg Methods[] = 
    {
        { "free", Destroy },
        { "get_size", GetSize },
        { "get_count", GetCount },
        { "get_frames", GetFrames },
        { "get_delays", GetDelays },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "load_animation", Create },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_animation(lua_State *L)
    {
    udata_define(L, ANIMATION_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
IMG_Animation * IMG_LoadAnimation_RW(SDL_RWops *src, int freesrc);
IMG_Animation * IMG_LoadAnimationTyped_RW(SDL_RWops *src, int freesrc, const char *type);
IMG_Animation * IMG_LoadGIFAnimation_RW(SDL_RWops *src);
#endif
