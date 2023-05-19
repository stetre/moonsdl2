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

static int freepxlfmt(lua_State *L, ud_t *ud)
    {
    pxlfmt_t *pxlfmt = (pxlfmt_t*)ud->handle;
    int isborrowed = IsBorrowed(ud);
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "pxlfmt")) return 0;
    if(!isborrowed)
        SDL_FreeFormat(pxlfmt);
    return 0;
    }

int newpxlfmt(lua_State *L, pxlfmt_t *pxlfmt, ud_t *parent_ud)
    {
    ud_t *ud;
    ud = newuserdata(L, pxlfmt, PXLFMT_MT, "pxlfmt");
    ud->parent_ud = parent_ud;
    ud->destructor = freepxlfmt;
    if(parent_ud!=NULL) MarkBorrowed(ud);
    return 1;
    }

static int Create(lua_State *L)
    {
    uint32_t format = checkpixelformat(L, 1);
    pxlfmt_t *pxlfmt = SDL_AllocFormat(format);
    if(pxlfmt==NULL) GetError(L);
    return newpxlfmt(L, pxlfmt, NULL);
    }

static int SetPalette(lua_State *L)
    {
    pxlfmt_t *pxlfmt = checkpxlfmt(L, 1, NULL);
    palette_t *palette = checkpalette(L, 2, NULL);
    if(SDL_SetPixelFormatPalette(pxlfmt, palette) != 0) GetError(L);
    return 0;
    }

static int MapRGB(lua_State *L)
    {
    pxlfmt_t *pxlfmt = checkpxlfmt(L, 1, NULL);
    uint8_t r = luaL_checkinteger(L, 2);
    uint8_t g = luaL_checkinteger(L, 3);
    uint8_t b = luaL_checkinteger(L, 4);
    uint32_t pixel = SDL_MapRGB(pxlfmt, r, g, b);
    lua_pushinteger(L, pixel);
    return 1;
    }

static int MapRGBA(lua_State *L)
    {
    pxlfmt_t *pxlfmt = checkpxlfmt(L, 1, NULL);
    uint8_t r = luaL_checkinteger(L, 2);
    uint8_t g = luaL_checkinteger(L, 3);
    uint8_t b = luaL_checkinteger(L, 4);
    uint8_t a = luaL_checkinteger(L, 5);
    uint32_t pixel = SDL_MapRGBA(pxlfmt, r, g, b, a);
    lua_pushinteger(L, pixel);
    return 1;
    }

static int GetRGB(lua_State *L)
    {
    uint8_t r, g, b;
    pxlfmt_t *pxlfmt = checkpxlfmt(L, 1, NULL);
    uint32_t pixel = luaL_checkinteger(L, 2);
    SDL_GetRGB(pixel, pxlfmt, &r, &g, &b);
    lua_pushinteger(L, r);
    lua_pushinteger(L, g);
    lua_pushinteger(L, b);
    return 3;
    }

static int GetRGBA(lua_State *L)
    {
    uint8_t r, g, b, a;
    pxlfmt_t *pxlfmt = checkpxlfmt(L, 1, NULL);
    uint32_t pixel = luaL_checkinteger(L, 2);
    SDL_GetRGBA(pixel, pxlfmt, &r, &g, &b, &a);
    lua_pushinteger(L, r);
    lua_pushinteger(L, g);
    lua_pushinteger(L, b);
    lua_pushinteger(L, a);
    return 4;
    }

static int GetFormat(lua_State *L)
    {
    pxlfmt_t *pxlfmt = checkpxlfmt(L, 1, NULL);
    pushpixelformat(L, pxlfmt->format);
    return 1;
    }

static int BitsPerPixel(lua_State *L)
    {
    pxlfmt_t *pxlfmt = testpxlfmt(L, 1, NULL);
    if(pxlfmt)
        lua_pushinteger(L, pxlfmt->BitsPerPixel);
    else
        {
        int format = checkpixelformat(L, 1);
        lua_pushinteger(L, SDL_BITSPERPIXEL(format));
        }
    return 1;
    }

