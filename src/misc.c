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

/*----------------------------------------------------------------------*
 | SDL_rect
 *----------------------------------------------------------------------*/

#if 0
static int PointInRect(lua_State *L)
    {
    point_t p;
    rect_t r;
    checkpoint(L, 1, &p);
    checkrect(L, 2, &r);
    lua_pushboolean(L, SDL_PointInRect(&p, &r));
    return 1;
    }

static int RectEmpty(lua_State *L)
    {
    rect_t r;
    checkrect(L, 1, &r);
    lua_pushboolean(L, SDL_RectEmpty(&r));
    return 1;
    }

static int RectEquals(lua_State *L)
    {
    rect_t a;
    rect_t b;
    checkrect(L, 1, &a);
    checkrect(L, 2, &b);
    lua_pushboolean(L, SDL_RectEquals(&a, &b));
    return 1;
    }

static int HasIntersection(lua_State *L)
    {
    rect_t a;
    rect_t b;
    checkrect(L, 1, &a);
    checkrect(L, 2, &b);
    lua_pushboolean(L, SDL_HasIntersection(&a, &b));
    return 1;
    }

static int IntersectRect(lua_State *L)
    {
    rect_t a;
    rect_t b;
    rect_t r;
    checkrect(L, 1, &a);
    checkrect(L, 2, &b);
    if(!SDL_IntersectRect(&a, &b, &r)) return 0;
    pushrect(L, &r);
    return 1;
    }

static int UnionRect(lua_State *L)
    {
    rect_t a;
    rect_t b;
    rect_t r;
    checkrect(L, 1, &a);
    checkrect(L, 2, &b);
    SDL_UnionRect(&a, &b, &r);
    pushrect(L, &r);
    return 1;
    }

static int EnclosePoints(lua_State *L)
    {
    int count, ec;
    rect_t clip;
    rect_t result;
    bool_t rc;
    point_t *points;
    ec = testrect(L, 2, &clip);
    points = checkpointlist(L, 1, &count, NULL);
    rc = SDL_EnclosePoints(points, count, (ec == ERR_NOTPRESENT)? NULL : &clip, &result);
    Free(L, points);
    lua_pushboolean(L, rc);
    pushrect(L, &result);
    return 2;
    }

static int IntersectRectAndLine(lua_State *L)
    {
    rect_t r;
    int x1 = luaL_checkinteger(L, 2);
    int y1 = luaL_checkinteger(L, 3);
    int x2 = luaL_checkinteger(L, 4);
    int y2 = luaL_checkinteger(L, 5);
    checkrect(L, 1, &r);
    if(!SDL_IntersectRectAndLine(&r, &x1, &y1, &x2, &y2)) return 0;
    lua_pushinteger(L, x1);
    lua_pushinteger(L, y1);
    lua_pushinteger(L, x2);
    lua_pushinteger(L, y2);
    return 4;
    }
#endif

//££



static int PointInFRect(lua_State *L)
    {
    fpoint_t p;
    frect_t r;
    checkfpoint(L, 1, &p);
    checkfrect(L, 2, &r);
    lua_pushboolean(L, SDL_PointInFRect(&p, &r));
    return 1;
    }

static int FRectEmpty(lua_State *L)
    {
    frect_t r;
    checkfrect(L, 1, &r);
    lua_pushboolean(L, SDL_FRectEmpty(&r));
    return 1;
    }

static int FRectEquals(lua_State *L)
    {
    float epsilon;
    frect_t a, b;
    checkfrect(L, 1, &a);
    checkfrect(L, 2, &b);
    if(lua_isnoneornil(L, 3))
        lua_pushboolean(L, SDL_FRectEquals(&a, &b));
    else
        {
        epsilon = luaL_checknumber(L, 3);
        lua_pushboolean(L, SDL_FRectEqualsEpsilon(&a, &b, epsilon));
        }
    return 1;
    }

static int HasIntersectionF(lua_State *L)
    {
    frect_t a, b;
    checkfrect(L, 1, &a);
    checkfrect(L, 2, &b);
    lua_pushboolean(L, SDL_HasIntersectionF(&a, &b));
    return 1;
    }

static int IntersectFRect(lua_State *L)
    {
    frect_t a, b, res;
    checkfrect(L, 1, &a);
    checkfrect(L, 2, &b);
    if(!SDL_IntersectFRect(&a, &b, &res)) return 0;
    pushfrect(L, &res);
    return 1;
    }

static int UnionFRect(lua_State *L)
    {
    frect_t a;
    frect_t b;
    frect_t r;
    checkfrect(L, 1, &a);
    checkfrect(L, 2, &b);
    SDL_UnionFRect(&a, &b, &r);
    pushfrect(L, &r);
    return 1;
    }

