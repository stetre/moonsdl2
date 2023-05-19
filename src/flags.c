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

/* checkkkflags: accepts a list of strings starting from index=arg
 * pushxxxflags -> pushes a list of strings 
 */

/*----------------------------------------------------------------------*
 | initflags
 *----------------------------------------------------------------------*/

static int checkinitflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_INIT_TIMER, "timer");
    CASE(SDL_INIT_AUDIO, "audio");
    CASE(SDL_INIT_VIDEO, "video");
    CASE(SDL_INIT_JOYSTICK, "joystick");
    CASE(SDL_INIT_HAPTIC, "haptic");
    CASE(SDL_INIT_GAMECONTROLLER, "gamecontroller");
    CASE(SDL_INIT_EVENTS, "events");
    CASE(SDL_INIT_SENSOR, "sensor");
    CASE(SDL_INIT_NOPARACHUTE, "noparachute");
    CASE(SDL_INIT_EVERYTHING, "everything");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushinitflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_INIT_TIMER, "timer");
    CASE(SDL_INIT_AUDIO, "audio");
    CASE(SDL_INIT_VIDEO, "video");
    CASE(SDL_INIT_JOYSTICK, "joystick");
    CASE(SDL_INIT_HAPTIC, "haptic");
    CASE(SDL_INIT_GAMECONTROLLER, "gamecontroller");
    CASE(SDL_INIT_EVENTS, "events");
    CASE(SDL_INIT_SENSOR, "sensor");
    CASE(SDL_INIT_NOPARACHUTE, "noparachute");
    CASE(SDL_INIT_EVERYTHING, "everything");
#undef CASE

    return n;
    }

static int InitFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushinitflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkinitflags(L, 1));
    return 1;
    }

#define Add_InitFlags(L) \
    ADD(INIT_TIMER);\
    ADD(INIT_AUDIO);\
    ADD(INIT_VIDEO);\
    ADD(INIT_JOYSTICK);\
    ADD(INIT_HAPTIC);\
    ADD(INIT_GAMECONTROLLER);\
    ADD(INIT_EVENTS);\
    ADD(INIT_SENSOR);\
    ADD(INIT_NOPARACHUTE);\
    ADD(INIT_EVERYTHING);\

/*----------------------------------------------------------------------*
 | windowflags
 *----------------------------------------------------------------------*/

