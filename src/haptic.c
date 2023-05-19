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

#define GetInt(field, name) do {    \
    lua_getfield(L, arg, name); P.field = luaL_optinteger(L, -1, 0); lua_pop(L, 1);\
} while(0)

#define GetInt3(field, name) do {   \
    lua_getfield(L, -1, name);      \
    if(lua_istable(L, -1))          \
        {                           \
        lua_rawgeti(L, -1, 1); P.field[0] = luaL_optinteger(L, -1, 0); lua_pop(L, 1);\
        lua_rawgeti(L, -1, 2); P.field[1] = luaL_optinteger(L, -1, 0); lua_pop(L, 1);\
        lua_rawgeti(L, -1, 3); P.field[2] = luaL_optinteger(L, -1, 0); lua_pop(L, 1);\
        }                           \
    else if(!lua_isnoneornil(L, -1)) return argerror(L, -1, ERR_TABLE); \
    /* else already zeroed */       \
    lua_pop(L, 1);                  \
} while(0)

#define GetDir() do {               \
    lua_getfield(L, arg, "dirtype"); P.direction.type = opthapticdirection(L, -1, 0); lua_pop(L, 1);\
    GetInt3(direction.dir, "dir");  \
} while(0)

static int checkhaptic_constant(lua_State *L, int arg, SDL_HapticEffect *effect)
    {
#define P   effect->constant
    GetDir();
    GetInt(length, "length");
    GetInt(delay, "delay");
    GetInt(button, "button");
    GetInt(interval, "interval");
    GetInt(level, "level");
    GetInt(attack_length, "attack_length");
    GetInt(attack_level, "attack_level");
    GetInt(fade_length, "fade_length");
    GetInt(fade_level, "fade_level");
    return 0;
#undef P
    }       

static int checkhaptic_periodic(lua_State *L, int arg, SDL_HapticEffect *effect)
    {
#define P   effect->periodic
    GetDir();
    GetInt(length, "length");
    GetInt(delay, "delay");
    GetInt(button, "button");
    GetInt(interval, "interval");
    GetInt(period, "period");
    GetInt(magnitude, "magnitude");
    GetInt(offset, "offset");
    GetInt(phase, "phase");
    GetInt(attack_length, "attack_length");
    GetInt(attack_level, "attack_level");
    GetInt(fade_length, "fade_length");
    GetInt(fade_level, "fade_level");
    return 0;
#undef P
    }       

static int checkhaptic_leftright(lua_State *L, int arg, SDL_HapticEffect *effect)
    {
#define P   effect->leftright
    GetInt(length, "length");
    GetInt(large_magnitude, "large_magnitude");
    GetInt(small_magnitude, "small_magnitude");
    return 0;
#undef P
    }       

static int checkhaptic_ramp(lua_State *L, int arg, SDL_HapticEffect *effect)
    {
#define P   effect->ramp
    GetDir();
    GetInt(length, "length");
    GetInt(delay, "delay");
    GetInt(button, "button");
    GetInt(interval, "interval");
    GetInt(start, "start");
    GetInt(end, "end");
    GetInt(attack_length, "attack_length");
    GetInt(attack_level, "attack_level");
    GetInt(fade_length, "fade_length");
    GetInt(fade_level, "fade_level");
    return 0;
#undef P
    }       

static int checkhaptic_condition(lua_State *L, int arg, SDL_HapticEffect *effect)
    {
#define P   effect->condition
    GetDir();
    GetInt(length, "length");
    GetInt(delay, "delay");
    GetInt(button, "button");
    GetInt(interval, "interval");
    GetInt3(right_sat, "right_sat");
    GetInt3(left_sat, "left_sat");
    GetInt3(right_coeff, "right_coeff");
    GetInt3(left_coeff, "left_coeff");
    GetInt3(deadband, "deadband");
    GetInt3(center, "center");
    return 0;
#undef P
    }       

