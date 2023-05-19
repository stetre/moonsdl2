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

static int freewindow(lua_State *L, ud_t *ud)
    {
    window_t *window = (window_t*)ud->handle;
    freechildren(L, GLCONTEXT_MT, ud);
    freechildren(L, SURFACE_MT, ud);
    freechildren(L, RENDERER_MT, ud);
    if(!freeuserdata(L, ud, "window")) return 0;
    SDL_DestroyWindow(window);
    return 0;
    }

static int newwindow(lua_State *L, window_t *window)
    {
    ud_t *ud;
    ud = newuserdata(L, window, WINDOW_MT, "window");
    ud->parent_ud = NULL;
    ud->destructor = freewindow;
    return 1;
    }

static int Create(lua_State *L)
    {
    const char *title = luaL_checkstring(L, 1);
    int x = luaL_optinteger(L, 2, SDL_WINDOWPOS_UNDEFINED);
    int y = luaL_optinteger(L, 3, SDL_WINDOWPOS_UNDEFINED);
    int w = luaL_checkinteger(L, 4);
    int h = luaL_checkinteger(L, 5);
    uint32_t flags = optflags(L, 6, 0); 
    window_t *window = SDL_CreateWindow(title, x, y, w, h, flags);
    if(window==NULL) GetError(L);
    return newwindow(L, window);
    }

#if 0
static int CreateShaped(lua_State *L)
    {
    const char *title = luaL_checkstring(L, 1);
    int x = luaL_optinteger(L, 2, SDL_WINDOWPOS_UNDEFINED);
    int y = luaL_optinteger(L, 3, SDL_WINDOWPOS_UNDEFINED);
    int w = luaL_checkinteger(L, 4);
    int h = luaL_checkinteger(L, 5);
    uint32_t flags = checkflags(L, 6); 
    window_t *window = SDL_CreateShapedWindow(title, x, y, w, h, flags);
    if(window==NULL) GetError(L);
    return newwindow(L, window);
    }
#endif

static int GetGrabbedWindow(lua_State *L)
    {
    window_t *window = SDL_GetGrabbedWindow();
    if(window==NULL) return 0;
    pushwindow(L, window);
    return 1;
    }

static int GetFlags(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    pushflags(L, SDL_GetWindowFlags(window));
    return 1;
    }

static int GetID(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    uint32_t id = SDL_GetWindowID(window);
    if(id==0) GetError(L);
    lua_pushinteger(L, id);
    return 1;
    }

#if 0 // used internally (see events.c)
static int GetWindowFromID(lua_State *L)
    {
    uint32_t id = luaL_checkinteger(L, 1);
    window_t *window = SDL_GetWindowFromID(id);
    if(window==NULL) GetError(L);
    pushwindow(L, window);
    return 1;
    }
#endif

static int SetTitle(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    const char *title = luaL_checkstring(L, 2);
    SDL_SetWindowTitle(window, title);
    return 0;
    }

static int GetTitle(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    const char *title = SDL_GetWindowTitle(window);
    if(title==NULL) GetError(L);
    lua_pushstring(L, title);
    return 1;
    }

static int SetPosition(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    SDL_SetWindowPosition(window, x, y);
    return 0;
    }

static int GetPosition(lua_State *L)
    {
    int x, y;
    window_t *window = checkwindow(L, 1, NULL);
    SDL_GetWindowPosition(window, &x, &y);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 2;
    }

static int SetSize(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int w = luaL_checkinteger(L, 1);
    int h = luaL_checkinteger(L, 2);
    SDL_SetWindowSize(window, w, h);
    return 0;
    }