static int checkwindowflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_WINDOW_FULLSCREEN, "fullscreen");
    CASE(SDL_WINDOW_OPENGL, "opengl");
    CASE(SDL_WINDOW_SHOWN, "shown");
    CASE(SDL_WINDOW_HIDDEN, "hidden");
    CASE(SDL_WINDOW_BORDERLESS, "borderless");
    CASE(SDL_WINDOW_RESIZABLE, "resizable");
    CASE(SDL_WINDOW_MINIMIZED, "minimized");
    CASE(SDL_WINDOW_MAXIMIZED, "maximized");
    CASE(SDL_WINDOW_MOUSE_GRABBED, "mouse grabbed");
    CASE(SDL_WINDOW_INPUT_FOCUS, "input focus");
    CASE(SDL_WINDOW_MOUSE_FOCUS, "mouse focus");
    CASE(SDL_WINDOW_FULLSCREEN_DESKTOP, "fullscreen desktop");
    CASE(SDL_WINDOW_FOREIGN, "foreign");
    CASE(SDL_WINDOW_ALLOW_HIGHDPI, "allow highdpi");
    CASE(SDL_WINDOW_MOUSE_CAPTURE, "mouse capture");
    CASE(SDL_WINDOW_ALWAYS_ON_TOP, "always on top");
    CASE(SDL_WINDOW_SKIP_TASKBAR, "skip taskbar");
    CASE(SDL_WINDOW_UTILITY, "utility");
    CASE(SDL_WINDOW_TOOLTIP, "tooltip");
    CASE(SDL_WINDOW_POPUP_MENU, "popup menu");
    CASE(SDL_WINDOW_KEYBOARD_GRABBED, "keyboard grabbed");
    CASE(SDL_WINDOW_VULKAN, "vulkan");
    CASE(SDL_WINDOW_METAL, "metal");
    CASE(SDL_WINDOW_INPUT_GRABBED, "input grabbed");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushwindowflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_WINDOW_FULLSCREEN, "fullscreen");
    CASE(SDL_WINDOW_OPENGL, "opengl");
    CASE(SDL_WINDOW_SHOWN, "shown");
    CASE(SDL_WINDOW_HIDDEN, "hidden");
    CASE(SDL_WINDOW_BORDERLESS, "borderless");
    CASE(SDL_WINDOW_RESIZABLE, "resizable");
    CASE(SDL_WINDOW_MINIMIZED, "minimized");
    CASE(SDL_WINDOW_MAXIMIZED, "maximized");
    CASE(SDL_WINDOW_MOUSE_GRABBED, "mouse grabbed");
    CASE(SDL_WINDOW_INPUT_FOCUS, "input focus");
    CASE(SDL_WINDOW_MOUSE_FOCUS, "mouse focus");
    CASE(SDL_WINDOW_FULLSCREEN_DESKTOP, "fullscreen desktop");
    CASE(SDL_WINDOW_FOREIGN, "foreign");
    CASE(SDL_WINDOW_ALLOW_HIGHDPI, "allow highdpi");
    CASE(SDL_WINDOW_MOUSE_CAPTURE, "mouse capture");
    CASE(SDL_WINDOW_ALWAYS_ON_TOP, "always on top");
    CASE(SDL_WINDOW_SKIP_TASKBAR, "skip taskbar");
    CASE(SDL_WINDOW_UTILITY, "utility");
    CASE(SDL_WINDOW_TOOLTIP, "tooltip");
    CASE(SDL_WINDOW_POPUP_MENU, "popup menu");
    CASE(SDL_WINDOW_KEYBOARD_GRABBED, "keyboard grabbed");
    CASE(SDL_WINDOW_VULKAN, "vulkan");
    CASE(SDL_WINDOW_METAL, "metal");
    CASE(SDL_WINDOW_INPUT_GRABBED, "input grabbed");
#undef CASE

    return n;
    }

static int WindowFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushwindowflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkwindowflags(L, 1));
    return 1;
    }

#define Add_WindowFlags(L) \
    ADD(WINDOW_FULLSCREEN);\
    ADD(WINDOW_OPENGL);\
    ADD(WINDOW_SHOWN);\
    ADD(WINDOW_HIDDEN);\
    ADD(WINDOW_BORDERLESS);\
    ADD(WINDOW_RESIZABLE);\
    ADD(WINDOW_MINIMIZED);\
    ADD(WINDOW_MAXIMIZED);\
    ADD(WINDOW_MOUSE_GRABBED);\
    ADD(WINDOW_INPUT_FOCUS);\
    ADD(WINDOW_MOUSE_FOCUS);\
    ADD(WINDOW_FULLSCREEN_DESKTOP);\
    ADD(WINDOW_FOREIGN);\
    ADD(WINDOW_ALLOW_HIGHDPI);\
    ADD(WINDOW_MOUSE_CAPTURE);\
    ADD(WINDOW_ALWAYS_ON_TOP);\
    ADD(WINDOW_SKIP_TASKBAR);\
    ADD(WINDOW_UTILITY);\
    ADD(WINDOW_TOOLTIP);\
    ADD(WINDOW_POPUP_MENU);\
    ADD(WINDOW_KEYBOARD_GRABBED);\
    ADD(WINDOW_VULKAN);\
    ADD(WINDOW_METAL);\
    ADD(WINDOW_INPUT_GRABBED);\


/*----------------------------------------------------------------------*
 | rendererflags
 *----------------------------------------------------------------------*/

static int checkrendererflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_RENDERER_SOFTWARE, "software");
    CASE(SDL_RENDERER_ACCELERATED, "accelerated");
    CASE(SDL_RENDERER_PRESENTVSYNC, "presentvsync");
    CASE(SDL_RENDERER_TARGETTEXTURE, "targettexture");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushrendererflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_RENDERER_SOFTWARE, "software");
    CASE(SDL_RENDERER_ACCELERATED, "accelerated");
    CASE(SDL_RENDERER_PRESENTVSYNC, "presentvsync");
    CASE(SDL_RENDERER_TARGETTEXTURE, "targettexture");
#undef CASE

    return n;
    }

