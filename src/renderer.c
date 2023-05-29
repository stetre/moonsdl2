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

static int pushrendererinfo(lua_State *L, const SDL_RendererInfo *info)
    {
    uint32_t i;
    lua_newtable(L);
    if(info->name)
        { lua_pushstring(L, info->name); lua_setfield(L, -2, "name"); }
    pushflags(L, info->flags); lua_setfield(L, -2, "flags");
    lua_newtable(L);
    for(i=0; i<info->num_texture_formats; i++)
        {
        pushpixelformat(L, info->texture_formats[i]);
        lua_rawseti(L, -2, i+1);
        }
    lua_setfield(L, -2, "texture_formats");
    lua_pushinteger(L, info->max_texture_width); lua_setfield(L, -2, "max_texture_width");
    lua_pushinteger(L, info->max_texture_height); lua_setfield(L, -2, "max_texture_height");
    return 1;
    }

static int freerenderer(lua_State *L, ud_t *ud)
    {
    renderer_t *renderer = (renderer_t*)ud->handle;
//  freechildren(L, _MT, ud);
    freechildren(L, TEXTURE_MT, ud);
    if(!freeuserdata(L, ud, "renderer")) return 0;
    SDL_DestroyRenderer(renderer);
    return 0;
    }

static int newrenderer(lua_State *L, renderer_t *renderer, ud_t *parent_ud)
    {
    ud_t *ud;
    ud = newuserdata(L, renderer, RENDERER_MT, "renderer");
    ud->parent_ud = parent_ud;
    ud->destructor = freerenderer;
    return 1;
    }

static int CreateRenderer(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    int index = optindex(L, 2, -1);
    uint32_t flags = optflags(L, 3, 0);
    renderer_t *renderer = SDL_CreateRenderer(window, index, flags);
    if(!renderer) GetError(L);
    return newrenderer(L, renderer, UD(window));
    }

static int CreateSoftwareRenderer(lua_State *L)
    {
    surface_t *surface = checksurface(L, 1, NULL);
    renderer_t *renderer = SDL_CreateSoftwareRenderer(surface);
    if(!renderer) GetError(L);
    return newrenderer(L, renderer, UD(surface));
    }

static int GetInfo(lua_State *L)
    {
    SDL_RendererInfo info;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = SDL_GetRendererInfo(renderer, &info);
    CheckError(L, ec);
    return pushrendererinfo(L, &info);  
    }

static int GetWindow(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    window_t *window = SDL_RenderGetWindow(renderer);
    if(!window) GetError(L);
    pushwindow(L, window);
    return 1;   
    }

static int GetOutputSize(lua_State *L)
    {
    int w, h;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = SDL_GetRendererOutputSize(renderer, &w, &h);
    CheckError(L, ec);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
    }

static int TargetSupported(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    lua_pushboolean(L, SDL_RenderTargetSupported(renderer));
    return 1;
    }

static int SetTarget(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    texture_t *texture = opttexture(L, 2, NULL);
    int ec = SDL_SetRenderTarget(renderer, texture);
    CheckError(L, ec);
    return 0;
    }

static int GetTarget(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    texture_t *texture = SDL_GetRenderTarget(renderer);
    if(!texture) return 0;
    pushtexture(L, texture);
    return 1;
    }

static int SetLogicalSize(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int w = luaL_checkinteger(L, 2);
    int h = luaL_checkinteger(L, 3);
    int ec = SDL_RenderSetLogicalSize(renderer, w, h);
    CheckError(L, ec);
    return 0;
    }