static int EncloseFPoints(lua_State *L)
    {
    int count, ec;
    frect_t clip;
    frect_t result;
    bool_t rc;
    fpoint_t *points;
    ec = testfrect(L, 2, &clip);
    points = checkfpointlist(L, 1, &count, NULL);
    rc = SDL_EncloseFPoints(points, count, (ec == ERR_NOTPRESENT)? NULL : &clip, &result);
    Free(L, points);
    lua_pushboolean(L, rc);
    pushfrect(L, &result);
    return 2;
    }


static int IntersectFRectAndLine(lua_State *L)
    {
    frect_t r;
    float x1, y1, x2, y2;
    checkfrect(L, 1, &r);
    if(lua_istable(L, 2))
        {
        fpoint_t point1, point2;
        checkfpoint(L, 2, &point1);
        checkfpoint(L, 3, &point2);
        if(!SDL_IntersectFRectAndLine(&r, &point1.x, &point1.y,  &point2.x, &point2.y)) return 0;
        pushfpoint(L, &point1);
        pushfpoint(L, &point2);
        return 2;
        }
    x1 = luaL_checknumber(L, 2);
    y1 = luaL_checknumber(L, 3);
    x2 = luaL_checknumber(L, 4);
    y2 = luaL_checknumber(L, 5);
    if(!SDL_IntersectFRectAndLine(&r, &x1, &y1, &x2, &y2)) return 0;
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
    }

/*----------------------------------------------------------------------*
 | Misc
 *----------------------------------------------------------------------*/

#if 0
static int OpenURL(lua_State *L)
    {
    const char *url = luaL_checkstring(L, 1);
    int ec = SDL_OpenURL(url);
    CheckError(L, ec);
    return 0;
    }
#endif

static int GetPreferredLocales(lua_State *L)
    {
    int i;
    SDL_Locale *locales = SDL_GetPreferredLocales();
    if(!locales) GetError(L);
    lua_newtable(L);
    for(i = 0;; i++)
        {
        if(locales[i].language == NULL) break;
        lua_newtable(L);
        lua_pushstring(L, locales[i].language);
        lua_setfield(L, -2, "language");
        if(locales[i].country)
            {
            lua_pushstring(L, locales[i].country );
            lua_setfield(L, -2, "country");
            }
        lua_rawseti(L, -2, i+1);
        }
    SDL_free(locales);
    return 1;
    }

static int GetPlatform(lua_State *L)
    {
    lua_pushstring(L, SDL_GetPlatform());
    return 1;
    }

static int GetPowerInfo(lua_State *L)
    {
    int secs, pct;
    SDL_PowerState state = SDL_GetPowerInfo(&secs, &pct);
    pushpowerstate(L, state);
    lua_pushinteger(L, secs);
    lua_pushinteger(L, pct);
    return 3;
    }

/*----------------------------------------------------------------------*
 | path
 *----------------------------------------------------------------------*/

static int GetBasePath(lua_State *L)
    {
    char *path = SDL_GetBasePath();
    if(!path) GetError(L);
    lua_pushstring(L, path);
    SDL_free(path);
    return 1;
    }

static int GetPrefPath(lua_State *L)
    {
    const char *org = luaL_checkstring(L, 1);
    const char *app = luaL_checkstring(L, 2);
    char *path = SDL_GetPrefPath(org, app);
    if(!path) GetError(L);
    lua_pushstring(L, path);
    SDL_free(path);
    return 1;
    }

#if 0 // 7yy
        { "",  },
static int (lua_State *L)
    {
    (void)L;
    return 0;
    }

#endif

/* ----------------------------------------------------------------------- */

static const struct luaL_Reg Functions[] = 
    {
//      { "point_in_rect", PointInRect },
//      { "rect_empty", RectEmpty },
//      { "rect_equals", RectEquals },
//      { "has_intersection", HasIntersection },
//      { "intersect_rect", IntersectRect },
//      { "union_rect", UnionRect },
//      { "enclose_points", EnclosePoints },
//      { "intersect_rect_and_line", IntersectRectAndLine },
        { "point_in_rect", PointInFRect },
        { "rect_empty", FRectEmpty },
        { "rect_equals", FRectEquals },
        { "has_intersection", HasIntersectionF },
        { "intersect_rect", IntersectFRect },
        { "union_rect", UnionFRect },
        { "enclose_points", EncloseFPoints },
        { "intersect_rect_and_line", IntersectFRectAndLine },
//      { "open_url", OpenURL },
        { "get_preferred_locales", GetPreferredLocales },
        { "get_platform", GetPlatform  },
        { "get_power_info", GetPowerInfo },
        { "get_base_path", GetBasePath },
        { "get_pref_path", GetPrefPath },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_misc(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