static int BytesPerPixel(lua_State *L)
    {
    pxlfmt_t *pxlfmt = testpxlfmt(L, 1, NULL);
    if(pxlfmt)
        lua_pushinteger(L, pxlfmt->BytesPerPixel);
    else
        {
        int format = checkpixelformat(L, 1);
        lua_pushinteger(L, SDL_BYTESPERPIXEL(format));
        }
    return 1;
    }

static int CalculateGammaRamp(lua_State *L)
    {
    int i;
    uint16_t ramp[256];
    float gamma = luaL_checknumber(L, 1);
    SDL_CalculateGammaRamp(gamma, ramp);
    lua_newtable(L);
    for(i=0; i<256; i++)
        {
        lua_pushinteger(L, ramp[i]);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }

static int GetPixelFormatValue(lua_State *L)
    {
    uint32_t format = checkpixelformat(L, 1);
    lua_pushinteger(L, format);
    return 1;
    }

static int PixelFormatEnumToMasks(lua_State *L)
    {
    int bpp;
    uint32_t Rmask, Gmask, Bmask, Amask;
    uint32_t format = checkpixelformat(L, 1);
    if(!SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask)) GetError(L);
    lua_pushinteger(L, bpp);
    lua_pushinteger(L, Rmask);
    lua_pushinteger(L, Gmask);
    lua_pushinteger(L, Bmask);
    lua_pushinteger(L, Amask);
    return 5;
    }

static int MasksToPixelFormatEnum(lua_State *L)
    {
    int bpp = luaL_checkinteger(L, 1);
    uint32_t Rmask = luaL_checkinteger(L, 1);
    uint32_t Gmask = luaL_checkinteger(L, 2);
    uint32_t Bmask = luaL_checkinteger(L, 3);
    uint32_t Amask = luaL_checkinteger(L, 4);
    uint32_t format = SDL_MasksToPixelFormatEnum(bpp, Rmask, Gmask, Bmask, Amask);
    pushpixelformat(L, format);
    return 1;
    }


#define F(Func, func)                           \
static int Func(lua_State *L)                   \
    {                                           \
    uint32_t format = checkpixelformat(L, 1);   \
    lua_pushboolean(L, func(format));           \
    return 1;                                   \
    }
F(ISPIXELFORMAT_INDEXED, SDL_ISPIXELFORMAT_INDEXED)
F(ISPIXELFORMAT_PACKED, SDL_ISPIXELFORMAT_PACKED)
F(ISPIXELFORMAT_ARRAY, SDL_ISPIXELFORMAT_ARRAY)
F(ISPIXELFORMAT_ALPHA, SDL_ISPIXELFORMAT_ALPHA)
F(ISPIXELFORMAT_FOURCC, SDL_ISPIXELFORMAT_FOURCC)
#undef F

static int PixelFormatType(lua_State *L) //@@ return type, order, layout as enums?
    {
    uint32_t format = checkpixelformat(L, 1);
    (void)format;
//#define SDL_PIXELFLAG(X)    (((X) >> 28) & 0x0F)
//#define SDL_PIXELTYPE(X)    (((X) >> 24) & 0x0F)
//#define SDL_PIXELORDER(X)   (((X) >> 20) & 0x0F)
//#define SDL_PIXELLAYOUT(X)  (((X) >> 16) & 0x0F)
    return 0;
    }

#if 0 // 8yy
        { "",  },
static int (lua_State *L)
    {
    pxlfmt_t *pxlfmt = checkpxlfmt(L, 1, NULL);
    (void)pxlfmt;
    return 0;
    }

#endif

DESTROY_FUNC(pxlfmt)