static int GetLogicalSize(lua_State *L)
    {
    int w, h;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    SDL_RenderGetLogicalSize(renderer, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
    }

static int SetIntegerScale(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    bool_t enable = checkboolean(L, 2);
    int ec = SDL_RenderSetIntegerScale(renderer, enable);
    CheckError(L, ec);
    return 0;
    }

static int GetIntegerScale(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    lua_pushboolean(L, SDL_RenderGetIntegerScale(renderer));
    return 1;
    }

static int SetViewport(lua_State *L)
    {
    rect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = optrect(L, 2, &rect);
    ec = SDL_RenderSetViewport(renderer, (ec==ERR_NOTPRESENT)? NULL : &rect);
    CheckError(L, ec);
    return 0;
    }

static int GetViewport(lua_State *L)
    {
    rect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    SDL_RenderGetViewport(renderer, &rect);
    pushrect(L, &rect);
    return 1;
    }

static int SetClipRect(lua_State *L)
    {
    rect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = optrect(L, 2, &rect);
    ec = SDL_RenderSetClipRect(renderer, (ec==ERR_NOTPRESENT)? NULL : &rect);
    CheckError(L, ec);
    return 0;
    }

static int GetClipRect(lua_State *L)
    {
    rect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    SDL_RenderGetClipRect(renderer, &rect);
    pushrect(L, &rect);
    return 1;
    }

static int IsClipEnabled(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    lua_pushboolean(L, SDL_RenderIsClipEnabled(renderer));
    return 1;
    }

static int SetScale(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    int ec = SDL_RenderSetScale(renderer, x, y);
    CheckError(L, ec);
    return 0;
    }

static int GetScale(lua_State *L)
    {
    float x, y;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    SDL_RenderGetScale(renderer, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
    }

static int WindowToLogical(lua_State *L)
    {
    float logicalx, logicaly;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int windowx = luaL_checkinteger(L, 2);
    int windowy = luaL_checkinteger(L, 3);
    SDL_RenderWindowToLogical(renderer, windowx, windowy, &logicalx, &logicaly);
    lua_pushnumber(L, logicalx);
    lua_pushnumber(L, logicaly);
    return 2;
    }

static int LogicalToWindow(lua_State *L)
    {
    int windowx, windowy;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    float logicalx = luaL_checknumber(L, 2);
    float logicaly = luaL_checknumber(L, 3);
    SDL_RenderLogicalToWindow(renderer, logicalx, logicaly, &windowx, &windowy);
    lua_pushinteger(L, windowx);
    lua_pushinteger(L, windowy);
    return 2;
    }

static int SetDrawColor(lua_State *L)
    {
    int ec;
    color_t color;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    checkcolor(L, 2, &color);
    ec = SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    CheckError(L, ec);
    return 0;
    }

static int GetDrawColor(lua_State *L)
    {
    color_t color;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
    CheckError(L, ec);
    pushcolor(L, &color);
    return 1;
    }

static int SetDrawBlendMode(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    SDL_BlendMode mode = luaL_checkinteger(L, 2);
    int ec = SDL_SetRenderDrawBlendMode(renderer, mode);
    CheckError(L, ec);
    return 0;
    }

static int GetDrawBlendMode(lua_State *L)
    {
    int ec;
    SDL_BlendMode mode;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    ec = SDL_GetRenderDrawBlendMode(renderer, &mode);
    CheckError(L, ec);
    lua_pushinteger(L, mode);
    return 1;
    }

static int Clear(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = SDL_RenderClear(renderer);
    CheckError(L, ec);
    return 0;
    }

#if 0 // the float versions work also on integers
static int DrawPoint(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int ec = SDL_RenderDrawPoint(renderer, x, y);
    CheckError(L, ec);
    return 0;
    }

static int DrawPoints(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    point_t *points = checkpointlist(L, 2, &count, NULL);
    int ec = SDL_RenderDrawPoints(renderer, points, count);
    Free(L, points);
    CheckError(L, ec);
    return 0;
    }

static int DrawLine(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int x1 = luaL_checkinteger(L, 2);
    int y1 = luaL_checkinteger(L, 3);
    int x2 = luaL_checkinteger(L, 4);
    int y2 = luaL_checkinteger(L, 5);
    int ec = SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    CheckError(L, ec);
    return 0;
    }

static int DrawLines(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    point_t *points = checkpointlist(L, 2, &count, NULL);
    int ec = SDL_RenderDrawLines(renderer, points, count);
    Free(L, points);
    CheckError(L, ec);
    return 0;
    }

static int DrawRect(lua_State *L)
    {
    rect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    checkrect(L, 2, &rect);
    int ec = SDL_RenderDrawRect(renderer, &rect);
    CheckError(L, ec);
    return 0;
    }

static int DrawRects(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    rect_t *rects = checkrectlist(L, 2, &count, NULL);
    int ec = SDL_RenderDrawRects(renderer, rects, count);
    Free(L, rects);
    CheckError(L, ec);
    return 0;
    }

static int FillRect(lua_State *L)
    {
    rect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = optrect(L, 2, &rect);
    ec = SDL_RenderFillRect(renderer, (ec==ERR_NOTPRESENT) ? NULL: &rect);
    CheckError(L, ec);
    return 0;
    }

static int FillRects(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    rect_t *rects = checkrectlist(L, 2, &count, NULL);
    int ec = SDL_RenderFillRects(renderer, rects, count);
    Free(L, rects);
    CheckError(L, ec);
    return 0;
    }

static int Copy(lua_State *L)
    {
    rect_t src, dst;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    texture_t *texture = checktexture(L, 2, NULL);
    int srcpres = optrect(L, 3, &src);
    int dstpres = optrect(L, 4, &dst);
    int ec = SDL_RenderCopy(renderer, texture,
            (srcpres==ERR_NOTPRESENT) ? NULL : &src, (dstpres==ERR_NOTPRESENT) ? NULL : &dst);
    CheckError(L, ec);
    return 0;
    }

static int CopyEx(lua_State *L)
    {
    rect_t src, dst;
    point_t center;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    texture_t *texture = checktexture(L, 2, NULL);
    int srcpres = optrect(L, 3, &src);
    int dstpres = optrect(L, 4, &dst);
    double angle = luaL_checknumber(L, 5);
    int centerpres = optpoint(L, 6, &center);
    SDL_RendererFlip flip = optflags(L, 7, 0);
    int ec = SDL_RenderCopyEx(renderer, texture,
            (srcpres==ERR_NOTPRESENT) ? NULL : &src, (dstpres==ERR_NOTPRESENT) ? NULL : &dst,
            angle, (centerpres==ERR_NOTPRESENT) ? NULL : &center, flip);
    CheckError(L, ec);
    return 0;
    }
#endif

static int DrawPointF(lua_State *L)
    {
    int ec;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    if(lua_istable(L, 2))
        {
        fpoint_t point;
        checkfpoint(L, 2, &point);
        ec = SDL_RenderDrawPointF(renderer, point.x, point.y);
        }
    else
        {
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        ec = SDL_RenderDrawPointF(renderer, x, y);
        }
    CheckError(L, ec);
    return 0;
    }

static int DrawPointsF(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    fpoint_t *points = checkfpointlist(L, 2, &count, NULL);
    int ec = SDL_RenderDrawPointsF(renderer, points, count);
    Free(L, points);
    CheckError(L, ec);
    return 0;
    }

static int DrawLineF(lua_State *L)
    {
    int ec;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    if(lua_istable(L, 2))
        {
        fpoint_t point1, point2;
        checkfpoint(L, 2, &point1);
        checkfpoint(L, 3, &point2);
        ec = SDL_RenderDrawLineF(renderer, point1.x, point1.y, point2.x, point2.y);
        }
    else
        {
        float x1 = luaL_checknumber(L, 2);
        float y1 = luaL_checknumber(L, 3);
        float x2 = luaL_checknumber(L, 4);
        float y2 = luaL_checknumber(L, 5);
        ec = SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
        }
    CheckError(L, ec);
    return 0;
    }

static int DrawLinesF(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    fpoint_t *points = checkfpointlist(L, 2, &count, NULL);
    int ec = SDL_RenderDrawLinesF(renderer, points, count);
    Free(L, points);
    CheckError(L, ec);
    return 0;
    }

static int DrawRectF(lua_State *L)
    {
    int ec;
    frect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    checkfrect(L, 2, &rect);
    ec = SDL_RenderDrawRectF(renderer, &rect);
    CheckError(L, ec);
    return 0;
    }

static int DrawRectsF(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    frect_t *rects = checkfrectlist(L, 2, &count, NULL);
    int ec = SDL_RenderDrawRectsF(renderer, rects, count);
    Free(L, rects);
    CheckError(L, ec);
    return 0;
    }

static int FillRectF(lua_State *L)
    {
    frect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = optfrect(L, 2, &rect);
    ec = SDL_RenderFillRectF(renderer, (ec==ERR_NOTPRESENT) ? NULL: &rect);
    CheckError(L, ec);
    return 0;
    }

static int FillRectsF(lua_State *L)
    {
    int count;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    frect_t *rects = checkfrectlist(L, 2, &count, NULL);
    int ec = SDL_RenderFillRectsF(renderer, rects, count);
    Free(L, rects);
    CheckError(L, ec);
    return 0;
    }

static int CopyF(lua_State *L)
    {
    rect_t src;
    frect_t dst;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    texture_t *texture = checktexture(L, 2, NULL);
    int srcpres = optrect(L, 3, &src);
    int dstpres = optfrect(L, 4, &dst);
    int ec = SDL_RenderCopyF(renderer, texture,
            (srcpres==ERR_NOTPRESENT) ? NULL : &src, (dstpres==ERR_NOTPRESENT) ? NULL : &dst);
    CheckError(L, ec);
    return 0;
    }

static int CopyExF(lua_State *L)
    {
    rect_t src;
    frect_t dst;
    fpoint_t center;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    texture_t *texture = checktexture(L, 2, NULL);
    int srcpres = optrect(L, 3, &src);
    int dstpres = optfrect(L, 4, &dst);
    double angle = luaL_optnumber(L, 5, 0);
    int centerpres = optfpoint(L, 6, &center);
    SDL_RendererFlip flip = optflags(L, 7, 0);
    int ec = SDL_RenderCopyExF(renderer, texture,
            (srcpres==ERR_NOTPRESENT) ? NULL : &src, (dstpres==ERR_NOTPRESENT) ? NULL : &dst,
            angle, (centerpres==ERR_NOTPRESENT) ? NULL : &center, flip);
    CheckError(L, ec);
    return 0;
    }

static int RenderGeometry(lua_State *L)
    {
    int err;
    int num_vertices, num_indices;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    texture_t *texture = opttexture(L, 2, NULL);
    vertex_t *vertices = checkvertexlist(L, 3, &num_vertices, NULL);
    int *indices = checkintlist(L, 4, &num_indices, &err);
    if(err < 0)
        { Free(L, vertices); return argerror(L, 4, err); }
    //if(err == ERR_NOTPRESENT) { indices = NULL; num_indices=0; } not needed
    err = SDL_RenderGeometry(renderer, texture, vertices, num_vertices, indices, num_indices);
    Free(L, vertices);
    Free(L, indices);
    if(err!=0) GetError(L);
    return 0;
    }

#if 0
//@@int SDL_RenderGeometryRaw(SDL_Renderer *renderer, SDL_Texture *texture, const float *xy, int xy_stride, const SDL_Color *color, int color_stride, const float *uv, int uv_stride, int num_vertices, const void *indices, int num_indices, int size_indices);
static int RenderGeometryRaw(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    (void)renderer;
    return 0;
    }
#endif

static int ReadPixels(lua_State *L)
    {
    rect_t rect;
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = optrect(L, 2, &rect);
    uint32_t format = optpixelformat(L, 3, 0);
    void *pixels = checklightuserdata(L, 4);
    int pitch = luaL_checkinteger(L, 5);
    if(SDL_RenderReadPixels(renderer, (ec==ERR_NOTPRESENT)? NULL : &rect, format, pixels, pitch)!=0)
        GetError(L);
    return 0;
    }

static int Present(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    SDL_RenderPresent(renderer);
    return 0;
    }

static int Flush(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int ec = SDL_RenderFlush(renderer);
    CheckError(L, ec);
    return 0;
    }

static int SetVSync(lua_State *L)
    {
    renderer_t *renderer = checkrenderer(L, 1, NULL);
    int vsync = checkboolean(L, 2);
    int ec = SDL_RenderSetVSync(renderer, vsync);
    CheckError(L, ec);
    return 0;
    }

static int GetRenderDriversInfo(lua_State *L)
    {
    SDL_RendererInfo info;
    int n = SDL_GetNumRenderDrivers();
    if(n < 0) GetError(L);
    lua_newtable(L);
    for(int i=0; i<n; i++)
        {
        if(SDL_GetRenderDriverInfo(i, &info) != 0) GetError(L);
        pushrendererinfo(L, &info);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }



DESTROY_FUNC(renderer)

static const struct luaL_Reg Methods[] = 
    {
        { "destroy", Destroy },
        { "get_info", GetInfo },
        { "get_window", GetWindow },
        { "get_output_size", GetOutputSize },
        { "target_supported", TargetSupported },
        { "set_target", SetTarget },
        { "get_target", GetTarget },
        { "set_logical_size", SetLogicalSize },
        { "get_logical_size", GetLogicalSize },
        { "set_integer_scale", SetIntegerScale },
        { "get_integer_scale", GetIntegerScale },
        { "set_viewport", SetViewport },
        { "get_viewport", GetViewport },
        { "set_clip_rect", SetClipRect },
        { "get_clip_rect", GetClipRect },
        { "is_clip_enabled", IsClipEnabled },
        { "set_scale", SetScale },
        { "get_scale", GetScale },
        { "window_to_logical", WindowToLogical },
        { "logical_to_window", LogicalToWindow },
        { "set_draw_color", SetDrawColor },
        { "get_draw_color", GetDrawColor },
        { "set_draw_blend_mode", SetDrawBlendMode },
        { "get_draw_blend_mode", GetDrawBlendMode },
        { "clear", Clear },
//      { "draw_point", DrawPoint },
//      { "draw_points", DrawPoints },
//      { "draw_line", DrawLine },
//      { "draw_lines", DrawLines },
//      { "draw_rect", DrawRect },
//      { "draw_rects", DrawRects },
//      { "fill_rect", FillRect },
//      { "fill_rects", FillRects },
//      { "copy", Copy },
//      { "copy_ex", CopyEx },
        { "draw_point", DrawPointF },
        { "draw_points", DrawPointsF },
        { "draw_line", DrawLineF },
        { "draw_lines", DrawLinesF },
        { "draw_rect", DrawRectF },
        { "draw_rects", DrawRectsF },
        { "fill_rect", FillRectF },
        { "fill_rects", FillRectsF },
        { "copy", CopyF },
        { "copy_ex", CopyExF },
        { "render_geometry", RenderGeometry },
//      { "render_geometry_raw", RenderGeometryRaw },
        { "read_pixels", ReadPixels },
        { "present", Present },
        { "flush", Flush },
        { "set_vsync", SetVSync },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_renderer", CreateRenderer },
        { "create_software_renderer", CreateSoftwareRenderer },
        { "get_render_drivers_info", GetRenderDriversInfo },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_renderer(lua_State *L)
    {
    udata_define(L, RENDERER_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }


#if 0
//void *SDL_RenderGetMetalLayer(SDL_Renderer *renderer);
//void *SDL_RenderGetMetalCommandEncoder(SDL_Renderer *renderer);

typedef struct SDL_Vertex {
    SDL_FPoint position;        
    SDL_Color  color;           
    SDL_FPoint tex_coord;       
} SDL_Vertex;

#endif
