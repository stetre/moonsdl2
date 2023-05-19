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

#ifndef objectsDEFINED
#define objectsDEFINED

#include "tree.h"
#include "udata.h"

/* host accessible memory: */
typedef struct {
    char *ptr; 
    size_t size;
} hostmem_t;

typedef struct moonsdl2_timer_s {
    int ref;
    SDL_TimerID id;
} moonsdl2_timer_t;

typedef struct moonsdl2_hintcb_s {
    int ref;
    char *name;
} moonsdl2_hintcb_t;

/* renaming (for internal use) */
#define bool_t SDL_bool
#define color_t SDL_Color
#define point_t SDL_Point
#define fpoint_t SDL_FPoint
#define rect_t SDL_Rect
#define frect_t SDL_FRect
#define vertex_t SDL_Vertex
#define window_t SDL_Window
#define surface_t SDL_Surface
#define pxlfmt_t SDL_PixelFormat
#define palette_t SDL_Palette
#define renderer_t SDL_Renderer
#define texture_t SDL_Texture
#define font_t TTF_Font
#define chunk_t Mix_Chunk
#define music_t Mix_Music
#define timer_t moonsdl2_timer_t
#define glcontext_t void /* SDL_GLContext is a void* */
#define joystick_t SDL_Joystick
#define gamecontroller_t SDL_GameController
#define cursor_t SDL_Cursor
#define sensor_t SDL_Sensor
#define animation_t IMG_Animation
#define hintcb_t moonsdl2_hintcb_t
#define haptic_t SDL_Haptic
#define messagebox_t SDL_MessageBoxData

/* Objects' metatable names */
#define HOSTMEM_MT "moonfftw_hostmem"
#define WINDOW_MT "moonsdl2_window"
#define SURFACE_MT "moonsdl2_surface"
#define PXLFMT_MT "moonsdl2_pxlfmt"
#define PALETTE_MT "moonsdl2_palette"
#define RENDERER_MT "moonsdl2_renderer"
#define TEXTURE_MT "moonsdl2_texture"
#define FONT_MT "moonsdl2_font"
#define CHUNK_MT "moonsdl2_chunk"
#define MUSIC_MT "moonsdl2_music"
#define TIMER_MT "moonsdl2_timer"
#define GLCONTEXT_MT "moonsdl2_glcontext"
#define JOYSTICK_MT "moonsdl2_joystick"
#define GAMECONTROLLER_MT "moonsdl2_gamecontroller"
#define CURSOR_MT "moonsdl2_cursor"
#define SENSOR_MT "moonsdl2_sensor"
#define ANIMATION_MT "moonsdl2_animation"
#define HINTCB_MT "moonsdl2_hintcb"
#define HAPTIC_MT "moonsdl2_haptic"
#define MESSAGEBOX_MT "moonsdl2_messagebox"


/* Userdata memory associated with objects */
#define ud_t moonsdl2_ud_t
typedef struct moonsdl2_ud_s ud_t;

struct moonsdl2_ud_s {
    void *handle; /* the object handle bound to this userdata */
    int (*destructor)(lua_State *L, ud_t *ud);  /* self destructor */
    ud_t *parent_ud; /* the ud of the parent object */
    uint32_t marks;
    int ref1, ref2, ref3, ref4; /* references for callbacks, automatically unreference at deletion */
    int groupid;
    void *info; /* object specific info (ud_info_t, subject to Free() at destruction, if not NULL) */
};
    
/* Marks.  m_ = marks word (uint32_t) , i_ = bit number (0 .. 31)  */
#define MarkGet(m_,i_)  (((m_) & ((uint32_t)1<<(i_))) == ((uint32_t)1<<(i_)))
#define MarkSet(m_,i_)  do { (m_) = ((m_) | ((uint32_t)1<<(i_))); } while(0)
#define MarkReset(m_,i_) do { (m_) = ((m_) & (~((uint32_t)1<<(i_)))); } while(0)

#define IsValid(ud)             MarkGet((ud)->marks, 0)
#define MarkValid(ud)           MarkSet((ud)->marks, 0) 
#define CancelValid(ud)         MarkReset((ud)->marks, 0)

