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

static int freefont(lua_State *L, ud_t *ud)
    {
    font_t *font = (font_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "font")) return 0;
    TTF_CloseFont(font);
    return 0;
    }

static int newfont(lua_State *L, font_t *font)
    {
    ud_t *ud;
    ud = newuserdata(L, font, FONT_MT, "font");
    ud->parent_ud = NULL;
    ud->destructor = freefont;
    return 1;
    }

static int OpenFont(lua_State *L)
    {
    font_t *font;
    const char *file = luaL_checkstring(L, 1);
    int ptsize = luaL_checkinteger(L, 2);
    if(lua_isnoneornil(L, 3))
        font = TTF_OpenFont(file, ptsize);
    else
        {
        long index = luaL_checkinteger(L, 3);
        font = TTF_OpenFontIndex(file, ptsize, index);
        }
    if(font==NULL) GetError(L);
    return newfont(L, font);
    }

static int OpenFontDPI(lua_State *L)
    {
    font_t *font;
    const char *file = luaL_checkstring(L, 1);
    int ptsize = luaL_checkinteger(L, 2);
    unsigned int hdpi = luaL_checkinteger(L, 3);
    unsigned int vdpi = luaL_checkinteger(L, 4);
    if(lua_isnoneornil(L, 5))
        font = TTF_OpenFontDPI(file, ptsize, hdpi, vdpi);
    else
        {
        long index = luaL_checkinteger(L, 5);
        font = TTF_OpenFontIndexDPI(file, ptsize, index, hdpi, vdpi);
        }
    if(font==NULL) GetError(L);
    return newfont(L, font);
    }

static int SetSize(lua_State *L)
    {
    int ec;
    font_t *font = checkfont(L, 1, NULL);
    int ptsize = luaL_checkinteger(L, 2);
    if(lua_isnoneornil(L, 3))
        ec = TTF_SetFontSize(font, ptsize);
    else
        {
        unsigned int hdpi = luaL_checkinteger(L, 3);
        unsigned int vdpi = luaL_checkinteger(L, 4);
        ec = TTF_SetFontSizeDPI(font, ptsize, hdpi, vdpi);
        }
    CheckError(L, ec);
    return 0;
    }

static int GetStyle(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    pushflags(L, TTF_GetFontStyle(font));
    return 1;
    }

static int SetStyle(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    int style = checkflags(L, 2); // ttfstyleflags
    TTF_SetFontStyle(font, style);
    return 0;
    }

static int GetOutline(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    lua_pushinteger(L, TTF_GetFontOutline(font));
    return 1;
    }

static int SetOutline(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    int outline = luaL_checkinteger(L, 2);
    TTF_SetFontOutline(font, outline);
    return 0;
    }

static int GetHinting(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    pushfonthinting(L, TTF_GetFontHinting(font));
    return 1;
    }

static int SetHinting(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    int hinting = checkfonthinting(L, 2);
    TTF_SetFontHinting(font, hinting);
    return 0;
    }

#define F(Func, func)                       \
static int Func(lua_State *L)               \
    {                                       \
    font_t *font = checkfont(L, 1, NULL);   \
    lua_pushinteger(L, func(font));         \
    return 1;                               \
    }
F(Height, TTF_FontHeight)
F(Ascent, TTF_FontAscent)
F(Descent, TTF_FontDescent)
F(LineSkip, TTF_FontLineSkip)
F(Faces, TTF_FontFaces)
#undef F


static int GetKerning(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    lua_pushboolean(L, TTF_GetFontKerning(font));
    return 1;
    }

static int SetKerning(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    int allowed = checkboolean(L, 2);
    TTF_SetFontKerning(font, allowed);
    return 0;
    }

static int FaceIsFixedWidth(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    lua_pushboolean(L, TTF_FontFaceIsFixedWidth(font));
    return 1;
    }

static int FaceFamilyName(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    const char *name = TTF_FontFaceFamilyName(font);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

static int FaceStyleName(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    const char *name = TTF_FontFaceStyleName(font);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

static int GlyphIsProvided32(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    uint32_t ch = luaL_checkinteger(L, 2);
    lua_pushboolean(L, TTF_GlyphIsProvided32(font, ch));
    return 1;
    }

static int GlyphMetrics32(lua_State *L)
    {
    int minx, maxx, miny, maxy, advance;
    font_t *font = checkfont(L, 1, NULL);
    uint32_t ch = luaL_checkinteger(L, 2);
    int ec = TTF_GlyphMetrics32(font, ch, &minx, &maxx, &miny, &maxy, &advance);
    CheckError(L, ec);
    lua_pushinteger(L, minx);
    lua_pushinteger(L, maxx);
    lua_pushinteger(L, miny);
    lua_pushinteger(L, maxy);
    lua_pushinteger(L, advance);
    return 5;
    }

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    int w, h;                                       \
    font_t *font = checkfont(L, 1, NULL);           \
    const char *text = luaL_checkstring(L, 2);      \
    int ec = func(font, text, &w, &h);              \
    CheckError(L, ec);                              \
    lua_pushinteger(L, w);                          \
    lua_pushinteger(L, h);                          \
    return 2;                                       \
    }
F(SizeText, TTF_SizeText)
F(SizeUTF8, TTF_SizeUTF8)
#undef F

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    int extent, count;                              \
    font_t *font = checkfont(L, 1, NULL);           \
    const char *text = luaL_checkstring(L, 2);      \
    int measure_width = luaL_checkinteger(L, 3);    \
    int ec = func(font, text, measure_width, &extent, &count);\
    CheckError(L, ec);                              \
    lua_pushinteger(L, extent);                     \
    lua_pushinteger(L, count);                      \
    return 2;                                       \
    }
