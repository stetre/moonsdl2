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

static int freetimer(lua_State *L, ud_t *ud)
    {
    timer_t *timer = (timer_t*)ud->handle;
    SDL_RemoveTimer(timer->id);
    luaL_unref(L, LUA_REGISTRYINDEX, timer->ref);
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "timer")) return 0;
    Free(L, timer);
    return 0;
    }

static int newtimer(lua_State *L, timer_t *timer)
    {
    ud_t *ud;
    ud = newuserdata(L, timer, TIMER_MT, "timer");
    ud->parent_ud = NULL;
    ud->destructor = freetimer;
    CancelRemoved(ud);
    return 1;
    }

static uint32_t Callback(uint32_t interval, void *param)
    {
    /* This is run on a separate thread, so here we just push an event
     * which will be served later in the main thread were we can safely
     * execute the Lua callback or delete the timer (if it was removed
     * by the user. 
     * Note that flushing timer-related events would break this module.
     */
    SDL_Event event;
    timer_t *timer = (timer_t*)param;
    event.user.type = TIMEREVENT;
    event.user.code = 0;
    event.user.data1 = timer;
    SDL_PushEvent(&event);
    return interval; // this restarts the timer
    }

int timerevent(lua_State *L, const SDL_Event *event)
    {
    /* Called in the main thread when the event is served */
    int top, rc, restart;
    timer_t *timer = (timer_t*)event->user.data1;
    ud_t *ud = UD(timer);
    if(!ud) return 0; /* just to be sure ...*/
    // If the timer was removed by the user, just delete it:
    if(IsRemoved(ud)) return freetimer(L, ud);
    // Execute the Lua callback function passed by the user:
    top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, timer->ref);
    pushtimer(L, timer);
    rc = lua_pcall(L, 1, 1, 0); 
    if(rc!=LUA_OK) lua_error(L);
    restart = lua_toboolean(L, -1);
    lua_settop(L, top);
    /* The timer has already been restarted in Callback(). The user must
     * return true if she actually want it to keep running, otherwise at
     * the next expiry it will be deleted and the Lua callback will not
     * be executed. */
    if(!restart) MarkRemoved(ud);
    return 0;
    }

static int Create(lua_State *L)
    {
    timer_t *timer;
    uint32_t interval = checkmilliseconds(L, 1);
    if(!lua_isfunction(L, 2)) return argerror(L, 2, ERR_FUNCTION);
    timer = Malloc(L, sizeof(timer_t));
    lua_pushvalue(L, 2);
    timer->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    timer->id = SDL_AddTimer(interval, Callback, timer);
    if(timer->id==0)
        {
        luaL_unref(L, LUA_REGISTRYINDEX, timer->ref);
        Free(L, timer);
        GetError(L);
        }
    return newtimer(L, timer);
    }

static int Remove(lua_State *L)
    {
    ud_t *ud;
    (void)checktimer(L, 1, &ud);
    MarkRemoved(ud);
    return 0;
    }

DESTROY_FUNC(timer)

static const struct luaL_Reg Methods[] = 
    {
        { "stop", Remove },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "start_timer", Create },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_timer(lua_State *L)
    {
    udata_define(L, TIMER_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

