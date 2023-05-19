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

static void pushcontrollerbuttonbind(lua_State *L, SDL_GameControllerButtonBind *bb)
    {
    lua_newtable(L);
    pushcontrollerbindtype(L, bb->bindType); lua_setfield(L, -2, "type");
    switch(bb->bindType)
        {
        case SDL_CONTROLLER_BINDTYPE_NONE: break;
        case SDL_CONTROLLER_BINDTYPE_BUTTON:
            lua_pushinteger(L, bb->value.button); lua_setfield(L, -2, "button"); 
            break;
        case SDL_CONTROLLER_BINDTYPE_AXIS:
            lua_pushinteger(L, bb->value.axis); lua_setfield(L, -2, "axis"); 
            break;
        case SDL_CONTROLLER_BINDTYPE_HAT:
            lua_pushinteger(L, bb->value.hat.hat); lua_setfield(L, -2, "hat"); 
            pushhatstate(L, bb->value.hat.hat_mask); lua_setfield(L, -2, "mask"); 
            break;
        default: unexpected(L);
        }
    }

static int freegamecontroller(lua_State *L, ud_t *ud)
    {
    gamecontroller_t *gamecontroller = (gamecontroller_t*)ud->handle;
    freechildren(L, HAPTIC_MT, ud);
    freechildren(L, JOYSTICK_MT, ud);
    if(!freeuserdata(L, ud, "gamecontroller")) return 0;
    SDL_GameControllerClose(gamecontroller);
    return 0;
    }

static int newgamecontroller(lua_State *L, gamecontroller_t *gamecontroller)
    {
    ud_t *ud;
    ud = newuserdata(L, gamecontroller, GAMECONTROLLER_MT, "gamecontroller");
    ud->parent_ud = NULL;
    ud->destructor = freegamecontroller;
    return 1;
    }

static int GameControllerOpen(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    gamecontroller_t *gamecontroller = SDL_GameControllerOpen(joystick_index);
    if(gamecontroller==NULL) GetError(L);
    return newgamecontroller(L, gamecontroller);
    }

static int IsGameController(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    lua_pushboolean(L, SDL_IsGameController(joystick_index));
    return 1;
    }

static int GameControllerFromInstanceID(lua_State *L)
    {
    SDL_JoystickID joyid = luaL_checkinteger(L, 1);
    gamecontroller_t *gamecontroller = SDL_GameControllerFromInstanceID(joyid);
    if(gamecontroller==NULL) GetError(L);
    pushgamecontroller(L, gamecontroller);
    return 1;
    }

static int GameControllerFromPlayerIndex(lua_State *L)
    {
    int player_index = luaL_checkinteger(L, 1);
    gamecontroller_t *gamecontroller = SDL_GameControllerFromPlayerIndex(player_index);
    if(gamecontroller==NULL) GetError(L);
    pushgamecontroller(L, gamecontroller);
    return 1;
    }

static int GameControllerAddMappingsFromFile(lua_State *L)
    {
    const char *file = luaL_checkstring(L, 1);
    int n = SDL_GameControllerAddMappingsFromFile(file);
    if(n == -1) GetError(L);
    lua_pushinteger(L, n);
    return 1;
    }

static int GameControllerAddMapping(lua_State *L)
    {
    const char *s = luaL_checkstring(L, 1);
    int rc = SDL_GameControllerAddMapping(s);
    if(rc==-1) GetError(L);
    lua_pushstring(L, rc == 0 ? "updated" : "added");
    return 1;
    }

static int GameControllerNumMappings(lua_State *L)
    {
    lua_pushinteger(L, SDL_GameControllerNumMappings());
    return 1;
    }

static int GameControllerMappingForIndex(lua_State *L)
    {
    int mapping_index = luaL_checkinteger(L, 1);
    char *s = SDL_GameControllerMappingForIndex(mapping_index);
    if(!s) return 0;
    lua_pushstring(L, s);
    SDL_free(s);
    return 1;
    }

