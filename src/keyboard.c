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
  
static uint16_t checkkeymod(lua_State *L, int arg)
    {
    uint16_t mod = 0;
    if(!lua_istable(L, arg))
        return argerror(L, arg, ERR_TABLE);
#define Get(what, mask) do {                            \
        lua_getfield(L, arg, what);                     \
        if(lua_toboolean(L, -1)) mod = mod | mask;      \
        lua_pop(L, 1);                                  \
} while(0)
    Get("lshift", KMOD_LSHIFT);
    Get("rshift", KMOD_RSHIFT);
    Get("shift", KMOD_SHIFT);
    Get("lctrl", KMOD_LCTRL);
    Get("rctrl", KMOD_RCTRL);
    Get("ctrl", KMOD_CTRL);
    Get("lalt", KMOD_LALT);
    Get("ralt", KMOD_RALT);
    Get("alt", KMOD_ALT);
    Get("lgui", KMOD_LGUI);
    Get("rgui", KMOD_RGUI);
    Get("gui", KMOD_GUI);
    Get("num", KMOD_NUM);
    Get("caps", KMOD_CAPS);
    Get("mode", KMOD_MODE);
    Get("scroll", KMOD_SCROLL);
#undef Get
    return mod;
    }

void pushkeymod(lua_State *L, uint16_t mod)
    {
    lua_newtable(L);
    lua_pushboolean(L, mod == KMOD_NONE); lua_setfield(L, -2, "none");
    lua_pushboolean(L, mod & KMOD_LSHIFT); lua_setfield(L, -2, "lshift");
    lua_pushboolean(L, mod & KMOD_RSHIFT); lua_setfield(L, -2, "rshift");
    lua_pushboolean(L, mod & KMOD_SHIFT); lua_setfield(L, -2, "shift");
    lua_pushboolean(L, mod & KMOD_LCTRL); lua_setfield(L, -2, "lctrl");
    lua_pushboolean(L, mod & KMOD_RCTRL); lua_setfield(L, -2, "rctrl");
    lua_pushboolean(L, mod & KMOD_CTRL); lua_setfield(L, -2, "ctrl");
    lua_pushboolean(L, mod & KMOD_LALT); lua_setfield(L, -2, "lalt");
    lua_pushboolean(L, mod & KMOD_RALT); lua_setfield(L, -2, "ralt");
    lua_pushboolean(L, mod & KMOD_ALT); lua_setfield(L, -2, "alt");
    lua_pushboolean(L, mod & KMOD_LGUI); lua_setfield(L, -2, "lgui");
    lua_pushboolean(L, mod & KMOD_RGUI); lua_setfield(L, -2, "rgui");
    lua_pushboolean(L, mod & KMOD_GUI); lua_setfield(L, -2, "gui");
    lua_pushboolean(L, mod & KMOD_NUM); lua_setfield(L, -2, "num");
    lua_pushboolean(L, mod & KMOD_CAPS); lua_setfield(L, -2, "caps");
    lua_pushboolean(L, mod & KMOD_MODE); lua_setfield(L, -2, "mode");
    lua_pushboolean(L, mod & KMOD_SCROLL); lua_setfield(L, -2, "scroll");
    }

static int GetKeyboardFocus(lua_State *L)
    {
    window_t *window = SDL_GetKeyboardFocus();
    if(!UD(window)) return 0;
    pushwindow(L, window);
    return 1;
    }

static int GetKeyState(lua_State *L)
    {
    int numkeys, scancode;
    const uint8_t *state = SDL_GetKeyboardState(&numkeys);
    if(lua_isinteger(L, 1))
        scancode = luaL_checkinteger(L, 1);
    else
        {
        const char *name = luaL_checkstring(L, 1);
        scancode = SDL_GetScancodeFromName(name);
        }
    if((scancode == SDL_SCANCODE_UNKNOWN)||( scancode >= numkeys))
        return argerror(L, 1, ERR_VALUE);
    pushkeystate(L, (state[scancode]==1) ? SDL_PRESSED : SDL_RELEASED);
    return 1;
    }

static int GetModState(lua_State *L)
    {
    SDL_Keymod mod = SDL_GetModState();
    pushkeymod(L, mod);
    return 1;
    }

static int SetModState(lua_State *L)
    {
    SDL_Keymod mod = checkkeymod(L, 1);
    SDL_SetModState(mod);
    return 0;
    }