static int checkhaptic_custom(lua_State *L, int arg, SDL_HapticEffect *effect)
    {
#define P   effect->custom
    int count;
    GetDir();
    GetInt(length, "length");
    GetInt(delay, "delay");
    GetInt(button, "button");
    GetInt(interval, "interval");
    GetInt(channels, "channels");
    GetInt(period, "period");
    GetInt(samples, "samples");
    GetInt(attack_length, "attack_length");
    GetInt(attack_level, "attack_level");
    GetInt(fade_length, "fade_length");
    GetInt(fade_level, "fade_level");
    lua_getfield(L, arg, "data");
    P.data = checkuint16list(L, -1, &count, NULL);
    lua_pop(L, 1);
    if(count != (P.samples * P.channels))
        { Free(L, P.data); return luaL_error(L, "data must contain samples*channels samples"); }
    return 0;
#undef P
    }       

static void freehaptic_effect(lua_State *L, SDL_HapticEffect *effect)
    {
    if(effect->type == SDL_HAPTIC_CUSTOM)
        Free(L, effect->custom.data);
    }

static int checkhaptic_effect(lua_State *L, int arg, SDL_HapticEffect *effect)
    {
    if(!lua_istable(L, arg)) return argerror(L, arg, ERR_TABLE);
    memset(effect, 0, sizeof(SDL_HapticEffect));
    lua_getfield(L, arg, "type");
    effect->type = checkhaptictype(L, -1);
    lua_pop(L, 1);
    switch(effect->type)
        {
        case SDL_HAPTIC_CONSTANT:       return checkhaptic_constant(L, arg, effect);
        case SDL_HAPTIC_SINE:
        case SDL_HAPTIC_TRIANGLE:
        case SDL_HAPTIC_SAWTOOTHUP:
        case SDL_HAPTIC_SAWTOOTHDOWN:   return checkhaptic_periodic(L, arg, effect);
        case SDL_HAPTIC_LEFTRIGHT:      return checkhaptic_leftright(L, arg, effect);
        case SDL_HAPTIC_RAMP:           return checkhaptic_ramp(L, arg, effect);
        case SDL_HAPTIC_SPRING:
        case SDL_HAPTIC_DAMPER:
        case SDL_HAPTIC_INERTIA:
        case SDL_HAPTIC_FRICTION:       return checkhaptic_condition(L, arg, effect);
        case SDL_HAPTIC_CUSTOM:         return checkhaptic_custom(L, arg, effect);
//      These are not effect types but 'features':
//      case SDL_HAPTIC_GAIN:
//      case SDL_HAPTIC_AUTOCENTER:
//      case SDL_HAPTIC_STATUS:
//      case SDL_HAPTIC_PAUSE:
        default: return luaL_argerror(L, arg, "invalid haptic type");
        }
    return 0;
    }

static int freehaptic(lua_State *L, ud_t *ud)
    {
    haptic_t *haptic = (haptic_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "haptic")) return 0;
    SDL_HapticClose(haptic);
    return 0;
    }

static int newhaptic(lua_State *L, haptic_t *haptic, ud_t *parent_ud)
    {
    ud_t *ud;
    ud = newuserdata(L, haptic, HAPTIC_MT, "haptic");
    ud->parent_ud = parent_ud;
    ud->destructor = freehaptic;
    return 1;
    }

static int Open(lua_State *L)
    {
    ud_t *ud = NULL;
    haptic_t *haptic;
    if(lua_isinteger(L, 1))
        {
        int haptic_index = checkindex(L, 1);
        haptic = SDL_HapticOpen(haptic_index);
        }
    else
        {
        joystick_t *joystick;
        gamecontroller_t *gamecontroller = testgamecontroller(L, 1, NULL);
        if(gamecontroller)
            {
            joystick = SDL_GameControllerGetJoystick(gamecontroller); 
            if(!joystick) GetError(L);
            ud = UD(gamecontroller);
            }
        else
            {
            joystick = checkjoystick(L, 1, NULL);
            ud = UD(joystick);
            }
        haptic = SDL_HapticOpenFromJoystick(joystick);
        }
    if(haptic==NULL) GetError(L);
    if(UD(haptic)) // already open
        pushhaptic(L, haptic);
    else
        newhaptic(L, haptic, ud);
    return 1;
    }

static int OpenFromMouse(lua_State *L)
    {
    haptic_t *haptic = SDL_HapticOpenFromMouse();
    if(haptic==NULL) GetError(L);
    if(UD(haptic)) // already open
        pushhaptic(L, haptic);
    else
        newhaptic(L, haptic, NULL);
    return 1;
    }