static int GameControllerMappingForGUID(lua_State *L)
    {
    SDL_JoystickGUID guid;
    char *s;
    checkjoystickguid(L, 1, &guid);
    s = SDL_GameControllerMappingForGUID(guid);
    if(!s) return 0;
    lua_pushstring(L, s);
    SDL_free(s);
    return 1;
    }

static int GameControllerName(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    const char *name = SDL_GameControllerNameForIndex(joystick_index);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

static int GameControllerType(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    pushcontrollertype(L, SDL_GameControllerTypeForIndex(joystick_index));
    return 1;
    }

static int GameControllerPath(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    const char *path = SDL_GameControllerPathForIndex(joystick_index);
    if(!path) return 0;
    lua_pushstring(L, path);
    return 1;
    }

static int GameControllerMappingForDeviceIndex(lua_State *L)
    {
    int joystick_index = checkindex(L, 1);
    char *s = SDL_GameControllerMappingForDeviceIndex(joystick_index);
    if(!s) return 0;
    lua_pushstring(L, s);
    SDL_free(s);
    return 1;
    }

static int GameControllerEnable(lua_State *L)
    {
    if(SDL_GameControllerEventState(SDL_ENABLE) < 0) GetError(L);
    return 0;
    }

static int GameControllerDisable(lua_State *L)
    {
    if(SDL_GameControllerEventState(SDL_DISABLE) < 0) GetError(L);
    return 0;
    }

static int GameControllerEventState(lua_State *L)
    {
    int rc = SDL_GameControllerEventState(SDL_QUERY);
    switch(rc)
        {
        case SDL_ENABLE: lua_pushstring(L, "enabled"); return 1;
        case SDL_DISABLE: lua_pushstring(L, "disabled"); return 1;
        default: break;
        }
    GetError(L);
    return 0;
    }

static int GameControllerUpdate(lua_State *L)
    {
    (void)L;
    SDL_GameControllerUpdate();
    return 0;
    }

#if 0
static int GameControllerGetAxisFromString(lua_State *L)
    {
    const char *s = luaL_checkstring(L, 1);
    pushcontrolleraxis(L, SDL_GameControllerGetAxisFromString(s));
    return 1;
    }

static int GameControllerGetStringForAxis(lua_State *L)
    {
    SDL_GameControllerAxis axis = checkcontrolleraxis(L, 1);
    const char *s = SDL_GameControllerGetStringForAxis(axis);
    if(!s) return 0;
    lua_pushstring(L, s);
    return 1;
    }

static int GameControllerGetButtonFromString(lua_State *L)
    {
    const char *s = luaL_checkstring(L, 1);
    pushcontrollerbutton(L, SDL_GameControllerGetButtonFromString(s));
    return 1;
    }

static int GameControllerGetStringForButton(lua_State *L)
    {
    SDL_GameControllerButton button = checkcontrollerbutton(L, 1);
    const char *s = SDL_GameControllerGetStringForButton(button);
    if(!s) return 0;
    lua_pushstring(L, s);
    return 1;
    }
#endif

static int GetMapping(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    char *s = SDL_GameControllerMapping(gamecontroller);
    if(!s) return 0;
    lua_pushstring(L, s);
    SDL_free(s);
    return 1;
    }

static int GetName(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    const char *name = SDL_GameControllerName(gamecontroller);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

static int GetType(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    pushcontrollertype(L, SDL_GameControllerGetType(gamecontroller));
    return 1;
    }

static int GetPlayerIndex(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    int player_index = SDL_GameControllerGetPlayerIndex(gamecontroller);
    if(player_index == -1) return 0;
    lua_pushinteger(L, player_index);
    return 1;
    }

static int SetPlayerIndex(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    int player_index = luaL_optinteger(L, 2, -1);
    SDL_GameControllerSetPlayerIndex(gamecontroller, player_index);
    return 0;
    }

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL); \
    uint16_t val = func(gamecontroller);            \
    if(val==0) return 0;                            \
    lua_pushinteger(L, val);                        \
    return 1;                                       \
    }
