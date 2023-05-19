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

static int freetexture(lua_State *L, ud_t *ud)
    {
    texture_t *texture = (texture_t*)ud->handle;
    freechildren(L, SURFACE_MT, ud);
    if(!freeuserdata(L, ud, "texture")) return 0;
    SDL_DestroyTexture(texture);
    return 0;
    }

int newtexture(lua_State *L, texture_t *texture, ud_t *parent_ud)
    {
    ud_t *ud;
    ud = newuserdata(L, texture, TEXTURE_MT, "texture");
    ud->parent_ud = parent_ud;
    ud->destructor = freetexture;
    return 1;
    }

static int Create(lua_State *L)
    {
    texture_t *texture;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    surface_t *surface = testsurface(L, 2, NULL);
    if(surface)
        texture = SDL_CreateTextureFromSurface(renderer, surface);
    else
        {
        uint32_t format = checkpixelformat(L, 2);
        int access = checktextureaccess(L, 3);
        int w = luaL_checkinteger(L, 4);
        int h = luaL_checkinteger(L, 5);
        texture = SDL_CreateTexture(renderer, format, access, w, h);
        }
    if(texture==NULL) GetError(L);
    return newtexture(L, texture, UD(renderer));
    }

static int LoadTexture(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    const char *file = luaL_checkstring(L, 2);
    texture_t *texture = IMG_LoadTexture(renderer, file);
    if(!texture) GetError(L);
    return newtexture(L, texture, UD(renderer));
    }


static int GL_Bind(lua_State *L)
    {
    float texw, texh;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = SDL_GL_BindTexture(texture, &texw, &texh);
    CheckError(L, ec);
    lua_pushnumber(L, texw);
    lua_pushnumber(L, texh);
    return 2;
    }

static int GL_Unbind(lua_State *L)
    {
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = SDL_GL_UnbindTexture(texture);
    CheckError(L, ec);
    return 0;
    }

static int Query(lua_State *L)
    {
    uint32_t format;
    int access, w, h;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = SDL_QueryTexture(texture, &format, &access, &w, &h);
    CheckError(L, ec);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    pushpixelformat(L, format);
    pushtextureaccess(L, access);
    return 4;
    }

static int SetColorMod(lua_State *L)
    {
    texture_t *texture = checktexture(L, 1, NULL);
    uint8_t r = luaL_checkinteger(L, 2);
    uint8_t g = luaL_checkinteger(L, 3);
    uint8_t b = luaL_checkinteger(L, 4);
    int ec = SDL_SetTextureColorMod(texture, r, g, b);
    CheckError(L, ec);
    return 0;
    }

static int GetColorMod(lua_State *L)
    {
    uint8_t r, g, b;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = SDL_GetTextureColorMod(texture, &r, &g, &b);
    CheckError(L, ec);
    lua_pushinteger(L, r);
    lua_pushinteger(L, g);
    lua_pushinteger(L, b);
    return 3;
    }

static int SetAlphaMod(lua_State *L)
    {
    texture_t *texture = checktexture(L, 1, NULL);
    uint8_t alpha = luaL_checkinteger(L, 2);
    int ec = SDL_SetTextureAlphaMod(texture, alpha);
    CheckError(L, ec);
    return 0;
    }

static int GetAlphaMod(lua_State *L)
    {
    uint8_t alpha;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = SDL_GetTextureAlphaMod(texture, &alpha);
    CheckError(L, ec);
    lua_pushinteger(L, alpha);
    return 1;
    }

static int SetBlendMode(lua_State *L)
    {
    texture_t *texture = checktexture(L, 1, NULL);
    SDL_BlendMode mode = luaL_checkinteger(L, 2);
    int ec = SDL_SetTextureBlendMode(texture, mode);
    CheckError(L, ec);
    return 0;
    }

static int GetBlendMode(lua_State *L)
    {
    SDL_BlendMode mode;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = SDL_GetTextureBlendMode(texture, &mode);
    CheckError(L, ec);
    lua_pushinteger(L, mode);
    return 1;
    }

static int SetScaleMode(lua_State *L)
    {
    texture_t *texture = checktexture(L, 1, NULL);
    SDL_ScaleMode mode = checkscalemode(L, 2);
    int ec = SDL_SetTextureScaleMode(texture, mode);
    CheckError(L, ec);
    return 0;
    }

static int GetScaleMode(lua_State *L)
    {
    SDL_ScaleMode mode;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = SDL_GetTextureScaleMode(texture, &mode);
    CheckError(L, ec);
    pushscalemode(L, mode);
    return 1;
    }

static int Update(lua_State *L)
    {
    rect_t rect;
    texture_t *texture = checktexture(L, 1, NULL);
    void *pixels = checklightuserdata(L, 3);
    int pitch = luaL_checkinteger(L, 4);
    int ec = optrect(L, 2, &rect);
    ec = SDL_UpdateTexture(texture, (ec==ERR_NOTPRESENT) ? NULL : &rect, pixels, pitch);
    CheckError(L, ec);
    return 0;
    }