F(MeasureText, TTF_MeasureText)
F(MeasureUTF8, TTF_MeasureUTF8)
#undef F

#define F(Func, func, wfunc)                        \
static int Func(lua_State *L)                       \
    {                                               \
    color_t fg;                                     \
    surface_t *surface;                             \
    font_t *font = checkfont(L, 1, NULL);           \
    const char *text = luaL_checkstring(L, 2);      \
    checkcolor(L, 3, &fg);                          \
    if(lua_isnoneornil(L, 4))                       \
        surface = func(font, text, fg);             \
    else                                            \
        {                                           \
        uint32_t wraplen = luaL_checkinteger(L, 4); \
        surface = wfunc(font, text, fg, wraplen);   \
        }                                           \
    if(!surface) GetError(L);                       \
    return newsurface(L, surface, UD(font));        \
    }
F(RenderText_Solid, TTF_RenderText_Solid, TTF_RenderText_Solid_Wrapped)
F(RenderUTF8_Solid, TTF_RenderUTF8_Solid, TTF_RenderUTF8_Solid_Wrapped)
F(RenderText_Blended, TTF_RenderText_Blended, TTF_RenderText_Blended_Wrapped)
F(RenderUTF8_Blended, TTF_RenderUTF8_Blended, TTF_RenderUTF8_Blended_Wrapped)
#undef F

#define F(Func, func, wfunc)                        \
static int Func(lua_State *L)                       \
    {                                               \
    color_t fg, bg;                                 \
    surface_t *surface;                             \
    font_t *font = checkfont(L, 1, NULL);           \
    const char *text = luaL_checkstring(L, 2);      \
    checkcolor(L, 3, &fg);                          \
    checkcolor(L, 4, &bg);                          \
    if(lua_isnoneornil(L, 5))                       \
        surface = func(font, text, fg, bg);         \
    else                                            \
        {                                           \
        uint32_t wraplen = luaL_checkinteger(L, 5); \
        surface = wfunc(font, text, fg, bg, wraplen);\
        }                                           \
    if(!surface) GetError(L);                       \
    return newsurface(L, surface, UD(font));        \
    }
F(RenderText_Shaded, TTF_RenderText_Shaded, TTF_RenderText_Shaded_Wrapped)
F(RenderUTF8_Shaded, TTF_RenderUTF8_Shaded, TTF_RenderUTF8_Shaded_Wrapped)
F(RenderText_LCD, TTF_RenderText_LCD, TTF_RenderText_LCD_Wrapped)
F(RenderUTF8_LCD, TTF_RenderUTF8_LCD, TTF_RenderUTF8_LCD_Wrapped)
#undef F

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    color_t fg;                                     \
    surface_t *surface;                             \
    font_t *font = checkfont(L, 1, NULL);           \
    uint32_t ch = luaL_checkinteger(L, 2);          \
    checkcolor(L, 3, &fg);                          \
    surface = func(font, ch, fg);                   \
    if(!surface) GetError(L);                       \
    return newsurface(L, surface, UD(font));        \
    }
F(RenderGlyph32_Solid, TTF_RenderGlyph32_Solid)
F(RenderGlyph32_Blended, TTF_RenderGlyph32_Blended)
#undef F

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    color_t fg, bg;                                 \
    surface_t *surface;                             \
    font_t *font = checkfont(L, 1, NULL);           \
    uint32_t ch = luaL_checkinteger(L, 2);          \
    checkcolor(L, 3, &fg);                          \
    checkcolor(L, 4, &bg);                          \
    surface = func(font, ch, fg, bg);               \
    if(!surface) GetError(L);                       \
    return newsurface(L, surface, UD(font));        \
    }
F(RenderGlyph32_Shaded, TTF_RenderGlyph32_Shaded)
F(RenderGlyph32_LCD, TTF_RenderGlyph32_LCD)
#undef F