static int RendererFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushrendererflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkrendererflags(L, 1));
    return 1;
    }

#define Add_RendererFlags(L) \
    ADD(RENDERER_SOFTWARE);\
    ADD(RENDERER_ACCELERATED);\
    ADD(RENDERER_PRESENTVSYNC);\
    ADD(RENDERER_TARGETTEXTURE);\

/*----------------------------------------------------------------------*
 | flipflags
 *----------------------------------------------------------------------*/

static int checkflipflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_FLIP_NONE, "none");
    CASE(SDL_FLIP_HORIZONTAL, "horizontal");
    CASE(SDL_FLIP_VERTICAL, "vertical");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushflipflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_FLIP_NONE, "none");
    CASE(SDL_FLIP_HORIZONTAL, "horizontal");
    CASE(SDL_FLIP_VERTICAL, "vertical");
#undef CASE

    return n;
    }

static int FlipFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushflipflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkflipflags(L, 1));
    return 1;
    }

#define Add_FlipFlags(L) \
    ADD(FLIP_NONE);\
    ADD(FLIP_HORIZONTAL);\
    ADD(FLIP_VERTICAL);\


/*----------------------------------------------------------------------*
 | audioallowflags
 *----------------------------------------------------------------------*/

static int checkaudioallowflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_AUDIO_ALLOW_FREQUENCY_CHANGE, "frequency change");
    CASE(SDL_AUDIO_ALLOW_FORMAT_CHANGE, "format change");
    CASE(SDL_AUDIO_ALLOW_CHANNELS_CHANGE, "channels change");
    CASE(SDL_AUDIO_ALLOW_SAMPLES_CHANGE, "samples change");
    CASE(SDL_AUDIO_ALLOW_ANY_CHANGE, "any change");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushaudioallowflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_AUDIO_ALLOW_FREQUENCY_CHANGE, "frequency change");
    CASE(SDL_AUDIO_ALLOW_FORMAT_CHANGE, "format change");
    CASE(SDL_AUDIO_ALLOW_CHANNELS_CHANGE, "channels change");
    CASE(SDL_AUDIO_ALLOW_SAMPLES_CHANGE, "samples change");
    CASE(SDL_AUDIO_ALLOW_ANY_CHANGE, "any change");
#undef CASE

    return n;
    }

static int AudioAllowFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushaudioallowflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkaudioallowflags(L, 1));
    return 1;
    }

#define Add_AudioAllowFlags(L) \
    ADD(AUDIO_ALLOW_FREQUENCY_CHANGE);\
    ADD(AUDIO_ALLOW_FORMAT_CHANGE);\
    ADD(AUDIO_ALLOW_CHANNELS_CHANGE);\
    ADD(AUDIO_ALLOW_SAMPLES_CHANGE);\
    ADD(AUDIO_ALLOW_ANY_CHANGE);\



/*----------------------------------------------------------------------*
 | messageboxflags
 *----------------------------------------------------------------------*/

static int checkmessageboxflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_MESSAGEBOX_ERROR, "error");
    CASE(SDL_MESSAGEBOX_WARNING, "warning");
    CASE(SDL_MESSAGEBOX_INFORMATION, "information");
    CASE(SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT, "buttons left to right");
    CASE(SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT, "buttons right to left");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushmessageboxflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_MESSAGEBOX_ERROR, "error");
    CASE(SDL_MESSAGEBOX_WARNING, "warning");
    CASE(SDL_MESSAGEBOX_INFORMATION, "information");
    CASE(SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT, "buttons left to right");
    CASE(SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT, "buttons right to left");
#undef CASE

    return n;
    }

static int MessageBoxFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushmessageboxflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkmessageboxflags(L, 1));
    return 1;
    }

#define Add_MessageBoxFlags(L) \
    ADD(MESSAGEBOX_ERROR);\
    ADD(MESSAGEBOX_WARNING);\
    ADD(MESSAGEBOX_INFORMATION);\
    ADD(MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT);\
    ADD(MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT);\


/*----------------------------------------------------------------------*
 | messageboxbuttonflags
 *----------------------------------------------------------------------*/

static int checkmessageboxbuttonflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, "returnkey default");
    CASE(SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, "escapekey default");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushmessageboxbuttonflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, "returnkey default");
    CASE(SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, "escapekey default");
