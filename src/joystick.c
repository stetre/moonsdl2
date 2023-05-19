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

int checkjoystickguid(lua_State *L, int arg, SDL_JoystickGUID *dst)
    {
    if(!lua_istable(L, arg))
        return argerror(L, arg, ERR_TABLE); 
    for(int i=0; i<16; i++)
        {
        lua_rawgeti(L, arg, i+1);
        dst->data[i] = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        }
    return ERR_SUCCESS;
    }

int pushjoystickguid(lua_State *L, SDL_JoystickGUID *guid)
    {
    lua_newtable(L);
    for(int i=0; i<16; i++)
        {
        lua_pushinteger(L, guid->data[i]);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }

static int freejoystick(lua_State *L, ud_t *ud)
    {
    joystick_t *joystick = (joystick_t*)ud->handle;
    int borrowed = IsBorrowed(ud);
    freechildren(L, HAPTIC_MT, ud);
    if(!freeuserdata(L, ud, "joystick")) return 0;
    if(!borrowed)
        SDL_JoystickClose(joystick);
    return 0;
    }

int newjoystick(lua_State *L, joystick_t *joystick, ud_t *parent_ud)
    {
    ud_t *ud;
    ud = newuserdata(L, joystick, JOYSTICK_MT, "joystick");
    ud->parent_ud = parent_ud;
    ud->destructor = freejoystick;
    if(parent_ud) MarkBorrowed(ud);
    return 1;
    }

static int Open(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    joystick_t *joystick = SDL_JoystickOpen(joystick_index);
    if(joystick==NULL) GetError(L);
    return newjoystick(L, joystick, NULL);
    }

static int FromInstanceID(lua_State *L)
    {
    SDL_JoystickID instance_id = luaL_checkinteger(L, 1);
    joystick_t *joystick = SDL_JoystickFromInstanceID(instance_id);
    if(joystick==NULL) GetError(L);
    pushjoystick(L, joystick);
    return 1;
    }

static int FromPlayerIndex(lua_State *L)
    {
    int player_index = luaL_checkinteger(L, 1);
    joystick_t *joystick = SDL_JoystickFromPlayerIndex(player_index);
    if(joystick==NULL) GetError(L);
    pushjoystick(L, joystick);
    return 1;
    }

static int SetVirtualAxis(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int axis = checkindex(L, 2);
    int16_t value = luaL_checkinteger(L, 3);
    int ec = SDL_JoystickSetVirtualAxis(joystick, axis, value);
    if(!ec) GetError(L);
    return 0;
    }

static int SetVirtualButton(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int button = checkindex(L, 2);
    uint8_t value = checkkeystate(L, 3);
    int ec = SDL_JoystickSetVirtualButton(joystick, button, value);
    if(!ec) GetError(L);
    return 0;
    }

static int SetVirtualHat(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int hat = checkindex(L, 2);
    uint8_t value = checkhatstate(L, 3);
    int ec = SDL_JoystickSetVirtualHat(joystick, hat, value);
    if(!ec) GetError(L);
    return 0;
    }

static int GetName(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    const char *name = SDL_JoystickName(joystick);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 0;
    }

static int GetPlayerIndex(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int player_index = SDL_JoystickGetPlayerIndex(joystick);
    if(player_index == -1) return 0;
    lua_pushinteger(L, player_index);
    return 1;
    }

static int SetPlayerIndex(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int player_index = luaL_optinteger(L, 2, -1);
    SDL_JoystickSetPlayerIndex(joystick, player_index);
    return 0;
    }

static int GetGUID(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    SDL_JoystickGUID guid = SDL_JoystickGetGUID(joystick);
    pushjoystickguid(L, &guid);
    return 1;
    }

#define F(Func, func)                                   \
static int Func(lua_State *L)                           \
    {                                                   \
    joystick_t *joystick = checkjoystick(L, 1, NULL);   \
    uint16_t val = func(joystick);                      \
    if(val==0) return 0;                                \
    lua_pushinteger(L, val);                            \
    return 1;                                           \
    }
F(GetVendor, SDL_JoystickGetVendor)
F(GetProduct, SDL_JoystickGetProduct)
F(GetProductVersion, SDL_JoystickGetProductVersion)
#undef F

static int GetSerial(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    const char *s = SDL_JoystickGetSerial(joystick);
    if(s==NULL) return 0;
    lua_pushstring(L, s);
    return 1;
    }

static int GetType(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    SDL_JoystickType type = SDL_JoystickGetType(joystick);
    pushjoysticktype(L, type);
    return 1;
    }

static int GetAttached(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    lua_pushboolean(L, SDL_JoystickGetAttached(joystick));
    return 1;
    }

static int GetId(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    SDL_JoystickID id = SDL_JoystickInstanceID(joystick);
    if(id < 0) GetError(L);
    lua_pushinteger(L, id);
    return 1;
    }

#define F(Func, func)                                   \
static int Func(lua_State *L)                           \
    {                                                   \
    joystick_t *joystick = checkjoystick(L, 1, NULL);   \
    int n = func(joystick);                             \
    if(n < 0) GetError(L);                              \
    lua_pushinteger(L, n);                              \
    return 1;                                           \
    }
F(NumAxes, SDL_JoystickNumAxes)
F(NumBalls, SDL_JoystickNumBalls)
F(NumHats, SDL_JoystickNumHats)
F(NumButtons, SDL_JoystickNumButtons)
#undef F

static int GetAxis(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int axis = checkindex(L, 2);
    lua_pushinteger(L, SDL_JoystickGetAxis(joystick, axis));
    return 1;
    }

static int GetAxisInitialState(lua_State *L)
    {
    int16_t state;
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int axis = checkindex(L, 2);
    int initialized = SDL_JoystickGetAxisInitialState(joystick, axis, &state);
    if(!initialized) return 0;
    lua_pushinteger(L, state);
    return 1;
    }

static int GetHat(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int hat = checkindex(L, 2);
    pushhatstate(L, SDL_JoystickGetHat(joystick, hat));
    return 1;
    }

static int GetBall(lua_State *L)
    {
    int dx, dy;
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int ball = checkindex(L, 2);
    if(SDL_JoystickGetBall(joystick, ball, &dx, &dy) != 0) GetError(L);
    lua_pushinteger(L, dx);
    lua_pushinteger(L, dy);
    return 2;
    }

static int GetButton(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int button = checkindex(L, 2);
    pushkeystate(L, SDL_JoystickGetButton(joystick, button));
    return 1;
    }

static int Rumble(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    uint16_t low_frequency_rumble = luaL_checkinteger(L, 2);
    uint16_t high_frequency_rumble = luaL_checkinteger(L, 3);
    uint32_t duration_ms = checkmilliseconds(L, 4);
    if(SDL_JoystickRumble(joystick, low_frequency_rumble, high_frequency_rumble, duration_ms)!=0)
        GetError(L);
    return 0;
    }

static int RumbleTriggers(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    uint16_t left_rumble = luaL_checkinteger(L, 2);
    uint16_t right_rumble = luaL_checkinteger(L, 3);
    uint32_t duration_ms = checkmilliseconds(L, 4);
    if(SDL_JoystickRumbleTriggers(joystick, left_rumble, right_rumble, duration_ms)!=0)
        GetError(L);
    return 0;
    }

#define F(Func, func)                                   \
static int Func(lua_State *L)                           \
    {                                                   \
    joystick_t *joystick = checkjoystick(L, 1, NULL);   \
    lua_pushboolean(L, func(joystick));                 \
    return 1;                                           \
    }
F(HasLED, SDL_JoystickHasLED)
F(HasRumble, SDL_JoystickHasRumble)
F(HasRumbleTriggers, SDL_JoystickHasRumbleTriggers)
#undef F

static int SetLED(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    uint8_t r = luaL_checkinteger(L, 2);
    uint8_t g = luaL_checkinteger(L, 3);
    uint8_t b = luaL_checkinteger(L, 4);
    if(SDL_JoystickSetLED(joystick, r, g, b) != 0) GetError(L);
    return 0;
    }

static int SendEffect(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    const void *data = checklightuserdata(L, 2);
    int size = luaL_checkinteger(L, 3);
    if(SDL_JoystickSendEffect(joystick, data, size) != 0) GetError(L);
    return 0;
    }

static int CurrentPowerLevel(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    pushjoystickpowerlevel(L, SDL_JoystickCurrentPowerLevel(joystick));
    return 1;
    }

static int GetPath(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    const char *path = SDL_JoystickPath(joystick);
    if(!path) return 0;
    lua_pushstring(L, path);
    return 1;
    }

static int GetFirmwareVersion(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    uint16_t ver = SDL_JoystickGetFirmwareVersion(joystick);
    lua_pushinteger(L, ver);
    return 1;
    }

#if 0
static int LockJoysticks(lua_State *L)
    {
    (void)L;
    SDL_LockJoysticks();
    return 0;
    }

static int UnlockJoysticks(lua_State *L)
    {
    (void)L;
    SDL_UnlockJoysticks();
    return 0;
    }
#endif

static int NumJoysticks(lua_State *L) /* joystick_index = 1, ..., NumJoysticks */
    {
    int n = SDL_NumJoysticks();
    if(n < 0) GetError(L);
    lua_pushinteger(L, n);
    return 1;
    }

static int JoystickNameForIndex(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    const char *name = SDL_JoystickNameForIndex(joystick_index);
    if(!name) GetError(L);
    lua_pushstring(L, name);
    return 1;
    }

static int JoystickGetDevicePlayerIndex(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    int player_index = SDL_JoystickGetDevicePlayerIndex(joystick_index);
    if(player_index == -1) return 0;
    lua_pushinteger(L, player_index);
    return 1;
    }

static int JoystickGetDeviceGUID(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(joystick_index);
    pushjoystickguid(L, &guid);
    return 1;
    }

#define F(Func, func)                                   \
static int Func(lua_State *L)                           \
    {                                                   \
    int joystick_index = checkindex(L, 1);              \
    uint16_t val = func(joystick_index);                \
    if(val==0) return 0;                                \
    lua_pushinteger(L, val);                            \
    return 1;                                           \
    }
F(JoystickGetDeviceVendor, SDL_JoystickGetDeviceVendor)
F(JoystickGetDeviceProduct, SDL_JoystickGetDeviceProduct)
F(JoystickGetDeviceProductVersion, SDL_JoystickGetDeviceProductVersion)
#undef F

static int JoystickGetDeviceType(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    pushjoysticktype(L, SDL_JoystickGetDeviceType(joystick_index));
    return 1;
    }

static int JoystickGetDeviceInstanceID(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    SDL_JoystickID instance_id = SDL_JoystickGetDeviceInstanceID(joystick_index);
    if(instance_id==-1) return 0;
    lua_pushinteger(L, instance_id);
    return 1;
    }

static int JoystickAttachVirtual(lua_State *L)
    {
    SDL_JoystickType type = checkjoysticktype(L, 1);
    int naxes = luaL_checkinteger(L, 2);
    int nbuttons = luaL_checkinteger(L, 3);
    int nhats = luaL_checkinteger(L, 4);
    int joystick_index = SDL_JoystickAttachVirtual(type, naxes, nbuttons, nhats);
    if(joystick_index == -1) GetError(L);
    pushindex(L, joystick_index);
    return 1;
    }

static int JoystickDetachVirtual(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    int ec = SDL_JoystickDetachVirtual(joystick_index);
    CheckError(L, ec);
    return 0;
    }

static int JoystickIsVirtual(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    lua_pushboolean(L, SDL_JoystickIsVirtual(joystick_index));
    return 1;
    }

static int JoystickUpdate(lua_State *L)
    {
    (void)L;
    SDL_JoystickUpdate();
    return 0;
    }

static int JoystickEnable(lua_State *L)
    {
    if(SDL_JoystickEventState(SDL_ENABLE) < 0) GetError(L);
    return 0;
    }

static int JoystickDisable(lua_State *L)
    {
    if(SDL_JoystickEventState(SDL_DISABLE) < 0) GetError(L);
    return 0;
    }

static int JoystickEventState(lua_State *L)
    {
    int rc = SDL_JoystickEventState(SDL_QUERY);
    switch(rc)
        {
        case SDL_ENABLE: lua_pushstring(L, "enabled"); return 1;
        case SDL_DISABLE: lua_pushstring(L, "disabled"); return 1;
        default: break;
        }
    GetError(L);
    return 0;
    }

static int JoystickGetGUIDString(lua_State *L)
    {
    char s[40];
    SDL_JoystickGUID guid;
    checkjoystickguid(L, 1, &guid);
    SDL_JoystickGetGUIDString(guid, s, 40);
    lua_pushstring(L, s);
    return 1;
    }

static int JoystickGetGUIDFromString(lua_State *L)
    {
    const char *s = luaL_checkstring(L, 1);
    SDL_JoystickGUID guid = SDL_JoystickGetGUIDFromString(s);
    pushjoystickguid(L, &guid);
    return 1;
    }

static int JoystickPath(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    const char *path = SDL_JoystickPathForIndex(joystick_index);
    if(!path) return 0;
    lua_pushstring(L, path);
    return 1;
    }

static int IsHaptic(lua_State *L)
    {
    joystick_t *joystick = checkjoystick(L, 1, NULL);
    int ec = SDL_JoystickIsHaptic(joystick);
    if(ec < 0) GetError(L);
    lua_pushboolean(L, ec);
    return 1;
    }



static int GetJoystickGUIDInfo(lua_State *L)
    {
    uint16_t vendor, product, version, crc16;
    SDL_JoystickGUID guid;
    checkjoystickguid(L, 1, &guid);
    SDL_GetJoystickGUIDInfo(guid, &vendor, &product, &version, &crc16);
    lua_newtable(L);
    lua_pushinteger(L, vendor); lua_setfield(L, -2, "vendor");
    lua_pushinteger(L, product); lua_setfield(L, -2, "product");
    lua_pushinteger(L, version); lua_setfield(L, -2, "version");
    lua_pushinteger(L, crc16); lua_setfield(L, -2, "crc16");
    return 1;
    }

DESTROY_FUNC(joystick)

static const struct luaL_Reg Methods[] = 
    {
        { "close", Destroy },
        { "set_virtual_axis", SetVirtualAxis },
        { "set_virtual_button", SetVirtualButton },
        { "set_virtual_hat", SetVirtualHat },
        { "get_name", GetName },
        { "get_player_index", GetPlayerIndex },
        { "set_player_index", SetPlayerIndex },
        { "get_guid", GetGUID },
        { "get_vendor", GetVendor },
        { "get_product", GetProduct },
        { "get_product_version", GetProductVersion },
        { "get_serial", GetSerial },
        { "get_type", GetType },
        { "get_attached", GetAttached },
        { "get_id", GetId },
        { "num_axes", NumAxes },
        { "num_balls", NumBalls },
        { "num_hats", NumHats },
        { "num_buttons", NumButtons },
        { "get_axis", GetAxis },
        { "get_axis_initial_state", GetAxisInitialState },
        { "get_hat", GetHat },
        { "get_ball", GetBall },
        { "get_button", GetButton },
        { "rumble", Rumble },
        { "rumble_triggers", RumbleTriggers },
        { "has_led", HasLED },
        { "has_rumble", HasRumble },
        { "has_rumble_triggers", HasRumbleTriggers },
        { "set_led", SetLED },
        { "send_effect", SendEffect },
        { "get_current_power_level", CurrentPowerLevel },
        { "get_path", GetPath },
        { "get_firmware_version", GetFirmwareVersion },
        { "is_haptic", IsHaptic },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "joystick_open", Open },
        { "joystick_from_id", FromInstanceID },
        { "joystick_from_player_index", FromPlayerIndex },
//      { "lock_joysticks", LockJoysticks },
//      { "unlock_joysticks", UnlockJoysticks },
        { "num_joysticks", NumJoysticks },
        { "joystick_name", JoystickNameForIndex },
        { "joystick_player_index", JoystickGetDevicePlayerIndex },
        { "joystick_guid", JoystickGetDeviceGUID },
        { "joystick_vendor", JoystickGetDeviceVendor },
        { "joystick_product", JoystickGetDeviceProduct },
        { "joystick_product_version", JoystickGetDeviceProductVersion },
        { "joystick_type", JoystickGetDeviceType },
        { "joystick_id", JoystickGetDeviceInstanceID },
        { "joystick_path", JoystickPath },
        { "joystick_attach_virtual", JoystickAttachVirtual },
        { "joystick_detach_virtual", JoystickDetachVirtual },
        { "joystick_is_virtual", JoystickIsVirtual },
        { "joystick_event_enable", JoystickEnable},
        { "joystick_event_disable", JoystickDisable },
        { "joystick_event_state", JoystickEventState },
        { "joystick_update", JoystickUpdate },
        { "joystick_guid_to_string", JoystickGetGUIDString },
        { "joystick_string_to_guid", JoystickGetGUIDFromString },
        { "joystick_guid_info", GetJoystickGUIDInfo },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_joystick(lua_State *L)
    {
    udata_define(L, JOYSTICK_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
#define ADD(what)   do { lua_pushinteger(L, SDL_##what); lua_setfield(L, -2, #what); } while(0)
    ADD(JOYSTICK_AXIS_MIN);
    ADD(JOYSTICK_AXIS_MAX);
#undef ADD
    }

#if 0
//int SDL_JoystickAttachVirtualEx(const SDL_VirtualJoystickDesc *desc);
#endif

