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

static int freechunk(lua_State *L, ud_t *ud)
    {
    chunk_t *chunk = (chunk_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "chunk")) return 0;
    Mix_FreeChunk(chunk);
    return 0;
    }

static int newchunk(lua_State *L, chunk_t *chunk)
    {
    ud_t *ud;
    ud = newuserdata(L, chunk, CHUNK_MT, "chunk");
    ud->parent_ud = NULL;
    ud->destructor = freechunk;
    return 1;
    }

static int Create(lua_State *L)
    {
    const char *file = luaL_checkstring(L, 1);
    chunk_t *chunk = Mix_LoadWAV(file);
    if(chunk==NULL) GetError(L);
    return newchunk(L, chunk);
    }

static int GetVolume(lua_State *L)
    {
    chunk_t *chunk = checkchunk(L, 1, NULL); 
    lua_pushinteger(L, Mix_VolumeChunk(chunk, -1));
    return 1;
    }

static int SetVolume(lua_State *L)
    {
    chunk_t *chunk = checkchunk(L, 1, NULL); 
    int volume = luaL_checkinteger(L, 2);
    Mix_VolumeChunk(chunk, volume);
    return 0;
    }

static int Play(lua_State *L)
    {
    chunk_t *chunk = checkchunk(L, 1, NULL); 
    int track = luaL_optinteger(L, 2, -1);
    int loops = luaL_optinteger(L, 3, 0);
    int maxms = optmilliseconds(L, 4, -1);
    track = Mix_PlayChannelTimed(track, chunk, loops, maxms);
    lua_pushinteger(L, track);
    return 1;
    }

static int FadeIn(lua_State *L)
    {
    chunk_t *chunk = checkchunk(L, 1, NULL); 
    int ms = checkmilliseconds(L, 2);
    int track = luaL_optinteger(L, 3, -1);
    int loops = luaL_optinteger(L, 4, 0);
    int maxms = optmilliseconds(L, 5, -1);
    track = Mix_FadeInChannelTimed(track, chunk, loops, ms, maxms);
    lua_pushinteger(L, track);
    return 1;
    }

DESTROY_FUNC(chunk)

static const struct luaL_Reg Methods[] = 
    {
        { "free", Destroy },
        { "get_volume", GetVolume },
        { "set_volume", SetVolume },
        { "play", Play },
        { "fade_in", FadeIn },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "load_wav", Create },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_chunk(lua_State *L)
    {
    udata_define(L, CHUNK_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
Mix_Chunk* Mix_LoadWAV_RW(SDL_RWops *src, int freesrc);
Mix_Chunk* Mix_QuickLoad_WAV(Uint8 *mem);
Mix_Chunk* Mix_QuickLoad_RAW(Uint8 *mem, Uint32 len);
//int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops);
//int Mix_FadeInChannel(int channel, Mix_Chunk *chunk, int loops, int ms);
#endif