#define IsBorrowed(ud)          MarkGet((ud)->marks, 1)
#define MarkBorrowed(ud)        MarkSet((ud)->marks, 1) 
#define CancelBorrowed(ud)      MarkReset((ud)->marks, 1)

#define IsRemoved(ud)          MarkGet((ud)->marks, 2)
#define MarkRemoved(ud)        MarkSet((ud)->marks, 2) 
#define CancelRemoved(ud)      MarkReset((ud)->marks, 2)

#if 0
/* .c */
#define  moonsdl2_
#endif

#define setmetatable moonsdl2_setmetatable
int setmetatable(lua_State *L, const char *mt);

#define newuserdata moonsdl2_newuserdata
ud_t *newuserdata(lua_State *L, void *handle, const char *mt, const char *tracename);
#define freeuserdata moonsdl2_freeuserdata
int freeuserdata(lua_State *L, ud_t *ud, const char *tracename);
#define pushuserdata moonsdl2_pushuserdata 
int pushuserdata(lua_State *L, ud_t *ud);

#define freechildren moonsdl2_freechildren
int freechildren(lua_State *L,  const char *mt, ud_t *parent_ud);

#define userdata_unref(L, handle) udata_unref((L),(handle))

#define UD(handle) userdata((handle)) /* dispatchable objects only */
#define userdata moonsdl2_userdata
ud_t *userdata(void *handle);
#define testxxx moonsdl2_testxxx
void *testxxx(lua_State *L, int arg, ud_t **udp, const char *mt);
#define checkxxx moonsdl2_checkxxx
void *checkxxx(lua_State *L, int arg, ud_t **udp, const char *mt);
#define optxxx moonsdl2_optxxx
void *optxxx(lua_State *L, int arg, ud_t **udp, const char *mt);
#define pushxxx moonsdl2_pushxxx
int pushxxx(lua_State *L, void *handle);
#if 0
#define freexxxlist moonsdl2_freexxxlist
void freexxxlist(lua_State *L, vector_ptr_t *v);
#define checkxxxlist moonsdl2_checkxxxlist
vector_ptr_t* checkxxxlist(lua_State *L, int arg, int *err, const char *mt);
#endif

/* hostmem.c */
#define checkhostmem(L, arg, udp) (hostmem_t*)checkxxx((L), (arg), (udp), HOSTMEM_MT)
#define testhostmem(L, arg, udp) (hostmem_t*)testxxx((L), (arg), (udp), HOSTMEM_MT)
#define opthostmem(L, arg, udp) (hostmem_t*)optxxx((L), (arg), (udp), HOSTMEM_MT)
#define pushhostmem(L, handle) pushxxx((L), (void*)(handle))
#define checkhostmemlist(L, arg, count, err) checkxxxlist((L), (arg), (count), (err), HOSTMEM_MT)

/* window.c */
#define checkwindow(L, arg, udp) (window_t*)checkxxx((L), (arg), (udp), WINDOW_MT)
#define testwindow(L, arg, udp) (window_t*)testxxx((L), (arg), (udp), WINDOW_MT)
#define optwindow(L, arg, udp) (window_t*)optxxx((L), (arg), (udp), WINDOW_MT)
#define freewindowlist freexxxlist
#define checkwindowlist(L, arg, err) checkxxxlist((L), (arg), (err), WINDOW_MT)
#define pushwindow(L, handle) pushxxx((L), (void*)(handle))

/* surface.c */
#define newsurface moonsdl2_newsurface
int newsurface(lua_State *L, surface_t *surface, ud_t *parent_ud);
#define checksurface(L, arg, udp) (surface_t*)checkxxx((L), (arg), (udp), SURFACE_MT)
#define testsurface(L, arg, udp) (surface_t*)testxxx((L), (arg), (udp), SURFACE_MT)
#define optsurface(L, arg, udp) (surface_t*)optxxx((L), (arg), (udp), SURFACE_MT)
#define freesurfacelist freexxxlist
#define checksurfacelist(L, arg, err) checkxxxlist((L), (arg), (err), SURFACE_MT)
#define pushsurface(L, handle) pushxxx((L), (void*)(handle))

