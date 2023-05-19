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

#ifndef enumsDEFINED
#define enumsDEFINED

/* enums.c */
#define enums_free_all moonsdl2_enums_free_all
void enums_free_all(lua_State *L);
#define enums_test moonsdl2_enums_test
int enums_test(lua_State *L, int domain, int arg, int *err);
#define enums_opt moonsdl2_enums_opt
int enums_opt(lua_State *L, int domain, int arg, int defval);
#define enums_check moonsdl2_enums_check
int enums_check(lua_State *L, int domain, int arg);
#define enums_push moonsdl2_enums_push
int enums_push(lua_State *L, int domain, int code);
#define enums_values moonsdl2_enums_values
int enums_values(lua_State *L, int domain);
#define enums_checklist moonsdl2_enums_checklist
int* enums_checklist(lua_State *L, int domain, int arg, int *count, int *err);
#define enums_freelist moonsdl2_enums_freelist
void enums_freelist(lua_State *L, int *list);


/* Enum domains */
#define DOMAIN_                         0
#define DOMAIN_HINT_PRIORITY            1
#define DOMAIN_LOG_PRIORITY             2
#define DOMAIN_PIXELFORMAT              3
#define DOMAIN_EVENT_TYPE               4
#define DOMAIN_DISPLAYEVENT             5
#define DOMAIN_WINDOWEVENT              6
#define DOMAIN_KEYSTATE                 7
#define DOMAIN_BUTTON                   9 
#define DOMAIN_MOUSEWHEEL_DIR           10
#define DOMAIN_CONTROLLER_TYPE          11
#define DOMAIN_CONTROLLER_BINDTYPE      12
#define DOMAIN_CONTROLLER_AXIS          13
#define DOMAIN_CONTROLLER_BUTTON        14
#define DOMAIN_TEXTUREACCESS            15
#define DOMAIN_BLENDOPERATION           16
#define DOMAIN_BLENDFACTOR              17
#define DOMAIN_SCALEMODE                18
#define DOMAIN_SWAPINTERVAL             19
#define DOMAIN_GLATTR                   20
#define DOMAIN_ORIENTATION              21
#define DOMAIN_FLASHOPERATION           22
#define DOMAIN_HITTEST_RESULT           23
#define DOMAIN_JOYSTICK_TYPE            24
#define DOMAIN_JOYSTICK_POWER_LEVEL     25
#define DOMAIN_SYSTEM_CURSOR            26
#define DOMAIN_LOG_CATEGORY             27
#define DOMAIN_POWERSTATE               28
#define DOMAIN_FONT_HINTING             29
#define DOMAIN_MUSIC_TYPE               30
#define DOMAIN_FADING                   31
#define DOMAIN_AUDIO_FORMAT             32
#define DOMAIN_HAT_STATE                33
#define DOMAIN_FONT_DIRECTION           34
#define DOMAIN_FONT_ALIGN               35
#define DOMAIN_TOUCH_DEVICE_TYPE        36
#define DOMAIN_RWOPS_TYPE               37
#define DOMAIN_RWOPS_WHENCE             38
#define DOMAIN_HAPTIC_TYPE              39
#define DOMAIN_HAPTIC_DIRECTION         40
#define DOMAIN_FULLSCREEN_MODE          41
#define DOMAIN_SENSOR_TYPE              42
#define DOMAIN_GL_PROFILE               43

#define testhintpriority(L, arg, err) enums_test((L), DOMAIN_HINT_PRIORITY, (arg), (err))
#define opthintpriority(L, arg, defval) enums_opt((L), DOMAIN_HINT_PRIORITY, (arg), (defval))
#define checkhintpriority(L, arg) enums_check((L), DOMAIN_HINT_PRIORITY, (arg))
#define pushhintpriority(L, val) enums_push((L), DOMAIN_HINT_PRIORITY, (int)(val))
#define valueshintpriority(L) enums_values((L), DOMAIN_HINT_PRIORITY)

