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

#ifndef internalDEFINED
#define internalDEFINED

#ifdef LINUX
#define _ISOC11_SOURCE /* see man aligned_alloc(3) */
#endif
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "moonsdl2.h"

#define TOSTR_(x) #x
#define TOSTR(x) TOSTR_(x)

#include "tree.h"
#include "objects.h"
#include "enums.h"

/* Note: all the dynamic symbols of this library (should) start with 'moonsdl2_' .
 * The only exception is the luaopen_moonsdl2() function, which is searched for
 * with that name by Lua.
 * MoonSDL2's string references on the Lua registry also start with 'moonsdl2_'.
 */

#if 0
/* .c */
#define  moonsdl2_
#endif

/* flags.c */
#define checkflags(L, arg) luaL_checkinteger((L), (arg))
#define optflags(L, arg, defval) luaL_optinteger((L), (arg), (defval))
#define pushflags(L, val) lua_pushinteger((L), (val))

/* utils.c */
void moonsdl2_utils_init(lua_State *L);
#define copytable moonsdl2_copytable
int copytable(lua_State *L);
#define noprintf moonsdl2_noprintf
int noprintf(const char *fmt, ...); 
#define now moonsdl2_now
double now(void);
#define sleeep moonsdl2_sleeep
void sleeep(double seconds);
#define since(t) (now() - (t))
#define notavailable moonsdl2_notavailable
int notavailable(lua_State *L, ...);
#define Malloc moonsdl2_Malloc
void *Malloc(lua_State *L, size_t size);
#define MallocNoErr moonsdl2_MallocNoErr
void *MallocNoErr(lua_State *L, size_t size);
#define Strdup moonsdl2_Strdup
char *Strdup(lua_State *L, const char *s);
#define Strndup moonsdl2_Strndup
char *Strndup(lua_State *L, const char *s, size_t len);
#define Free moonsdl2_Free
void Free(lua_State *L, void *ptr);
#define checkboolean moonsdl2_checkboolean
int checkboolean(lua_State *L, int arg);
#define testboolean moonsdl2_testboolean
int testboolean(lua_State *L, int arg, int *err);
#define optboolean moonsdl2_optboolean
int optboolean(lua_State *L, int arg, int d);
#define checklightuserdata moonsdl2_checklightuserdata
void *checklightuserdata(lua_State *L, int arg);
#define checklightuserdataorzero moonsdl2_checklightuserdataorzero
void *checklightuserdataorzero(lua_State *L, int arg);
#define optlightuserdata moonsdl2_optlightuserdata
void *optlightuserdata(lua_State *L, int arg);
#define testindex moonsdl2_testindex
int testindex(lua_State *L, int arg, int *err);
#define checkindex moonsdl2_checkindex
int checkindex(lua_State *L, int arg);
#define optindex moonsdl2_optindex
int optindex(lua_State *L, int arg, int optval);
#define pushindex moonsdl2_pushindex
void pushindex(lua_State *L, int val);

#define checkmilliseconds luaL_checkinteger
#define optmilliseconds luaL_optinteger
#define pushmilliseconds lua_pushinteger

