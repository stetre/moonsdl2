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
 
static int audio_is_open;
int moonsdl2_close_audio(lua_State *L)
    {
    (void)L;
    if(audio_is_open)
        {
        Mix_CloseAudio();
        audio_is_open = 0;
        }
    return 0;
    }

static int OpenAudio(lua_State *L)
    {
    int frequency = luaL_checkinteger(L, 1);
    uint16_t format = optaudioformat(L, 2, MIX_DEFAULT_FORMAT);
    int channels = luaL_checkinteger(L, 3); // proper audio 'channels' (1=mono, 2=stereo, etc)
    int chunksize = luaL_checkinteger(L, 4);
    if(Mix_OpenAudio(frequency, format, channels, chunksize)!=0) GetError(L);
    audio_is_open++;
    return 0;
    }

#if 0
static int OpenAudioDevice(lua_State *L)
    {
    int frequency = luaL_checkinteger(L, 1);
    uint16_t format = optaudioformat(L, 2, MIX_DEFAULT_FORMAT);
    int channels = luaL_checkinteger(L, 3); // proper audio 'channels' (1=mono, 2=stereo, etc)
    int chunksize = luaL_checkinteger(L, 4);
    const char *device = luaL_optstring(L, 5, NULL);
    int allowed_changes = checkflags(L, 6); // audioallowflags
    int ec = Mix_OpenAudioDevice(frequency, format, channels, chunksize, device, allowed_changes);
    CheckError(L, ec);
    audio_is_open++;
    return 0;
    }
#endif

static int QuerySpec(lua_State *L)
    {
    int frequency, channels;
    uint16_t format;
    int opened = Mix_QuerySpec(&frequency, &format, &channels);
    lua_pushboolean(L, opened);
    if(!opened)
        return 1;
    lua_pushinteger(L, frequency);
    pushaudioformat(L, format);
    lua_pushinteger(L, channels);
    return 4;
    }

#define F(Func, what)                               \
static int Func(lua_State *L)                       \
    {                                               \
    int count = Mix_GetNum##what##s();              \
    lua_newtable(L);                                \
    for(int i=0; i<count; i++)                      \
        {                                           \
        lua_pushstring(L, Mix_Get##what(i));        \
        lua_rawseti(L, -2, i+1);                    \
        }                                           \
    return 1;                                       \
    }
F(GetMusicDecoders, MusicDecoder)
F(GetChunkDecoders, ChunkDecoder)
#undef F


#define F(Func, func)                           \
static int Func(lua_State *L)                   \
    {                                           \
    const char *name = luaL_checkstring(L, 1);  \
    lua_pushboolean(L, func(name));             \
    return 1;                                   \
    }
F(HasChunkDecoder, Mix_HasChunkDecoder)
F(HasMusicDecoder, Mix_HasMusicDecoder)
#undef F

/*----------------------------------------------------------------------*
 | SDL_audio_device.h
 *----------------------------------------------------------------------*/

#if 0
/* We need these if we want to query for available devices */

static void pushaudiospec(lua_State *L, const SDL_AudioSpec *spec)
    {
    lua_newtable(L);
    pushaudioformat(L, spec->format); lua_setfield(L, -2, "format");
    lua_pushinteger(L, spec->channels); lua_setfield(L, -2, "channels");
    lua_pushinteger(L, spec->silence); lua_setfield(L, -2, "silence");
    lua_pushinteger(L, spec->samples); lua_setfield(L, -2, "samples");
    lua_pushinteger(L, spec->padding); lua_setfield(L, -2, "padding");
    lua_pushinteger(L, spec->size); lua_setfield(L, -2, "size");
    //SDL_AudioCallback callback; 
    //void *userdata;             
    }

static int GetAudioDeviceNames(lua_State *L)
    {
    int index;
    int count = SDL_GetNumAudioDevices(0);
    if(count < 0) GetError(L);
    lua_newtable(L);
    for(index = 0; index< count; index++)
        {
        lua_pushstring(L, SDL_GetAudioDeviceName(index, 0));
        lua_rawseti(L, -2, index+1);
        }
    return 1;
    }

static int GetAudioDeviceSpec(lua_State *L)
    {
    SDL_AudioSpec spec;
    int index = checkindex(L, 1);
    if(SDL_GetAudioDeviceSpec(index, 0, &spec) != 0) GetError(L);
    pushaudiospec(L, &spec);
    return 1;
    }
#endif

/*----------------------------------------------------------------------*
 | chunk tracks
 *----------------------------------------------------------------------*/


/* Note: SDL_mixer "channels" are renamed here as "tracks" */

static int NumTracks(lua_State *L)
    {
    lua_pushinteger(L, Mix_AllocateChannels(-1));
    return 1;
    }

static int AllocateTracks(lua_State *L)
    {
    int num = luaL_checkinteger(L, 1);
    if(Mix_AllocateChannels(num) != num)
        return failure(L, ERR_OPERATION);
    return 0;
    }

static int ReserveTracks(lua_State *L)
    {
    int num = luaL_checkinteger(L, 1);
    lua_pushinteger(L, Mix_ReserveChannels(num));
    return 1;
    }

static int SetPanning(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, MIX_CHANNEL_POST);
    uint8_t left = luaL_checkinteger(L, 2);
    uint8_t right = luaL_checkinteger(L, 3);
    int ec = Mix_SetPanning(track, left, right);
    if(!ec) GetError(L);
    return 0;
    }