#define testlogpriority(L, arg, err) enums_test((L), DOMAIN_LOG_PRIORITY, (arg), (err))
#define optlogpriority(L, arg, defval) enums_opt((L), DOMAIN_LOG_PRIORITY, (arg), (defval))
#define checklogpriority(L, arg) enums_check((L), DOMAIN_LOG_PRIORITY, (arg))
#define pushlogpriority(L, val) enums_push((L), DOMAIN_LOG_PRIORITY, (int)(val))
#define valueslogpriority(L) enums_values((L), DOMAIN_LOG_PRIORITY)

#define testpixelformat(L, arg, err) enums_test((L), DOMAIN_PIXELFORMAT, (arg), (err))
#define optpixelformat(L, arg, defval) enums_opt((L), DOMAIN_PIXELFORMAT, (arg), (defval))
#define checkpixelformat(L, arg) enums_check((L), DOMAIN_PIXELFORMAT, (arg))
#define pushpixelformat(L, val) enums_push((L), DOMAIN_PIXELFORMAT, (int)(val))
#define valuespixelformat(L) enums_values((L), DOMAIN_PIXELFORMAT)

#define testeventtype(L, arg, err) enums_test((L), DOMAIN_EVENT_TYPE, (arg), (err))
#define opteventtype(L, arg, defval) enums_opt((L), DOMAIN_EVENT_TYPE, (arg), (defval))
#define checkeventtype(L, arg) enums_check((L), DOMAIN_EVENT_TYPE, (arg))
#define pusheventtype(L, val) enums_push((L), DOMAIN_EVENT_TYPE, (int)(val))
#define valueseventtype(L) enums_values((L), DOMAIN_EVENT_TYPE)

#define testdisplayeventtype(L, arg, err) enums_test((L), DOMAIN_DISPLAYEVENT, (arg), (err))
#define optdisplayeventtype(L, arg, defval) enums_opt((L), DOMAIN_DISPLAYEVENT, (arg), (defval))
#define checkdisplayeventtype(L, arg) enums_check((L), DOMAIN_DISPLAYEVENT, (arg))
#define pushdisplayeventtype(L, val) enums_push((L), DOMAIN_DISPLAYEVENT, (int)(val))
#define valuesdisplayeventtype(L) enums_values((L), DOMAIN_DISPLAYEVENT)

#define testwindoweventtype(L, arg, err) enums_test((L), DOMAIN_WINDOWEVENT, (arg), (err))
#define optwindoweventtype(L, arg, defval) enums_opt((L), DOMAIN_WINDOWEVENT, (arg), (defval))
#define checkwindoweventtype(L, arg) enums_check((L), DOMAIN_WINDOWEVENT, (arg))
#define pushwindoweventtype(L, val) enums_push((L), DOMAIN_WINDOWEVENT, (int)(val))
#define valueswindoweventtype(L) enums_values((L), DOMAIN_WINDOWEVENT)

#define testkeystate(L, arg, err) enums_test((L), DOMAIN_KEYSTATE, (arg), (err))
#define optkeystate(L, arg, defval) enums_opt((L), DOMAIN_KEYSTATE, (arg), (defval))
#define checkkeystate(L, arg) enums_check((L), DOMAIN_KEYSTATE, (arg))
#define pushkeystate(L, val) enums_push((L), DOMAIN_KEYSTATE, (int)(val))
#define valueskeystate(L) enums_values((L), DOMAIN_KEYSTATE)

#define testbutton(L, arg, err) enums_test((L), DOMAIN_BUTTON, (arg), (err))
#define optbutton(L, arg, defval) enums_opt((L), DOMAIN_BUTTON, (arg), (defval))
#define checkbutton(L, arg) enums_check((L), DOMAIN_BUTTON, (arg))
#define pushbutton(L, val) enums_push((L), DOMAIN_BUTTON, (int)(val))
#define valuesbutton(L) enums_values((L), DOMAIN_BUTTON)

