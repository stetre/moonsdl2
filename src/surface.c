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

static int freesurface(lua_State *L, ud_t *ud)
    {
    surface_t *surface = (surface_t*)ud->handle;
    int isborrowed = IsBorrowed(ud);
    freechildren(L, CURSOR_MT, ud);
    freechildren(L, PXLFMT_MT, ud);
    freechildren(L, RENDERER_MT, ud);
    if(!freeuserdata(L, ud, "surface")) return 0;
    if(!isborrowed)
        SDL_FreeSurface(surface);
    return 0;
    }

int newsurface(lua_State *L, surface_t *surface, ud_t *parent_ud)
    {
    ud_t *ud;
    ud = newuserdata(L, surface, SURFACE_MT, "surface");
    ud->parent_ud = parent_ud;
    ud->destructor = freesurface;
    if(parent_ud!=NULL) MarkBorrowed(ud);
    return 1;
    }

static int CreateRGBSurface(lua_State *L)
    {
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    int depth = luaL_checkinteger(L, 3);
    uint32_t rmask = luaL_optinteger(L, 4, 0);
    uint32_t gmask = luaL_optinteger(L, 5, 0);
    uint32_t bmask = luaL_optinteger(L, 6, 0);
    uint32_t amask = luaL_optinteger(L, 7, 0);
    surface_t *surface = SDL_CreateRGBSurface(0, width, height, depth, rmask, gmask, bmask, amask);
    if(surface==NULL) GetError(L);
    return newsurface(L, surface, NULL);
    }

static int CreateRGBSurfaceWithFormat(lua_State *L)
    {
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    int depth = luaL_checkinteger(L, 3);
    uint32_t format = checkpixelformat(L, 4);
    surface_t *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format);
    if(surface==NULL) GetError(L);
    return newsurface(L, surface, NULL);
    }

static int CreateSurface(lua_State *L)
    {
    if(lua_isinteger(L, 4) || lua_isnoneornil(L, 4))
        return CreateRGBSurface(L);
    else
        return CreateRGBSurfaceWithFormat(L);
    }


static int CreateRGBSurfaceFrom(lua_State *L)
    {
    void *pixels = checklightuserdata(L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    int depth = luaL_checkinteger(L, 4);
    int pitch = luaL_checkinteger(L, 5);
    uint32_t rmask = luaL_optinteger(L, 6, 0);
    uint32_t gmask = luaL_optinteger(L, 7, 0);
    uint32_t bmask = luaL_optinteger(L, 8, 0);
    uint32_t amask = luaL_optinteger(L, 9, 0);
    surface_t *surface =
        SDL_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, rmask, gmask, bmask, amask);
    if(surface==NULL) GetError(L);
    return newsurface(L, surface, NULL);
    }

static int CreateRGBSurfaceWithFormatFrom(lua_State *L)
    {
    void *pixels = checklightuserdata(L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    int depth = luaL_checkinteger(L, 4);
    int pitch = luaL_checkinteger(L, 5);
    uint32_t format = checkpixelformat(L, 6);
    surface_t *surface =
        SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, pitch, format);
    if(surface==NULL) GetError(L);
    return newsurface(L, surface, NULL);
    }

static int LoadBMP(lua_State *L)
    {
    const char *file = luaL_checkstring(L, 1);
    surface_t *surface = SDL_LoadBMP(file);
    if(surface==NULL) GetError(L);
    return newsurface(L, surface, NULL);
    }

static int Duplicate(lua_State *L)
    {
    surface_t *src = checksurface(L, 1, NULL);
    surface_t *surface = SDL_DuplicateSurface(src);
    if(surface==NULL) GetError(L);
    return newsurface(L, surface, NULL);
    }

static int Convert(lua_State *L)
    {
    surface_t *surface;
    surface_t *src = checksurface(L, 1, NULL);
    if(lua_isstring(L, 2))
        {
        uint32_t format = checkpixelformat(L, 2);
        surface = SDL_ConvertSurfaceFormat(src, format, 0);
        }
    else
        {
        pxlfmt_t *fmt = checkpxlfmt(L, 2, NULL);
        surface = SDL_ConvertSurface(src, fmt, 0);
        }
    if(surface==NULL) GetError(L);
    return newsurface(L, surface, NULL);
    }

static int GetFormat(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    pxlfmt_t *pxlfmt = surface->format;
    pushpixelformat(L, pxlfmt->format);
    return 1;
    }

