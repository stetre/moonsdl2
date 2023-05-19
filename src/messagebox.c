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

static void freemessageboxbuttondata(lua_State *L, SDL_MessageBoxButtonData *data)
    {
    if(data && data->text) { Free(L, (void*)data->text); data->text = NULL; }
    }

static int checkmessageboxbuttondata(lua_State *L, int arg, SDL_MessageBoxButtonData *data)
    {
    if(!lua_istable(L, arg)) return ERR_TABLE;
    lua_getfield(L, arg, "flags"); //messageboxbuttonflags
    if(!lua_isinteger(L, -1)) return ERR_VALUE;
    data->flags = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, arg, "buttonid");
    if(!lua_isinteger(L, -1)) return ERR_VALUE;
    data->buttonid = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, arg, "text");
    if(!lua_isstring(L, -1)) return ERR_VALUE;
    data->text = Strdup(L, lua_tostring(L, -1));
    lua_pop(L, 1);
    return ERR_SUCCESS;
    }

static void freemessageboxdata(lua_State *L, messagebox_t *data)
    {
    Free(L, (void*)data->title);
    Free(L, (void*)data->message);
    if(data->buttons)
        {
        for(int i = 0; i < data->numbuttons; i++)
            freemessageboxbuttondata(L, (SDL_MessageBoxButtonData*)&data->buttons[i]);
        }
    Free(L, (void*)data->buttons);
    Free(L, (void*)data->colorScheme);
    }

static messagebox_t *checkmessageboxdata(lua_State *L, int arg)
    {
    messagebox_t *messagebox = Malloc(L, sizeof(messagebox_t));
    if(!lua_istable(L, arg))
        { Free(L, messagebox); argerror(L, arg, ERR_TABLE); return NULL; }
    lua_getfield(L, arg, "flags"); //messageboxflags
    if(!lua_isinteger(L, -1))
        { Free(L, messagebox); argerror(L, arg, ERR_VALUE); return NULL; }
    messagebox->flags = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, arg, "window");
    messagebox->window = optwindow(L, -1, NULL);
    lua_pop(L, 1);
    lua_getfield(L, arg, "title");
    if(!lua_isstring(L, -1))
        { Free(L, messagebox); argerror(L, arg, ERR_VALUE); return NULL; }
    messagebox->title = Strdup(L, lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_getfield(L, arg, "message");
    if(!lua_isstring(L, -1))
        { Free(L, messagebox); argerror(L, arg, ERR_VALUE); return NULL; }
    messagebox->message = Strdup(L, lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_getfield(L, arg, "buttons");
    if(!lua_istable(L, -1))
        { Free(L, messagebox); argerror(L, arg, ERR_VALUE); return NULL; }
    messagebox->numbuttons = luaL_len(L, -1);
    messagebox->buttons = MallocNoErr(L, sizeof(SDL_MessageBoxButtonData)*messagebox->numbuttons);
    if(!messagebox->buttons)
        { Free(L, messagebox); errmemory(L); return NULL; }
    for(int i = 0; i<messagebox->numbuttons; i++)
        {
        int err;
        lua_rawgeti(L, -1, i+1);
        err = checkmessageboxbuttondata(L, -1, (SDL_MessageBoxButtonData*)&messagebox->buttons[i]);
        if(err) { Free(L, messagebox); argerror(L, arg, ERR_VALUE); return NULL; }
        lua_pop(L, 1);
        }
    lua_pop(L, 1);
    // data->colorScheme = NULL by default (it can be set or deleted with the set_color_scheme method)
    return messagebox;
    }

static int freemessagebox(lua_State *L, ud_t *ud)
    {
    messagebox_t *messagebox = (messagebox_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "messagebox")) return 0;
    freemessageboxdata(L, messagebox);
    return 0;
    }

static int newmessagebox(lua_State *L, messagebox_t *messagebox)
    {
    ud_t *ud;
    ud = newuserdata(L, messagebox, MESSAGEBOX_MT, "messagebox");
    ud->parent_ud = NULL;
    ud->destructor = freemessagebox;
    return 1;
    }

static int Create(lua_State *L)
    {
    messagebox_t *messagebox = checkmessageboxdata(L, 1);
    return newmessagebox(L, messagebox);
    }

static int Show(lua_State *L)
    {
    int buttonid;
    messagebox_t *messagebox = checkmessagebox(L, 1, NULL);
    if(SDL_ShowMessageBox(messagebox, &buttonid) != 0) GetError(L);
    lua_pushinteger(L, buttonid);
    return 1;
    }

static int SetColorScheme(lua_State *L)
    {
    int count, err;
    messagebox_t *messagebox = checkmessagebox(L, 1, NULL);
    color_t *colors = checkcolorlist(L, 2, &count, &err);
    if(err==ERR_NOTPRESENT)
        {
        if(messagebox->colorScheme)
            {
            Free(L, (void*)messagebox->colorScheme);
            messagebox->colorScheme = NULL;
            }
        return 0;
        }
    if(err)
        return argerror(L, 2, err);
    if(count != 5) // ie SDL_MESSAGEBOX_COLOR_MAX
        { Free(L, colors); return argerror(L, 2, ERR_LENGTH); }
    if(messagebox->colorScheme == NULL)
        {
        messagebox->colorScheme = MallocNoErr(L, sizeof(SDL_MessageBoxColorType));
        if(!messagebox->colorScheme)
            { Free(L, colors); return errmemory(L); }
        }
    for(int i = 0; i < 5; i++)
        {
        // get rid of the const and set the color components:
        SDL_MessageBoxColorScheme *scheme = (SDL_MessageBoxColorScheme*)messagebox->colorScheme;
        scheme->colors[i].r = colors[i].r;
        scheme->colors[i].g = colors[i].g;
        scheme->colors[i].b = colors[i].b;
        }
    Free(L, colors);
    return 0;
    }

static int ShowSimpleMessageBox(lua_State *L)
    {
    uint32_t flags = checkflags(L, 1);
    const char *title = luaL_checkstring(L, 2);
    const char *message = luaL_checkstring(L, 3);
    window_t *window = optwindow(L, 4, NULL);
    if(SDL_ShowSimpleMessageBox(flags, title, message, window) != 0) GetError(L);
    return 0;
    }

DESTROY_FUNC(messagebox)

static const struct luaL_Reg Methods[] = 
    {
        { "delete", Destroy },
        { "show", Show },
        { "set_color_scheme", SetColorScheme },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_message_box", Create },
        { "show_simple_message_box", ShowSimpleMessageBox },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_messagebox(lua_State *L)
    {
    udata_define(L, MESSAGEBOX_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