static int Index(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int index = SDL_HapticIndex(haptic);
    if(index < 0) GetError(L);
    pushindex(L, index);
    return 1;
    }

static int NumEffects(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int n = SDL_HapticNumEffects(haptic);
    if(n < 0) GetError(L);
    lua_pushinteger(L, n);
    return 1;
    }

static int NumEffectsPlaying(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int n = SDL_HapticNumEffectsPlaying(haptic);
    if(n < 0) GetError(L);
    lua_pushinteger(L, n);
    return 1;
    }

static int Query(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    unsigned int supported = SDL_HapticQuery(haptic);
    if(supported==0) GetError(L);
    lua_newtable(L); // hapticfeatures
#define Set(code, name) \
    do { lua_pushboolean(L, supported & code); lua_setfield(L, -2, name); } while(0)
    Set(SDL_HAPTIC_CONSTANT, "constant");
    Set(SDL_HAPTIC_SINE, "sine");
    Set(SDL_HAPTIC_LEFTRIGHT, "leftright");
    Set(SDL_HAPTIC_TRIANGLE, "triangle");
    Set(SDL_HAPTIC_SAWTOOTHUP, "sawtoothup");
    Set(SDL_HAPTIC_SAWTOOTHDOWN, "sawtoothdown");
    Set(SDL_HAPTIC_RAMP, "ramp");
    Set(SDL_HAPTIC_SPRING, "spring");
    Set(SDL_HAPTIC_DAMPER, "damper");
    Set(SDL_HAPTIC_INERTIA, "inertia");
    Set(SDL_HAPTIC_FRICTION, "friction");
    Set(SDL_HAPTIC_CUSTOM, "custom");
    Set(SDL_HAPTIC_GAIN, "gain");
    Set(SDL_HAPTIC_AUTOCENTER, "autocenter");
    Set(SDL_HAPTIC_STATUS, "status");
    Set(SDL_HAPTIC_PAUSE, "pause");
#undef Set
    return 1;
    }

static int NumAxes(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int n = SDL_HapticNumAxes(haptic);
    if(n < 0) GetError(L);
    lua_pushinteger(L, n);
    return 1;
    }

static int EffectSupported(lua_State *L)
    {
    int ec;
    SDL_HapticEffect effect;
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    checkhaptic_effect(L, 2, &effect);
    ec = SDL_HapticEffectSupported(haptic, &effect);
    freehaptic_effect(L, &effect);
    if(ec <0) GetError(L);
    lua_pushboolean(L, ec);
    return 1;
    }

static int NewEffect(lua_State *L)
    {
    int id;
    SDL_HapticEffect effect;
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    checkhaptic_effect(L, 2, &effect);
    id = SDL_HapticNewEffect(haptic, &effect);
    freehaptic_effect(L, &effect);
    if(id <0) GetError(L);
    lua_pushinteger(L, id);
    return 1;
    }

static int UpdateEffect(lua_State *L)
    {
    int ec;
    SDL_HapticEffect effect;
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int id = luaL_checkinteger(L, 2);
    checkhaptic_effect(L, 3, &effect);
    ec = SDL_HapticUpdateEffect(haptic, id, &effect);
    freehaptic_effect(L, &effect);
    if(ec != 0) GetError(L);
    return 0;
    }

static int RunEffect(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int id = luaL_checkinteger(L, 2);
    uint32_t iterations = luaL_optinteger(L, 3, 1); // see also sdl.HAPTIC_INFINITY
    if(SDL_HapticRunEffect(haptic, id, iterations) != 0) GetError(L);
    return 0;
    }

static int RunEffectIndefinitely(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int id = luaL_checkinteger(L, 2);
    if(SDL_HapticRunEffect(haptic, id, SDL_HAPTIC_INFINITY) != 0) GetError(L);
    return 0;
    }

static int StopEffect(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int id = luaL_checkinteger(L, 2);
    if(SDL_HapticStopEffect(haptic, id) != 0) GetError(L);
    return 0;
    }

static int DestroyEffect(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int id = luaL_checkinteger(L, 2);
    SDL_HapticDestroyEffect(haptic, id);
    return 0;
    }