#define testmousewheeldir(L, arg, err) enums_test((L), DOMAIN_MOUSEWHEEL_DIR, (arg), (err))
#define optmousewheeldir(L, arg, defval) enums_opt((L), DOMAIN_MOUSEWHEEL_DIR, (arg), (defval))
#define checkmousewheeldir(L, arg) enums_check((L), DOMAIN_MOUSEWHEEL_DIR, (arg))
#define pushmousewheeldir(L, val) enums_push((L), DOMAIN_MOUSEWHEEL_DIR, (int)(val))
#define valuesmousewheeldir(L) enums_values((L), DOMAIN_MOUSEWHEEL_DIR)

#define testcontrollertype(L, arg, err) enums_test((L), DOMAIN_CONTROLLER_TYPE, (arg), (err))
#define optcontrollertype(L, arg, defval) enums_opt((L), DOMAIN_CONTROLLER_TYPE, (arg), (defval))
#define checkcontrollertype(L, arg) enums_check((L), DOMAIN_CONTROLLER_TYPE, (arg))
#define pushcontrollertype(L, val) enums_push((L), DOMAIN_CONTROLLER_TYPE, (int)(val))
#define valuescontrollertype(L) enums_values((L), DOMAIN_CONTROLLER_TYPE)

#define testcontrollerbindtype(L, arg, err) enums_test((L), DOMAIN_CONTROLLER_BINDTYPE, (arg), (err))
#define optcontrollerbindtype(L, arg, defval) enums_opt((L), DOMAIN_CONTROLLER_BINDTYPE, (arg), (defval))
#define checkcontrollerbindtype(L, arg) enums_check((L), DOMAIN_CONTROLLER_BINDTYPE, (arg))
#define pushcontrollerbindtype(L, val) enums_push((L), DOMAIN_CONTROLLER_BINDTYPE, (int)(val))
#define valuescontrollerbindtype(L) enums_values((L), DOMAIN_CONTROLLER_BINDTYPE)

#define testcontrolleraxis(L, arg, err) enums_test((L), DOMAIN_CONTROLLER_AXIS, (arg), (err))
#define optcontrolleraxis(L, arg, defval) enums_opt((L), DOMAIN_CONTROLLER_AXIS, (arg), (defval))
#define checkcontrolleraxis(L, arg) enums_check((L), DOMAIN_CONTROLLER_AXIS, (arg))
#define pushcontrolleraxis(L, val) enums_push((L), DOMAIN_CONTROLLER_AXIS, (int)(val))
#define valuescontrolleraxis(L) enums_values((L), DOMAIN_CONTROLLER_AXIS)

#define testcontrollerbutton(L, arg, err) enums_test((L), DOMAIN_CONTROLLER_BUTTON, (arg), (err))
#define optcontrollerbutton(L, arg, defval) enums_opt((L), DOMAIN_CONTROLLER_BUTTON, (arg), (defval))
#define checkcontrollerbutton(L, arg) enums_check((L), DOMAIN_CONTROLLER_BUTTON, (arg))
#define pushcontrollerbutton(L, val) enums_push((L), DOMAIN_CONTROLLER_BUTTON, (int)(val))
#define valuescontrollerbutton(L) enums_values((L), DOMAIN_CONTROLLER_BUTTON)

#define testtextureaccess(L, arg, err) enums_test((L), DOMAIN_TEXTUREACCESS, (arg), (err))
#define opttextureaccess(L, arg, defval) enums_opt((L), DOMAIN_TEXTUREACCESS, (arg), (defval))
#define checktextureaccess(L, arg) enums_check((L), DOMAIN_TEXTUREACCESS, (arg))
#define pushtextureaccess(L, val) enums_push((L), DOMAIN_TEXTUREACCESS, (int)(val))
#define valuestextureaccess(L) enums_values((L), DOMAIN_TEXTUREACCESS)

#define testblendoperation(L, arg, err) enums_test((L), DOMAIN_BLENDOPERATION, (arg), (err))
#define optblendoperation(L, arg, defval) enums_opt((L), DOMAIN_BLENDOPERATION, (arg), (defval))
#define checkblendoperation(L, arg) enums_check((L), DOMAIN_BLENDOPERATION, (arg))
#define pushblendoperation(L, val) enums_push((L), DOMAIN_BLENDOPERATION, (int)(val))
#define valuesblendoperation(L) enums_values((L), DOMAIN_BLENDOPERATION)