F(GetVendor, SDL_GameControllerGetVendor)
F(GetProduct, SDL_GameControllerGetProduct)
F(GetProductVersion, SDL_GameControllerGetProductVersion)
#undef F

static int GetSerial(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    const char *s = SDL_GameControllerGetSerial(gamecontroller);
    if(!s) return 0;
    lua_pushstring(L, s);
    return 1;
    }

static int GetBindForAxis(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_GameControllerAxis axis = checkcontrolleraxis(L, 2);
    SDL_GameControllerButtonBind bb = SDL_GameControllerGetBindForAxis(gamecontroller, axis);
    pushcontrollerbuttonbind(L, &bb);
    return 1;
    }

static int HasAxis(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_GameControllerAxis axis = checkcontrolleraxis(L, 2);
    lua_pushboolean(L, SDL_GameControllerHasAxis(gamecontroller, axis));
    return 1;
    }

static int GetAxis(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_GameControllerAxis axis = checkcontrolleraxis(L, 2);
    lua_pushinteger(L, SDL_GameControllerGetAxis(gamecontroller, axis));
    return 1;
    }

static int GetBindForButton(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_GameControllerButton button = checkcontrollerbutton(L, 2);
    SDL_GameControllerButtonBind bb = SDL_GameControllerGetBindForButton(gamecontroller, button);
    pushcontrollerbuttonbind(L, &bb);
    return 1;
    }

static int HasButton(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_GameControllerButton button = checkcontrollerbutton(L, 2);
    lua_pushboolean(L, SDL_GameControllerHasButton(gamecontroller, button));
    return 1;
    }

static int GetButton(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_GameControllerButton button = checkcontrollerbutton(L, 2);
    pushkeystate(L, SDL_GameControllerGetButton(gamecontroller, button));
    return 1;
    }

static int GetNumTouchpads(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    lua_pushinteger(L, SDL_GameControllerGetNumTouchpads(gamecontroller));
    return 1;
    }

static int GetNumTouchpadFingers(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    int touchpad = checkindex(L, 2);
    lua_pushinteger(L, SDL_GameControllerGetNumTouchpadFingers(gamecontroller, touchpad));
    return 1;
    }

static int GetTouchpadFinger(lua_State *L)
    {
    uint8_t state;
    float x, y, pressure;
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    int touchpad = checkindex(L, 2);
    int finger = checkindex(L, 3);
    int ec = SDL_GameControllerGetTouchpadFinger(gamecontroller, touchpad, finger,
            &state, &x, &y, &pressure);
    if(ec) GetError(L);
    pushkeystate(L, state);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, pressure);
    return 4;
    }

static int HasSensor(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_SensorType type = checksensortypeorint(L, 2);
    lua_pushboolean(L, SDL_GameControllerHasSensor(gamecontroller, type));
    return 1;
    }

static int SetSensorEnabled(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_SensorType type = checksensortypeorint(L, 2);
    int enabled = checkboolean(L, 3);
    if(SDL_GameControllerSetSensorEnabled(gamecontroller, type, enabled) != 0) GetError(L);
    return 0;
    }

static int IsSensorEnabled(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_SensorType type = checksensortypeorint(L, 2);
    lua_pushboolean(L, SDL_GameControllerIsSensorEnabled(gamecontroller, type));
    return 1;
    }

static int GetSensorDataRate(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_SensorType type = checksensortypeorint(L, 2);
    lua_pushnumber(L, SDL_GameControllerGetSensorDataRate(gamecontroller, type));
    return 1;
    }

static int GetSensorData(lua_State *L)
    {
    int ec;
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    SDL_SensorType type = checksensortypeorint(L, 2);
    int num_values = luaL_checkinteger(L, 3);
    float *data = Malloc(L, sizeof(float)*num_values);
    uint64_t *timestamp = MallocNoErr(L, sizeof(uint64_t)*num_values);
    if(!timestamp) { Free(L, data); return errmemory(L); }
//  ec = SDL_GameControllerGetSensorData(gamecontroller, type, data, num_values);
    ec = SDL_GameControllerGetSensorDataWithTimestamp(gamecontroller, type, timestamp, data, num_values);
    if(ec)
        { Free(L, data); Free(L, timestamp); GetError(L); }
    lua_newtable(L); // data
    lua_newtable(L); // timestamp
    for(int i=0; i<num_values; i++)
        {
        lua_pushnumber(L, data[i]);
        lua_rawseti(L, -3, i+1);
        lua_pushinteger(L, timestamp[i]); // usec
        lua_rawseti(L, -2, i+1);
        }
    Free(L, data);
    Free(L, timestamp);
    return 2;
    }