static int UpdateYUV(lua_State *L)
    {
    rect_t rect;
    texture_t *texture = checktexture(L, 1, NULL);
    void *yplane = checklightuserdata(L, 3);
    int ypitch = luaL_checkinteger(L, 4);
    void *uplane = checklightuserdata(L, 5);
    int upitch = luaL_checkinteger(L, 6);
    void *vplane = checklightuserdata(L, 7);
    int vpitch = luaL_checkinteger(L, 8);
    int ec = optrect(L, 2, &rect);
    ec = SDL_UpdateYUVTexture(texture, (ec==ERR_NOTPRESENT) ? NULL : &rect,
            yplane, ypitch, uplane, upitch, vplane, vpitch);
    CheckError(L, ec);
    return 0;
    }

static int UpdateNV(lua_State *L)
    {
    rect_t rect;
    texture_t *texture = checktexture(L, 1, NULL);
    void *yplane = checklightuserdata(L, 3);
    int ypitch = luaL_checkinteger(L, 4);
    void *uvplane = checklightuserdata(L, 5);
    int uvpitch = luaL_checkinteger(L, 6);
    int ec = optrect(L, 2, &rect);
    ec = SDL_UpdateNVTexture(texture, (ec==ERR_NOTPRESENT) ? NULL : &rect,
            yplane, ypitch, uvplane, uvpitch);
    CheckError(L, ec);
    return 0;
    }

static int Lock(lua_State *L)
    {
    rect_t rect;
    void *pixels;
    int pitch;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = optrect(L, 2, &rect);
    ec = SDL_LockTexture(texture, (ec==ERR_NOTPRESENT) ? NULL : &rect, &pixels, &pitch);
    CheckError(L, ec);
    lua_pushlightuserdata(L, pixels);
    lua_pushinteger(L, pitch);
    return 2;
    }

static int LockToSurface(lua_State *L)
    {
    rect_t rect;
    surface_t *surface;
    texture_t *texture = checktexture(L, 1, NULL);
    int ec = optrect(L, 2, &rect);
    ec = SDL_LockTextureToSurface(texture, (ec==ERR_NOTPRESENT) ? NULL : &rect, &surface);
    if(UD(surface))
        pushsurface(L, surface);
    else
        newsurface(L, surface, UD(texture));
    return 1;
    }

static int Unlock(lua_State *L)
    {
    texture_t *texture = checktexture(L, 1, NULL);
    SDL_UnlockTexture(texture);
    return 0;
    }

static int ComposeCustomBlendMode(lua_State *L)
    {
    SDL_BlendFactor srcColorFactor = checkblendfactor(L, 1);
    SDL_BlendFactor dstColorFactor = checkblendfactor(L, 2);
    SDL_BlendOperation colorOperation = checkblendoperation(L, 3);
    SDL_BlendFactor srcAlphaFactor = checkblendfactor(L, 4);
    SDL_BlendFactor dstAlphaFactor  = checkblendfactor(L, 5);
    SDL_BlendOperation alphaOperation = checkblendoperation(L, 6);
    SDL_BlendMode mode = 
        SDL_ComposeCustomBlendMode(srcColorFactor, dstColorFactor, colorOperation,
                                   srcAlphaFactor, dstAlphaFactor, alphaOperation);
    lua_pushinteger(L, mode);
    return 1;
    }

DESTROY_FUNC(texture)

static const struct luaL_Reg Methods[] = 
    {
        { "destroy", Destroy },
        { "gl_bind", GL_Bind },
        { "gl_unbind", GL_Unbind },
        { "query", Query },
        { "set_color_mod", SetColorMod },
        { "get_color_mod", GetColorMod },
        { "set_alpha_mod", SetAlphaMod },
        { "get_alpha_mod", GetAlphaMod },
        { "set_blend_mode", SetBlendMode },
        { "get_blend_mode", GetBlendMode },
        { "set_scale_mode", SetScaleMode },
        { "get_scale_mode", GetScaleMode },
        { "update", Update },
        { "update_yuv", UpdateYUV },
        { "update_nv", UpdateNV },
        { "lock", Lock },
        { "lock_to_surface", LockToSurface },
        { "unlock", Unlock },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_texture", Create },
        { "load_texture", LoadTexture },
        { "compose_custom_blend_mode", ComposeCustomBlendMode },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_texture(lua_State *L)
    {
    udata_define(L, TEXTURE_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
#define ADD(what)   do { lua_pushinteger(L, SDL_##what); lua_setfield(L, -2, #what); } while(0)
    ADD(BLENDMODE_NONE);
    ADD(BLENDMODE_BLEND);
    ADD(BLENDMODE_ADD);
    ADD(BLENDMODE_MOD);
    ADD(BLENDMODE_MUL);
    ADD(BLENDMODE_INVALID);
#undef ADD
    }

#if 0
//int SDL_SetTextureUserData(SDL_Texture *texture, void *userdata);
//void* SDL_GetTextureUserData(SDL_Texture *texture);
#endif