#define testblendfactor(L, arg, err) enums_test((L), DOMAIN_BLENDFACTOR, (arg), (err))
#define optblendfactor(L, arg, defval) enums_opt((L), DOMAIN_BLENDFACTOR, (arg), (defval))
#define checkblendfactor(L, arg) enums_check((L), DOMAIN_BLENDFACTOR, (arg))
#define pushblendfactor(L, val) enums_push((L), DOMAIN_BLENDFACTOR, (int)(val))
#define valuesblendfactor(L) enums_values((L), DOMAIN_BLENDFACTOR)

#define testscalemode(L, arg, err) enums_test((L), DOMAIN_SCALEMODE, (arg), (err))
#define optscalemode(L, arg, defval) enums_opt((L), DOMAIN_SCALEMODE, (arg), (defval))
#define checkscalemode(L, arg) enums_check((L), DOMAIN_SCALEMODE, (arg))
#define pushscalemode(L, val) enums_push((L), DOMAIN_SCALEMODE, (int)(val))
#define valuesscalemode(L) enums_values((L), DOMAIN_SCALEMODE)

#define SWAPINTERVAL_IMMEDIATE          0
#define SWAPINTERVAL_VSYNC              1
#define SWAPINTERVAL_ADAPTIVE_VSYNC     -1
#define testswapinterval(L, arg, err) enums_test((L), DOMAIN_SWAPINTERVAL, (arg), (err))
#define optswapinterval(L, arg, defval) enums_opt((L), DOMAIN_SWAPINTERVAL, (arg), (defval))
#define checkswapinterval(L, arg) enums_check((L), DOMAIN_SWAPINTERVAL, (arg))
#define pushswapinterval(L, val) enums_push((L), DOMAIN_SWAPINTERVAL, (int)(val))
#define valuesswapinterval(L) enums_values((L), DOMAIN_SWAPINTERVAL)

#define testglattr(L, arg, err) enums_test((L), DOMAIN_GLATTR, (arg), (err))
#define optglattr(L, arg, defval) enums_opt((L), DOMAIN_GLATTR, (arg), (defval))
#define checkglattr(L, arg) enums_check((L), DOMAIN_GLATTR, (arg))
#define pushglattr(L, val) enums_push((L), DOMAIN_GLATTR, (int)(val))
#define valuesglattr(L) enums_values((L), DOMAIN_GLATTR)

#define testorientation(L, arg, err) enums_test((L), DOMAIN_ORIENTATION, (arg), (err))
#define optorientation(L, arg, defval) enums_opt((L), DOMAIN_ORIENTATION, (arg), (defval))
#define checkorientation(L, arg) enums_check((L), DOMAIN_ORIENTATION, (arg))
#define pushorientation(L, val) enums_push((L), DOMAIN_ORIENTATION, (int)(val))
#define valuesorientation(L) enums_values((L), DOMAIN_ORIENTATION)

#define testflashoperation(L, arg, err) enums_test((L), DOMAIN_FLASHOPERATION, (arg), (err))
#define optflashoperation(L, arg, defval) enums_opt((L), DOMAIN_FLASHOPERATION, (arg), (defval))
#define checkflashoperation(L, arg) enums_check((L), DOMAIN_FLASHOPERATION, (arg))
#define pushflashoperation(L, val) enums_push((L), DOMAIN_FLASHOPERATION, (int)(val))
#define valuesflashoperation(L) enums_values((L), DOMAIN_FLASHOPERATION)

#define testhittestresult(L, arg, err) enums_test((L), DOMAIN_HITTEST_RESULT, (arg), (err))
#define opthittestresult(L, arg, defval) enums_opt((L), DOMAIN_HITTEST_RESULT, (arg), (defval))
#define checkhittestresult(L, arg) enums_check((L), DOMAIN_HITTEST_RESULT, (arg))
#define pushhittestresult(L, val) enums_push((L), DOMAIN_HITTEST_RESULT, (int)(val))
#define valueshittestresult(L) enums_values((L), DOMAIN_HITTEST_RESULT)