/* datastructs.c */
#define testcolor moonsdl2_testcolor
int testcolor(lua_State *L, int arg, color_t *dst);
#define optcolor moonsdl2_optcolor
int optcolor(lua_State *L, int arg, color_t *dst);
#define checkcolor moonsdl2_checkcolor
int checkcolor(lua_State *L, int arg, color_t *dst);
#define pushcolor moonsdl2_pushcolor
void pushcolor(lua_State *L, const color_t *val);
#define checkcolorlist moonsdl2_checkcolorlist
color_t *checkcolorlist(lua_State *L, int arg, int *countp, int *err);
#define pushcolorlist moonsdl2_pushcolorlist
void pushcolorlist(lua_State *L, const color_t *colors , int count);
#define testpoint moonsdl2_testpoint
int testpoint(lua_State *L, int arg, point_t *dst);
#define optpoint moonsdl2_optpoint
int optpoint(lua_State *L, int arg, point_t *dst);
#define checkpoint moonsdl2_checkpoint
int checkpoint(lua_State *L, int arg, point_t *dst);
#define pushpoint moonsdl2_pushpoint
void pushpoint(lua_State *L, const point_t *val);
#define checkpointlist moonsdl2_checkpointlist
point_t *checkpointlist(lua_State *L, int arg, int *countp, int *err);
#define pushpointlist moonsdl2_pushpointlist
void pushpointlist(lua_State *L, const point_t *points , int count);
#define testfpoint moonsdl2_testfpoint
int testfpoint(lua_State *L, int arg, fpoint_t *dst);
#define optfpoint moonsdl2_optfpoint
int optfpoint(lua_State *L, int arg, fpoint_t *dst);
#define checkfpoint moonsdl2_checkfpoint
int checkfpoint(lua_State *L, int arg, fpoint_t *dst);
#define pushfpoint moonsdl2_pushfpoint
void pushfpoint(lua_State *L, const fpoint_t *val);
#define checkfpointlist moonsdl2_checkfpointlist
fpoint_t *checkfpointlist(lua_State *L, int arg, int *countp, int *err);
#define pushfpointlist moonsdl2_pushfpointlist
void pushfpointlist(lua_State *L, const fpoint_t *fpoints , int count);
#define testrect moonsdl2_testrect
int testrect(lua_State *L, int arg, rect_t *dst);
#define optrect moonsdl2_optrect
int optrect(lua_State *L, int arg, rect_t *dst);
#define checkrect moonsdl2_checkrect
int checkrect(lua_State *L, int arg, rect_t *dst);
#define pushrect moonsdl2_pushrect
void pushrect(lua_State *L, const rect_t *val);
#define checkrectlist moonsdl2_checkrectlist
rect_t *checkrectlist(lua_State *L, int arg, int *countp, int *err);
#define pushrectlist moonsdl2_pushrectlist
void pushrectlist(lua_State *L, const rect_t *rects , int count);
#define testfrect moonsdl2_testfrect
int testfrect(lua_State *L, int arg, frect_t *dst);
#define optfrect moonsdl2_optfrect
int optfrect(lua_State *L, int arg, frect_t *dst);
#define checkfrect moonsdl2_checkfrect
int checkfrect(lua_State *L, int arg, frect_t *dst);
#define pushfrect moonsdl2_pushfrect
void pushfrect(lua_State *L, const frect_t *val);
#define checkfrectlist moonsdl2_checkfrectlist
frect_t *checkfrectlist(lua_State *L, int arg, int *countp, int *err);
#define pushfrectlist moonsdl2_pushfrectlist
void pushfrectlist(lua_State *L, const frect_t *frects , int count);
#define testvertex moonsdl2_testvertex
int testvertex(lua_State *L, int arg, vertex_t *dst);
#define optvertex moonsdl2_optvertex
int optvertex(lua_State *L, int arg, vertex_t *dst);
#define checkvertex moonsdl2_checkvertex
int checkvertex(lua_State *L, int arg, vertex_t *dst);
#define pushvertex moonsdl2_pushvertex
void pushvertex(lua_State *L, const vertex_t *val);
#define checkvertexlist moonsdl2_checkvertexlist
vertex_t *checkvertexlist(lua_State *L, int arg, int *countp, int *err);
#define pushvertexlist moonsdl2_pushvertexlist
void pushvertexlist(lua_State *L, const vertex_t *vertices , int count);
#define checkintlist moonsdl2_checkintlist
int* checkintlist(lua_State *L, int arg, int *count, int *err);
#define pushintlist moonsdl2_pushintlist
void pushintlist(lua_State *L, int *list, int count);
#define checkuint16list moonsdl2_checkuint16list
uint16_t* checkuint16list(lua_State *L, int arg, int *count, int *err);
#define pushuint16list moonsdl2_pushuint16list
void pushuint16list(lua_State *L, uint16_t *list, int count);

/* Internal error codes */
#define ERR_NOTPRESENT       1
#define ERR_SUCCESS          0
#define ERR_GENERIC         -1
#define ERR_TYPE            -2
#define ERR_ELEMTYPE        -3
#define ERR_VALUE           -4
#define ERR_ELEMVALUE       -5
#define ERR_TABLE           -6
#define ERR_FUNCTION        -7
#define ERR_EMPTY           -8
#define ERR_MEMORY          -9
#define ERR_MALLOC_ZERO     -10
#define ERR_LENGTH          -11
#define ERR_POOL            -12
#define ERR_BOUNDARIES      -13
#define ERR_RANGE           -14
#define ERR_FOPEN           -15
#define ERR_OPERATION       -16
#define ERR_UNKNOWN         -17
#define errstring moonsdl2_errstring
const char* errstring(int err);

/* event.c */
#define TIMEREVENT moonsdl2_TIMEREVENT
extern uint32_t TIMEREVENT;
int moonsdl2_reserve_event_codes(lua_State *L);

/* mixer.c */
int moonsdl2_close_audio(lua_State *L);

/* tracing.c */
#define trace_objects moonsdl2_trace_objects
extern int trace_objects;

#define raiseerror moonsdl2_raiseerror
int raiseerror(lua_State *L, const int ec);
#define pusherror moonsdl2_pusherror
int pusherror(lua_State *L, const int ec);

/* timer.c */
#define timerevent moonsdl2_timerevent
int timerevent(lua_State *L, const SDL_Event *event);

/* keyboard.c */
#define pushkeymod moonsdl2_pushkeymod
void pushkeymod(lua_State *L, uint16_t mod);

/* mouse.c */
#define pushmousebuttonstate moonsdl2_pushmousebuttonstate
void pushmousebuttonstate(lua_State *L, uint32_t state);

/* joystick.c */
#define checkjoystickguid moonsdl2_checkjoystickguid
int checkjoystickguid(lua_State *L, int arg, SDL_JoystickGUID *dst);
#define pushjoystickguid moonsdl2_pushjoystickguid
int pushjoystickguid(lua_State *L, SDL_JoystickGUID *guid);