#undef CASE

    return n;
    }

static int MessageBoxButtonFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushmessageboxbuttonflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkmessageboxbuttonflags(L, 1));
    return 1;
    }

#define Add_MessageBoxButtonFlags(L) \
    ADD(MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT);\
    ADD(MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT);\

/*----------------------------------------------------------------------*
 | glcontextflags
 *----------------------------------------------------------------------*/

static int checkglcontextflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(SDL_GL_CONTEXT_DEBUG_FLAG, "debug");
    CASE(SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, "forward compatible");
    CASE(SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG, "robust access");
    CASE(SDL_GL_CONTEXT_RESET_ISOLATION_FLAG, "reset isolation");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushglcontextflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(SDL_GL_CONTEXT_DEBUG_FLAG, "debug");
    CASE(SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, "forward compatible");
    CASE(SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG, "robust access");
    CASE(SDL_GL_CONTEXT_RESET_ISOLATION_FLAG, "reset isolation");
#undef CASE

    return n;
    }

static int GLContextFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushglcontextflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkglcontextflags(L, 1));
    return 1;
    }

#define Add_GLContextFlags(L) \
    ADD(GL_CONTEXT_DEBUG_FLAG);\
    ADD(GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);\
    ADD(GL_CONTEXT_ROBUST_ACCESS_FLAG);\
    ADD(GL_CONTEXT_RESET_ISOLATION_FLAG);\


/************************************************************************
 * SDL_image
 ************************************************************************/


/*----------------------------------------------------------------------*
 | imginitflags 
 *----------------------------------------------------------------------*/

static int checkimginitflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(IMG_INIT_JPG, "jpg");
    CASE(IMG_INIT_PNG, "png");
    CASE(IMG_INIT_TIF, "tif");
    CASE(IMG_INIT_WEBP, "webp");
    CASE(IMG_INIT_JXL, "jxl");
    CASE(IMG_INIT_AVIF, "avif");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushimginitflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(IMG_INIT_JPG, "jpg");
    CASE(IMG_INIT_PNG, "png");
    CASE(IMG_INIT_TIF, "tif");
    CASE(IMG_INIT_WEBP, "webp");
    CASE(IMG_INIT_JXL, "jxl");
    CASE(IMG_INIT_AVIF, "avif");
#undef CASE

    return n;
    }

static int ImgInitFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushimginitflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkimginitflags(L, 1));
    return 1;
    }

#define Add_ImgInitFlags(L) \
    ADD(IMG_INIT_JPG);\
    ADD(IMG_INIT_PNG);\
    ADD(IMG_INIT_TIF);\
    ADD(IMG_INIT_WEBP);\
    ADD(IMG_INIT_JXL);\
    ADD(IMG_INIT_AVIF);\

/************************************************************************
 * SDL_mixer
 ************************************************************************/

/*----------------------------------------------------------------------*
 | mixinitflags 
 *----------------------------------------------------------------------*/

static int checkmixinitflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(MIX_INIT_FLAC, "flac");
    CASE(MIX_INIT_MOD, "mod");
    CASE(MIX_INIT_MP3, "mp3");
    CASE(MIX_INIT_OGG, "ogg");
    CASE(MIX_INIT_MID, "mid");
    CASE(MIX_INIT_OPUS, "opus");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushmixinitflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(MIX_INIT_FLAC, "flac");
    CASE(MIX_INIT_MOD, "mod");
    CASE(MIX_INIT_MP3, "mp3");
    CASE(MIX_INIT_OGG, "ogg");
    CASE(MIX_INIT_MID, "mid");
    CASE(MIX_INIT_OPUS, "opus");
#undef CASE

    return n;
    }

static int MixInitFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushmixinitflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkmixinitflags(L, 1));
    return 1;
    }

#define Add_MixInitFlags(L) \
    ADD(MIX_INIT_FLAC);\
    ADD(MIX_INIT_MOD);\
    ADD(MIX_INIT_MP3);\
    ADD(MIX_INIT_OGG);\
    ADD(MIX_INIT_MID);\
    ADD(MIX_INIT_OPUS);\


/************************************************************************
 * SDL_ttf  
 ************************************************************************/

