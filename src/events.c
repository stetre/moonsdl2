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

// Set event fields, based on E
#define SetIndex(fname, sname) \
    do { pushindex(L, E.fname); lua_setfield(L, -2, sname); } while(0)
#define SetInt(fname, sname) \
    do { lua_pushinteger(L, E.fname); lua_setfield(L, -2, sname); } while(0)
#define SetFloat(fname, sname) \
    do { lua_pushnumber(L, E.fname); lua_setfield(L, -2, sname); } while(0)
#define SetBool(fname, sname) \
    do { lua_pushboolean(L, E.fname); lua_setfield(L, -2, sname); } while(0)
#define SetString(fname, sname) \
    do { lua_pushstring(L, E.fname); lua_setfield(L, -2, sname); } while(0)
#define SetEnum(pushfunc, fname, sname) \
    do { pushfunc(L, E.fname); lua_setfield(L, -2, sname); } while(0)
#define SetFlags(fname, sname) \
    do { pushflags(L, E.fname); lua_setfield(L, -2, sname); } while(0)
#define SetWindow(id) do {                                                  \
    window_t *window = SDL_GetWindowFromID(E.id);                           \
    if(window) { pushwindow(L, window); lua_setfield(L, -2, "window"); }    \
} while(0)
#define SetMouse(id) do {                                                   \
    lua_pushinteger(L, E.id); lua_setfield(L, -2, "id");                    \
} while(0)
#define SetJoystick(id) do {                                                    \
    joystick_t *joystick = SDL_JoystickFromInstanceID(E.id);                    \
    if(joystick) { pushjoystick(L, joystick); lua_setfield(L, -2, "joystick"); }\
    lua_pushinteger(L, E.id); lua_setfield(L, -2, "id");                        \
} while(0)
#define SetGameController(id) do {                                              \
    gamecontroller_t *gamecontroller = SDL_GameControllerFromInstanceID(E.id);  \
    if(gamecontroller) { pushgamecontroller(L, gamecontroller); lua_setfield(L, -2, "controller"); }\
    lua_pushinteger(L, E.id); lua_setfield(L, -2, "id");                        \
} while(0)
#define SetAudioDevice(id) do {                                                 \
    audio_device_t *audio_device = audio_device_from_id(L, E.id);               \
    if(audio_device) { pushaudio_device(L, audio_device); lua_setfield(L, -2, "audio_device"); }\
    lua_pushinteger(L, E.id); lua_setfield(L, -2, "id");                    \
} while(0)
#define SetSensor(id) do {                                                  \
    sensor_t *sensor = SDL_SensorFromInstanceID(E.id);                      \
    if(sensor) { pushsensor(L, sensor); lua_setfield(L, -2, "sensor"); }    \
    lua_pushinteger(L, E.id); lua_setfield(L, -2, "id");                    \
} while(0)

#define SetIntArray(field, name, n) do {    \
    lua_newtable(L);                        \
    for(int i=0; i<n; i++)                  \
        { lua_pushinteger(L, E.field[i]); lua_rawseti(L, -2, i+1); }    \
    lua_setfield(L, -2, name);              \
} while(0)

#define SetFloatArray(field, name, n) do {  \
    lua_newtable(L);                        \
    for(int i=0; i<n; i++)                  \
        { lua_pushnumber(L, E.field[i]); lua_rawseti(L, -2, i+1); } \
    lua_setfield(L, -2, name);              \
} while(0)

static int DisplayEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->display
    SetIndex(display, "display");
    SetEnum(pushdisplayeventtype, event, "event");
    SetInt(data1, "data");
    return 1;
#undef E
    }

static int WindowEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->window
    SetWindow(windowID);
    SetEnum(pushwindoweventtype, event, "event");
    SetInt(data1, "data1");
    SetInt(data2, "data2");
    return 1;
#undef E
    }