static int GetPxlfmt(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    pxlfmt_t *pxlfmt = surface->format;
    if(UD(pxlfmt))
        pushpxlfmt(L, pxlfmt);
    else
        newpxlfmt(L, pxlfmt, UD(surface));
    return 1;
    }

static int GetSize(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    lua_pushinteger(L, surface->w);
    lua_pushinteger(L, surface->h);
    lua_pushinteger(L, surface->pitch);
    return 3;
    }

static int SetPalette(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    palette_t *palette = checkpalette(L, 2, NULL);
    if(SDL_SetSurfacePalette(surface, palette) != 0) GetError(L);
    return 0;
    }

static int Lock(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    int ec = SDL_LockSurface(surface);
    CheckError(L, ec);
    return 0;
    }

static int Unlock(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    SDL_UnlockSurface(surface);
    return 0;
    }

static int MustLock(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    lua_pushboolean(L, SDL_MUSTLOCK(surface));
    return 1;
    }

static int SaveBMP(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    const char *file = luaL_checkstring(L, 2);
    int ec = SDL_SaveBMP(surface, file);
    CheckError(L, ec);
    return 0;
    }

static int SavePNG(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    const char *file = luaL_checkstring(L, 2);
    int ec = IMG_SavePNG(surface, file);
    CheckError(L, ec);
    return 0;
    }

static int SaveJPG(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    const char *file = luaL_checkstring(L, 2);
    int quality = luaL_checkinteger(L, 3);
    int ec = IMG_SaveJPG(surface, file, quality);
    CheckError(L, ec);
    return 0;
    }


static int SetRLE(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    int flag = checkboolean(L, 2);
    int ec = SDL_SetSurfaceRLE(surface, flag);
    CheckError(L, ec);
    return 0;
    }

static int HasRLE(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    lua_pushboolean(L, SDL_HasSurfaceRLE(surface));
    return 1;
    }

static int SetColorKey(lua_State *L)
    {
    int ec;
    surface_t *surface = checksurface(L, 1, NULL);
    if(lua_isnoneornil(L, 2))
        ec = SDL_SetColorKey(surface, SDL_FALSE, 0);
    else
        {
        uint32_t key = luaL_checkinteger(L, 2);
        ec = SDL_SetColorKey(surface, SDL_TRUE, key);
        }
    CheckError(L, ec);
    return 0;
    }

static int HasColorKey(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    lua_pushboolean(L, SDL_HasColorKey(surface));
    return 1;
    }

static int GetColorKey(lua_State *L)
    {
    uint32_t key;
    surface_t *surface = checksurface(L, 1, NULL);
    int ec = SDL_GetColorKey(surface, &key);
    if(ec == -1) return 0;
    lua_pushinteger(L, key);
    return 1;
    }

static int SetColorMod(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    uint8_t r = luaL_checkinteger(L, 2);
    uint8_t g = luaL_checkinteger(L, 3);
    uint8_t b = luaL_checkinteger(L, 4);
    int ec = SDL_SetSurfaceColorMod(surface, r, g, b);
    CheckError(L, ec);
    return 0;
    }

static int GetColorMod(lua_State *L)
    {
    uint8_t r, g, b;
    surface_t *surface = checksurface(L, 1, NULL);
    int ec = SDL_GetSurfaceColorMod(surface, &r, &g, &b);
    CheckError(L, ec);
    lua_pushinteger(L, r);
    lua_pushinteger(L, g);
    lua_pushinteger(L, b);
    return 3;
    }

static int SetAlphaMod(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    uint8_t alpha = luaL_checkinteger(L, 2);
    int ec = SDL_SetSurfaceAlphaMod(surface, alpha);
    CheckError(L, ec);
    return 0;
    }

static int GetAlphaMod(lua_State *L)
    {
    uint8_t alpha;
    surface_t *surface = checksurface(L, 1, NULL);
    int ec = SDL_GetSurfaceAlphaMod(surface, &alpha);
    CheckError(L, ec);
    lua_pushinteger(L, alpha);
    return 1;
    }

static int SetBlendMode(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    SDL_BlendMode mode = luaL_checkinteger(L, 2);
    int ec = SDL_SetSurfaceBlendMode(surface, mode);
    CheckError(L, ec);
    return 0;
    }

static int GetBlendMode(lua_State *L)
    {
    SDL_BlendMode mode;
    surface_t *surface = checksurface(L, 1, NULL);
    int ec = SDL_GetSurfaceBlendMode(surface, &mode);
    CheckError(L, ec);
    lua_pushinteger(L, mode);
    return 1;
    }