#define testjoysticktype(L, arg, err) enums_test((L), DOMAIN_JOYSTICK_TYPE, (arg), (err))
#define optjoysticktype(L, arg, defval) enums_opt((L), DOMAIN_JOYSTICK_TYPE, (arg), (defval))
#define checkjoysticktype(L, arg) enums_check((L), DOMAIN_JOYSTICK_TYPE, (arg))
#define pushjoysticktype(L, val) enums_push((L), DOMAIN_JOYSTICK_TYPE, (int)(val))
#define valuesjoysticktype(L) enums_values((L), DOMAIN_JOYSTICK_TYPE)

#define testjoystickpowerlevel(L, arg, err) enums_test((L), DOMAIN_JOYSTICK_POWER_LEVEL, (arg), (err))
#define optjoystickpowerlevel(L, arg, defval) enums_opt((L), DOMAIN_JOYSTICK_POWER_LEVEL, (arg), (defval))
#define checkjoystickpowerlevel(L, arg) enums_check((L), DOMAIN_JOYSTICK_POWER_LEVEL, (arg))
#define pushjoystickpowerlevel(L, val) enums_push((L), DOMAIN_JOYSTICK_POWER_LEVEL, (int)(val))
#define valuesjoystickpowerlevel(L) enums_values((L), DOMAIN_JOYSTICK_POWER_LEVEL)

#define testsystemcursor(L, arg, err) enums_test((L), DOMAIN_SYSTEM_CURSOR, (arg), (err))
#define optsystemcursor(L, arg, defval) enums_opt((L), DOMAIN_SYSTEM_CURSOR, (arg), (defval))
#define checksystemcursor(L, arg) enums_check((L), DOMAIN_SYSTEM_CURSOR, (arg))
#define pushsystemcursor(L, val) enums_push((L), DOMAIN_SYSTEM_CURSOR, (int)(val))
#define valuessystemcursor(L) enums_values((L), DOMAIN_SYSTEM_CURSOR)

#define testlogcategory(L, arg, err) enums_test((L), DOMAIN_LOG_CATEGORY, (arg), (err))
#define optlogcategory(L, arg, defval) enums_opt((L), DOMAIN_LOG_CATEGORY, (arg), (defval))
#define checklogcategory(L, arg) enums_check((L), DOMAIN_LOG_CATEGORY, (arg))
#define pushlogcategory(L, val) enums_push((L), DOMAIN_LOG_CATEGORY, (int)(val))
#define valueslogcategory(L) enums_values((L), DOMAIN_LOG_CATEGORY)

#define testpowerstate(L, arg, err) enums_test((L), DOMAIN_POWERSTATE, (arg), (err))
#define optpowerstate(L, arg, defval) enums_opt((L), DOMAIN_POWERSTATE, (arg), (defval))
#define checkpowerstate(L, arg) enums_check((L), DOMAIN_POWERSTATE, (arg))
#define pushpowerstate(L, val) enums_push((L), DOMAIN_POWERSTATE, (int)(val))
#define valuespowerstate(L) enums_values((L), DOMAIN_POWERSTATE)

#define testfonthinting(L, arg, err) enums_test((L), DOMAIN_FONT_HINTING, (arg), (err))
#define optfonthinting(L, arg, defval) enums_opt((L), DOMAIN_FONT_HINTING, (arg), (defval))
#define checkfonthinting(L, arg) enums_check((L), DOMAIN_FONT_HINTING, (arg))
#define pushfonthinting(L, val) enums_push((L), DOMAIN_FONT_HINTING, (int)(val))
#define valuesfonthinting(L) enums_values((L), DOMAIN_FONT_HINTING)

#define testmusictype(L, arg, err) enums_test((L), DOMAIN_MUSIC_TYPE, (arg), (err))
#define optmusictype(L, arg, defval) enums_opt((L), DOMAIN_MUSIC_TYPE, (arg), (defval))
#define checkmusictype(L, arg) enums_check((L), DOMAIN_MUSIC_TYPE, (arg))
#define pushmusictype(L, val) enums_push((L), DOMAIN_MUSIC_TYPE, (int)(val))
#define valuesmusictype(L) enums_values((L), DOMAIN_MUSIC_TYPE)