static int KeyboardEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->key
    const char* name;
    SetWindow(windowID);
    SetEnum(pushkeystate, state, "state");
    SetBool(repeat, "repeat");
    SetInt(keysym.sym, "keycode");
    SetInt(keysym.scancode, "scancode");
    name = SDL_GetKeyName(E.keysym.sym);
    lua_pushstring(L, name); lua_setfield(L, -2, "name");
    pushkeymod(L, E.keysym.mod); lua_setfield(L, -2, "mod");
    return 1;
#undef E
    }

static int TextEditingEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->editExt
    SetWindow(windowID);
    SetString(text, "text");
    SetInt(start, "start");
    SetInt(length, "length");
    if(E.type == SDL_TEXTEDITING_EXT)
        {
        /* For the user, only the 'textediting' event type exists: */
        pusheventtype(L, SDL_TEXTEDITING); lua_setfield(L, -2, "type");
        SDL_free(E.text);
        }
    return 1;
#undef E
    }

static int TextInputEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->text
    SetWindow(windowID);
    SetString(text, "text");
    return 1;
#undef E
    }

static int MouseMotionEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->motion
    SetWindow(windowID);
    SetMouse(which);
    SetInt(x, "x");
    SetInt(y, "y");
    SetInt(xrel, "xrel");
    SetInt(yrel, "yrel");
    pushmousebuttonstate(L, E.state); lua_setfield(L, -2, "state");
    return 1;
#undef E
    }

static int MouseButtonEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->button
    SetWindow(windowID);
    SetMouse(which);
    SetInt(x, "x");
    SetInt(y, "y");
    SetEnum(pushbutton, button, "button");
    SetEnum(pushkeystate, state, "state");
    SetInt(clicks, "clicks");
    return 1;
#undef E
    }

static int MouseWheelEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->wheel
    SetWindow(windowID);
    SetMouse(which);
    SetInt(x, "x");
    SetInt(y, "y");
    SetFloat(preciseX, "precise_x");
    SetFloat(preciseY, "precise_y");
    SetEnum(pushmousewheeldir, direction, "direction");
    return 1;
#undef E
    }

static int JoyAxisEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->jaxis
    SetJoystick(which);
    SetIndex(axis, "axis");
    SetInt(value, "value");
    return 1;
#undef E
    }

static int JoyBallEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->jball
    SetJoystick(which);
    SetIndex(ball, "ball");
    SetInt(xrel, "xrel");
    SetInt(yrel, "yrel");
    return 1;
#undef E
    }

static int JoyHatEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->jhat
    SetJoystick(which);
    SetIndex(hat, "hat");
    pushhatstate(L, E.value); lua_setfield(L, -2, "value");
    return 1;
#undef E
    }

static int JoyButtonEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->jbutton
    SetJoystick(which);
    SetIndex(button, "button");
    pushkeystate(L, E.state); lua_setfield(L, -2, "state");
    return 1;
#undef E
    }

static int JoyDeviceAddedEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->jdevice
    SetIndex(which, "index");
    return 1;
#undef E
    }

static int JoyDeviceRemovedEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->jdevice
    SetJoystick(which);
    return 1;
#undef E
    }

static int JoyBatteryEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->jbattery
    SetJoystick(which);
    SetEnum(pushjoystickpowerlevel, level, "level");
    return 1;
#undef E
    }

static int ControllerAxisEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->caxis
    SetGameController(which);
    SetEnum(pushcontrolleraxis, axis, "axis");
    SetInt(value, "value");
    return 1;
#undef E
    }

static int ControllerButtonEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->cbutton
    SetGameController(which);
    SetEnum(pushcontrollerbutton, button, "button");
    pushkeystate(L, E.state); lua_setfield(L, -2, "state");
    return 1;
#undef E
    }

static int ControllerDeviceAddedEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->cdevice
    SetIndex(which, "index");
    return 1;
#undef E
    }

static int ControllerDeviceEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->cdevice
    SetGameController(which);
    return 1;
#undef E
    }

static int ControllerTouchpadEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->ctouchpad
    SetGameController(which);
    SetIndex(touchpad, "touchpad");
    SetIndex(finger, "finger");
    SetFloat(x, "x");
    SetFloat(y, "y");
    SetFloat(pressure, "pressure");
    return 1;