static int UnsetPanning(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, MIX_CHANNEL_POST);
    int ec = Mix_SetPanning(track, 255, 255);
    if(!ec) GetError(L);
    return 0;
    }

static int SetDistance(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, MIX_CHANNEL_POST);
    uint8_t distance = luaL_checkinteger(L, 2);
    int ec = Mix_SetDistance(track, distance);
    if(!ec) GetError(L);
    return 0;
    }

static int SetPosition(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, MIX_CHANNEL_POST);
    int16_t angle = luaL_checknumber(L, 2);
    uint8_t distance = luaL_checkinteger(L, 3);
    int ec = Mix_SetPosition(track, angle, distance);
    if(!ec) GetError(L);
    return 0;
    }

static int SetReverseStereo(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, MIX_CHANNEL_POST);
    int flip = checkboolean(L, 2);
    int ec = Mix_SetReverseStereo(track, flip);
    if(!ec) GetError(L);
    return 0;
    }

static int GetVolume(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, -1);
    lua_pushinteger(L, Mix_Volume(track, -1));
    return 1;
    }

static int SetVolume(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, -1);
    int volume = luaL_checkinteger(L, 2);
    Mix_Volume(track, volume);
    return 0;
    }

static int GetMasterVolume(lua_State *L)
    {
    lua_pushinteger(L, Mix_MasterVolume(-1));
    return 1;
    }

static int SetMasterVolume(lua_State *L)
    {
    int volume = luaL_checkinteger(L, 1);
    Mix_MasterVolume(volume);
    return 0;
    }

static int Expire(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, -1);
    int ticks = optmilliseconds(L, 1, -1);
    int n = Mix_ExpireChannel(track, ticks);
    lua_pushinteger(L, n);
    return 1;
    }

static int FadeIn(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, -1);
    int ms = checkmilliseconds(L, 2);
    chunk_t *chunk = checkchunk(L, 3, NULL); 
    int loops = luaL_optinteger(L, 4, 0);
    int maxms = optmilliseconds(L, 5, -1);
    track = Mix_FadeInChannelTimed(track, chunk, loops, ms, maxms);
    lua_pushinteger(L, track);
    return 1;
    }

static int FadeOut(lua_State *L)
    {
    int track = luaL_optinteger(L, 1, -1);
    int ms = checkmilliseconds(L, 2);
    Mix_FadeOutChannel(track, ms);
    return 0;
    }

static int Fading(lua_State *L)
    {
    int track = luaL_checkinteger(L, 1);
    pushfading(L, Mix_FadingChannel(track));
    return 1;
    }

#define F(Func, func)                                           \
static int Func(lua_State *L)                                   \
    {                                                           \
    int track = luaL_optinteger(L, 1, -1);  /* -1=all tracks */\
    func(track);                                                \
    return 0;                                                   \
    }
F(HaltChannel, Mix_HaltChannel)
F(Pause, Mix_Pause)
F(Resume, Mix_Resume)
#undef F


#define F(Func, func)                                           \
static int Func(lua_State *L)                                   \
    {                                                           \
    int track = luaL_optinteger(L, 1, -1);  /* -1=all tracks */ \
    int n = func(track);                                        \
    lua_pushboolean(L, n);                                      \
    if(track!=-1) return 1;                                     \
    lua_pushinteger(L, n);  /* no of tracks paused/playing */   \
    return 2;                                                   \
    }
F(Paused, Mix_Paused)
F(Playing, Mix_Playing)
#undef F

static int GetChunk(lua_State *L)
    {
    int track = luaL_checkinteger(L, 1);
    chunk_t *chunk = Mix_GetChunk(track);
    if(!chunk) return 0;
    pushchunk(L, chunk);
    return 1;
    }