/* display.c */
#define checkdisplaymode moonsdl2_checkdisplaymode
int checkdisplaymode(lua_State *L, int arg, SDL_DisplayMode *dst);
#define pushdisplaymode moonsdl2_pushdisplaymode
int pushdisplaymode(lua_State *L, SDL_DisplayMode *mode);

/* sensor.c */
#define checksensortypeorint moonsdl2_checksensortypeorint
SDL_SensorType checksensortypeorint(lua_State *L, int arg);
#define pushsensortypeorint moonsdl2_pushsensortypeorint
void pushsensortypeorint(lua_State *L, SDL_SensorType type);

/* main.c */
extern lua_State *moonsdl2_L;
extern int moonsdl2_audio_is_open;
int luaopen_moonsdl2(lua_State *L);
void moonsdl2_open_tracing(lua_State *L);
void moonsdl2_open_flags(lua_State *L);
void moonsdl2_open_enums(lua_State *L);
void moonsdl2_open_log(lua_State *L);
void moonsdl2_open_hints(lua_State *L);
void moonsdl2_open_window(lua_State *L);
void moonsdl2_open_surface(lua_State *L);
void moonsdl2_open_pxlfmt(lua_State *L);
void moonsdl2_open_palette(lua_State *L);
void moonsdl2_open_events(lua_State *L);
void moonsdl2_open_renderer(lua_State *L);
void moonsdl2_open_texture(lua_State *L);
void moonsdl2_open_keyboard(lua_State *L);
void moonsdl2_open_mouse(lua_State *L);
void moonsdl2_open_cursor(lua_State *L);
void moonsdl2_open_joystick(lua_State *L);
void moonsdl2_open_gamecontroller(lua_State *L);
void moonsdl2_open_animation(lua_State *L);
void moonsdl2_open_font(lua_State *L);
void moonsdl2_open_chunk(lua_State *L);
void moonsdl2_open_music(lua_State *L);
void moonsdl2_open_misc(lua_State *L);
void moonsdl2_open_timer(lua_State *L);
void moonsdl2_open_glcontext(lua_State *L);
void moonsdl2_open_display(lua_State *L);
void moonsdl2_open_mixer(lua_State *L);
void moonsdl2_open_sensor(lua_State *L);
void moonsdl2_open_haptic(lua_State *L);
void moonsdl2_open_touch(lua_State *L);
void moonsdl2_open_messagebox(lua_State *L);

/*------------------------------------------------------------------------------*
 | Debug and other utilities                                                    |
 *------------------------------------------------------------------------------*/

/* If this is printed, it denotes a suspect bug: */
#define UNEXPECTED_ERROR "unexpected error (%s, %d)", __FILE__, __LINE__
#define unexpected(L) luaL_error((L), UNEXPECTED_ERROR)

/* Errors with internal error code (ERR_XXX) */
#define failure(L, errcode) luaL_error((L), errstring((errcode)))
#define argerror(L, arg, errcode) luaL_argerror((L), (arg), errstring((errcode)))
#define errmemory(L) luaL_error((L), errstring((ERR_MEMORY)))

#define notsupported(L) luaL_error((L), "operation not supported")
#define badvalue(L, s)   lua_pushfstring((L), "invalid value '%s'", (s))

#define GetError(L)     do { lua_pushstring(L, SDL_GetError()); return lua_error(L); } while(0)
#define CheckError(L, ec)   \
    do { if((ec)!=0) { lua_pushstring(L, SDL_GetError()); return lua_error(L); } } while(0)

/* Reference/unreference variables on the Lua registry */
#define Unreference(L, ref) do {                        \
    if((ref)!= LUA_NOREF)                               \
        {                                               \
        luaL_unref((L), LUA_REGISTRYINDEX, (ref));      \
        (ref) = LUA_NOREF;                              \
        }                                               \
} while(0)

#define Reference(L, arg, ref)  do {                    \
    Unreference((L), (ref));                            \
    lua_pushvalue(L, (arg));                            \
    (ref) = luaL_ref(L, LUA_REGISTRYINDEX);             \
} while(0)

/* DEBUG -------------------------------------------------------- */
#if defined(DEBUG)

#define DBG printf
#define TR() do { printf("trace %s %d\n",__FILE__,__LINE__); } while(0)
#define BK() do { printf("break %s %d\n",__FILE__,__LINE__); getchar(); } while(0)
#define TSTART double ts = now();
#define TSTOP do {                                          \
    ts = since(ts); ts = ts*1e6;                            \
    printf("%s %d %.3f us\n", __FILE__, __LINE__, ts);      \
    ts = now();                                             \
} while(0);

#else 

#define DBG noprintf
#define TR()
#define BK()
#define TSTART do {} while(0) 
#define TSTOP do {} while(0)    

#endif /* DEBUG ------------------------------------------------- */

#endif /* internalDEFINED */
