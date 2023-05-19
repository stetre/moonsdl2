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

/*------------------------------------------------------------------------------*
 | Code<->string map for enumerations                                           |
 *------------------------------------------------------------------------------*/

/* code <-> string record */
#define rec_t struct rec_s
struct rec_s {
    RB_ENTRY(rec_s) CodeEntry;
    RB_ENTRY(rec_s) StringEntry;
    int domain;
    int code;  /* (domain, code) = search key in code tree */
    char     *str;  /* (domain, str) = search key in string tree */
};

/* compare functions */
static int cmp_code(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return (rec1->code < rec2->code ? -1 : rec1->code > rec2->code);
    } 

static int cmp_str(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return strcmp(rec1->str, rec2->str);
    } 

static RB_HEAD(CodeTree, rec_s) CodeHead = RB_INITIALIZER(&CodeHead);
RB_PROTOTYPE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 
RB_GENERATE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 

static RB_HEAD(StringTree, rec_s) StringHead = RB_INITIALIZER(&StringHead);
RB_PROTOTYPE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
RB_GENERATE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
 
static rec_t *code_remove(rec_t *rec) 
    { return RB_REMOVE(CodeTree, &CodeHead, rec); }
static rec_t *code_insert(rec_t *rec) 
    { return RB_INSERT(CodeTree, &CodeHead, rec); }