/* pxlfmt.c */
#define newpxlfmt moonsdl2_newpxlfmt
int newpxlfmt(lua_State *L, pxlfmt_t *pxlfmt, ud_t *parent_ud);
#define checkpxlfmt(L, arg, udp) (pxlfmt_t*)checkxxx((L), (arg), (udp), PXLFMT_MT)
#define testpxlfmt(L, arg, udp) (pxlfmt_t*)testxxx((L), (arg), (udp), PXLFMT_MT)
#define optpxlfmt(L, arg, udp) (pxlfmt_t*)optxxx((L), (arg), (udp), PXLFMT_MT)
#define freepxlfmtlist freexxxlist
#define checkpxlfmtlist(L, arg, err) checkxxxlist((L), (arg), (err), PXLFMT_MT)
#define pushpxlfmt(L, handle) pushxxx((L), (void*)(handle))

/* palette.c */
#define checkpalette(L, arg, udp) (palette_t*)checkxxx((L), (arg), (udp), PALETTE_MT)
#define testpalette(L, arg, udp) (palette_t*)testxxx((L), (arg), (udp), PALETTE_MT)
#define optpalette(L, arg, udp) (palette_t*)optxxx((L), (arg), (udp), PALETTE_MT)
#define freepalettelist freexxxlist
#define checkpalettelist(L, arg, err) checkxxxlist((L), (arg), (err), PALETTE_MT)
#define pushpalette(L, handle) pushxxx((L), (void*)(handle))

/* renderer.c */
#define checkrenderer(L, arg, udp) (renderer_t*)checkxxx((L), (arg), (udp), RENDERER_MT)
#define testrenderer(L, arg, udp) (renderer_t*)testxxx((L), (arg), (udp), RENDERER_MT)
#define optrenderer(L, arg, udp) (renderer_t*)optxxx((L), (arg), (udp), RENDERER_MT)
#define freerendererlist freexxxlist
#define checkrendererlist(L, arg, err) checkxxxlist((L), (arg), (err), RENDERER_MT)
#define pushrenderer(L, handle) pushxxx((L), (void*)(handle))

/* texture.c */
#define newtexture moonsdl2_newtexture
int newtexture(lua_State *L, texture_t *texture, ud_t *parent_ud);
#define checktexture(L, arg, udp) (texture_t*)checkxxx((L), (arg), (udp), TEXTURE_MT)
#define testtexture(L, arg, udp) (texture_t*)testxxx((L), (arg), (udp), TEXTURE_MT)
#define opttexture(L, arg, udp) (texture_t*)optxxx((L), (arg), (udp), TEXTURE_MT)
#define freetexturelist freexxxlist
#define checktexturelist(L, arg, err) checkxxxlist((L), (arg), (err), TEXTURE_MT)
#define pushtexture(L, handle) pushxxx((L), (void*)(handle))

/* font.c */
#define checkfont(L, arg, udp) (font_t*)checkxxx((L), (arg), (udp), FONT_MT)
#define testfont(L, arg, udp) (font_t*)testxxx((L), (arg), (udp), FONT_MT)
#define optfont(L, arg, udp) (font_t*)optxxx((L), (arg), (udp), FONT_MT)
#define freefontlist freexxxlist
#define checkfontlist(L, arg, err) checkxxxlist((L), (arg), (err), FONT_MT)
#define pushfont(L, handle) pushxxx((L), (void*)(handle))

/* chunk.c */
#define checkchunk(L, arg, udp) (chunk_t*)checkxxx((L), (arg), (udp), CHUNK_MT)
#define testchunk(L, arg, udp) (chunk_t*)testxxx((L), (arg), (udp), CHUNK_MT)
#define optchunk(L, arg, udp) (chunk_t*)optxxx((L), (arg), (udp), CHUNK_MT)
#define freechunklist freexxxlist
#define checkchunklist(L, arg, err) checkxxxlist((L), (arg), (err), CHUNK_MT)
#define pushchunk(L, handle) pushxxx((L), (void*)(handle))

/* music.c */
#define checkmusic(L, arg, udp) (music_t*)checkxxx((L), (arg), (udp), MUSIC_MT)
#define testmusic(L, arg, udp) (music_t*)testxxx((L), (arg), (udp), MUSIC_MT)
#define optmusic(L, arg, udp) (music_t*)optxxx((L), (arg), (udp), MUSIC_MT)
#define freemusiclist freexxxlist
#define checkmusiclist(L, arg, err) checkxxxlist((L), (arg), (err), MUSIC_MT)
#define pushmusic(L, handle) pushxxx((L), (void*)(handle))

