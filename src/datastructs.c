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

/* uint16_t list ---------------------------------------------------------*/

uint16_t* checkuint16list(lua_State *L, int arg, int *count, int *err)
    {
    uint16_t* list;
    int i;

    *count = 0;
    *err = 0;
    if(lua_isnoneornil(L, arg))
        { *err = ERR_NOTPRESENT; return NULL; }
    if(lua_type(L, arg) != LUA_TTABLE)
        { *err = ERR_TABLE; return NULL; }

    *count = luaL_len(L, arg);
    if(*count == 0)
        { *err = ERR_EMPTY; return NULL; }

    list = (uint16_t*)MallocNoErr(L, sizeof(uint16_t) * (*count));
    if(!list)
        { *count = 0; *err = ERR_MEMORY; return NULL; }

    for(i=0; i<*count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(!lua_isinteger(L, -1))
            { lua_pop(L, 1); Free(L, list); *count = 0; *err = ERR_TYPE; return NULL; }
        list[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
        }
    return list;
    }

void pushuint16list(lua_State *L, uint16_t *list, int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        lua_pushinteger(L, list[i]);
        lua_rawseti(L, -2, i+1);
        }
    }


/* int list ---------------------------------------------------------*/

int* checkintlist(lua_State *L, int arg, int *count, int *err)
    {
    int* list;
    int i;

    *count = 0;
    *err = 0;
    if(lua_isnoneornil(L, arg))
        { *err = ERR_NOTPRESENT; return NULL; }
    if(lua_type(L, arg) != LUA_TTABLE)
        { *err = ERR_TABLE; return NULL; }

    *count = luaL_len(L, arg);
    if(*count == 0)
        { *err = ERR_EMPTY; return NULL; }

    list = (int*)MallocNoErr(L, sizeof(int) * (*count));
    if(!list)
        { *count = 0; *err = ERR_MEMORY; return NULL; }

    for(i=0; i<*count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(!lua_isinteger(L, -1))
            { lua_pop(L, 1); Free(L, list); *count = 0; *err = ERR_TYPE; return NULL; }
        list[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
        }
    return list;
    }

void pushintlist(lua_State *L, int *list, int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        lua_pushinteger(L, list[i]);
        lua_rawseti(L, -2, i+1);
        }
    }


/* color_t ----------------------------------------------------------*/

int testcolor(lua_State *L, int arg, color_t *dst)
    {
    int isnum;
    int t = lua_type(L, arg);
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
#define POP if(!isnum) { lua_pop(L, 1); return ERR_VALUE; } lua_pop(L, 1);
    lua_rawgeti(L, arg, 1); dst->r = lua_tointegerx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 2); dst->g = lua_tointegerx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 3); dst->b = lua_tointegerx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 4);
    if(lua_isnoneornil(L, -1))
        { dst->a = 0xff; lua_pop(L, 1); }
    else
        { dst->a = lua_tointegerx(L, -1, &isnum); POP }
#undef POP
    return 0;
    }