/*----------------------------------------------------------------------*
 | groups of chunk-tracks
 *----------------------------------------------------------------------*/

static int GroupAdd(lua_State *L)
    {
    int ec;
    int tag = luaL_checkinteger(L, 1);
    int track = luaL_checkinteger(L, 2);
    if(lua_isnoneornil(L, 3))
        ec = Mix_GroupChannel(track, tag);
    else
        {
        int totrack = luaL_checkinteger(L, 3);
        ec = Mix_GroupChannels(track, totrack, tag);
        }
    CheckError(L, ec);
    return 0;
    }

static int GroupRemove(lua_State *L)
    {
    int ec;
    int track = luaL_checkinteger(L, 1);
    if(lua_isnoneornil(L, 2))
        ec = Mix_GroupChannel(track, -1);
    else
        {
        int totrack = luaL_checkinteger(L, 2);
        ec = Mix_GroupChannels(track, totrack, -1);
        }
    CheckError(L, ec);
    return 0;
    }

#define F(Func, func)                       \
static int Func(lua_State *L)               \
    {                                       \
    int tag = luaL_checkinteger(L, 1);      \
    int track = func(tag);                  \
    if(track==-1) return 0;                 \
    lua_pushinteger(L, track);              \
    return 1;                               \
    }
F(GroupAvailable, Mix_GroupAvailable)
F(GroupOldest, Mix_GroupOldest)
F(GroupNewer, Mix_GroupNewer)
#undef F

static int GroupCount(lua_State *L)
    {
    int tag = luaL_checkinteger(L, 1);
    lua_pushinteger(L, Mix_GroupCount(tag));
    return 1;
    }

static int HaltGroup(lua_State *L)
    {
    int tag = luaL_optinteger(L, 1, -1);
    Mix_HaltGroup(tag);
    return 0;
    }

static int FadeOutGroup(lua_State *L)
    {
    int tag = luaL_checkinteger(L, 1);
    int ms = checkmilliseconds(L, 2);
    lua_pushinteger(L, Mix_FadeOutGroup(tag, ms));
    return 0;
    }

/*----------------------------------------------------------------------*
 | sound fonts
 *----------------------------------------------------------------------*/

static int SetSoundFonts(lua_State *L)
    {
    const char *paths = luaL_optstring(L, 1, NULL);
    if(Mix_SetSoundFonts(paths) == 0) GetError(L);
    return 0;
    }

static int GetSoundFonts(lua_State *L)
    {
    const char *paths = Mix_GetSoundFonts();
    if(paths==NULL) return 0;
    lua_pushstring(L, paths);
    return 1;
    }

static int EachFontCallback(const char* path, void* data)
    {
    int rc, top;
    lua_State *L = moonsdl2_L;
    int ref = *((int*)data);
    top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_pushstring(L, path);
    rc = lua_pcall(L, 1, 1, 0);
    if(rc!=LUA_OK) lua_error(L);
    rc = lua_toboolean(L, -1);
    lua_settop(L, top);
    return rc;
    }

static int EachSoundFont(lua_State *L)
    {
    int ref;
    if(!lua_isfunction(L, 1)) return argerror(L, 1, ERR_FUNCTION);
    lua_pushvalue(L, 1);
    ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushboolean(L, Mix_EachSoundFont(EachFontCallback, &ref));
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    return 1;
    }


static int TrackRef = LUA_NOREF;
static int MusicRef = LUA_NOREF;

static void TrackFinished(int track)
    {
    int top;
    lua_State *L = moonsdl2_L;
    if(TrackRef == LUA_NOREF) return;
    top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, TrackRef);
    lua_pushinteger(L, track);
    if(lua_pcall(L, 1, 0, 0)!=LUA_OK) lua_error(L);
    lua_settop(L, top);
    return;
    }

static void MusicFinished(void)
    {
    int top;
    lua_State *L = moonsdl2_L;
    if(MusicRef == LUA_NOREF) return;
    top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, MusicRef);
    if(lua_pcall(L, 0, 0, 0)!=LUA_OK) lua_error(L);
    lua_settop(L, top);
    return;
    }

static int SetTrackFinishedCallback(lua_State *L)
    {
    if(lua_isnoneornil(L, 1)) 
        { 
        Mix_ChannelFinished(NULL);
        Unreference(L, TrackRef);
        return 0;
        }
    if(!lua_isfunction(L, 1))
        return argerror(L, 1, ERR_FUNCTION);
    Reference(L, 1, TrackRef);
    Mix_ChannelFinished(TrackFinished);
    return 0;
    }