static int SetClipRect(lua_State *L)
    {
    rect_t rect;
    surface_t *surface = checksurface(L, 1, NULL);
    int rectpres = optrect(L, 2, &rect);
    int rc = SDL_SetClipRect(surface, (rectpres==ERR_NOTPRESENT)? NULL : &rect);
    lua_pushboolean(L, rc);
    return 1;
    }

static int GetClipRect(lua_State *L)
    {
    rect_t rect;
    surface_t *surface = checksurface(L, 1, NULL);
    SDL_GetClipRect(surface, &rect);
    pushrect(L, &rect);
    return 1;
    }

static int FillRect(lua_State *L)
    {
    rect_t rect;
    surface_t *surface = checksurface(L, 1, NULL);
    int rectpres = optrect(L, 2, &rect);
    uint32_t color = luaL_checkinteger(L, 3);
    int ec = SDL_FillRect(surface, (rectpres==ERR_NOTPRESENT)? NULL : &rect, color);
    CheckError(L, ec);
    return 0;
    }

static int FillRects(lua_State *L)
    {
    int count;
    surface_t *surface = checksurface(L, 1, NULL);
    rect_t *rects = checkrectlist(L, 2, &count, NULL);
    uint32_t color = luaL_checkinteger(L, 3);
    int ec = SDL_FillRects(surface, rects, count, color);
    Free(L, rects);
    CheckError(L, ec);
    return 0;
    }

#define F(Func, func)                       \
static int Func(lua_State *L)               \
    {                                       \
    rect_t srcrect, dstrect;                \
    int sp, dp;                             \
    surface_t *src = checksurface(L, 1, NULL);      \
    surface_t *dst = checksurface(L, 2, NULL);      \
    sp = optrect(L, 3, &srcrect);           \
    dp = optrect(L, 4, &dstrect);           \
    if(func(src, (sp==ERR_NOTPRESENT)? NULL : &srcrect,         \
            dst, (dp==ERR_NOTPRESENT)? NULL : &dstrect) != 0)   \
        GetError(L);                                            \
    return 0;                                                   \
    }
F(UpperBlit, SDL_UpperBlit)
//F(LowerBlit, SDL_LowerBlit)
F(SoftStretch, SDL_SoftStretch)
F(SoftStretchLinear, SDL_SoftStretchLinear)
F(UpperBlitScaled, SDL_UpperBlitScaled)
//F(LowerBlitScaled, SDL_LowerBlitScaled)
#undef F
 
static int Load(lua_State *L)
    {
    const char *file = luaL_checkstring(L, 1);
    surface_t *surface = IMG_Load(file);
    if(!surface) GetError(L);
    return newsurface(L, surface, NULL);
    }

#if 0 // 8yy
        { "",  },
static int (lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    (void)surface;
    return 0;
    }

#endif

DESTROY_FUNC(surface)