#define testfading(L, arg, err) enums_test((L), DOMAIN_FADING, (arg), (err))
#define optfading(L, arg, defval) enums_opt((L), DOMAIN_FADING, (arg), (defval))
#define checkfading(L, arg) enums_check((L), DOMAIN_FADING, (arg))
#define pushfading(L, val) enums_push((L), DOMAIN_FADING, (int)(val))
#define valuesfading(L) enums_values((L), DOMAIN_FADING)

#define testaudioformat(L, arg, err) enums_test((L), DOMAIN_AUDIO_FORMAT, (arg), (err))
#define optaudioformat(L, arg, defval) enums_opt((L), DOMAIN_AUDIO_FORMAT, (arg), (defval))
#define checkaudioformat(L, arg) enums_check((L), DOMAIN_AUDIO_FORMAT, (arg))
#define pushaudioformat(L, val) enums_push((L), DOMAIN_AUDIO_FORMAT, (int)(val))
#define valuesaudioformat(L) enums_values((L), DOMAIN_AUDIO_FORMAT)

#define testhatstate(L, arg, err) enums_test((L), DOMAIN_HAT_STATE, (arg), (err))
#define opthatstate(L, arg, defval) enums_opt((L), DOMAIN_HAT_STATE, (arg), (defval))
#define checkhatstate(L, arg) enums_check((L), DOMAIN_HAT_STATE, (arg))
#define pushhatstate(L, val) enums_push((L), DOMAIN_HAT_STATE, (int)(val))
#define valueshatstate(L) enums_values((L), DOMAIN_HAT_STATE)

#define testfontdirection(L, arg, err) enums_test((L), DOMAIN_FONT_DIRECTION, (arg), (err))
#define optfontdirection(L, arg, defval) enums_opt((L), DOMAIN_FONT_DIRECTION, (arg), (defval))
#define checkfontdirection(L, arg) enums_check((L), DOMAIN_FONT_DIRECTION, (arg))
#define pushfontdirection(L, val) enums_push((L), DOMAIN_FONT_DIRECTION, (int)(val))
#define valuesfontdirection(L) enums_values((L), DOMAIN_FONT_DIRECTION)

#define testfontalign(L, arg, err) enums_test((L), DOMAIN_FONT_ALIGN, (arg), (err))
#define optfontalign(L, arg, defval) enums_opt((L), DOMAIN_FONT_ALIGN, (arg), (defval))
#define checkfontalign(L, arg) enums_check((L), DOMAIN_FONT_ALIGN, (arg))
#define pushfontalign(L, val) enums_push((L), DOMAIN_FONT_ALIGN, (int)(val))
#define valuesfontalign(L) enums_values((L), DOMAIN_FONT_ALIGN)

#define testtouchdevicetype(L, arg, err) enums_test((L), DOMAIN_TOUCH_DEVICE_TYPE, (arg), (err))
#define opttouchdevicetype(L, arg, defval) enums_opt((L), DOMAIN_TOUCH_DEVICE_TYPE, (arg), (defval))
#define checktouchdevicetype(L, arg) enums_check((L), DOMAIN_TOUCH_DEVICE_TYPE, (arg))
#define pushtouchdevicetype(L, val) enums_push((L), DOMAIN_TOUCH_DEVICE_TYPE, (int)(val))
#define valuestouchdevicetype(L) enums_values((L), DOMAIN_TOUCH_DEVICE_TYPE)

#define testrwopstype(L, arg, err) enums_test((L), DOMAIN_RWOPS_TYPE, (arg), (err))
#define optrwopstype(L, arg, defval) enums_opt((L), DOMAIN_RWOPS_TYPE, (arg), (defval))
#define checkrwopstype(L, arg) enums_check((L), DOMAIN_RWOPS_TYPE, (arg))
#define pushrwopstype(L, val) enums_push((L), DOMAIN_RWOPS_TYPE, (int)(val))
#define valuesrwopstype(L) enums_values((L), DOMAIN_RWOPS_TYPE)