static int GetKeyFromScancode(lua_State *L)
    {
    SDL_Scancode scancode = luaL_checkinteger(L, 1);
    SDL_Keycode keycode = SDL_GetKeyFromScancode(scancode);
    lua_pushinteger(L, keycode);
    return 1;
    }

static int GetScancodeFromKey(lua_State *L)
    {
    SDL_Keycode keycode = luaL_checkinteger(L, 1);
    SDL_Scancode scancode = SDL_GetScancodeFromKey(keycode);
    lua_pushinteger(L, scancode);
    return 1;
    }

#define F(Func, func)                               \
static int Func(lua_State *L)                       \
    {                                               \
    int code = luaL_checkinteger(L, 1);             \
    lua_pushstring(L, func(code));                  \
    return 1;                                       \
    }
F(GetScancodeName, SDL_GetScancodeName)
F(GetKeyName, SDL_GetKeyName)
#undef F

#define F(Func, func, unknowncode)                  \
static int Func(lua_State *L)                       \
    {                                               \
    const char *name = luaL_checkstring(L, 1);      \
    int code = func(name);                          \
    if(code == unknowncode) GetError(L);            \
    lua_pushinteger(L, code);                       \
    return 1;                                       \
    }
F(GetScancodeFromName, SDL_GetScancodeFromName, SDL_SCANCODE_UNKNOWN)
F(GetKeyFromName, SDL_GetKeyFromName, SDLK_UNKNOWN)
#undef F

static int StartTextInput(lua_State *L)
    {
    (void)L;
    SDL_StartTextInput();
    return 0;
    }

static int IsTextInputActive(lua_State *L)
    {
    lua_pushboolean(L, SDL_IsTextInputActive());
    return 1;
    }

static int StopTextInput(lua_State *L)
    {
    (void)L;
    SDL_StopTextInput();
    return 0;
    }

static int SetTextInputRect(lua_State *L)
    {
    rect_t rect;
    int ec = optrect(L, 1, &rect);
    SDL_SetTextInputRect((ec==ERR_NOTPRESENT)? NULL : &rect);
    return 0;
    }

static int HasScreenKeyboardSupport(lua_State *L)
    {
    lua_pushboolean(L, SDL_HasScreenKeyboardSupport());
    return 1;
    }

static int IsScreenKeyboardShown(lua_State *L)
    {
    window_t *window = checkwindow(L, 1, NULL);
    lua_pushboolean(L, SDL_IsScreenKeyboardShown(window));
    return 1;
    }

static int ResetKeyboard(lua_State *L)
    {
    (void)L;
    SDL_ResetKeyboard();
    return 0;
    }

static int ClearComposition(lua_State *L)
    {
    (void)L;
    SDL_ClearComposition();
    return 0;
    }

static int IsTextInputShown(lua_State *L)
    {
    lua_pushboolean(L, SDL_IsTextInputShown());
    return 1;
    }

static int SetClipboardText(lua_State *L)
    {
    const char *text = luaL_optstring(L, 1, NULL);
    int ec = SDL_SetClipboardText(text);
    CheckError(L, ec);
    return 0;
    }

static int GetClipboardText(lua_State *L)
    {
    char *text = SDL_GetClipboardText();
    if(!text) GetError(L);
    lua_pushstring(L, text);
    SDL_free(text);
    return 1;
    }

static int HasClipboardText(lua_State *L)
    {
    lua_pushboolean(L, SDL_HasClipboardText());
    return 1;
    }

static int SetPrimarySelectionText(lua_State *L)
    {
    const char *text = luaL_checkstring(L, 1);
    int ec = SDL_SetPrimarySelectionText(text);
    CheckError(L, ec);
    return 0;
    }

static int GetPrimarySelectionText(lua_State *L)
    {
    char *text = SDL_GetPrimarySelectionText();
    if(!text) GetError(L);
    lua_pushstring(L, text);
    SDL_free(text);
    return 1;
    }

static int HasPrimarySelectionText(lua_State *L)
    {
    lua_pushboolean(L, SDL_HasPrimarySelectionText());
    return 1;
    }

/* ----------------------------------------------------------------------- */