/* timer.c */
#define checktimer(L, arg, udp) (timer_t*)checkxxx((L), (arg), (udp), TIMER_MT)
#define testtimer(L, arg, udp) (timer_t*)testxxx((L), (arg), (udp), TIMER_MT)
#define opttimer(L, arg, udp) (timer_t*)optxxx((L), (arg), (udp), TIMER_MT)
#define freetimerlist freexxxlist
#define checktimerlist(L, arg, err) checkxxxlist((L), (arg), (err), TIMER_MT)
#define pushtimer(L, handle) pushxxx((L), (void*)(handle))

/* glcontext.c */
#define checkglcontext(L, arg, udp) (glcontext_t*)checkxxx((L), (arg), (udp), GLCONTEXT_MT)
#define testglcontext(L, arg, udp) (glcontext_t*)testxxx((L), (arg), (udp), GLCONTEXT_MT)
#define optglcontext(L, arg, udp) (glcontext_t*)optxxx((L), (arg), (udp), GLCONTEXT_MT)
#define freeglcontextlist freexxxlist
#define checkglcontextlist(L, arg, err) checkxxxlist((L), (arg), (err), GLCONTEXT_MT)
#define pushglcontext(L, handle) pushxxx((L), (void*)(handle))

/* joystick.c */
#define newjoystick moonsdl2_newjoystick
int newjoystick(lua_State *L, joystick_t *joystick, ud_t *parent_ud);
#define checkjoystick(L, arg, udp) (joystick_t*)checkxxx((L), (arg), (udp), JOYSTICK_MT)
#define testjoystick(L, arg, udp) (joystick_t*)testxxx((L), (arg), (udp), JOYSTICK_MT)
#define optjoystick(L, arg, udp) (joystick_t*)optxxx((L), (arg), (udp), JOYSTICK_MT)
#define freejoysticklist freexxxlist
#define checkjoysticklist(L, arg, err) checkxxxlist((L), (arg), (err), JOYSTICK_MT)
#define pushjoystick(L, handle) pushxxx((L), (void*)(handle))

/* gamecontroller.c */
#define checkgamecontroller(L, arg, udp) (gamecontroller_t*)checkxxx((L), (arg), (udp), GAMECONTROLLER_MT)
#define testgamecontroller(L, arg, udp) (gamecontroller_t*)testxxx((L), (arg), (udp), GAMECONTROLLER_MT)
#define optgamecontroller(L, arg, udp) (gamecontroller_t*)optxxx((L), (arg), (udp), GAMECONTROLLER_MT)
#define freegamecontrollerlist freexxxlist
#define checkgamecontrollerlist(L, arg, err) checkxxxlist((L), (arg), (err), GAMECONTROLLER_MT)
#define pushgamecontroller(L, handle) pushxxx((L), (void*)(handle))

/* cursor.c */
#define checkcursor(L, arg, udp) (cursor_t*)checkxxx((L), (arg), (udp), CURSOR_MT)
#define testcursor(L, arg, udp) (cursor_t*)testxxx((L), (arg), (udp), CURSOR_MT)
#define optcursor(L, arg, udp) (cursor_t*)optxxx((L), (arg), (udp), CURSOR_MT)
#define freecursorlist freexxxlist
#define checkcursorlist(L, arg, err) checkxxxlist((L), (arg), (err), CURSOR_MT)
#define pushcursor(L, handle) pushxxx((L), (void*)(handle))

/* sensor.c */
#define checksensor(L, arg, udp) (sensor_t*)checkxxx((L), (arg), (udp), SENSOR_MT)
#define testsensor(L, arg, udp) (sensor_t*)testxxx((L), (arg), (udp), SENSOR_MT)
#define optsensor(L, arg, udp) (sensor_t*)optxxx((L), (arg), (udp), SENSOR_MT)
#define freesensorlist freexxxlist
#define checksensorlist(L, arg, err) checkxxxlist((L), (arg), (err), SENSOR_MT)
#define pushsensor(L, handle) pushxxx((L), (void*)(handle))

