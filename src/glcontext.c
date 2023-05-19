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

static int freeglcontext(lua_State *L, ud_t *ud)
    {
    glcontext_t *glcontext = (glcontext_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "glcontext")) return 0;
    SDL_GL_DeleteContext(glcontext);
    return 0;
    }

static int newglcontext(lua_State *L, glcontext_t *glcontext, ud_t *parent_ud)
    {
    ud_t *ud;
    ud = newuserdata(L, glcontext, GLCONTEXT_MT, "glcontext");
    ud->parent_ud = parent_ud;
    ud->destructor = freeglcontext;
    return 1;
    }

static int Create(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, 0);
    glcontext_t *glcontext = SDL_GL_CreateContext(window);
    if(glcontext==NULL) GetError(L);
    return newglcontext(L, glcontext, UD(window));
    }

static int GL_MakeCurrent(lua_State *L)
    {
    glcontext_t *glcontext = checkglcontext(L, 1, NULL);
    window_t *window = optwindow(L, 2, NULL);
    int ec = SDL_GL_MakeCurrent(window, glcontext);
    CheckError(L, ec);
    return 0;
    }

static int GL_ExtensionSupported(lua_State *L)
    {
    const char *extension = luaL_checkstring(L, 1);
    lua_pushboolean(L, SDL_GL_ExtensionSupported(extension));
    return 1;
    }

static int GL_GetCurrentWindow(lua_State *L)
    {
    window_t *window = SDL_GL_GetCurrentWindow();
    if(window==NULL) return 0;
    pushwindow(L, window);
    return 1;
    }

static int GL_GetDrawableSize(lua_State *L)
    {
    int w, h;
    window_t *window = checkwindow(L, 1, NULL);
    SDL_GL_GetDrawableSize(window, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
    }

static int GL_SwapWindow(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    SDL_GL_SwapWindow(window);
    return 0;
    }

static int GL_GetCurrentContext(lua_State *L)
    {
    glcontext_t *glcontext = SDL_GL_GetCurrentContext();
    if(glcontext==NULL) return 0;
    pushglcontext(L, glcontext);
    return 1;
    }

static int GL_SetSwapInterval(lua_State *L)
    {
    int interval = checkswapinterval(L, 1);
    int ec = SDL_GL_SetSwapInterval(interval);
    CheckError(L, ec);
    return 0;
    }

static int GL_GetSwapInterval(lua_State *L)
    {
    int interval = SDL_GL_GetSwapInterval();
    pushswapinterval(L, interval);
    return 1;
    }

static int GL_ResetAttributes(lua_State *L)
    {
    (void)L;
    SDL_GL_ResetAttributes();
    return 0;
    }

static int GL_SetAttribute(lua_State *L)
    {
    SDL_GLattr attr = checkglattr(L, 1);
    int value = luaL_checkinteger(L, 2);
    if(SDL_GL_SetAttribute(attr, value)!=0) GetError(L);
    return 0;
    }

static int GL_GetAttribute(lua_State *L)
    {
    int value;
    SDL_GLattr attr = checkglattr(L, 1);
    if(SDL_GL_GetAttribute(attr, &value)!=0) GetError(L);
    lua_pushinteger(L, value);
    return 1;
    }

static int GL_SetBooleanAttribute(lua_State *L)
    {
    SDL_GLattr attr = checkglattr(L, 1);
    int value = checkboolean(L, 2);
    if(SDL_GL_SetAttribute(attr, value)!=0) GetError(L);
    return 0;
    }

static int GL_GetBooleanAttribute(lua_State *L)
    {
    int value;
    SDL_GLattr attr = checkglattr(L, 1);
    if(SDL_GL_GetAttribute(attr, &value)!=0) GetError(L);
    lua_pushboolean(L, value);
    return 1;
    }

static int GL_SetContextVersion(lua_State *L)
    {
    int major = luaL_checkinteger(L, 1);
    int minor = luaL_checkinteger(L, 2);
    int profile = checkglprofile(L, 3);
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major)!=0) GetError(L);
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor)!=0) GetError(L);
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile)!=0) GetError(L);
    return 0;
    }

static int GL_GetContextVersion(lua_State *L)
    {
    int major, minor, profile;
    if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major)!=0) GetError(L);
    if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor)!=0) GetError(L);
    if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile)!=0) GetError(L);
    lua_pushinteger(L, major);
    lua_pushinteger(L, minor);
    pushglprofile(L, profile);
    return 3;
    }

static int GL_SetContextFlags(lua_State *L)
    {
    int flags = checkflags(L, 1);
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags)!=0) GetError(L);
    return 0;
    }

static int GL_GetContextFlags(lua_State *L)
    {
    int flags;
    if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &flags)!=0) GetError(L);
    pushflags(L, flags);
    return 1;
    }

DESTROY_FUNC(glcontext)

static const struct luaL_Reg Methods[] = 
    {
        { "delete", Destroy },
        { "make_current", GL_MakeCurrent },
        { NULL, NULL } /* sentinel */
    };


static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg WindowMethods[] = 
    {
        { "gl_get_drawable_size", GL_GetDrawableSize },
        { "gl_swap", GL_SwapWindow },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "gl_create_context", Create },
        { "gl_extension_supported", GL_ExtensionSupported },
        { "gl_reset_attributes", GL_ResetAttributes },
        { "gl_set_attribute", GL_SetAttribute },
        { "gl_get_attribute", GL_GetAttribute },
        { "gl_get_current_window", GL_GetCurrentWindow },
        { "gl_get_drawable_size", GL_GetDrawableSize },
        { "gl_swap_window", GL_SwapWindow },
        { "gl_get_current_context", GL_GetCurrentContext },
        { "gl_set_swap_interval", GL_SetSwapInterval },
        { "gl_get_swap_interval", GL_GetSwapInterval },
        { "gl_set_boolean_attribute", GL_SetBooleanAttribute },
        { "gl_get_boolean_attribute", GL_GetBooleanAttribute },
        { "gl_set_context_version", GL_SetContextVersion },
        { "gl_get_context_version", GL_GetContextVersion },
        { "gl_set_context_flags", GL_SetContextFlags },
        { "gl_get_context_flags", GL_GetContextFlags },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_glcontext(lua_State *L)
    {
    udata_define(L, GLCONTEXT_MT, Methods, MetaMethods);
    udata_addmethods(L, WINDOW_MT, WindowMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
//int SDL_GL_LoadLibrary(const char *path);
//void *SDL_GL_GetProcAddress(const char *proc);
//void SDL_GL_UnloadLibrary(void);

#endif