static const struct luaL_Reg Methods[] = 
    {
        { "free", Destroy },
        { "set_palette", SetPalette },
        { "map_rgb", MapRGB },
        { "map_rgba", MapRGBA },
        { "get_rgb", GetRGB },
        { "get_rgba", GetRGBA },
        { "get_format", GetFormat },
        { "bits_per_pixel", BitsPerPixel },
        { "bytes_per_pixel", BytesPerPixel },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "alloc_pxlfmt", Create },
        { "calculate_gamma_ramp", CalculateGammaRamp },
        { "bits_per_pixel", BitsPerPixel },
        { "bytes_per_pixel", BytesPerPixel },
        { "get_pixel_format_value", GetPixelFormatValue },
        { "is_pixel_format_indexed", ISPIXELFORMAT_INDEXED },
        { "is_pixel_format_packed", ISPIXELFORMAT_PACKED },
        { "is_pixel_format_array",  ISPIXELFORMAT_ARRAY},
        { "is_pixel_format_alpha", ISPIXELFORMAT_ALPHA },
        { "is_pixel_format_fourcc", ISPIXELFORMAT_FOURCC },
        { "pixel_format_enum_to_masks", PixelFormatEnumToMasks },
        { "masks_to_pixel_format_enum", MasksToPixelFormatEnum },
        { "pixel_format_type", PixelFormatType },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_pxlfmt(lua_State *L)
    {
    udata_define(L, PXLFMT_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
#define ADD(what)   do { lua_pushinteger(L, SDL_##what); lua_setfield(L, -2, #what); } while(0)
    ADD(ALPHA_OPAQUE);
    ADD(ALPHA_TRANSPARENT);
#undef ADD
    }

#if 0
//Servono?
//const char* SDL_GetPixelFormatName(uint32_t format) -> no, the enum string is good enough


typedef enum {
    SDL_PIXELTYPE_UNKNOWN,
    SDL_PIXELTYPE_INDEX1,
    SDL_PIXELTYPE_INDEX4,
    SDL_PIXELTYPE_INDEX8,
    SDL_PIXELTYPE_PACKED8,
    SDL_PIXELTYPE_PACKED16,
    SDL_PIXELTYPE_PACKED32,
    SDL_PIXELTYPE_ARRAYU8,
    SDL_PIXELTYPE_ARRAYU16,
    SDL_PIXELTYPE_ARRAYU32,
    SDL_PIXELTYPE_ARRAYF16,
    SDL_PIXELTYPE_ARRAYF32
} SDL_PixelType;

typedef enum {
    SDL_BITMAPORDER_NONE,
    SDL_BITMAPORDER_4321,
    SDL_BITMAPORDER_1234
} SDL_BitmapOrder;

typedef enum {
    SDL_PACKEDORDER_NONE,
    SDL_PACKEDORDER_XRGB,
    SDL_PACKEDORDER_RGBX,
    SDL_PACKEDORDER_ARGB,
    SDL_PACKEDORDER_RGBA,
    SDL_PACKEDORDER_XBGR,
    SDL_PACKEDORDER_BGRX,
    SDL_PACKEDORDER_ABGR,
    SDL_PACKEDORDER_BGRA
} SDL_PackedOrder;


/* !!! FIXME: in 2.1, make these not overlap differently with
   !!! FIXME:  SDL_PACKEDORDER_*, so we can simplify SDL_ISPIXELFORMAT_ALPHA */
typedef enum {
    SDL_ARRAYORDER_NONE,
    SDL_ARRAYORDER_RGB,
    SDL_ARRAYORDER_RGBA,
    SDL_ARRAYORDER_ARGB,
    SDL_ARRAYORDER_BGR,
    SDL_ARRAYORDER_BGRA,
    SDL_ARRAYORDER_ABGR
} SDL_ArrayOrder;


typedef enum {
    SDL_PACKEDLAYOUT_NONE,
    SDL_PACKEDLAYOUT_332,
    SDL_PACKEDLAYOUT_4444,
    SDL_PACKEDLAYOUT_1555,
    SDL_PACKEDLAYOUT_5551,
    SDL_PACKEDLAYOUT_565,
    SDL_PACKEDLAYOUT_8888,
    SDL_PACKEDLAYOUT_2101010,
    SDL_PACKEDLAYOUT_1010102
} SDL_PackedLayout;

#define SDL_DEFINE_PIXELFOURCC(A, B, C, D) SDL_FOURCC(A, B, C, D)

#define SDL_DEFINE_PIXELFORMAT(type, order, layout, bits, bytes) \
    ((1 << 28) | ((type) << 24) | ((order) << 20) | ((layout) << 16) | \
     ((bits) << 8) | ((bytes) << 0))

#endif