/* animation.c */
#define checkanimation(L, arg, udp) (animation_t*)checkxxx((L), (arg), (udp), ANIMATION_MT)
#define testanimation(L, arg, udp) (animation_t*)testxxx((L), (arg), (udp), ANIMATION_MT)
#define optanimation(L, arg, udp) (animation_t*)optxxx((L), (arg), (udp), ANIMATION_MT)
#define freeanimationlist freexxxlist
#define checkanimationlist(L, arg, err) checkxxxlist((L), (arg), (err), ANIMATION_MT)
#define pushanimation(L, handle) pushxxx((L), (void*)(handle))

/* hints.c */
#define checkhintcb(L, arg, udp) (hintcb_t*)checkxxx((L), (arg), (udp), HINTCB_MT)
#define testhintcb(L, arg, udp) (hintcb_t*)testxxx((L), (arg), (udp), HINTCB_MT)
#define opthintcb(L, arg, udp) (hintcb_t*)optxxx((L), (arg), (udp), HINTCB_MT)
#define freehintcblist freexxxlist
#define checkhintcblist(L, arg, err) checkxxxlist((L), (arg), (err), HINTCB_MT)
#define pushhintcb(L, handle) pushxxx((L), (void*)(handle))

/* haptic.c */
#define checkhaptic(L, arg, udp) (haptic_t*)checkxxx((L), (arg), (udp), HAPTIC_MT)
#define testhaptic(L, arg, udp) (haptic_t*)testxxx((L), (arg), (udp), HAPTIC_MT)
#define opthaptic(L, arg, udp) (haptic_t*)optxxx((L), (arg), (udp), HAPTIC_MT)
#define freehapticlist freexxxlist
#define checkhapticlist(L, arg, err) checkxxxlist((L), (arg), (err), HAPTIC_MT)
#define pushhaptic(L, handle) pushxxx((L), (void*)(handle))

/* messagebox.c */
#define checkmessagebox(L, arg, udp) (messagebox_t*)checkxxx((L), (arg), (udp), MESSAGEBOX_MT)
#define testmessagebox(L, arg, udp) (messagebox_t*)testxxx((L), (arg), (udp), MESSAGEBOX_MT)
#define optmessagebox(L, arg, udp) (messagebox_t*)optxxx((L), (arg), (udp), MESSAGEBOX_MT)
#define freemessageboxlist freexxxlist
#define checkmessageboxlist(L, arg, err) checkxxxlist((L), (arg), (err), MESSAGEBOX_MT)
#define pushmessagebox(L, handle) pushxxx((L), (void*)(handle))


#if 0 // 8yy
/* zzz.c */
#define checkzzz(L, arg, udp) (zzz_t*)checkxxx((L), (arg), (udp), ZZZ_MT)
#define testzzz(L, arg, udp) (zzz_t*)testxxx((L), (arg), (udp), ZZZ_MT)
#define optzzz(L, arg, udp) (zzz_t*)optxxx((L), (arg), (udp), ZZZ_MT)
#define freezzzlist freexxxlist
#define checkzzzlist(L, arg, err) checkxxxlist((L), (arg), (err), ZZZ_MT)
#define pushzzz(L, handle) pushxxx((L), (void*)(handle))

#endif

#define RAW_FUNC(xxx)                       \
static int Raw(lua_State *L)                \
    {                                       \
    lua_pushinteger(L, (uintptr_t)check##xxx(L, 1, NULL));  \
    return 1;                               \
    }

#define TYPE_FUNC(xxx) /* NONCL */          \
static int Type(lua_State *L)               \
    {                                       \
    (void)check##xxx(L, 1, NULL);           \
    lua_pushstring(L, ""#xxx);              \
    return 1;                               \
    }

#define DESTROY_FUNC(xxx)                   \
static int Destroy(lua_State *L)            \
    {                                       \
    ud_t *ud;                               \
    (void)test##xxx(L, 1, &ud);             \
    if(!ud) return 0; /* already deleted */ \
    return ud->destructor(L, ud);           \
    }

#define PARENT_FUNC(xxx)                    \
static int Parent(lua_State *L)             \
    {                                       \
    ud_t *ud;                               \
    (void)check##xxx(L, 1, &ud);            \
    if(!ud->parent_ud) return 0;            \
    return pushuserdata(L, ud->parent_ud);  \
    }

#endif /* objectsDEFINED */