/*----------------------------------------------------------------------*
 | ttfstyleflags 
 *----------------------------------------------------------------------*/

static int checkttfstyleflags(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
    CASE(TTF_STYLE_NORMAL, "normal");
    CASE(TTF_STYLE_BOLD, "bold");
    CASE(TTF_STYLE_ITALIC, "italic");
    CASE(TTF_STYLE_UNDERLINE, "underline");
    CASE(TTF_STYLE_STRIKETHROUGH, "strikethrough");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushttfstyleflags(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(TTF_STYLE_NORMAL, "normal");
    CASE(TTF_STYLE_BOLD, "bold");
    CASE(TTF_STYLE_ITALIC, "italic");
    CASE(TTF_STYLE_UNDERLINE, "underline");
    CASE(TTF_STYLE_STRIKETHROUGH, "strikethrough");
#undef CASE

    return n;
    }

static int TtfStyleFlags(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushttfstyleflags(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkttfstyleflags(L, 1));
    return 1;
    }

#define Add_TtfStyleFlags(L) \
    ADD(TTF_STYLE_NORMAL);\
    ADD(TTF_STYLE_BOLD);\
    ADD(TTF_STYLE_ITALIC);\
    ADD(TTF_STYLE_UNDERLINE);\
    ADD(TTF_STYLE_STRIKETHROUGH);\


/*----------------------------------------------------------------------*/

static int AddConstants(lua_State *L) /* sdl.XXX constants for SDL_XXX values */
    {
#define ADD(c) do { lua_pushinteger(L, SDL_##c); lua_setfield(L, -2, #c); } while(0)
    Add_InitFlags(L);
    Add_WindowFlags(L);
    Add_RendererFlags(L);
    Add_FlipFlags(L);
    Add_AudioAllowFlags(L);
    Add_MessageBoxFlags(L);
    Add_MessageBoxButtonFlags(L);
    Add_GLContextFlags(L);
#undef ADD
#define ADD(c) do { lua_pushinteger(L, c); lua_setfield(L, -2, #c); } while(0)
    Add_MixInitFlags(L);
    Add_ImgInitFlags(L);
    Add_TtfStyleFlags(L);
#undef ADD
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "initflags", InitFlags },
        { "windowflags", WindowFlags },
        { "rendererflags", RendererFlags },
        { "flipflags", FlipFlags },
        { "audioallowflags", AudioAllowFlags },
        { "messageboxflags", MessageBoxFlags },
        { "messageboxbuttonflags", MessageBoxButtonFlags },
        { "glcontextflags", GLContextFlags },
        { "mixinitflags", MixInitFlags },
        { "imginitflags", ImgInitFlags },
        { "ttfstyleflags", TtfStyleFlags },
        { NULL, NULL } /* sentinel */
    };


void moonsdl2_open_flags(lua_State *L)
    {
    AddConstants(L);
    luaL_setfuncs(L, Functions, 0);
    }


#if 0 // scaffolding

/*----------------------------------------------------------------------*
 | zzz
 *----------------------------------------------------------------------*/

static int checkzzz(lua_State *L, int arg) 
    {
    const char *s;
    int flags = 0;
    
    while(lua_isstring(L, arg))
        {
        s = lua_tostring(L, arg++);
#define CASE(CODE,str) if((strcmp(s, str)==0)) do { flags |= CODE; goto done; } while(0)
        CASE(ZZZ_, "");
#undef CASE
        return luaL_argerror(L, --arg, badvalue(L,s));
        done: ;
        }

    return flags;
    }

static int pushzzz(lua_State *L, int flags)
    {
    int n = 0;

#define CASE(CODE,str) do { if((flags & CODE)==CODE) { lua_pushstring(L, str); n++; } } while(0)
    CASE(ZZZ_, "");
#undef CASE

    return n;
    }

static int Zzz(lua_State *L)
    {
    if(lua_type(L, 1) == LUA_TNUMBER)
        return pushzzz(L, luaL_checkinteger(L, 1));
    lua_pushinteger(L, checkzzz(L, 1));
    return 1;
    }

    Add_Zzz(L);
        { "zzz", Zzz },
#define Add_Zzz(L) \
    ADD(ZZZ_);\

#endif