static const struct luaL_Reg Methods[] = 
    {
        { "free", Destroy },
        { "duplicate", Duplicate },
        { "convert", Convert },
        { "get_format", GetFormat },
        { "get_pxlfmt", GetPxlfmt },
        { "get_size", GetSize },
        { "set_palette", SetPalette },
        { "lock", Lock },
        { "unlock", Unlock },
        { "must_lock", MustLock },
        { "save_bmp", SaveBMP },
        { "save_png", SavePNG },
        { "save_jpg", SaveJPG },
        { "set_rle", SetRLE },
        { "has_rle", HasRLE },
        { "set_color_key", SetColorKey },
        { "has_color_key", HasColorKey },
        { "get_color_key", GetColorKey },
        { "set_color_mod", SetColorMod },
        { "get_color_mod", GetColorMod },
        { "set_alpha_mod", SetAlphaMod },
        { "get_alpha_mod", GetAlphaMod },
        { "set_blend_mode", SetBlendMode },
        { "get_blend_mode", GetBlendMode },
        { "set_clip_rect", SetClipRect },
        { "get_clip_rect", GetClipRect },
        { "fill_rect", FillRect },
        { "fill_rects", FillRects },
        { "blit", UpperBlit  },
//      { "lower_blit", LowerBlit },
        { "soft_stretch", SoftStretch },
        { "soft_stretch_linear", SoftStretchLinear },
        { "blit_scaled", UpperBlitScaled  },
//      { "lower_blit_scaled", LowerBlitScaled },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_rgb_surface", CreateSurface },
        { "create_rgb_surface_with_format", CreateSurface },
        { "create_rgb_surface_from", CreateRGBSurfaceFrom },
        { "create_rgb_surface_with_format_from", CreateRGBSurfaceWithFormatFrom },
        { "load_bmp", LoadBMP },
        { "load_image", Load },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_surface(lua_State *L)
    {
    udata_define(L, SURFACE_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
//int SDL_SaveBMP_RW(SDL_Surface * surface, SDL_RWops * dst, int freedst);
//SDL_Surface *SDL_LoadBMP_RW(SDL_RWops * src, int freesrc);

#define SDL_SWSURFACE       0           
#define SDL_PREALLOC        0x00000001  
#define SDL_RLEACCEL        0x00000002  
#define SDL_DONTFREE        0x00000004  
#define SDL_SIMD_ALIGNED    0x00000008  

typedef struct SDL_Surface {
    Uint32 flags;               
    SDL_PixelFormat *format;    
    int w, h;                   
    int pitch;                  
    void *pixels;               
    void *userdata;             
    int locked;                 
    void *list_blitmap;         
    SDL_Rect clip_rect;         
    struct SDL_BlitMap *map;    
    int refcount;               
} SDL_Surface;

typedef int (*SDL_blit)(struct SDL_Surface * src, SDL_Rect * srcrect, struct SDL_Surface * dst, SDL_Rect * dstrect);

typedef enum {
    SDL_YUV_CONVERSION_JPEG,        
    SDL_YUV_CONVERSION_BT601,       
    SDL_YUV_CONVERSION_BT709,       
    SDL_YUV_CONVERSION_AUTOMATIC    
} SDL_YUV_CONVERSION_MODE;
//@@void SDL_SetYUVConversionMode(SDL_YUV_CONVERSION_MODE mode);
//@@SDL_YUV_CONVERSION_MODE SDL_GetYUVConversionMode(void);
//@@SDL_YUV_CONVERSION_MODE SDL_GetYUVConversionModeForResolution(int width, int height);

//@@int SDL_ConvertPixels(int width, int height, Uint32 src_format, const void * src, int src_pitch, Uint32 dst_format, void * dst, int dst_pitch);
//@@int SDL_PremultiplyAlpha(int width, int height, Uint32 src_format, const void * src, int src_pitch, Uint32 dst_format, void * dst, int dst_pitch);


// SDL_image ---------------------------------------------------------------------------

SDL_Surface* IMG_LoadTyped_RW(SDL_RWops *src, int freesrc, const char *type);
SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc);
SDL_Texture* IMG_LoadTexture_RW(SDL_Renderer *renderer, SDL_RWops *src, int freesrc);
SDL_Texture* IMG_LoadTextureTyped_RW(SDL_Renderer *renderer, SDL_RWops *src, int freesrc, const char *type);

int IMG_isICO(SDL_RWops *src);
int IMG_isCUR(SDL_RWops *src);
int IMG_isBMP(SDL_RWops *src);
int IMG_isGIF(SDL_RWops *src);
int IMG_isJPG(SDL_RWops *src);
int IMG_isLBM(SDL_RWops *src);
int IMG_isPCX(SDL_RWops *src);
int IMG_isPNG(SDL_RWops *src);
int IMG_isPNM(SDL_RWops *src);
int IMG_isSVG(SDL_RWops *src);
int IMG_isTIF(SDL_RWops *src);
int IMG_isXCF(SDL_RWops *src);
int IMG_isXPM(SDL_RWops *src);
int IMG_isXV(SDL_RWops *src);
int IMG_isWEBP(SDL_RWops *src);

SDL_Surface* IMG_LoadICO_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadCUR_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadBMP_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadGIF_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadLBM_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadPCX_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadPNG_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadPNM_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadSVG_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadTGA_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadTIF_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadXCF_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadXPM_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadXV_RW(SDL_RWops *src);
SDL_Surface* IMG_LoadWEBP_RW(SDL_RWops *src);
SDL_Surface* IMG_ReadXPMFromArray(char **xpm);

int IMG_SavePNG_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst);
int IMG_SaveJPG_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst, int quality);

int IMG_isAVIF(SDL_RWops *src);
int IMG_isJXL(SDL_RWops *src);
int IMG_isQOI(SDL_RWops *src);
SDL_Surface * IMG_LoadAVIF_RW(SDL_RWops *src);
SDL_Surface * IMG_LoadJXL_RW(SDL_RWops *src);
SDL_Surface * IMG_LoadQOI_RW(SDL_RWops *src);
SDL_Surface * IMG_LoadSizedSVG_RW(SDL_RWops *src, int width, int height);
SDL_Surface * IMG_ReadXPMFromArrayToRGB888(char **xpm);
#endif