static int SetMusicFinishedCallback(lua_State *L)
    {
    if(lua_isnoneornil(L, 1)) 
        { 
        Mix_HookMusicFinished(NULL);
        Unreference(L, MusicRef);
        return 0;
        }
    if(!lua_isfunction(L, 1))
        return argerror(L, 1, ERR_FUNCTION);
    Reference(L, 1, MusicRef);
    Mix_HookMusicFinished(MusicFinished);
    return 0;
    }



#if 0 // 7yy
        { "",  },
static int (lua_State *L)
    {
    (void)L;
    return 0;
    }

#endif

/* ----------------------------------------------------------------------- */

static const struct luaL_Reg Functions[] = 
    {
//      { "get_audio_device_names", GetAudioDeviceNames },
//      { "get_audio_device_spec", GetAudioDeviceSpec },
        { "open_audio", OpenAudio },
//      { "open_audio_device", OpenAudioDevice },
        { "close_audio", moonsdl2_close_audio },
        { "query_audio", QuerySpec },
        { "get_chunk_decoders", GetChunkDecoders },
        { "get_music_decoders", GetMusicDecoders },
        { "has_chunk_decoder", HasChunkDecoder },
        { "has_music_decoder", HasMusicDecoder },
        { "num_tracks", NumTracks },
        { "allocate_tracks", AllocateTracks },
        { "reserve_tracks", ReserveTracks },
        { "track_set_panning", SetPanning },
        { "track_unset_panning", UnsetPanning },
        { "track_set_distance", SetDistance },
        { "track_set_position", SetPosition },
        { "track_set_reverse_stereo", SetReverseStereo },
        { "track_get_volume", GetVolume },
        { "track_set_volume", SetVolume },
        { "track_expire", Expire },
        { "track_fade_in", FadeIn },
        { "track_fade_out", FadeOut },
        { "track_fading", Fading },
        { "track_halt", HaltChannel },
        { "track_pause", Pause },
        { "track_resume", Resume },
        { "track_paused", Paused },
        { "track_playing", Playing },
        { "track_get_chunk", GetChunk },
        { "group_add", GroupAdd },
        { "group_remove", GroupRemove },
        { "group_available", GroupAvailable },
        { "group_count", GroupCount },
        { "group_oldest", GroupOldest },
        { "group_newest", GroupNewer },
        { "group_halt", HaltGroup },
        { "group_fade_out", FadeOutGroup },
        { "get_master_volume", GetMasterVolume },
        { "set_master_volume", SetMasterVolume },
        { "set_track_finished_callback", SetTrackFinishedCallback },
        { "set_music_finished_callback", SetMusicFinishedCallback },
        { "set_sound_fonts", SetSoundFonts },
        { "get_sound_fonts", GetSoundFonts },
        { "each_sound_font", EachSoundFont },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_mixer(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    audio_is_open = 0;
#define ADD(what)   do { lua_pushinteger(L, what); lua_setfield(L, -2, #what); } while(0)
    ADD(MIX_MAX_VOLUME);
    ADD(MIX_DEFAULT_FREQUENCY);
    ADD(MIX_DEFAULT_CHANNELS); // mono/stereo etc (not tracks!)
#undef ADD
    pushaudioformat(L, MIX_DEFAULT_FORMAT);
    lua_setfield(L, -2, "MIX_DEFAULT_FORMAT");
    }

#if 0
//int Mix_ModMusicJumpToOrder(int order);
//int Mix_SetTimidityCfg(const char *path);
//const char* Mix_GetTimidityCfg(void);
//void* Mix_GetMusicHookData(void);

// Investigate threading issues before implementing these!
//@@void Mix_SetPostMix(void (*mix_func)(void *udata, Uint8 *stream, int len), void *arg);
//@@void Mix_HookMusic(void (*mix_func)(void *udata, Uint8 *stream, int len), void *arg);
//@@ Mix Effects
typedef void (*Mix_EffectFunc_t)(int chan, void *stream, int len, void *udata);
typedef void (*Mix_EffectDone_t)(int chan, void *udata);
//int Mix_RegisterEffect(int chan, Mix_EffectFunc_t f, Mix_EffectDone_t d, void *arg);
//int Mix_UnregisterEffect(int track, Mix_EffectFunc_t f);
//int Mix_UnregisterAllEffects(int track);
#define MIX_EFFECTSMAXSPEED  "MIX_EFFECTSMAXSPEED"
#endif