static int GetKerningSizeGlyphs32(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    uint32_t previous_ch = luaL_checkinteger(L, 2);
    uint32_t ch = luaL_checkinteger(L, 3);
    int sz = TTF_GetFontKerningSizeGlyphs32(font, previous_ch, ch);
    if(sz==-1) GetError(L);
    lua_pushinteger(L, sz);
    return 1;
    }

static int SetSDF(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    int onoff = checkboolean(L, 2);
    int ec = TTF_SetFontSDF(font, onoff);
    CheckError(L, ec);
    return 0;
    }

static int GetSDF(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    lua_pushboolean(L, TTF_GetFontSDF(font));
    return 1;
    }

static int SetDirection(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    int dir = checkfontdirection(L, 2);
    if(TTF_SetFontDirection(font, dir) !=0) GetError(L);
    return 0;
    }

static int SetScriptName(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    const char *script = luaL_checkstring(L, 2);
    if(TTF_SetFontScriptName(font, script) !=0) GetError(L);
    return 0;
    }

static int SetWrappedAlign(lua_State *L)
    {
    font_t *font = checkfont(L, 1, NULL);
    int align = checkfontalign(L, 2);
    TTF_SetFontWrappedAlign(font, align);
    return 0;
    }

DESTROY_FUNC(font)

static const struct luaL_Reg Methods[] = 
    {
        { "close", Destroy },
        { "set_size", SetSize },
        { "get_style", GetStyle },
        { "set_style", SetStyle },
        { "get_outline", GetOutline },
        { "set_outline", SetOutline },
        { "get_hinting", GetHinting },
        { "set_hinting", SetHinting },
        { "height", Height },
        { "ascent", Ascent },
        { "descent", Descent },
        { "line_skip", LineSkip },
        { "get_kerning", GetKerning },
        { "set_kerning", SetKerning },
        { "faces", Faces },
        { "face_is_fixed_width", FaceIsFixedWidth },
        { "face_family_name", FaceFamilyName },
        { "face_style_name", FaceStyleName },
        { "glyph_is_provided", GlyphIsProvided32 },
        { "glyph_metrics", GlyphMetrics32 },
        { "size_text", SizeText },
        { "size_utf8", SizeUTF8 },
        { "measure_text", MeasureText },
        { "measure_utf8", MeasureUTF8 },
        { "render_text_solid", RenderText_Solid },
        { "render_utf8_solid", RenderUTF8_Solid },
        { "render_glyph_solid", RenderGlyph32_Solid },
        { "render_text_shaded", RenderText_Shaded },
        { "render_utf8_shaded", RenderUTF8_Shaded },
        { "render_glyph_shaded", RenderGlyph32_Shaded },
        { "render_text_blended", RenderText_Blended },
        { "render_utf8_blended", RenderUTF8_Blended },
        { "render_glyph_blended", RenderGlyph32_Blended },
//      { "render_text", RenderText_Shaded },
//      { "render_utf8", RenderUTF8_Shaded },
        { "render_text_lcd", RenderText_LCD },
        { "render_utf8_lcd", RenderUTF8_LCD },
        { "render_glyph_lcd", RenderGlyph32_LCD },
        { "get_kerning_size", GetKerningSizeGlyphs32 },
        { "set_sdf", SetSDF },
        { "get_sdf", GetSDF },
        { "set_direction", SetDirection },
        { "set_script_name", SetScriptName },
        { "set_wrapped_align", SetWrappedAlign },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "open_font", OpenFont },
        { "open_font_dpi", OpenFontDPI },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_font(lua_State *L)
    {
    udata_define(L, FONT_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

#if 0
void TTF_GetFreeTypeVersion(int *major, int *minor, int *patch);
void TTF_GetHarfBuzzVersion(int *major, int *minor, int *patch);
TTF_Font *TTF_OpenFontRW(SDL_RWops *src, int freesrc, int ptsize);
TTF_Font *TTF_OpenFontIndexRW(SDL_RWops *src, int freesrc, int ptsize, long index);
TTF_Font *TTF_OpenFontDPIRW(SDL_RWops *src, int freesrc, int ptsize, unsigned int hdpi, unsigned int vdpi);
TTF_Font *TTF_OpenFontIndexDPIRW(SDL_RWops *src, int freesrc, int ptsize, long index, unsigned int hdpi, unsigned int vdpi);
#define UNICODE_BOM_NATIVE  0xFEFF
#define UNICODE_BOM_SWAPPED 0xFFFE

//surface_t * TTF_RenderUNICODE_LCD(TTF_Font *font, const Uint16 *text, SDL_Color fg, SDL_Color bg);
//surface_t * TTF_RenderUNICODE_LCD_Wrapped(TTF_Font *font, const Uint16 *text, SDL_Color fg, SDL_Color bg, Uint32 wrapLength);
//surface_t * TTF_RenderGlyph_LCD(TTF_Font *font, Uint16 ch, SDL_Color fg, SDL_Color bg);

#endif