static rec_t *code_search(int domain, int code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_FIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_first(int domain, int code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_NFIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_next(rec_t *rec)
    { return RB_NEXT(CodeTree, &CodeHead, rec); }
#if 0
static rec_t *code_prev(rec_t *rec)
    { return RB_PREV(CodeTree, &CodeHead, rec); }
static rec_t *code_min(void)
    { return RB_MIN(CodeTree, &CodeHead); }
static rec_t *code_max(void)
    { return RB_MAX(CodeTree, &CodeHead); }
static rec_t *code_root(void)
    { return RB_ROOT(&CodeHead); }
#endif
 
static rec_t *str_remove(rec_t *rec) 
    { return RB_REMOVE(StringTree, &StringHead, rec); }
static rec_t *str_insert(rec_t *rec) 
    { return RB_INSERT(StringTree, &StringHead, rec); }
static rec_t *str_search(int domain, const char* str) 
    { rec_t tmp; tmp.domain = domain; tmp.str = (char*)str; return RB_FIND(StringTree, &StringHead, &tmp); }
#if 0
static rec_t *str_first(int domain, const char* str ) 
    { rec_t tmp; tmp.domain = domain; tmp.str = str; return RB_NFIND(StringTree, &StringHead, &tmp); }
static rec_t *str_next(rec_t *rec)
    { return RB_NEXT(StringTree, &StringHead, rec); }
static rec_t *str_prev(rec_t *rec)
    { return RB_PREV(StringTree, &StringHead, rec); }
static rec_t *str_min(void)
    { return RB_MIN(StringTree, &StringHead); }
static rec_t *str_max(void)
    { return RB_MAX(StringTree, &StringHead); }
static rec_t *str_root(void)
    { return RB_ROOT(&StringHead); }
#endif


static int enums_new(lua_State *L, int domain, int code, const char *str)
    {
    rec_t *rec;
    if((rec = (rec_t*)Malloc(L, sizeof(rec_t))) == NULL) 
        return luaL_error(L, errstring(ERR_MEMORY));
    memset(rec, 0, sizeof(rec_t));
    rec->domain = domain;
    rec->code = code;
    rec->str = Strdup(L, str);
    //printf("added enum %.8x %s\n", code, str); 
    if(code_search(domain, code) || str_search(domain, str))
        { 
        Free(L, rec->str);
        Free(L, rec); 
        return unexpected(L); /* duplicate value */
        }
    code_insert(rec);
    str_insert(rec);
    return 0;
    }

static void enums_free(lua_State *L, rec_t* rec)
    {
    if(code_search(rec->domain, rec->code) == rec)
        code_remove(rec);
    if(str_search(rec->domain, rec->str) == rec)
        str_remove(rec);
    Free(L, rec->str);
    Free(L, rec);   
    }

void enums_free_all(lua_State *L)
    {
    rec_t *rec;
    while((rec = code_first(0, 0)))
        enums_free(L, rec);
    }

#if 0
int enums_code(int domain, const char *str, int* found)
    {
    rec_t *rec = str_search(domain, str);
    if(!rec)
        { *found = 0; return 0; }
    *found = 1; 
    return rec->code;
    }

const char* enums_string(int domain, int code)
    {
    rec_t *rec = code_search(domain, code);
    if(!rec)
        return NULL;
    return rec->str;
    }

#endif

int enums_test(lua_State *L, int domain, int arg, int *err)
    {
    rec_t *rec;
    const char *s = luaL_optstring(L, arg, NULL);
    if(!s) { *err = ERR_NOTPRESENT; return 0; }
    rec = str_search(domain, s);
    if(!rec) { *err = ERR_VALUE; return 0; }
    *err = ERR_SUCCESS;
    return rec->code;
    }

int enums_opt(lua_State *L, int domain, int arg, int defval)
    {
    rec_t *rec;
    const char *s = luaL_optstring(L, arg, NULL);
    if(!s) { return defval; }
    rec = str_search(domain, s);
    if(!rec) return luaL_argerror(L, arg, badvalue(L, s));
    return rec->code;
    }

int enums_check(lua_State *L, int domain, int arg)
    {
    rec_t *rec;
    const char *s = luaL_checkstring(L, arg);
    rec = str_search(domain, s);
    if(!rec) return luaL_argerror(L, arg, badvalue(L, s));
    return rec->code;
    }

int enums_push(lua_State *L, int domain, int code)
    {
    rec_t *rec = code_search(domain, code);
    if(!rec) return unexpected(L);
    lua_pushstring(L, rec->str);
    return 1;
    }

int enums_values(lua_State *L, int domain)
    {
    int i;
    rec_t *rec;

    lua_newtable(L);
    i = 1;
    rec = code_first(domain, 0);
    while(rec)
        {
        if(rec->domain == domain)
            {
            lua_pushstring(L, rec->str);
            lua_rawseti(L, -2, i++);
            }
        rec = code_next(rec);
        }

    return 1;
    }


int* enums_checklist(lua_State *L, int domain, int arg, int *count, int *err)
    {
    int* list;
    int i;

    *count = 0;
    *err = 0;
    if(lua_isnoneornil(L, arg))
        { *err = ERR_NOTPRESENT; return NULL; }
    if(lua_type(L, arg) != LUA_TTABLE)
        { *err = ERR_TABLE; return NULL; }

    *count = luaL_len(L, arg);
    if(*count == 0)
        { *err = ERR_NOTPRESENT; return NULL; }

    list = (int*)MallocNoErr(L, sizeof(int) * (*count));
    if(!list)
        { *count = 0; *err = ERR_MEMORY; return NULL; }

    for(i=0; i<*count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        list[i] = enums_test(L, domain, -1, err);
        lua_pop(L, 1);
        if(*err)
            { Free(L, list); *count = 0; return NULL; }
        }
    return list;
    }

void enums_freelist(lua_State *L, int *list)
    {
    if(!list) return;
    Free(L, list);
    }

/*------------------------------------------------------------------------------*
 |                                                                              |
 *------------------------------------------------------------------------------*/

static int Enum(lua_State *L)
/* { strings } = sdl.enum('type') lists all the values for a given enum type */
    { 
    const char *s = luaL_checkstring(L, 1); 
#define CASE(xxx) if(strcmp(s, ""#xxx) == 0) return values##xxx(L)
    CASE(hintpriority);
    CASE(logpriority);
    CASE(pixelformat);
    CASE(eventtype);
    CASE(displayeventtype);
    CASE(windoweventtype);
    CASE(keystate);
    CASE(button);
    CASE(mousewheeldir);
    CASE(controllertype);
    CASE(controllerbindtype);
    CASE(controlleraxis);
    CASE(controllerbutton);
    CASE(textureaccess);
    CASE(blendoperation);
    CASE(blendfactor);
    CASE(scalemode);
    CASE(swapinterval);
    CASE(glattr);
    CASE(orientation);
    CASE(flashoperation);
    CASE(hittestresult);
    CASE(joysticktype);
    CASE(joystickpowerlevel);
    CASE(systemcursor);
    CASE(logcategory);
    CASE(powerstate);
    CASE(fonthinting);
    CASE(musictype);
    CASE(fading);
    CASE(audioformat);
    CASE(hatstate);
    CASE(fontdirection);
    CASE(fontalign);
    CASE(touchdevicetype);
    CASE(rwopstype);
    CASE(rwopswhence);
    CASE(haptictype);
    CASE(hapticdirection);
    CASE(fullscreenmode);
    CASE(sensortype);
    CASE(glprofile);
#undef CASE
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "enum", Enum },
        { NULL, NULL } /* sentinel */
    };


void moonsdl2_open_enums(lua_State *L)
    {
    int domain;

    luaL_setfuncs(L, Functions, 0);

    /* Add all the code<->string mappings */
#define ADD(what, s) do {                               \
    /*lua_pushstring(L, s); lua_setfield(L, -2, #what); */\
    enums_new(L, domain, SDL_##what, s);                \
} while(0)
    domain = DOMAIN_HINT_PRIORITY;
    ADD(HINT_DEFAULT, "default");
    ADD(HINT_NORMAL, "normal");
    ADD(HINT_OVERRIDE, "override");

    domain = DOMAIN_LOG_PRIORITY;
    ADD(LOG_PRIORITY_VERBOSE, "verbose");
    ADD(LOG_PRIORITY_DEBUG, "debug");
    ADD(LOG_PRIORITY_INFO, "info");
    ADD(LOG_PRIORITY_WARN, "warn");
    ADD(LOG_PRIORITY_ERROR, "error");
    ADD(LOG_PRIORITY_CRITICAL, "critical");

    domain = DOMAIN_PIXELFORMAT;
    ADD(PIXELFORMAT_UNKNOWN, "unknown");
    ADD(PIXELFORMAT_INDEX1LSB, "index1lsb");
    ADD(PIXELFORMAT_INDEX1MSB, "index1msb");
    ADD(PIXELFORMAT_INDEX4LSB, "index4lsb");
    ADD(PIXELFORMAT_INDEX4MSB, "index4msb");
    ADD(PIXELFORMAT_INDEX8, "index8");
    ADD(PIXELFORMAT_RGB332, "rgb332");
//  ADD(PIXELFORMAT_XRGB4444, "xrgb4444"); -> rgb444
    ADD(PIXELFORMAT_RGB444, "rgb444");
//  ADD(PIXELFORMAT_XBGR4444, "xbgr4444"); -> bgr444
    ADD(PIXELFORMAT_BGR444, "bgr444");
//  ADD(PIXELFORMAT_XRGB1555, "xrgb1555"); -> rgb555
    ADD(PIXELFORMAT_RGB555, "rgb555");
//  ADD(PIXELFORMAT_XBGR1555, "xbgr1555"); -> bgr555
    ADD(PIXELFORMAT_BGR555, "bgr555");
    ADD(PIXELFORMAT_ARGB4444, "argb4444");
    ADD(PIXELFORMAT_RGBA4444, "rgba4444");
    ADD(PIXELFORMAT_ABGR4444, "abgr4444");
    ADD(PIXELFORMAT_BGRA4444, "bgra4444");
    ADD(PIXELFORMAT_ARGB1555, "argb1555");
    ADD(PIXELFORMAT_RGBA5551, "rgba5551");
    ADD(PIXELFORMAT_ABGR1555, "abgr1555");
    ADD(PIXELFORMAT_BGRA5551, "bgra5551");
    ADD(PIXELFORMAT_RGB565, "rgb565");
    ADD(PIXELFORMAT_BGR565, "bgr565");
    ADD(PIXELFORMAT_RGB24, "rgb24");
    ADD(PIXELFORMAT_BGR24, "bgr24");
//  ADD(PIXELFORMAT_XRGB8888, "xrgb8888"); -> rgb888
    ADD(PIXELFORMAT_RGB888, "rgb888");
    ADD(PIXELFORMAT_RGBX8888, "rgbx8888");
//  ADD(PIXELFORMAT_XBGR8888, "xbgr8888"); -> bgr888
    ADD(PIXELFORMAT_BGR888, "bgr888");
    ADD(PIXELFORMAT_BGRX8888, "bgrx8888");
    ADD(PIXELFORMAT_ARGB8888, "argb8888");
    ADD(PIXELFORMAT_RGBA8888, "rgba8888");
    ADD(PIXELFORMAT_ABGR8888, "abgr8888");
    ADD(PIXELFORMAT_BGRA8888, "bgra8888");
    ADD(PIXELFORMAT_ARGB2101010, "argb2101010");
    ADD(PIXELFORMAT_YV12, "yv12");
    ADD(PIXELFORMAT_IYUV, "iyuv");
    ADD(PIXELFORMAT_YUY2, "yuy2");
    ADD(PIXELFORMAT_UYVY, "uyvy");
    ADD(PIXELFORMAT_YVYU, "yvyu");
    ADD(PIXELFORMAT_NV12, "nv12");
    ADD(PIXELFORMAT_NV21, "nv21");
    ADD(PIXELFORMAT_EXTERNAL_OES, "external oes");

    domain = DOMAIN_EVENT_TYPE;
    ADD(FIRSTEVENT, "firstevent");
    ADD(QUIT, "quit");
    ADD(APP_TERMINATING, "app terminating");
    ADD(APP_LOWMEMORY, "app lowmemory");
    ADD(APP_WILLENTERBACKGROUND, "app willenterbackground");
    ADD(APP_DIDENTERBACKGROUND, "app didenterbackground");
    ADD(APP_WILLENTERFOREGROUND, "app willenterforeground");
    ADD(APP_DIDENTERFOREGROUND, "app didenterforeground");
    ADD(LOCALECHANGED, "localechanged");
    ADD(DISPLAYEVENT, "displayevent");
    ADD(WINDOWEVENT, "windowevent");
    ADD(SYSWMEVENT, "syswmevent");
    ADD(KEYDOWN, "keydown");
    ADD(KEYUP, "keyup");
    ADD(TEXTEDITING, "textediting");
    ADD(TEXTINPUT, "textinput");
    ADD(KEYMAPCHANGED, "keymapchanged");
    ADD(TEXTEDITING_EXT, "textediting ext");
    ADD(MOUSEMOTION, "mousemotion");
    ADD(MOUSEBUTTONDOWN, "mousebuttondown");
    ADD(MOUSEBUTTONUP, "mousebuttonup");
    ADD(MOUSEWHEEL, "mousewheel");
    ADD(JOYAXISMOTION, "joyaxismotion");
    ADD(JOYBALLMOTION, "joyballmotion");
    ADD(JOYHATMOTION, "joyhatmotion");
    ADD(JOYBUTTONDOWN, "joybuttondown");
    ADD(JOYBUTTONUP, "joybuttonup");
    ADD(JOYDEVICEADDED, "joydeviceadded");
    ADD(JOYDEVICEREMOVED, "joydeviceremoved");
    ADD(JOYBATTERYUPDATED, "joybatteryupdated");
    ADD(CONTROLLERAXISMOTION, "controlleraxismotion");
    ADD(CONTROLLERBUTTONDOWN, "controllerbuttondown");
    ADD(CONTROLLERBUTTONUP, "controllerbuttonup");
    ADD(CONTROLLERDEVICEADDED, "controllerdeviceadded");
    ADD(CONTROLLERDEVICEREMOVED, "controllerdeviceremoved");
    ADD(CONTROLLERDEVICEREMAPPED, "controllerdeviceremapped");
    ADD(CONTROLLERTOUCHPADDOWN, "controllertouchpaddown");
    ADD(CONTROLLERTOUCHPADMOTION, "controllertouchpadmotion");
    ADD(CONTROLLERTOUCHPADUP, "controllertouchpadup");
    ADD(CONTROLLERSENSORUPDATE, "controllersensorupdate");
    ADD(FINGERDOWN, "fingerdown");
    ADD(FINGERUP, "fingerup");
    ADD(FINGERMOTION, "fingermotion");
    ADD(DOLLARGESTURE, "dollargesture");
    ADD(DOLLARRECORD, "dollarrecord");
    ADD(MULTIGESTURE, "multigesture");
    ADD(CLIPBOARDUPDATE, "clipboardupdate");
    ADD(DROPFILE, "dropfile");
    ADD(DROPTEXT, "droptext");
    ADD(DROPBEGIN, "dropbegin");
    ADD(DROPCOMPLETE, "dropcomplete");
    ADD(AUDIODEVICEADDED, "audiodeviceadded");
    ADD(AUDIODEVICEREMOVED, "audiodeviceremoved");
    ADD(SENSORUPDATE, "sensorupdate");
    ADD(RENDER_TARGETS_RESET, "render targets reset");
    ADD(RENDER_DEVICE_RESET, "render device reset");
    ADD(POLLSENTINEL, "pollsentinel");
    ADD(USEREVENT, "userevent");
    ADD(LASTEVENT, "lastevent");

    domain = DOMAIN_DISPLAYEVENT;
    ADD(DISPLAYEVENT_NONE, "none");
    ADD(DISPLAYEVENT_ORIENTATION, "orientation");
    ADD(DISPLAYEVENT_CONNECTED, "connected");
    ADD(DISPLAYEVENT_DISCONNECTED, "disconnected");

    domain = DOMAIN_WINDOWEVENT;
    ADD(WINDOWEVENT_NONE, "none");
    ADD(WINDOWEVENT_SHOWN, "shown");
    ADD(WINDOWEVENT_HIDDEN, "hidden");
    ADD(WINDOWEVENT_EXPOSED, "exposed");
    ADD(WINDOWEVENT_MOVED, "moved");
    ADD(WINDOWEVENT_RESIZED, "resized");
    ADD(WINDOWEVENT_SIZE_CHANGED, "size changed");
    ADD(WINDOWEVENT_MINIMIZED, "minimized");
    ADD(WINDOWEVENT_MAXIMIZED, "maximized");
    ADD(WINDOWEVENT_RESTORED, "restored");
    ADD(WINDOWEVENT_ENTER, "enter");
    ADD(WINDOWEVENT_LEAVE, "leave");
    ADD(WINDOWEVENT_FOCUS_GAINED, "focus gained");
    ADD(WINDOWEVENT_FOCUS_LOST, "focus lost");
    ADD(WINDOWEVENT_CLOSE, "close");
    ADD(WINDOWEVENT_TAKE_FOCUS, "take focus");
    ADD(WINDOWEVENT_HIT_TEST, "hit test");
    ADD(WINDOWEVENT_ICCPROF_CHANGED, "iccprof changed");
    ADD(WINDOWEVENT_DISPLAY_CHANGED, "display changed");

    domain = DOMAIN_KEYSTATE;
    ADD(RELEASED, "released");
    ADD(PRESSED, "pressed");

    domain = DOMAIN_BUTTON;
    ADD(BUTTON_LEFT, "left");
    ADD(BUTTON_RIGHT, "right");
    ADD(BUTTON_MIDDLE, "middle");
    ADD(BUTTON_X1, "x1");
    ADD(BUTTON_X2, "x2");

    domain = DOMAIN_MOUSEWHEEL_DIR;
    ADD(MOUSEWHEEL_NORMAL, "normal");
    ADD(MOUSEWHEEL_FLIPPED, "flipped");

    domain = DOMAIN_CONTROLLER_TYPE;
    ADD(CONTROLLER_TYPE_UNKNOWN, "unknown");
    ADD(CONTROLLER_TYPE_XBOX360, "xbox360");
    ADD(CONTROLLER_TYPE_XBOXONE, "xboxone");
    ADD(CONTROLLER_TYPE_PS3, "ps3");
    ADD(CONTROLLER_TYPE_PS4, "ps4");
    ADD(CONTROLLER_TYPE_NINTENDO_SWITCH_PRO, "nintendo switch pro");
    ADD(CONTROLLER_TYPE_VIRTUAL, "virtual");
    ADD(CONTROLLER_TYPE_PS5, "ps5");
    ADD(CONTROLLER_TYPE_AMAZON_LUNA, "amazon luna");
    ADD(CONTROLLER_TYPE_GOOGLE_STADIA, "google stadia");
    ADD(CONTROLLER_TYPE_NVIDIA_SHIELD, "nvidia shield");
    ADD(CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_LEFT, "nintendo switch joycon left");
    ADD(CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT, "nintendo switch joycon right");
    ADD(CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_PAIR, "nintendo switch joycon pair");

    domain = DOMAIN_CONTROLLER_BINDTYPE;
    ADD(CONTROLLER_BINDTYPE_NONE, "none");
    ADD(CONTROLLER_BINDTYPE_BUTTON, "button");
    ADD(CONTROLLER_BINDTYPE_AXIS, "axis");
    ADD(CONTROLLER_BINDTYPE_HAT, "hat");

    domain = DOMAIN_CONTROLLER_AXIS;
    ADD(CONTROLLER_AXIS_INVALID, "invalid");
    ADD(CONTROLLER_AXIS_LEFTX, "leftx");
    ADD(CONTROLLER_AXIS_LEFTY, "lefty");
    ADD(CONTROLLER_AXIS_RIGHTX, "rightx");
    ADD(CONTROLLER_AXIS_RIGHTY, "righty");
    ADD(CONTROLLER_AXIS_TRIGGERLEFT, "triggerleft");
    ADD(CONTROLLER_AXIS_TRIGGERRIGHT, "triggerright");

    domain = DOMAIN_CONTROLLER_BUTTON;
    ADD(CONTROLLER_BUTTON_INVALID, "invalid");
    ADD(CONTROLLER_BUTTON_A, "a");
    ADD(CONTROLLER_BUTTON_B, "b");
    ADD(CONTROLLER_BUTTON_X, "x");
    ADD(CONTROLLER_BUTTON_Y, "y");
    ADD(CONTROLLER_BUTTON_BACK, "back");
    ADD(CONTROLLER_BUTTON_GUIDE, "guide");
    ADD(CONTROLLER_BUTTON_START, "start");
    ADD(CONTROLLER_BUTTON_LEFTSTICK, "leftstick");
    ADD(CONTROLLER_BUTTON_RIGHTSTICK, "rightstick");
    ADD(CONTROLLER_BUTTON_LEFTSHOULDER, "leftshoulder");
    ADD(CONTROLLER_BUTTON_RIGHTSHOULDER, "rightshoulder");
    ADD(CONTROLLER_BUTTON_DPAD_UP, "dpad up");
    ADD(CONTROLLER_BUTTON_DPAD_DOWN, "dpad down");
    ADD(CONTROLLER_BUTTON_DPAD_LEFT, "dpad left");
    ADD(CONTROLLER_BUTTON_DPAD_RIGHT, "dpad right");
    ADD(CONTROLLER_BUTTON_MISC1, "misc1");
    ADD(CONTROLLER_BUTTON_PADDLE1, "paddle1");
    ADD(CONTROLLER_BUTTON_PADDLE2, "paddle2");
    ADD(CONTROLLER_BUTTON_PADDLE3, "paddle3");
    ADD(CONTROLLER_BUTTON_PADDLE4, "paddle4");
    ADD(CONTROLLER_BUTTON_TOUCHPAD, "touchpad");

    domain = DOMAIN_TEXTUREACCESS;
    ADD(TEXTUREACCESS_STATIC, "static");
    ADD(TEXTUREACCESS_STREAMING, "streaming");
    ADD(TEXTUREACCESS_TARGET, "target");

    domain = DOMAIN_BLENDOPERATION;
    ADD(BLENDOPERATION_ADD, "add");
    ADD(BLENDOPERATION_SUBTRACT, "subtract");
    ADD(BLENDOPERATION_REV_SUBTRACT, "rev subtract");
    ADD(BLENDOPERATION_MINIMUM, "minimum");
    ADD(BLENDOPERATION_MAXIMUM, "maximum");

    domain = DOMAIN_BLENDFACTOR;
    ADD(BLENDFACTOR_ZERO, "zero");
    ADD(BLENDFACTOR_ONE, "one");
    ADD(BLENDFACTOR_SRC_COLOR, "src color");
    ADD(BLENDFACTOR_ONE_MINUS_SRC_COLOR, "one minus src color");
    ADD(BLENDFACTOR_SRC_ALPHA, "src alpha");
    ADD(BLENDFACTOR_ONE_MINUS_SRC_ALPHA, "one minus src alpha");
    ADD(BLENDFACTOR_DST_COLOR, "dst color");
    ADD(BLENDFACTOR_ONE_MINUS_DST_COLOR, "one minus dst color");
    ADD(BLENDFACTOR_DST_ALPHA, "dst alpha");
    ADD(BLENDFACTOR_ONE_MINUS_DST_ALPHA, "one minus dst alpha");

    domain = DOMAIN_SCALEMODE;
    ADD(ScaleModeNearest, "nearest");
    ADD(ScaleModeLinear, "linear");
    ADD(ScaleModeBest, "best");

    domain = DOMAIN_GLATTR;
    ADD(GL_RED_SIZE, "red size");
    ADD(GL_GREEN_SIZE, "green size");
    ADD(GL_BLUE_SIZE, "blue size");
    ADD(GL_ALPHA_SIZE, "alpha size");
    ADD(GL_BUFFER_SIZE, "buffer size");
    ADD(GL_DOUBLEBUFFER, "doublebuffer");
    ADD(GL_DEPTH_SIZE, "depth size");
    ADD(GL_STENCIL_SIZE, "stencil size");
    ADD(GL_ACCUM_RED_SIZE, "accum red size");
    ADD(GL_ACCUM_GREEN_SIZE, "accum green size");
    ADD(GL_ACCUM_BLUE_SIZE, "accum blue size");
    ADD(GL_ACCUM_ALPHA_SIZE, "accum alpha size");
    ADD(GL_STEREO, "stereo");
    ADD(GL_MULTISAMPLEBUFFERS, "multisamplebuffers");
    ADD(GL_MULTISAMPLESAMPLES, "multisamplesamples");
    ADD(GL_ACCELERATED_VISUAL, "accelerated visual");
    ADD(GL_RETAINED_BACKING, "retained backing");
    ADD(GL_CONTEXT_MAJOR_VERSION, "context major version");
    ADD(GL_CONTEXT_MINOR_VERSION, "context minor version");
    ADD(GL_CONTEXT_EGL, "context egl");
    ADD(GL_CONTEXT_FLAGS, "context flags");
    ADD(GL_CONTEXT_PROFILE_MASK, "context profile mask");
    ADD(GL_SHARE_WITH_CURRENT_CONTEXT, "share with current context");
    ADD(GL_FRAMEBUFFER_SRGB_CAPABLE, "framebuffer srgb capable");
    ADD(GL_CONTEXT_RELEASE_BEHAVIOR, "context release behavior");
    ADD(GL_CONTEXT_RESET_NOTIFICATION, "context reset notification");
    ADD(GL_CONTEXT_NO_ERROR, "context no error");
    ADD(GL_FLOATBUFFERS, "floatbuffers");

    domain = DOMAIN_ORIENTATION;
    ADD(ORIENTATION_UNKNOWN, "unknown");
    ADD(ORIENTATION_LANDSCAPE, "landscape");
    ADD(ORIENTATION_LANDSCAPE_FLIPPED, "landscape flipped");
    ADD(ORIENTATION_PORTRAIT, "portrait");
    ADD(ORIENTATION_PORTRAIT_FLIPPED, "portrait flipped");

    domain = DOMAIN_FLASHOPERATION;
    ADD(FLASH_CANCEL, "cancel");
    ADD(FLASH_BRIEFLY, "briefly");
    ADD(FLASH_UNTIL_FOCUSED, "until focused");

    domain = DOMAIN_HITTEST_RESULT;
    ADD(HITTEST_NORMAL, "normal");
    ADD(HITTEST_DRAGGABLE, "draggable");
    ADD(HITTEST_RESIZE_TOPLEFT, "resize topleft");
    ADD(HITTEST_RESIZE_TOP, "resize top");
    ADD(HITTEST_RESIZE_TOPRIGHT, "resize topright");
    ADD(HITTEST_RESIZE_RIGHT, "resize right");
    ADD(HITTEST_RESIZE_BOTTOMRIGHT, "resize bottomright");
    ADD(HITTEST_RESIZE_BOTTOM, "resize bottom");
    ADD(HITTEST_RESIZE_BOTTOMLEFT, "resize bottomleft");
    ADD(HITTEST_RESIZE_LEFT, "resize left");

    domain = DOMAIN_JOYSTICK_TYPE;
    ADD(JOYSTICK_TYPE_UNKNOWN, "unknown");
    ADD(JOYSTICK_TYPE_GAMECONTROLLER, "gamecontroller");
    ADD(JOYSTICK_TYPE_WHEEL, "wheel");
    ADD(JOYSTICK_TYPE_ARCADE_STICK, "arcade stick");
    ADD(JOYSTICK_TYPE_FLIGHT_STICK, "flight stick");
    ADD(JOYSTICK_TYPE_DANCE_PAD, "dance pad");
    ADD(JOYSTICK_TYPE_GUITAR, "guitar");
    ADD(JOYSTICK_TYPE_DRUM_KIT, "drum kit");
    ADD(JOYSTICK_TYPE_ARCADE_PAD, "arcade pad");
    ADD(JOYSTICK_TYPE_THROTTLE, "throttle");

    domain = DOMAIN_JOYSTICK_POWER_LEVEL;
    ADD(JOYSTICK_POWER_UNKNOWN, "unknown");
    ADD(JOYSTICK_POWER_EMPTY, "empty");
    ADD(JOYSTICK_POWER_LOW, "low");
    ADD(JOYSTICK_POWER_MEDIUM, "medium");
    ADD(JOYSTICK_POWER_FULL, "full");
    ADD(JOYSTICK_POWER_WIRED, "wired");

    domain = DOMAIN_SYSTEM_CURSOR;
    ADD(SYSTEM_CURSOR_ARROW, "arrow");
    ADD(SYSTEM_CURSOR_IBEAM, "ibeam");
    ADD(SYSTEM_CURSOR_WAIT, "wait");
    ADD(SYSTEM_CURSOR_CROSSHAIR, "crosshair");
    ADD(SYSTEM_CURSOR_WAITARROW, "waitarrow");
    ADD(SYSTEM_CURSOR_SIZENWSE, "sizenwse");
    ADD(SYSTEM_CURSOR_SIZENESW, "sizenesw");
    ADD(SYSTEM_CURSOR_SIZEWE, "sizewe");
    ADD(SYSTEM_CURSOR_SIZENS, "sizens");
    ADD(SYSTEM_CURSOR_SIZEALL, "sizeall");
    ADD(SYSTEM_CURSOR_NO, "no");
    ADD(SYSTEM_CURSOR_HAND, "hand");

    domain = DOMAIN_LOG_CATEGORY;
    ADD(LOG_CATEGORY_APPLICATION, "application");
    ADD(LOG_CATEGORY_ERROR, "error");
    ADD(LOG_CATEGORY_ASSERT, "assert");
    ADD(LOG_CATEGORY_SYSTEM, "system");
    ADD(LOG_CATEGORY_AUDIO, "audio");
    ADD(LOG_CATEGORY_VIDEO, "video");
    ADD(LOG_CATEGORY_RENDER, "render");
    ADD(LOG_CATEGORY_INPUT, "input");
    ADD(LOG_CATEGORY_TEST, "test");

    domain = DOMAIN_POWERSTATE;
    ADD(POWERSTATE_UNKNOWN, "unknown");
    ADD(POWERSTATE_ON_BATTERY, "on battery");
    ADD(POWERSTATE_NO_BATTERY, "no battery");
    ADD(POWERSTATE_CHARGING, "charging");
    ADD(POWERSTATE_CHARGED, "charged");

    domain = DOMAIN_HAT_STATE;
    ADD(HAT_CENTERED, "centered");
    ADD(HAT_UP, "up");
    ADD(HAT_RIGHT, "right");
    ADD(HAT_DOWN, "down");
    ADD(HAT_LEFT, "left");
    ADD(HAT_RIGHTUP, "rightup");
    ADD(HAT_RIGHTDOWN, "rightdown");
    ADD(HAT_LEFTUP, "leftup");
    ADD(HAT_LEFTDOWN, "leftdown");
 
    domain = DOMAIN_TOUCH_DEVICE_TYPE;
    ADD(TOUCH_DEVICE_INVALID, "invalid");
    ADD(TOUCH_DEVICE_DIRECT, "direct");
    ADD(TOUCH_DEVICE_INDIRECT_ABSOLUTE, "indirect absolute");
    ADD(TOUCH_DEVICE_INDIRECT_RELATIVE, "indirect relative");

    domain = DOMAIN_RWOPS_TYPE;
    ADD(RWOPS_UNKNOWN, "unknown");
    ADD(RWOPS_WINFILE, "winfile");
    ADD(RWOPS_STDFILE, "stdfile");
    ADD(RWOPS_JNIFILE, "jnifile");
    ADD(RWOPS_MEMORY, "memory");
    ADD(RWOPS_MEMORY_RO, "memory ro");

    domain = DOMAIN_HAPTIC_TYPE;
    ADD(HAPTIC_CONSTANT, "constant");
    ADD(HAPTIC_SINE, "sine");
    ADD(HAPTIC_LEFTRIGHT, "leftright");
    ADD(HAPTIC_TRIANGLE, "triangle");
    ADD(HAPTIC_SAWTOOTHUP, "sawtoothup");
    ADD(HAPTIC_SAWTOOTHDOWN, "sawtoothdown");
    ADD(HAPTIC_RAMP, "ramp");
    ADD(HAPTIC_SPRING, "spring");
    ADD(HAPTIC_DAMPER, "damper");
    ADD(HAPTIC_INERTIA, "inertia");
    ADD(HAPTIC_FRICTION, "friction");
    ADD(HAPTIC_CUSTOM, "custom");
//  ADD(HAPTIC_GAIN, "gain");
//  ADD(HAPTIC_AUTOCENTER, "autocenter");
//  ADD(HAPTIC_STATUS, "status");
//  ADD(HAPTIC_PAUSE, "pause");

    domain = DOMAIN_HAPTIC_DIRECTION;
    ADD(HAPTIC_POLAR, "polar");
    ADD(HAPTIC_CARTESIAN, "cartesian");
    ADD(HAPTIC_SPHERICAL, "spherical");
    ADD(HAPTIC_STEERING_AXIS, "steering axis");

    domain = DOMAIN_SENSOR_TYPE;
    ADD(SENSOR_INVALID, "invalid");
    ADD(SENSOR_UNKNOWN, "unknown");
    ADD(SENSOR_ACCEL, "accel");
    ADD(SENSOR_GYRO, "gyro");

    domain = DOMAIN_GL_PROFILE;
    ADD(GL_CONTEXT_PROFILE_CORE, "core");
    ADD(GL_CONTEXT_PROFILE_COMPATIBILITY, "compatibility");
    ADD(GL_CONTEXT_PROFILE_ES, "es");
#undef ADD

    /* Enums with names not starting with SDL_ */
#define ADD(what, s) do { enums_new(L, domain, what, s); } while(0)
    domain = DOMAIN_RWOPS_WHENCE;
    ADD(RW_SEEK_SET, "seek set");
    ADD(RW_SEEK_CUR, "seek cur");
    ADD(RW_SEEK_END, "seek end");

    domain = DOMAIN_SWAPINTERVAL;
    ADD(SWAPINTERVAL_IMMEDIATE, "immediate");
    ADD(SWAPINTERVAL_VSYNC, "vsync");
    ADD(SWAPINTERVAL_ADAPTIVE_VSYNC, "adaptive vsync");

    domain = DOMAIN_FONT_HINTING;
    ADD(TTF_HINTING_NORMAL, "normal");
    ADD(TTF_HINTING_LIGHT, "light");
    ADD(TTF_HINTING_MONO, "mono");
    ADD(TTF_HINTING_NONE, "none");
    ADD(TTF_HINTING_LIGHT_SUBPIXEL, "light subpixel");

    domain = DOMAIN_MUSIC_TYPE;
    ADD(MUS_NONE, "none");
    ADD(MUS_CMD, "cmd");
    ADD(MUS_WAV, "wav");
    ADD(MUS_MOD, "mod");
    ADD(MUS_MID, "mid");
    ADD(MUS_OGG, "ogg");
    ADD(MUS_MP3, "mp3");
//  ADD(MUS_MP3_MAD_UNUSED, "mp3 mad unused");
    ADD(MUS_FLAC, "flac");
//  ADD(MUS_MODPLUG_UNUSED, "modplug unused");
    ADD(MUS_OPUS, "opus");

    domain = DOMAIN_FADING;
    ADD(MIX_NO_FADING, "no fading");
    ADD(MIX_FADING_OUT, "fading out");
    ADD(MIX_FADING_IN, "fading in");

    domain = DOMAIN_AUDIO_FORMAT;
    ADD(AUDIO_U8, "u8");
    ADD(AUDIO_S8, "s8");
    ADD(AUDIO_U16, "u16");
    ADD(AUDIO_S16, "s16");
    ADD(AUDIO_U16MSB, "u16msb");
    ADD(AUDIO_S16MSB, "s16msb");
    ADD(AUDIO_S32, "s32");
    ADD(AUDIO_S32MSB, "s32msb");
    ADD(AUDIO_F32, "f32");
    ADD(AUDIO_F32MSB, "f32msb");

    domain = DOMAIN_FONT_DIRECTION;
    ADD(TTF_DIRECTION_LTR, "ltr");
    ADD(TTF_DIRECTION_RTL, "rtl");
    ADD(TTF_DIRECTION_TTB, "ttb");
    ADD(TTF_DIRECTION_BTT, "btt");

    domain = DOMAIN_FONT_ALIGN;
    ADD(TTF_WRAPPED_ALIGN_LEFT, "left");
    ADD(TTF_WRAPPED_ALIGN_CENTER, "center");
    ADD(TTF_WRAPPED_ALIGN_RIGHT, "right");

#define FULLSCREEN_MODE_REAL SDL_WINDOW_FULLSCREEN
#define FULLSCREEN_MODE_FAKE SDL_WINDOW_FULLSCREEN_DESKTOP
#define FULLSCREEN_MODE_WINDOWED    0
    domain = DOMAIN_FULLSCREEN_MODE;
    ADD(FULLSCREEN_MODE_REAL, "real");
    ADD(FULLSCREEN_MODE_FAKE, "fake");
    ADD(FULLSCREEN_MODE_WINDOWED, "windowed");
#undef ADD
    }