#undef E
    }

static int ControllerSensorEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->csensor
    SetGameController(which);
    SetEnum(pushsensortypeorint, sensor, "sensor");
    SetFloatArray(data, "data", 3);
    return 1;
#undef E
    }

#if 0
static int AudioDeviceAddedEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->adevice
    SetIndex(which, "index");
    SetBool(iscapture, "iscapture");
    return 1;
#undef E
    }

static int AudioDeviceRemovedEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->adevice
    SetAudioDevice(which);
    SetBool(iscapture, "iscapture");
    return 1;
#undef E
    }
#endif

static int TouchFingerEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->tfinger
    SetInt(touchId, "touch_id");
    SetInt(fingerId, "finger_id");
    SetFloat(x, "x");
    SetFloat(y, "y");
    SetFloat(dx, "dy");
    SetFloat(dy, "dy");
    SetFloat(pressure, "pressure");
    SetWindow(windowID);
    return 1;
#undef E
    }

static int MultiGestureEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->mgesture
    SetInt(touchId, "touch_id");
    SetFloat(dTheta, "dtheta");
    SetFloat(dDist, "ddist");
    SetFloat(x, "x");
    SetFloat(y, "y");
    SetInt(numFingers, "num_fingers");
    return 1;
#undef E
    }

static int DollarGestureEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->dgesture
    SetInt(touchId, "touch_id");
    SetInt(gestureId, "gesture_id");
    SetInt(numFingers, "num_fingers");
    SetFloat(error, "error");
    SetFloat(x, "x");
    SetFloat(y, "y");
    return 1;
#undef E
    }

static int DropEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->drop
    SetWindow(windowID);
    if(E.file)
        {
        SetString(file, "file");
        SDL_free(E.file);
        }
    return 1;
#undef E
    }

static int SensorEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->sensor
    SetSensor(which);
    SetFloatArray(data, "data", 6);
    return 1;
#undef E
    }

#define QuitEvent(L, event) 1
#define OSEvent(L, event)   1 

static int UserEvent(lua_State *L, SDL_Event *event)
    {
#define E event->user
    intptr_t ref = (intptr_t)(E.data1);
    SetInt(code, "code");
    if(ref != LUA_NOREF)
        {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
        lua_setfield(L, -2, "data");
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        }
    //E.windowID; not used
    //E.data2; not used
    return 1;
#undef E
    }

#if 0
static int SysWMEvent(lua_State *L, const SDL_Event *event)
    {
#define E event->syswm
    (void)L; (void)event;
    //SDL_SysWMmsg *msg;  
    return 1;
#undef E
    }
#endif

/* EVENT TYPES RESERVED FOR INTERNAL USE */
uint32_t TIMEREVENT; /* used in timer.c */

int moonsdl2_reserve_event_codes(lua_State *L)
/* Called at init() to register the event codes */
    {
    TIMEREVENT = SDL_RegisterEvents(1);
    if(TIMEREVENT == (uint32_t)-1) return failure(L, ERR_OPERATION);
    return 0;
    }