static int Rumble(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    uint16_t low_frequency_rumble = luaL_checkinteger(L, 2);
    uint16_t high_frequency_rumble = luaL_checkinteger(L, 3);
    uint32_t duration_ms = checkmilliseconds(L, 4);
    if(SDL_GameControllerRumble(gamecontroller, low_frequency_rumble,
                high_frequency_rumble, duration_ms)!=0)
        GetError(L);
    return 0;
    }

static int RumbleTriggers(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    uint16_t left_rumble = luaL_checkinteger(L, 2);
    uint16_t right_rumble = luaL_checkinteger(L, 3);
    uint32_t duration_ms = checkmilliseconds(L, 4);
    if(SDL_GameControllerRumbleTriggers(gamecontroller, left_rumble, right_rumble, duration_ms)!=0)
        GetError(L);
    return 0;
    }

#define F(Func, func)                                                   \
static int Func(lua_State *L)                                           \
    {                                                                   \
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL); \
    lua_pushboolean(L, func(gamecontroller));                           \
    return 1;                                                           \
    }
F(GetAttached, SDL_GameControllerGetAttached)
F(HasLED, SDL_GameControllerHasLED)
F(HasRumble, SDL_GameControllerHasRumble)
F(HasRumbleTriggers, SDL_GameControllerHasRumbleTriggers)
#undef F

static int SetLED(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    uint8_t red = luaL_checkinteger(L, 2);
    uint8_t green = luaL_checkinteger(L, 3);
    uint8_t blue = luaL_checkinteger(L, 4);
    if(SDL_GameControllerSetLED(gamecontroller, red, green, blue)!=0) GetError(L);
    return 0;
    }

static int SendEffect(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    const void *data = checklightuserdata(L, 2);
    int size = luaL_checkinteger(L, 3);
    if(SDL_GameControllerSendEffect(gamecontroller, data, size) != 0) GetError(L);
    return 0;
    }

static int Path(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    const char * path = SDL_GameControllerPath(gamecontroller);
    if(!path) return 0;
    lua_pushstring(L, path);
    return 1;
    }

static int GetFirmwareVersion(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    uint16_t ver = SDL_GameControllerGetFirmwareVersion(gamecontroller);
    lua_pushinteger(L, ver);
    return 1;
    }

static int GetJoystick(lua_State *L)
    {
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    joystick_t *joystick = SDL_GameControllerGetJoystick(gamecontroller); 
    if(!joystick) GetError(L);
    if(UD(joystick))
        pushjoystick(L, joystick);
    else
        newjoystick(L, joystick, UD(gamecontroller)); // borrowed, do not free
    return 1;
    }

static int GetId(lua_State *L)
    {
    SDL_JoystickID id;
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    joystick_t *joystick = SDL_GameControllerGetJoystick(gamecontroller); 
    if(!joystick) GetError(L);
    id = SDL_JoystickInstanceID(joystick);
    if(id < 0) GetError(L);
    lua_pushinteger(L, id);
    return 1;
    }

static int GetGUID(lua_State *L)
    {
    SDL_JoystickGUID guid;
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    joystick_t *joystick = SDL_GameControllerGetJoystick(gamecontroller); 
    if(!joystick) GetError(L);
    guid = SDL_JoystickGetGUID(joystick);
    pushjoystickguid(L, &guid);
    return 1;
    }