static int GetEffectStatus(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int id = luaL_checkinteger(L, 2);
    int status = SDL_HapticGetEffectStatus(haptic, id);
    if(status < 0) GetError(L);
    lua_pushboolean(L, status);
    return 1;
    }

static int SetGain(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int gain = luaL_checkinteger(L, 2);
    if(SDL_HapticSetGain(haptic, gain) != 0) GetError(L);
    return 0;
    }

static int SetAutocenter(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int autocenter = luaL_checkinteger(L, 2);
    if(SDL_HapticSetAutocenter(haptic, autocenter) != 0) GetError(L);
    return 0;
    }

static int Pause(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    if(SDL_HapticPause(haptic)!=0) GetError(L);
    return 0;
    }

static int Unpause(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    if(SDL_HapticUnpause(haptic)!=0) GetError(L);
    return 0;
    }

static int StopAll(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    if(SDL_HapticStopAll(haptic)!=0) GetError(L);
    return 0;
    }

static int RumbleSupported(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int ec = SDL_HapticRumbleSupported(haptic);
    if(ec < 0) GetError(L);
    lua_pushboolean(L, ec);
    return 1;
    }

static int RumbleInit(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    if(SDL_HapticRumbleInit(haptic) != 0) GetError(L);
    return 0;
    }

static int RumblePlay(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    float strength = luaL_checknumber(L, 2);
    uint32_t duration = checkmilliseconds(L, 3);
    if(SDL_HapticRumblePlay(haptic, strength, duration) != 0) GetError(L);
    return 0;
    }

static int RumbleStop(lua_State *L)
    {
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    if(SDL_HapticRumbleStop(haptic) != 0) GetError(L);
    return 0;
    }

static int NumHaptics(lua_State *L)
    {
    int n = SDL_NumHaptics();
    if(n<0) GetError(L);
    lua_pushinteger(L, n);
    return 1;
    }

static int HapticOpened(lua_State *L)
    {
    int haptic_index = checkindex(L, 1);
    lua_pushboolean(L, SDL_HapticOpened(haptic_index));
    return 1;
    }

static int MouseIsHaptic(lua_State *L)
    {
    lua_pushboolean(L, SDL_MouseIsHaptic());
    return 1;
    }

static int GetName(lua_State *L)
    {
    const char *name;
    haptic_t *haptic = checkhaptic(L, 1, NULL);
    int haptic_index = SDL_HapticIndex(haptic);
    if(haptic_index < 0) GetError(L);
    name = SDL_HapticName(haptic_index);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

static int HapticName(lua_State *L)
    {
    int haptic_index = checkindex(L, 1);
    const char *name = SDL_HapticName(haptic_index);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

DESTROY_FUNC(haptic)

static const struct luaL_Reg Methods[] = 
    {
        { "close", Destroy },
        { "get_index", Index },
        { "num_effects", NumEffects },
        { "num_effects_playing", NumEffectsPlaying },
        { "get_name", GetName },
        { "query", Query },
        { "num_axes", NumAxes },
        { "effect_supported", EffectSupported },
        { "new_effect", NewEffect },
        { "update_effect", UpdateEffect },
        { "run_effect", RunEffect },
        { "run_effect_indefinitely", RunEffectIndefinitely },
        { "stop_effect", StopEffect },
        { "destroy_effect", DestroyEffect },
        { "is_effect_running", GetEffectStatus },
        { "set_gain", SetGain },
        { "set_autocenter", SetAutocenter },
        { "pause", Pause },
        { "resume", Unpause },
        { "stop_all", StopAll },
        { "rumble_supported", RumbleSupported },
        { "rumble_init", RumbleInit },
        { "rumble_play", RumblePlay },
        { "rumble_stop", RumbleStop },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "haptic_open", Open },
        { "haptic_open_from_mouse", OpenFromMouse },
        { "num_haptics", NumHaptics },
        { "is_haptic_opened", HapticOpened },
        { "mouse_is_haptic", MouseIsHaptic },
        { "haptic_name", HapticName },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_haptic(lua_State *L)
    {
    udata_define(L, HAPTIC_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
#define ADD(what)   do { lua_pushinteger(L, SDL_##what); lua_setfield(L, -2, #what); } while(0)
    ADD(HAPTIC_INFINITY);
#undef ADD
    }

