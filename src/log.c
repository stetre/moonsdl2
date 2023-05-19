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
 
static int checkcategory(lua_State *L, int arg)
    {
    int category;
    if(lua_isinteger(L, arg))
        category = luaL_checkinteger(L, arg) + SDL_LOG_CATEGORY_CUSTOM;
    else
        category = checklogcategory(L, arg);
    return category;
    }

static void pushcategory(lua_State *L, int category)
    {
    if(category >= SDL_LOG_CATEGORY_CUSTOM)
        lua_pushinteger(L, category - SDL_LOG_CATEGORY_CUSTOM);
    else
        pushlogcategory(L, category);
    }

static int LogSetAllPriority(lua_State *L)
    {
    int priority = checklogpriority(L, 1);
    SDL_LogSetAllPriority(priority);
    return 0;
    }

static int LogSetPriority(lua_State *L)
    {
    int category = checkcategory(L, 1);
    SDL_LogPriority priority = checklogpriority(L, 2);
    SDL_LogSetPriority(category, priority);
    return 0;
    }

static int LogGetPriority(lua_State *L)
    {
    int category = checkcategory(L, 1);
    SDL_LogPriority priority =  SDL_LogGetPriority(category);
    pushlogpriority(L, priority);
    return 1;
    }

static int LogResetPriorities(lua_State *L)
    {
    (void)L;
    SDL_LogResetPriorities();
    return 0;
    }

static int Log(lua_State *L)
    {
    const char* msg = luaL_checkstring(L, 1);
    SDL_Log("%s", msg);
    return 0;
    }

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    int category = checkcategory(L, 1);             \
    const char* msg = luaL_checkstring(L, 2);       \
    func(category, "%s", msg);                      \
    return 0;                                       \
    }
F(LogVerbose, SDL_LogVerbose)
F(LogDebug, SDL_LogDebug)
F(LogInfo, SDL_LogInfo)
F(LogWarn, SDL_LogWarn)
F(LogError, SDL_LogError)
F(LogCritical, SDL_LogCritical)
#undef F

static int LogMessage(lua_State *L)
    {
    int category = checkcategory(L, 1);
    SDL_LogPriority priority = checklogpriority(L, 2);
    const char* msg = luaL_checkstring(L, 3);
    SDL_LogMessage(category, priority, "%s", msg);
    return 0;
    }

static int CbRef = LUA_NOREF;

static void Callback(void *userdata, int category, SDL_LogPriority priority, const char *message)
    {
    lua_State *L = moonsdl2_L;
    int top = lua_gettop(L);
    (void)userdata;
    if(CbRef == LUA_NOREF) return;
    lua_rawgeti(L, LUA_REGISTRYINDEX, CbRef);
    pushcategory(L, category);
    pushlogpriority(L, priority);
    lua_pushstring(L, message);
    if(lua_pcall(L, 3, 0, 0) != LUA_OK) lua_error(L);
    lua_settop(L, top);
    }

static int LogSetOutputFunction(lua_State *L)
    {
    if(lua_isnoneornil(L, 1))
        {
        Unreference(L, CbRef);
        SDL_LogSetOutputFunction(NULL, NULL);
        return 0;
        }
    if(!lua_isfunction(L, 1))
        return argerror(L, 1, ERR_FUNCTION);
    Reference(L, 1, CbRef);
    SDL_LogSetOutputFunction(&Callback, NULL);
    return 0;
    }

/* ----------------------------------------------------------------------- */

static const struct luaL_Reg Functions[] = 
    {
        { "log_set_all_priority", LogSetAllPriority },
        { "log_set_priority", LogSetPriority },
        { "log_get_priority", LogGetPriority },
        { "log_reset_priorities", LogResetPriorities },
        { "log", Log },
        { "log_verbose", LogVerbose },
        { "log_debug", LogDebug },
        { "log_info", LogInfo },
        { "log_warn", LogWarn },
        { "log_error", LogError },
        { "log_critical", LogCritical },
        { "log_message", LogMessage },
        { "log_set_output_function", LogSetOutputFunction },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_log(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
//void SDL_LogMessageV(int category, SDL_LogPriority priority, const char *fmt, va_list ap);
//void SDL_LogGetOutputFunction(SDL_LogOutputFunction *callback, void **userdata);
#endif