#define testrwopswhence(L, arg, err) enums_test((L), DOMAIN_RWOPS_WHENCE, (arg), (err))
#define optrwopswhence(L, arg, defval) enums_opt((L), DOMAIN_RWOPS_WHENCE, (arg), (defval))
#define checkrwopswhence(L, arg) enums_check((L), DOMAIN_RWOPS_WHENCE, (arg))
#define pushrwopswhence(L, val) enums_push((L), DOMAIN_RWOPS_WHENCE, (int)(val))
#define valuesrwopswhence(L) enums_values((L), DOMAIN_RWOPS_WHENCE)

#define testhaptictype(L, arg, err) enums_test((L), DOMAIN_HAPTIC_TYPE, (arg), (err))
#define opthaptictype(L, arg, defval) enums_opt((L), DOMAIN_HAPTIC_TYPE, (arg), (defval))
#define checkhaptictype(L, arg) enums_check((L), DOMAIN_HAPTIC_TYPE, (arg))
#define pushhaptictype(L, val) enums_push((L), DOMAIN_HAPTIC_TYPE, (int)(val))
#define valueshaptictype(L) enums_values((L), DOMAIN_HAPTIC_TYPE)

#define testhapticdirection(L, arg, err) enums_test((L), DOMAIN_HAPTIC_DIRECTION, (arg), (err))
#define opthapticdirection(L, arg, defval) enums_opt((L), DOMAIN_HAPTIC_DIRECTION, (arg), (defval))
#define checkhapticdirection(L, arg) enums_check((L), DOMAIN_HAPTIC_DIRECTION, (arg))
#define pushhapticdirection(L, val) enums_push((L), DOMAIN_HAPTIC_DIRECTION, (int)(val))
#define valueshapticdirection(L) enums_values((L), DOMAIN_HAPTIC_DIRECTION)

#define testfullscreenmode(L, arg, err) enums_test((L), DOMAIN_FULLSCREEN_MODE, (arg), (err))
#define optfullscreenmode(L, arg, defval) enums_opt((L), DOMAIN_FULLSCREEN_MODE, (arg), (defval))
#define checkfullscreenmode(L, arg) enums_check((L), DOMAIN_FULLSCREEN_MODE, (arg))
#define pushfullscreenmode(L, val) enums_push((L), DOMAIN_FULLSCREEN_MODE, (int)(val))
#define valuesfullscreenmode(L) enums_values((L), DOMAIN_FULLSCREEN_MODE)

#define testsensortype(L, arg, err) enums_test((L), DOMAIN_SENSOR_TYPE, (arg), (err))
#define optsensortype(L, arg, defval) enums_opt((L), DOMAIN_SENSOR_TYPE, (arg), (defval))
#define checksensortype(L, arg) enums_check((L), DOMAIN_SENSOR_TYPE, (arg))
#define pushsensortype(L, val) enums_push((L), DOMAIN_SENSOR_TYPE, (int)(val))
#define valuessensortype(L) enums_values((L), DOMAIN_SENSOR_TYPE)

#define testglprofile(L, arg, err) enums_test((L), DOMAIN_GL_PROFILE, (arg), (err))
#define optglprofile(L, arg, defval) enums_opt((L), DOMAIN_GL_PROFILE, (arg), (defval))
#define checkglprofile(L, arg) enums_check((L), DOMAIN_GL_PROFILE, (arg))
#define pushglprofile(L, val) enums_push((L), DOMAIN_GL_PROFILE, (int)(val))
#define valuesglprofile(L) enums_values((L), DOMAIN_GL_PROFILE)

#if 0 /* scaffolding 7yy */
#define testxxx(L, arg, err) enums_test((L), DOMAIN_XXX, (arg), (err))
#define optxxx(L, arg, defval) enums_opt((L), DOMAIN_XXX, (arg), (defval))
#define checkxxx(L, arg) enums_check((L), DOMAIN_XXX, (arg))
#define pushxxx(L, val) enums_push((L), DOMAIN_XXX, (int)(val))
#define valuesxxx(L) enums_values((L), DOMAIN_XXX)
    CASE(xxx);

#endif

#endif /* enumsDEFINED */