static const struct luaL_Reg Functions[] = 
    {
        { "get_keyboard_focus", GetKeyboardFocus },
        { "get_key_state", GetKeyState },
        { "get_mod_state", GetModState },
        { "set_mod_state", SetModState },
        { "get_key_from_scancode", GetKeyFromScancode },
        { "get_scancode_from_key", GetScancodeFromKey },
        { "get_scancode_name", GetScancodeName },
        { "get_scancode_from_name", GetScancodeFromName },
        { "get_key_name", GetKeyName },
        { "get_key_from_name", GetKeyFromName },
        { "start_text_input", StartTextInput },
        { "is_text_input_active", IsTextInputActive },
        { "stop_text_input", StopTextInput },
        { "set_text_input_rect", SetTextInputRect },
        { "has_screen_keyboard_support", HasScreenKeyboardSupport },
        { "is_screen_keyboard_shown", IsScreenKeyboardShown },
        { "reset_keyboard", ResetKeyboard },
        { "clear_composition", ClearComposition },
        { "is_text_input_shown", IsTextInputShown },
        { "set_clipboard_text", SetClipboardText },
        { "get_clipboard_text", GetClipboardText },
        { "has_clipboard_text", HasClipboardText },
        { "set_primary_selection_text", SetPrimarySelectionText },
        { "get_primary_selection_text", GetPrimarySelectionText },
        { "has_primary_selection_text", HasPrimarySelectionText },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_keyboard(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    /* keycodes: sdl.KEYCODE_XXX for SDLK_XXX */
#define ADD(what) do { lua_pushinteger(L, SDLK_##what); lua_setfield(L, -2, "KEYCODE_"#what); } while(0)
    ADD(UNKNOWN);
    ADD(RETURN);
    ADD(ESCAPE);
    ADD(BACKSPACE);
    ADD(TAB);
    ADD(SPACE);
    ADD(EXCLAIM);
    ADD(QUOTEDBL);
    ADD(HASH);
    ADD(PERCENT);
    ADD(DOLLAR);
    ADD(AMPERSAND);
    ADD(QUOTE);
    ADD(LEFTPAREN);
    ADD(RIGHTPAREN);
    ADD(ASTERISK);
    ADD(PLUS);
    ADD(COMMA);
    ADD(MINUS);
    ADD(PERIOD);
    ADD(SLASH);
    ADD(0);
    ADD(1);
    ADD(2);
    ADD(3);
    ADD(4);
    ADD(5);
    ADD(6);
    ADD(7);
    ADD(8);
    ADD(9);
    ADD(COLON);
    ADD(SEMICOLON);
    ADD(LESS);
    ADD(EQUALS);
    ADD(GREATER);
    ADD(QUESTION);
    ADD(AT);
    ADD(LEFTBRACKET);
    ADD(BACKSLASH);
    ADD(RIGHTBRACKET);
    ADD(CARET);
    ADD(UNDERSCORE);
    ADD(BACKQUOTE);
    ADD(a);
    ADD(b);
    ADD(c);
    ADD(d);
    ADD(e);
    ADD(f);
    ADD(g);
    ADD(h);
    ADD(i);
    ADD(j);
    ADD(k);
    ADD(l);
    ADD(m);
    ADD(n);
    ADD(o);
    ADD(p);
    ADD(q);
    ADD(r);
    ADD(s);
    ADD(t);
    ADD(u);
    ADD(v);
    ADD(w);
    ADD(x);
    ADD(y);
    ADD(z);
    ADD(CAPSLOCK);
    ADD(F1);
    ADD(F2);
    ADD(F3);
    ADD(F4);
    ADD(F5);
    ADD(F6);
    ADD(F7);
    ADD(F8);
    ADD(F9);
    ADD(F10);
    ADD(F11);
    ADD(F12);
    ADD(PRINTSCREEN);
    ADD(SCROLLLOCK);
    ADD(PAUSE);
    ADD(INSERT);
    ADD(HOME);
    ADD(PAGEUP);
    ADD(DELETE);
    ADD(END);
    ADD(PAGEDOWN);
    ADD(RIGHT);
    ADD(LEFT);
    ADD(DOWN);
    ADD(UP);
    ADD(NUMLOCKCLEAR);
    ADD(KP_DIVIDE);
    ADD(KP_MULTIPLY);
    ADD(KP_MINUS);
    ADD(KP_PLUS);
    ADD(KP_ENTER);
    ADD(KP_1);
    ADD(KP_2);
    ADD(KP_3);
    ADD(KP_4);
    ADD(KP_5);
    ADD(KP_6);
    ADD(KP_7);
    ADD(KP_8);
    ADD(KP_9);
    ADD(KP_0);
    ADD(KP_PERIOD);
    ADD(APPLICATION);
    ADD(POWER);
    ADD(KP_EQUALS);
    ADD(F13);
    ADD(F14);
    ADD(F15);
    ADD(F16);
    ADD(F17);
    ADD(F18);
    ADD(F19);
    ADD(F20);
    ADD(F21);
    ADD(F22);
    ADD(F23);
    ADD(F24);
    ADD(EXECUTE);
    ADD(HELP);
    ADD(MENU);
    ADD(SELECT);
    ADD(STOP);
    ADD(AGAIN);
    ADD(UNDO);
    ADD(CUT);
    ADD(COPY);
    ADD(PASTE);
    ADD(FIND);
    ADD(MUTE);
    ADD(VOLUMEUP);
    ADD(VOLUMEDOWN);
    ADD(KP_COMMA);
    ADD(KP_EQUALSAS400);
    ADD(ALTERASE);
    ADD(SYSREQ);
    ADD(CANCEL);
    ADD(CLEAR);
    ADD(PRIOR);
    ADD(RETURN2);
    ADD(SEPARATOR);
    ADD(OUT);
    ADD(OPER);
    ADD(CLEARAGAIN);
    ADD(CRSEL);
    ADD(EXSEL);
    ADD(KP_00);
    ADD(KP_000);
    ADD(THOUSANDSSEPARATOR);
    ADD(DECIMALSEPARATOR);
    ADD(CURRENCYUNIT);
    ADD(CURRENCYSUBUNIT);
    ADD(KP_LEFTPAREN);
    ADD(KP_RIGHTPAREN);
    ADD(KP_LEFTBRACE);
    ADD(KP_RIGHTBRACE);
    ADD(KP_TAB);
    ADD(KP_BACKSPACE);
    ADD(KP_A);
    ADD(KP_B);
    ADD(KP_C);
    ADD(KP_D);
    ADD(KP_E);
    ADD(KP_F);
    ADD(KP_XOR);
    ADD(KP_POWER);
    ADD(KP_PERCENT);
    ADD(KP_LESS);
    ADD(KP_GREATER);
    ADD(KP_AMPERSAND);
    ADD(KP_DBLAMPERSAND);
    ADD(KP_VERTICALBAR);
    ADD(KP_DBLVERTICALBAR);
    ADD(KP_COLON);
    ADD(KP_HASH);
    ADD(KP_SPACE);
    ADD(KP_AT);
    ADD(KP_EXCLAM);
    ADD(KP_MEMSTORE);
    ADD(KP_MEMRECALL);
    ADD(KP_MEMCLEAR);
    ADD(KP_MEMADD);
    ADD(KP_MEMSUBTRACT);
    ADD(KP_MEMMULTIPLY);
    ADD(KP_MEMDIVIDE);
    ADD(KP_PLUSMINUS);
    ADD(KP_CLEAR);
    ADD(KP_CLEARENTRY);
    ADD(KP_BINARY);
    ADD(KP_OCTAL);
    ADD(KP_DECIMAL);
    ADD(KP_HEXADECIMAL);
    ADD(LCTRL);
    ADD(LSHIFT);
    ADD(LALT);
    ADD(LGUI);
    ADD(RCTRL);
    ADD(RSHIFT);
    ADD(RALT);
    ADD(RGUI);
    ADD(MODE);
    ADD(AUDIONEXT);
    ADD(AUDIOPREV);
    ADD(AUDIOSTOP);
    ADD(AUDIOPLAY);
    ADD(AUDIOMUTE);
    ADD(MEDIASELECT);
    ADD(WWW);
    ADD(MAIL);
    ADD(CALCULATOR);
    ADD(COMPUTER);
    ADD(AC_SEARCH);
    ADD(AC_HOME);
    ADD(AC_BACK);
    ADD(AC_FORWARD);
    ADD(AC_STOP);
    ADD(AC_REFRESH);
    ADD(AC_BOOKMARKS);
    ADD(BRIGHTNESSDOWN);
    ADD(BRIGHTNESSUP);
    ADD(DISPLAYSWITCH);
    ADD(KBDILLUMTOGGLE);
    ADD(KBDILLUMDOWN);
    ADD(KBDILLUMUP);
    ADD(EJECT);
    ADD(SLEEP);
    ADD(APP1);
    ADD(APP2);
    ADD(AUDIOREWIND);
    ADD(AUDIOFASTFORWARD);
    ADD(SOFTLEFT);
    ADD(SOFTRIGHT);
    ADD(CALL);
    ADD(ENDCALL);
#undef ADD

    /* scancodes: sdl.SCANCODE_XXX for SDL_SCANCODE_XXX */
#define ADD(what) do { lua_pushinteger(L, SDL_SCANCODE_##what); lua_setfield(L, -2, "SCANCODE_"#what); } while(0)
    ADD(UNKNOWN);
    ADD(A);
    ADD(B);
    ADD(C);
    ADD(D);
    ADD(E);
    ADD(F);
    ADD(G);
    ADD(H);
    ADD(I);
    ADD(J);
    ADD(K);
    ADD(L);
    ADD(M);
    ADD(N);
    ADD(O);
    ADD(P);
    ADD(Q);
    ADD(R);
    ADD(S);
    ADD(T);
    ADD(U);
    ADD(V);
    ADD(W);
    ADD(X);
    ADD(Y);
    ADD(Z);
    ADD(1);
    ADD(2);
    ADD(3);
    ADD(4);
    ADD(5);
    ADD(6);
    ADD(7);
    ADD(8);
    ADD(9);
    ADD(0);
    ADD(RETURN);
    ADD(ESCAPE);
    ADD(BACKSPACE);
    ADD(TAB);
    ADD(SPACE);
    ADD(MINUS);
    ADD(EQUALS);
    ADD(LEFTBRACKET);
    ADD(RIGHTBRACKET);
    ADD(BACKSLASH);
    ADD(NONUSHASH);
    ADD(SEMICOLON);
    ADD(APOSTROPHE);
    ADD(GRAVE);
    ADD(COMMA);
    ADD(PERIOD);
    ADD(SLASH);
    ADD(CAPSLOCK);
    ADD(F1);
    ADD(F2);
    ADD(F3);
    ADD(F4);
    ADD(F5);
    ADD(F6);
    ADD(F7);
    ADD(F8);
    ADD(F9);
    ADD(F10);
    ADD(F11);
    ADD(F12);
    ADD(PRINTSCREEN);
    ADD(SCROLLLOCK);
    ADD(PAUSE);
    ADD(INSERT);
    ADD(HOME);
    ADD(PAGEUP);
    ADD(DELETE);
    ADD(END);
    ADD(PAGEDOWN);
    ADD(RIGHT);
    ADD(LEFT);
    ADD(DOWN);
    ADD(UP);
    ADD(NUMLOCKCLEAR);
    ADD(KP_DIVIDE);
    ADD(KP_MULTIPLY);
    ADD(KP_MINUS);
    ADD(KP_PLUS);
    ADD(KP_ENTER);
    ADD(KP_1);
    ADD(KP_2);
    ADD(KP_3);
    ADD(KP_4);
    ADD(KP_5);
    ADD(KP_6);
    ADD(KP_7);
    ADD(KP_8);
    ADD(KP_9);
    ADD(KP_0);
    ADD(KP_PERIOD);
    ADD(NONUSBACKSLASH);
    ADD(APPLICATION);
    ADD(POWER);
    ADD(KP_EQUALS);
    ADD(F13);
    ADD(F14);
    ADD(F15);
    ADD(F16);
    ADD(F17);
    ADD(F18);
    ADD(F19);
    ADD(F20);
    ADD(F21);
    ADD(F22);
    ADD(F23);
    ADD(F24);
    ADD(EXECUTE);
    ADD(HELP);
    ADD(MENU);
    ADD(SELECT);
    ADD(STOP);
    ADD(AGAIN);
    ADD(UNDO);
    ADD(CUT);
    ADD(COPY);
    ADD(PASTE);
    ADD(FIND);
    ADD(MUTE);
    ADD(VOLUMEUP);
    ADD(VOLUMEDOWN);
    ADD(KP_COMMA);
    ADD(KP_EQUALSAS400);
    ADD(INTERNATIONAL1);
    ADD(INTERNATIONAL2);
    ADD(INTERNATIONAL3);
    ADD(INTERNATIONAL4);
    ADD(INTERNATIONAL5);
    ADD(INTERNATIONAL6);
    ADD(INTERNATIONAL7);
    ADD(INTERNATIONAL8);
    ADD(INTERNATIONAL9);
    ADD(LANG1);
    ADD(LANG2);
    ADD(LANG3);
    ADD(LANG4);
    ADD(LANG5);
    ADD(LANG6);
    ADD(LANG7);
    ADD(LANG8);
    ADD(LANG9);
    ADD(ALTERASE);
    ADD(SYSREQ);
    ADD(CANCEL);
    ADD(CLEAR);
    ADD(PRIOR);
    ADD(RETURN2);
    ADD(SEPARATOR);
    ADD(OUT);
    ADD(OPER);
    ADD(CLEARAGAIN);
    ADD(CRSEL);
    ADD(EXSEL);
    ADD(KP_00);
    ADD(KP_000);
    ADD(THOUSANDSSEPARATOR);
    ADD(DECIMALSEPARATOR);
    ADD(CURRENCYUNIT);
    ADD(CURRENCYSUBUNIT);
    ADD(KP_LEFTPAREN);
    ADD(KP_RIGHTPAREN);
    ADD(KP_LEFTBRACE);
    ADD(KP_RIGHTBRACE);
    ADD(KP_TAB);
    ADD(KP_BACKSPACE);
    ADD(KP_A);
    ADD(KP_B);
    ADD(KP_C);
    ADD(KP_D);
    ADD(KP_E);
    ADD(KP_F);
    ADD(KP_XOR);
    ADD(KP_POWER);
    ADD(KP_PERCENT);
    ADD(KP_LESS);
    ADD(KP_GREATER);
    ADD(KP_AMPERSAND);
    ADD(KP_DBLAMPERSAND);
    ADD(KP_VERTICALBAR);
    ADD(KP_DBLVERTICALBAR);
    ADD(KP_COLON);
    ADD(KP_HASH);
    ADD(KP_SPACE);
    ADD(KP_AT);
    ADD(KP_EXCLAM);
    ADD(KP_MEMSTORE);
    ADD(KP_MEMRECALL);
    ADD(KP_MEMCLEAR);
    ADD(KP_MEMADD);
    ADD(KP_MEMSUBTRACT);
    ADD(KP_MEMMULTIPLY);
    ADD(KP_MEMDIVIDE);
    ADD(KP_PLUSMINUS);
    ADD(KP_CLEAR);
    ADD(KP_CLEARENTRY);
    ADD(KP_BINARY);
    ADD(KP_OCTAL);
    ADD(KP_DECIMAL);
    ADD(KP_HEXADECIMAL);
    ADD(LCTRL);
    ADD(LSHIFT);
    ADD(LALT);
    ADD(LGUI);
    ADD(RCTRL);
    ADD(RSHIFT);
    ADD(RALT);
    ADD(RGUI);
    ADD(MODE);
    ADD(AUDIONEXT);
    ADD(AUDIOPREV);
    ADD(AUDIOSTOP);
    ADD(AUDIOPLAY);
    ADD(AUDIOMUTE);
    ADD(MEDIASELECT);
    ADD(WWW);
    ADD(MAIL);
    ADD(CALCULATOR);
    ADD(COMPUTER);
    ADD(AC_SEARCH);
    ADD(AC_HOME);
    ADD(AC_BACK);
    ADD(AC_FORWARD);
    ADD(AC_STOP);
    ADD(AC_REFRESH);
    ADD(AC_BOOKMARKS);
    ADD(BRIGHTNESSDOWN);
    ADD(BRIGHTNESSUP);
    ADD(DISPLAYSWITCH);
    ADD(KBDILLUMTOGGLE);
    ADD(KBDILLUMDOWN);
    ADD(KBDILLUMUP);
    ADD(EJECT);
    ADD(SLEEP);
    ADD(APP1);
    ADD(APP2);
    ADD(AUDIOREWIND);
    ADD(AUDIOFASTFORWARD);
    ADD(SOFTLEFT);
    ADD(SOFTRIGHT);
    ADD(CALL);
    ADD(ENDCALL);
#undef ADD
    }