static int pushevent(lua_State *L, SDL_Event *event)
    {
    if(event->type == TIMEREVENT)
        { timerevent(L, event); return 0; }

    lua_newtable(L);    
    pusheventtype(L, event->type); lua_setfield(L, -2, "type");
    pushmilliseconds(L, event->common.timestamp); lua_setfield(L, -2, "timestamp");
    switch(event->type)
        {
        case SDL_FIRSTEVENT: break;
        case SDL_QUIT: return QuitEvent(L, event);
        case SDL_APP_TERMINATING: break;
        case SDL_APP_LOWMEMORY: break;
        case SDL_APP_WILLENTERBACKGROUND: break;
        case SDL_APP_DIDENTERBACKGROUND: break;
        case SDL_APP_WILLENTERFOREGROUND: break;
        case SDL_APP_DIDENTERFOREGROUND: break;
        case SDL_LOCALECHANGED: break;
        case SDL_DISPLAYEVENT: return DisplayEvent(L, event);
        case SDL_WINDOWEVENT: return WindowEvent(L, event);
        case SDL_SYSWMEVENT: break; //return SysWMEvent(L, event);
        case SDL_KEYDOWN:
        case SDL_KEYUP: return KeyboardEvent(L, event);
        case SDL_TEXTEDITING:
        case SDL_TEXTEDITING_EXT: return TextEditingEvent(L, event);
        case SDL_TEXTINPUT: return TextInputEvent(L, event);
        case SDL_KEYMAPCHANGED: break;
        case SDL_MOUSEMOTION: return MouseMotionEvent(L, event);
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: return MouseButtonEvent(L, event);
        case SDL_MOUSEWHEEL: return MouseWheelEvent(L, event);
        case SDL_JOYAXISMOTION: return JoyAxisEvent(L, event);
        case SDL_JOYBALLMOTION: return JoyBallEvent(L, event);
        case SDL_JOYHATMOTION: return JoyHatEvent(L, event);
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP: return JoyButtonEvent(L, event);
        case SDL_JOYDEVICEADDED: return JoyDeviceAddedEvent(L, event);
        case SDL_JOYDEVICEREMOVED: return JoyDeviceRemovedEvent(L, event);
        case SDL_JOYBATTERYUPDATED: return JoyBatteryEvent(L, event);
        case SDL_CONTROLLERAXISMOTION: return ControllerAxisEvent(L, event);
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP: return ControllerButtonEvent(L, event);
        case SDL_CONTROLLERDEVICEADDED: return ControllerDeviceAddedEvent(L, event);
        case SDL_CONTROLLERDEVICEREMOVED:
        case SDL_CONTROLLERDEVICEREMAPPED: return ControllerDeviceEvent(L, event);
        case SDL_CONTROLLERTOUCHPADDOWN:
        case SDL_CONTROLLERTOUCHPADMOTION:
        case SDL_CONTROLLERTOUCHPADUP: return ControllerTouchpadEvent(L, event);
        case SDL_CONTROLLERSENSORUPDATE: return ControllerSensorEvent(L, event);
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION: return TouchFingerEvent(L, event);
        case SDL_DOLLARGESTURE:
        case SDL_DOLLARRECORD: return DollarGestureEvent(L, event);
        case SDL_MULTIGESTURE: return MultiGestureEvent(L, event);
        case SDL_CLIPBOARDUPDATE: break;
        case SDL_DROPFILE:
        case SDL_DROPTEXT:
        case SDL_DROPBEGIN:
        case SDL_DROPCOMPLETE: return DropEvent(L, event);
        case SDL_AUDIODEVICEADDED: break; //return AudioDeviceAddedEvent(L, event);
        case SDL_AUDIODEVICEREMOVED: break; //return AudioDeviceRemovedEvent(L, event);
        case SDL_SENSORUPDATE: return SensorEvent(L, event);
        case SDL_RENDER_TARGETS_RESET: break;
        case SDL_RENDER_DEVICE_RESET: break;
        case SDL_POLLSENTINEL: break;
        case SDL_USEREVENT: return UserEvent(L, event);
        case SDL_LASTEVENT: break;
        default:
            break;
        }
    return 1;
    }

#if 0
static int HasEvent(lua_State *L)
    {
    uint32_t type = checkeventtype(L, 1);
    lua_pushboolean(L, SDL_HasEvent(type));
    return 1;
    }

static int HasEvents(lua_State *L)
    {
    uint32_t mintype = checkeventtype(L, 1);
    uint32_t maxtype = checkeventtype(L, 2);
    lua_pushboolean(L, SDL_HasEvents(mintype, maxtype));
    return 1;
    }

static int FlushEvent(lua_State *L)
    {
    uint32_t type = checkeventtype(L, 1);
    SDL_FlushEvent(type);
    return 0;
    }

static int FlushEvents(lua_State *L)
    {
    uint32_t mintype = checkeventtype(L, 1);
    uint32_t maxtype = checkeventtype(L, 2);
    SDL_FlushEvents(mintype, maxtype);
    return 0;
    }
#endif

