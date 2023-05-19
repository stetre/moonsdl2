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

static int freemusic(lua_State *L, ud_t *ud)
    {
    music_t *music = (music_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "music")) return 0;
    Mix_FreeMusic(music);
    return 0;
    }

static int newmusic(lua_State *L, music_t *music)
    {
    ud_t *ud;
    ud = newuserdata(L, music, MUSIC_MT, "music");
    ud->parent_ud = NULL;
    ud->destructor = freemusic;
    return 1;
    }

static int Create(lua_State *L)
    {
    const char *file = luaL_checkstring(L, 1);
    music_t *music = Mix_LoadMUS(file);
    if(music==NULL) GetError(L);
    return newmusic(L, music);
    }

static int Play(lua_State *L)
    {
    music_t *music = checkmusic(L, 1, NULL); 
    int loops = luaL_optinteger(L, 2, 0);
    int ec = Mix_PlayMusic(music, loops);
    CheckError(L, ec);
    return 0;
    }

static int Type(lua_State *L)
    {
    music_t *music = checkmusic(L, 1, NULL); 
    pushmusictype(L, Mix_GetMusicType(music));
    return 1;
    }

static int FadeIn(lua_State *L)
    {
    music_t *music = checkmusic(L, 1, NULL); 
    int ms = checkmilliseconds(L, 2);
    double position = luaL_optnumber(L, 3, 0.0);
    int loops = luaL_optinteger(L, 4, 0);
    int ec = Mix_FadeInMusicPos(music, loops, ms, position);
    CheckError(L, ec);
    return 0;
    }

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    music_t *music = checkmusic(L, 1, NULL);        \
    const char *s = func(music);                    \
    if(!s) return 0;                                \
    lua_pushstring(L, s);                           \
    return 1;                                       \
    }
F(GetTitle, Mix_GetMusicTitle)
F(GetTitleTag, Mix_GetMusicTitleTag)
F(GetArtistTag, Mix_GetMusicArtistTag)
F(GetAlbumTag, Mix_GetMusicAlbumTag)
F(GetCopyrightTag, Mix_GetMusicCopyrightTag)
#undef F

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    music_t *music = checkmusic(L, 1, NULL);        \
    lua_pushnumber(L, func(music));                 \
    return 1;                                       \
    }
F(GetPosition, Mix_GetMusicPosition)
F(Duration, Mix_MusicDuration)
F(GetLoopStartTime, Mix_GetMusicLoopStartTime)
F(GetLoopEndTime, Mix_GetMusicLoopEndTime)
F(GetLoopLengthTime, Mix_GetMusicLoopLengthTime)
#undef F

/*----------------------------------------------------------------------*
 | music track   
 *----------------------------------------------------------------------*/

static int GetVolumeMusic(lua_State *L)
    {
    (void)optmusic(L, 1, NULL);
    lua_pushinteger(L, Mix_VolumeMusic(-1));
    return 1;
    }

static int SetVolumeMusic(lua_State *L)
    {
    int volume;
    (void)checkmusic(L, 1, NULL);
    volume = luaL_checkinteger(L, 2);
    Mix_VolumeMusic(volume);
    return 0;
    }

static int SetVolumeMusic1(lua_State *L)
    {
    int volume;
    volume = luaL_checkinteger(L, 1);
    Mix_VolumeMusic(volume);
    return 0;
    }

static int FadeOutMusic(lua_State *L)
    {
    int ms, ec;
    (void)checkmusic(L, 1, NULL);
    ms = luaL_checkinteger(L, 2);
    ec = Mix_FadeOutMusic(ms);
    lua_pushboolean(L, ec);
    return 1;
    }

static int FadeOutMusic1(lua_State *L)
    {
    int ms, ec;
    ms = luaL_checkinteger(L, 1);
    ec = Mix_FadeOutMusic(ms);
    lua_pushboolean(L, ec);
    return 1;
    }

static int FadingMusic(lua_State *L)
    {
    (void)optmusic(L, 1, NULL);
    pushfading(L, Mix_FadingMusic());
    return 1;
    }

#define F(Func, func)               \
static int Func(lua_State *L)       \
    {                               \
    (void)optmusic(L, 1, NULL);     \
    func();                         \
    return 0;                       \
    }
F(HaltMusic, Mix_HaltMusic)
F(PauseMusic, Mix_PauseMusic)
F(ResumeMusic, Mix_ResumeMusic)
F(RewindMusic, Mix_RewindMusic)
#undef F

#define F(Func, func)               \
static int Func(lua_State *L)       \
    {                               \
    (void)optmusic(L, 1, NULL);     \
    lua_pushboolean(L, func());     \
    return 1;                       \
    }
F(PausedMusic, Mix_PausedMusic)
F(PlayingMusic, Mix_PlayingMusic)
#undef F

static int SetMusicPosition(lua_State *L)
    {
    int ec;
    double position;
    (void)checkmusic(L, 1, NULL);
    position = luaL_checknumber(L, 2);
    ec = Mix_SetMusicPosition(position);
    CheckError(L, ec);
    return 0;
    }

static int SetMusicPosition1(lua_State *L)
    {
    int ec;
    double position;
    position = luaL_checknumber(L, 1);
    ec = Mix_SetMusicPosition(position);
    CheckError(L, ec);
    return 0;
    }

DESTROY_FUNC(music)

static const struct luaL_Reg Methods[] = 
    {
        { "free", Destroy },
        { "play", Play },
        { "fade_in", FadeIn },
        { "get_type", Type },
        { "get_title", GetTitle },
        { "get_title_tag", GetTitleTag },
        { "get_artist_tag", GetArtistTag },
        { "get_album_tag", GetAlbumTag },
        { "get_copyright_tag", GetCopyrightTag },
        { "get_volume", GetVolumeMusic },
        { "set_volume", SetVolumeMusic },
        { "get_position", GetPosition },
        { "set_position", SetMusicPosition },
        { "get_duration", Duration },
        { "get_loop_start_time", GetLoopStartTime },
        { "get_loop_end_time", GetLoopEndTime },
        { "get_loop_length_time", GetLoopLengthTime },
        { "fade_out", FadeOutMusic },
        { "fading", FadingMusic },
        { "halt", HaltMusic },
        { "pause", PauseMusic },
        { "resume", ResumeMusic },
        { "rewind", RewindMusic },
        { "paused", PausedMusic },
        { "playing", PlayingMusic },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "load_music", Create },
        { "music_get_volume", GetVolumeMusic },
        { "music_set_volume", SetVolumeMusic1 },
        { "music_set_position", SetMusicPosition1 },
        { "music_fade_out", FadeOutMusic1 },
        { "music_fading", FadingMusic },
        { "music_halt", HaltMusic },
        { "music_pause", PauseMusic },
        { "music_resume", ResumeMusic },
        { "music_rewind", RewindMusic },
        { "music_paused", PausedMusic },
        { "music_playing", PlayingMusic },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_music(lua_State *L)
    {
    udata_define(L, MUSIC_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
//Mix_Music* Mix_LoadMUS_RW(SDL_RWops *src, int freesrc);
//Mix_Music* Mix_LoadMUSType_RW(SDL_RWops *src, Mix_MusicType type, int freesrc);
#endif