static int IsHaptic(lua_State *L)
    {
    int ec;
    gamecontroller_t *gamecontroller = checkgamecontroller(L, 1, NULL);
    joystick_t *joystick = SDL_GameControllerGetJoystick(gamecontroller); 
    if(!joystick) GetError(L);
    ec = SDL_JoystickIsHaptic(joystick);
    if(ec < 0) GetError(L);
    lua_pushboolean(L, ec);
    return 1;
    }


DESTROY_FUNC(gamecontroller)

static const struct luaL_Reg Methods[] = 
    {
        { "close", Destroy },
        { "get_mapping", GetMapping },
        { "get_name", GetName },
        { "get_type", GetType },
        { "get_player_index", GetPlayerIndex },
        { "set_player_index", SetPlayerIndex },
        { "get_vendor", GetVendor },
        { "get_product", GetProduct },
        { "get_product_version", GetProductVersion },
        { "get_serial", GetSerial },
        { "get_attached", GetAttached },
        { "get_bind_for_axis", GetBindForAxis },
        { "has_axis", HasAxis },
        { "get_axis", GetAxis },
        { "get_bind_for_button", GetBindForButton },
        { "has_button", HasButton },
        { "get_button", GetButton },
        { "get_num_touchpads", GetNumTouchpads },
        { "get_num_touchpad_fingers", GetNumTouchpadFingers },
        { "get_touchpad_finger", GetTouchpadFinger },
        { "has_sensor", HasSensor },
        { "set_sensor_enabled", SetSensorEnabled },
        { "is_sensor_enabled", IsSensorEnabled },
        { "get_sensor_data_rate", GetSensorDataRate },
        { "get_sensor_data", GetSensorData },
        { "rumble", Rumble },
        { "rumble_triggers", RumbleTriggers },
        { "has_led", HasLED },
        { "has_rumble", HasRumble },
        { "has_rumble_triggers", HasRumbleTriggers },
        { "set_led", SetLED },
        { "send_effect", SendEffect },
        { "get_path", Path },
        { "get_firmware_version", GetFirmwareVersion },
        { "get_joystick", GetJoystick },
        { "get_id", GetId },
        { "get_guid", GetGUID },
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
        { "is_gamecontroller", IsGameController },
        { "gamecontroller_open", GameControllerOpen },
        { "gamecontroller_from_id", GameControllerFromInstanceID },
        { "gamecontroller_from_player_index", GameControllerFromPlayerIndex },
        { "gamecontroller_add_mappings_from_file", GameControllerAddMappingsFromFile },
        { "gamecontroller_add_mapping", GameControllerAddMapping },
        { "gamecontroller_num_mappings", GameControllerNumMappings },
        { "gamecontroller_mapping_for_index", GameControllerMappingForIndex },
        { "gamecontroller_mapping_for_guid", GameControllerMappingForGUID },
        { "gamecontroller_name", GameControllerName },
        { "gamecontroller_type", GameControllerType },
        { "gamecontroller_path", GameControllerPath },
        { "gamecontroller_mapping", GameControllerMappingForDeviceIndex },
        { "gamecontroller_event_enable", GameControllerEnable },
        { "gamecontroller_event_disable", GameControllerDisable },
        { "gamecontroller_event_state", GameControllerEventState },
        { "gamecontroller_update", GameControllerUpdate },
//      { "gamecontroller_get_axis_from_string", GameControllerGetAxisFromString },
//      { "gamecontroller_get_string_for_axis", GameControllerGetStringForAxis },
//      { "gamecontroller_get_button_from_string", GameControllerGetButtonFromString },
//      { "gamecontroller_get_string_for_button", GameControllerGetStringForButton },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_gamecontroller(lua_State *L)
    {
    udata_define(L, GAMECONTROLLER_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
//int SDL_GameControllerAddMappingsFromRW(SDL_RWops * rw, int freerw);
//const char* SDL_GameControllerGetAppleSFSymbolsNameForButton(SDL_GameController *gamecontroller, SDL_GameControllerButton button);
//const char* SDL_GameControllerGetAppleSFSymbolsNameForAxis(SDL_GameController *gamecontroller, SDL_GameControllerAxis axis);
#endif