static int PollEvent(lua_State *L)
    {
    SDL_Event event;
    int rc = SDL_PollEvent(&event);
    if(!rc) return 0;
    return pushevent(L, &event);
    }

static int WaitEvent(lua_State *L)
    {
    SDL_Event event;
    int rc;
    while(1)
        {
        rc = SDL_WaitEvent(&event);
        if(!rc) GetError(L);
        rc = pushevent(L, &event);
        if(rc) return 1; /* else, keep waiting */
        }
    }

static int WaitEventTimeout(lua_State *L)
    {
    SDL_Event event;
    int timeout = checkmilliseconds(L, 1);
    int rc = SDL_WaitEventTimeout(&event, timeout);
    if(!rc) return 0; // timeout expired
    return pushevent(L, &event);
    }

#if 0
static int RegisterEvents(lua_State *L)
    {
    int numevents = luaL_checkinteger(L, 1);
    uint32_t first = SDL_RegisterEvents(numevents);
    if(first == (uint32_t)-1)
        return failure(L, ERR_OPERATION);
    lua_pushinteger(L, first); // first reserved event type
    lua_pushinteger(L, first + numevents - 1); // last reserved event type
    return 2;
    }
#endif

static int PushEvent(lua_State *L) // user event only
    {
    SDL_Event event;
    intptr_t ref = LUA_NOREF;
    int32_t code = luaL_checkinteger(L, 1);
    if(!lua_isnoneornil(L, 2))
        {
        lua_pushvalue(L, 2);
        ref = luaL_ref(L, LUA_REGISTRYINDEX);
        }
    event.user.type = SDL_USEREVENT;
    event.user.timestamp = SDL_GetTicks();
    event.user.windowID = 0;
    event.user.code = code;
    event.user.data1 = (void*)ref;
    event.user.data2 = NULL;
    if(SDL_PushEvent(&event) < 0)
        {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        GetError(L);
        }
    return 0;
    }

#if 0
static int PumpEvents(lua_State *L)
    {
    (void)L;
    SDL_PumpEvents();
    return 0;
    }
#endif

static int EnableEvent(lua_State *L)
    {
    uint32_t type = checkeventtype(L, 1);
    SDL_EventState(type, SDL_ENABLE);
    return 0;
    }

static int DisableEvent(lua_State *L)
    {
    uint32_t type = checkeventtype(L, 1);
    SDL_EventState(type, SDL_DISABLE);
    return 0;
    }

static int GetEventState(lua_State *L)
    {
    uint32_t type = checkeventtype(L, 1);
    uint8_t state = SDL_EventState(type, SDL_QUERY);
    switch(state)
        {
        case SDL_ENABLE: lua_pushstring(L, "enabled"); return 1;
        case SDL_DISABLE: lua_pushstring(L, "disabled"); return 1;
        default: GetError(L);
        }
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
//      { "has_event", HasEvent },
//      { "has_events", HasEvents },
//      { "flush_event", FlushEvent },
//      { "flush_events", FlushEvents },
        { "poll_event", PollEvent },
        { "wait_event", WaitEvent },
        { "wait_event_timeout", WaitEventTimeout },
//      { "register_events", RegisterEvents },
        { "push_user_event", PushEvent },
//      { "pump_events", PumpEvents },
        { "enable_event", EnableEvent },
        { "disable_event", DisableEvent },
        { "get_event_state", GetEventState },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_events(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
typedef int (*SDL_EventFilter)(void *userdata, SDL_Event * event);
//void SDL_AddEventWatch(SDL_EventFilter filter, void *userdata);
//void SDL_DelEventWatch(SDL_EventFilter filter, void *userdata);
//void SDL_SetEventFilter(SDL_EventFilter filter, void *userdata);
//SDL_bool SDL_GetEventFilter(SDL_EventFilter * filter, void **userdata);
//void SDL_FilterEvents(SDL_EventFilter filter, void *userdata);
//int SDL_PeepEvents(SDL_Event * events, int numevents, SDL_eventaction action, Uint32 minType, Uint32 maxType);
#endif