int optcolor(lua_State *L, int arg, color_t *dst)
    {
    int ec = testcolor(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkcolor(lua_State *L, int arg, color_t *dst)
    {
    int ec = testcolor(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushcolor(lua_State *L, const color_t *val)
    {
    lua_newtable(L);
    lua_pushinteger(L, val->r); lua_rawseti(L, -2, 1);
    lua_pushinteger(L, val->g); lua_rawseti(L, -2, 2);
    lua_pushinteger(L, val->b); lua_rawseti(L, -2, 3);
    lua_pushinteger(L, val->a); lua_rawseti(L, -2, 4);
    }

color_t *checkcolorlist(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of colors and returns the corresponding
 * array of color_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    color_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(color_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testcolor(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushcolorlist(lua_State *L, const color_t *colors , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushcolor(L, &colors[i]);
        lua_rawseti(L, -2, i+1);
        }
    }

/* point_t ----------------------------------------------------------*/

int testpoint(lua_State *L, int arg, point_t *dst)
    {
    int isnum;
    int t = lua_type(L, arg);
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
#define POP if(!isnum) { lua_pop(L, 1); return ERR_VALUE; } lua_pop(L, 1);
    lua_rawgeti(L, arg, 1); dst->x = lua_tointegerx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 2); dst->y = lua_tointegerx(L, -1, &isnum); POP
#undef POP
    return 0;
    }

int optpoint(lua_State *L, int arg, point_t *dst)
    {
    int ec = testpoint(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkpoint(lua_State *L, int arg, point_t *dst)
    {
    int ec = testpoint(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushpoint(lua_State *L, const point_t *val)
    {
    lua_newtable(L);
    lua_pushinteger(L, val->x); lua_rawseti(L, -2, 1);
    lua_pushinteger(L, val->y); lua_rawseti(L, -2, 2);
    }

point_t *checkpointlist(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of points and returns the corresponding
 * array of point_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    point_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(point_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testpoint(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushpointlist(lua_State *L, const point_t *points , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushpoint(L, &points[i]);
        lua_rawseti(L, -2, i+1);
        }
    }


/* fpoint_t ----------------------------------------------------------*/

int testfpoint(lua_State *L, int arg, fpoint_t *dst)
    {
    int isnum;
    int t = lua_type(L, arg);
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
#define POP if(!isnum) { lua_pop(L, 1); return ERR_VALUE; } lua_pop(L, 1);
    lua_rawgeti(L, arg, 1); dst->x = lua_tonumberx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 2); dst->y = lua_tonumberx(L, -1, &isnum); POP
#undef POP
    return 0;
    }

int optfpoint(lua_State *L, int arg, fpoint_t *dst)
    {
    int ec = testfpoint(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkfpoint(lua_State *L, int arg, fpoint_t *dst)
    {
    int ec = testfpoint(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushfpoint(lua_State *L, const fpoint_t *val)
    {
    lua_newtable(L);
    lua_pushnumber(L, val->x); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, val->y); lua_rawseti(L, -2, 2);
    }

fpoint_t *checkfpointlist(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of fpoints and returns the corresponding
 * array of fpoint_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    fpoint_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(fpoint_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testfpoint(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushfpointlist(lua_State *L, const fpoint_t *fpoints , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushfpoint(L, &fpoints[i]);
        lua_rawseti(L, -2, i+1);
        }
    }

/* rect_t ----------------------------------------------------------*/

int testrect(lua_State *L, int arg, rect_t *dst)
    {
    int isnum;
    int t = lua_type(L, arg);
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
#define POP if(!isnum) { lua_pop(L, 1); return ERR_VALUE; } lua_pop(L, 1);
    lua_rawgeti(L, arg, 1); dst->x = lua_tointegerx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 2); dst->y = lua_tointegerx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 3); dst->w = lua_tointegerx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 4); dst->h = lua_tointegerx(L, -1, &isnum); POP
#undef POP
    return 0;
    }

int optrect(lua_State *L, int arg, rect_t *dst)
    {
    int ec = testrect(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkrect(lua_State *L, int arg, rect_t *dst)
    {
    int ec = testrect(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushrect(lua_State *L, const rect_t *val)
    {
    lua_newtable(L);
    lua_pushinteger(L, val->x); lua_rawseti(L, -2, 1);
    lua_pushinteger(L, val->y); lua_rawseti(L, -2, 2);
    lua_pushinteger(L, val->w); lua_rawseti(L, -2, 3);
    lua_pushinteger(L, val->h); lua_rawseti(L, -2, 4);
    }

rect_t *checkrectlist(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of rects and returns the corresponding
 * array of rect_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    rect_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(rect_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testrect(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushrectlist(lua_State *L, const rect_t *rects , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushrect(L, &rects[i]);
        lua_rawseti(L, -2, i+1);
        }
    }


/* frect_t ----------------------------------------------------------*/

int testfrect(lua_State *L, int arg, frect_t *dst)
    {
    int isnum;
    int t = lua_type(L, arg);
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
#define POP if(!isnum) { lua_pop(L, 1); return ERR_VALUE; } lua_pop(L, 1);
    lua_rawgeti(L, arg, 1); dst->x = lua_tonumberx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 2); dst->y = lua_tonumberx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 3); dst->w = lua_tonumberx(L, -1, &isnum); POP
    lua_rawgeti(L, arg, 4); dst->h = lua_tonumberx(L, -1, &isnum); POP
#undef POP
    return 0;
    }

int optfrect(lua_State *L, int arg, frect_t *dst)
    {
    int ec = testfrect(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkfrect(lua_State *L, int arg, frect_t *dst)
    {
    int ec = testfrect(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushfrect(lua_State *L, const frect_t *val)
    {
    lua_newtable(L);
    lua_pushnumber(L, val->x); lua_rawseti(L, -2, 1);
    lua_pushnumber(L, val->y); lua_rawseti(L, -2, 2);
    lua_pushnumber(L, val->w); lua_rawseti(L, -2, 3);
    lua_pushnumber(L, val->h); lua_rawseti(L, -2, 4);
    }

frect_t *checkfrectlist(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of frects and returns the corresponding
 * array of frect_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    frect_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(frect_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testfrect(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushfrectlist(lua_State *L, const frect_t *frects , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushfrect(L, &frects[i]);
        lua_rawseti(L, -2, i+1);
        }
    }


/* vertex_t ----------------------------------------------------------*/

int testvertex(lua_State *L, int arg, vertex_t *dst)
    {
    int t = lua_type(L, arg);
    memset(dst, 0, sizeof(vertex_t));
    switch(t)
        {
        case LUA_TNONE:
        case LUA_TNIL:  return ERR_NOTPRESENT;
        case LUA_TTABLE: break;
        default: return ERR_TABLE;
        }
    lua_rawgeti(L, arg, 1); testfpoint(L, -1, &dst->position); lua_pop(L, 1);
    lua_rawgeti(L, arg, 2); testcolor(L, -1, &dst->color); lua_pop(L, 1);
    lua_rawgeti(L, arg, 3); testfpoint(L, -1, &dst->tex_coord); lua_pop(L, 1);
    return 0;
    }

int optvertex(lua_State *L, int arg, vertex_t *dst)
    {
    int ec = testvertex(L, arg, dst);
    if(ec<0) return argerror(L, arg, ec);
    return ec;
    }

int checkvertex(lua_State *L, int arg, vertex_t *dst)
    {
    int ec = testvertex(L, arg, dst);
    if(ec) return argerror(L, arg, ec);
    return ec;
    }

void pushvertex(lua_State *L, const vertex_t *val)
    {
    lua_newtable(L);
    pushfpoint(L, &val->position); lua_rawseti(L, -2, 1);
    pushcolor(L, &val->color); lua_rawseti(L, -2, 2);
    pushfpoint(L, &val->tex_coord); lua_rawseti(L, -2, 3);
    }

vertex_t *checkvertexlist(lua_State *L, int arg, int *countp, int *err)
/* Check if the value at arg is a table of vertices and returns the corresponding
 * array of vertex_t, stroing the size in *countp. The array is Malloc()'d and the
 * caller is in charge of Free()ing it.
 * If err=NULL, raises an error on failure, otherwise returns NULL and stores
 * the ERR_XXX code in *err.
 */
    {
    int count, i;
    vertex_t *dst = NULL;
    *countp = 0;
#define ERR(ec) do { if(err) *err=(ec); else argerror(L, arg, (ec)); return NULL; } while(0)
    if(lua_isnoneornil(L, arg)) ERR(ERR_NOTPRESENT);
    if(lua_type(L, arg)!=LUA_TTABLE) ERR(ERR_TABLE);

    count = luaL_len(L, arg);
    if(count==0) ERR(ERR_EMPTY);
    dst = MallocNoErr(L, count*sizeof(vertex_t));
    if(!dst) ERR(ERR_MEMORY);
    for(i=0; i<count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        if(testvertex(L, -1, &dst[i])!=0)
            { Free(L, dst); ERR(ERR_TYPE); }
        lua_pop(L, 1);
        }
#undef ERR
    *countp = count;
    if(err) *err=0;
    return dst;
    }

void pushvertexlist(lua_State *L, const vertex_t *vertices , int count)
    {
    int i;
    lua_newtable(L);
    for(i=0; i<count; i++)
        {
        pushvertex(L, &vertices[i]);
        lua_rawseti(L, -2, i+1);
        }
    }