static int GetSize(lua_State *L)
    {
    int w, h;
    window_t *window = checkwindow(L, 1, NULL);
    SDL_GetWindowSize(window, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
    }

static int GetSizeInPixels(lua_State *L)
    {
    int w, h;
    window_t *window = checkwindow(L, 1, NULL);
    SDL_GetWindowSizeInPixels(window, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
    }

static int GetBordersSize(lua_State *L)
    {
    int top, left, bottom, right;
    window_t *window = checkwindow(L, 1, NULL);
    int ec = SDL_GetWindowBordersSize(window, &top, &left, &bottom, &right);
    CheckError(L, ec);
    lua_pushinteger(L, top);
    lua_pushinteger(L, left);
    lua_pushinteger(L, bottom);
    lua_pushinteger(L, right);
    return 4;
    }

static int SetMinimumSize(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int w = luaL_checkinteger(L, 1);
    int h = luaL_checkinteger(L, 2);
    SDL_SetWindowMinimumSize(window, w, h);
    return 0;
    }

static int GetMinimumSize(lua_State *L)
    {
    int w, h;
    window_t *window = checkwindow(L, 1, NULL);
    SDL_GetWindowMinimumSize(window, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
    }

static int SetMaximumSize(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int w = luaL_checkinteger(L, 1);
    int h = luaL_checkinteger(L, 2);
    SDL_SetWindowMaximumSize(window, w, h);
    return 0;
    }
static int GetMaximumSize(lua_State *L)
    {
    int w, h;
    window_t *window = checkwindow(L, 1, NULL);
    SDL_GetWindowMaximumSize(window, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
    }

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    window_t *window = checkwindow(L, 1, NULL);     \
    SDL_bool val = checkboolean(L, 2);              \
    func(window, val);                              \
    return 0;                                       \
    }
F(SetBordered, SDL_SetWindowBordered)
F(SetResizable, SDL_SetWindowResizable)
F(SetAlwaysOnTop, SDL_SetWindowAlwaysOnTop)
F(SetGrab, SDL_SetWindowGrab)
F(SetKeyboardGrab, SDL_SetWindowKeyboardGrab)
F(SetMouseGrab, SDL_SetWindowMouseGrab)
#undef F

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    window_t *window = checkwindow(L, 1, NULL);     \
    SDL_bool val = func(window);                    \
    lua_pushboolean(L, val);                        \
    return 1;                                       \
    }
F(GetGrab, SDL_GetWindowGrab)
F(GetKeyboardGrab, SDL_GetWindowKeyboardGrab)
F(GetMouseGrab, SDL_GetWindowMouseGrab)
#undef F


#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    window_t *window = checkwindow(L, 1, NULL);     \
    func(window);                                   \
    return 0;                                       \
    }
F(Show, SDL_ShowWindow)
F(Hide, SDL_HideWindow)
F(Raise, SDL_RaiseWindow)
F(Maximize, SDL_MaximizeWindow)
F(Minimize, SDL_MinimizeWindow)
F(Restore, SDL_RestoreWindow)
#undef F

static int SetFullscreen(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    uint32_t mode = checkfullscreenmode(L, 2);
    int ec = SDL_SetWindowFullscreen(window, mode);
    CheckError(L, ec);
    return 0;
    }

static int GetSurface(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    surface_t *surface = SDL_GetWindowSurface(window);
    if(window==NULL) GetError(L);
    if(UD(surface))
        pushsurface(L, surface);
    else
        newsurface(L, surface, UD(window));
    return 1;
    }

static int SetIcon(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    surface_t *icon = checksurface(L, 2, NULL);
    SDL_SetWindowIcon(window, icon);
    return 0;
    }

static int GetPixelFormat(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    uint32_t val = SDL_GetWindowPixelFormat(window);
    pushpixelformat(L, val);
    return 1;
    }

static int GetRenderer(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    renderer_t *renderer = SDL_GetRenderer(window);
    if(!renderer) return 0;
    pushrenderer(L, renderer);
    return 1;
    }

static int GetDisplayIndex(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int index = SDL_GetWindowDisplayIndex(window);
    if(index < 0) GetError(L);
    pushindex(L, index);
    return 1;
    }

static int SetDisplayMode(lua_State *L)
    {
    int ec;
    SDL_DisplayMode *mode = NULL;
    window_t *window = checkwindow(L, 1, NULL);
    if(!lua_isnoneornil(L, 2))
        checkdisplaymode(L, 2, mode);
    ec = SDL_SetWindowDisplayMode(window, mode);
    CheckError(L, ec);
    return 0;
    }

static int GetDisplayMode(lua_State *L)
    {
    SDL_DisplayMode mode;
    window_t *window = checkwindow(L, 1, NULL);
    int ec = SDL_GetWindowDisplayMode(window, &mode);
    CheckError(L, ec);
    pushdisplaymode(L, &mode);
    return 1;
    }

static int UpdateSurface(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int ec = SDL_UpdateWindowSurface(window);
    CheckError(L, ec);
    return 0;
    }

static int UpdateSurfaceRects(lua_State *L)
    {
    int count;
    window_t *window = checkwindow(L, 1, NULL);
    rect_t *rects = checkrectlist(L, 2, &count, NULL);
    int ec = SDL_UpdateWindowSurfaceRects(window, rects, count);
    Free(L, rects);
    CheckError(L, ec);
    return 0;
    }

static int SetMouseRect(lua_State *L)
    {
    rect_t rect;
    window_t *window = checkwindow(L, 1, NULL);
    int ec = optrect(L, 2, &rect);
    ec = SDL_SetWindowMouseRect(window, (ec==ERR_NOTPRESENT)? NULL: &rect);
    CheckError(L, ec);
    return 0;
    }

static int GetMouseRect(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    const rect_t *rect = SDL_GetWindowMouseRect(window);
    if(!rect) return 0;
    pushrect(L, rect);
    return 1;
    }

static int SetBrightness(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    float value = luaL_checknumber(L, 2);
    int ec = SDL_SetWindowBrightness(window, value);
    CheckError(L, ec);
    return 0;
    }

static int GetBrightness(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    float value = SDL_GetWindowBrightness(window);
    lua_pushnumber(L, value);
    return 1;
    }

static int SetOpacity(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    float value = luaL_checknumber(L, 2);
    int ec = SDL_SetWindowOpacity(window, value);
    CheckError(L, ec);
    return 0;
    }

static int GetOpacity(lua_State *L)
    {
    float  value;
    window_t *window = checkwindow(L, 1, NULL);
    int ec = SDL_GetWindowOpacity(window, &value);
    CheckError(L, ec);
    lua_pushnumber(L, value);
    return 1;
    }

static int SetModalFor(lua_State *L)
    {
    window_t *modal_window = checkwindow(L, 1, NULL);
    window_t *parent_window = checkwindow(L, 2, NULL);
    if(SDL_SetWindowModalFor(modal_window, parent_window)!=0) GetError(L);
    return 0;
    }

static int SetInputFocus(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int ec = SDL_SetWindowInputFocus(window);
    CheckError(L, ec);
    return 0;
    }

static int SetGammaRamp(lua_State *L)
    {
    int err, count;
    uint16_t *r = NULL;
    uint16_t *g = NULL;
    uint16_t *b = NULL;
    window_t *window = checkwindow(L, 1, NULL);
#define Cleanup() do { Free(L, r); Free(L, g); Free(L, b); } while(0)
    r = checkuint16list(L, 2, &count, &err);
    if(err < 0) { Cleanup(); return argerror(L, 2, err); }
    if(count != 256) { Cleanup(); return argerror(L, 2, ERR_LENGTH); }
    g = checkuint16list(L, 3, &count, &err);
    if(err < 0) { Cleanup(); return argerror(L, 3, err); }
    if(count != 256) { Cleanup(); return argerror(L, 3, ERR_LENGTH); }
    b = checkuint16list(L, 4, &count, &err);
    if(err < 0) { Cleanup(); return argerror(L, 4, err); }
    if(count != 256) { Cleanup(); return argerror(L, 4, ERR_LENGTH); }
    err = SDL_SetWindowGammaRamp(window, r, g, b);
    Cleanup();
    if(err!=0) GetError(L);
    return 0;
#undef Cleanup
    }

static int GetGammaRamp(lua_State *L)
    {
    uint16_t r[256], g[256], b[256];
    window_t *window = checkwindow(L, 1, NULL);
    int ec = SDL_GetWindowGammaRamp(window, r, g, b);
    CheckError(L, ec);
    pushuint16list(L, r, 256);
    pushuint16list(L, g, 256);
    pushuint16list(L, b, 256);
    return 3;
    }

static SDL_HitTestResult HitTestCallback(SDL_Window *window, const SDL_Point *area, void *data)
    {
    SDL_HitTestResult result;
    ud_t *ud = UD(window);
    lua_State *L = moonsdl2_L;
    int top = lua_gettop(L);
    (void)data;
    if(!ud) return unexpected(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ud->ref1);
    pushwindow(L, window);
    pushpoint(L, area);
    if(lua_pcall(L, 2, 1, 0)!=LUA_OK) lua_error(L);
    result = checkhittestresult(L, -1);
    lua_settop(L, top);
    return result;
    }

static int SetHitTest(lua_State *L)
    {
    ud_t *ud;
    window_t *window = checkwindow(L, 1, &ud);
    if(lua_isnoneornil(L, 2))
        {
        SDL_SetWindowHitTest(window, NULL, NULL);
        Unreference(L, ud->ref1);
        return 0;
        }
    if(!lua_isfunction(L, 2))
        return argerror(L, 2, ERR_FUNCTION);
    Reference(L, 2, ud->ref1);
    if(SDL_SetWindowHitTest(window, HitTestCallback, NULL)!=0) 
        {
        Unreference(L, ud->ref1);
        GetError(L);
        }
    return 0;
    }

static int Flash(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    SDL_FlashOperation operation = checkflashoperation(L, 2);
    int ec = SDL_FlashWindow(window, operation);
    CheckError(L, ec);
    return 0;
    }

#if 0
static int IsShaped(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    lua_pushboolean(L, SDL_IsShapedWindow(window));
    return 1;
    }

//@@int SDL_SetWindowShape(SDL_Window *window,SDL_Surface *shape,SDL_WindowShapeMode *shape_mode);
static int SetShape(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    (void)window;
    return 0;
    }

//@@int SDL_GetShapedWindowMode(SDL_Window *window,SDL_WindowShapeMode *shape_mode);
static int GetShapedMode(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    (void)window;
    return 0;
    }
#endif

#if 0 // 8yy
        { "",  },
static int (lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    (void)window;
    return 0;
    }

F(, )
        { "",  },
#endif

DESTROY_FUNC(window)

static const struct luaL_Reg Methods[] = 
    {
        { "destroy", Destroy },
        { "get_flags", GetFlags },
        { "get_id", GetID },
        { "set_title", SetTitle },
        { "get_title", GetTitle },
        { "set_position", SetPosition },
        { "get_position", GetPosition },
        { "set_size", SetSize },
        { "get_size", GetSize },
        { "get_size_in_pixels", GetSizeInPixels },
        { "get_borders_size", GetBordersSize },
        { "set_minimum_size", SetMinimumSize },
        { "get_minimum_size", GetMinimumSize },
        { "set_maximum_size", SetMaximumSize },
        { "get_maximum_size", GetMaximumSize },
        { "set_bordered", SetBordered },
        { "set_resizable", SetResizable },
        { "set_always_on_top", SetAlwaysOnTop },
        { "set_grab", SetGrab },
        { "set_keyboard_grab", SetKeyboardGrab },
        { "set_mouse_grab", SetMouseGrab },
        { "get_grab", GetGrab },
        { "get_keyboard_grab", GetKeyboardGrab },
        { "get_mouse_grab", GetMouseGrab },
        { "show", Show },
        { "hide", Hide },
        { "raise", Raise },
        { "maximize", Maximize },
        { "minimize", Minimize },
        { "restore", Restore },
        { "set_fullscreen", SetFullscreen },
        { "get_surface", GetSurface },
        { "set_icon", SetIcon },
        { "get_format", GetPixelFormat },
        { "get_renderer", GetRenderer },
        { "get_display_index", GetDisplayIndex },
        { "set_display_mode", SetDisplayMode },
        { "get_display_mode", GetDisplayMode },
        { "update_surface", UpdateSurface },
        { "update_surface_rects", UpdateSurfaceRects },
        { "set_mouse_rect", SetMouseRect },
        { "get_mouse_rect", GetMouseRect },
        { "set_brightness", SetBrightness },
        { "get_brightness", GetBrightness },
        { "set_opacity", SetOpacity },
        { "get_opacity", GetOpacity },
        { "set_modal_for", SetModalFor },
        { "set_input_focus", SetInputFocus },
        { "set_gamma_ramp", SetGammaRamp },
        { "get_gamma_ramp", GetGammaRamp },
        { "set_hit_test", SetHitTest },
        { "flash", Flash },
//      { "is_shaped", IsShaped },
//      { "set_shape", SetShape },
//      { "get_shaped_mode", GetShapedMode },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_window", Create },
//      { "create_shaped_window", CreateShaped },
//      { "get_window_from_id", GetWindowFromID },
        { "get_grabbed_window", GetGrabbedWindow },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_window(lua_State *L)
    {
    udata_define(L, WINDOW_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);

    /* Add constants */
#define ADD(what)   do { lua_pushinteger(L, SDL_##what); lua_setfield(L, -2, #what); } while(0)
    ADD(WINDOWPOS_UNDEFINED_MASK);
    ADD(WINDOWPOS_UNDEFINED);
    ADD(WINDOWPOS_CENTERED_MASK);
    ADD(WINDOWPOS_CENTERED);
#undef ADD
    }

#if 0
//SDL_Window * SDL_CreateWindowFrom(const void *data);
//void* SDL_SetWindowData(SDL_Window * window, const char *name, void *userdata);
//void *SDL_GetWindowData(SDL_Window * window, const char *name);
//void* SDL_GetWindowICCProfile(SDL_Window * window, size_t* size);

#define SDL_NONSHAPEABLE_WINDOW -1
#define SDL_INVALID_SHAPE_ARGUMENT -2
#define SDL_WINDOW_LACKS_SHAPE -3

typedef enum {
    ShapeModeDefault,
    ShapeModeBinarizeAlpha,
    ShapeModeReverseBinarizeAlpha,
    ShapeModeColorKey
} WindowShapeMode;

#define SDL_SHAPEMODEALPHA(mode) (mode == ShapeModeDefault || mode == ShapeModeBinarizeAlpha || mode == ShapeModeReverseBinarizeAlpha)

typedef union {
    Uint8 binarizationCutoff;
    SDL_Color colorKey;
} SDL_WindowShapeParams;

typedef struct SDL_WindowShapeMode {
    WindowShapeMode mode;
    SDL_WindowShapeParams parameters;
} SDL_WindowShapeMode;
#endif
